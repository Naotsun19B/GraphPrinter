// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "TextChunkHelper/Png/PngTextChunk.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeLock.h"
#include "Misc/Paths.h"

#if WITH_UNREALPNG

THIRD_PARTY_INCLUDES_START

#if PLATFORM_LINUX || PLATFORM_LINUXAARCH64
#include "ThirdParty/zlib/v1.2.8/include/Linux/x86_64-unknown-linux-gnu/zlib.h" 
#else
#if BEFORE_UE_5_00
#include "ThirdParty/zlib/zlib-1.2.5/Inc/zlib.h"
#else
#include "ThirdParty/zlib/1.2.12/include/zlib.h"
#endif
#endif

#if !WITH_LIBPNG_1_6
#include "ThirdParty/libPNG/libPNG-1.5.2/pnginfo.h"
#endif

#include <setjmp.h>
THIRD_PARTY_INCLUDES_END

// Disable warning "interaction between '_setjmp' and C++ object destruction is non-portable".
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4611)
#endif

namespace TextChunkHelper
{
	namespace PngTextChunkInternal
	{
		/**
		 * Classes for safely handling pointers used by libpng.
		 */
		class FPngReadGuard : public FNoncopyable
		{
		public:
			// Constructor.
			FPngReadGuard(
				png_voidp Context,
				png_error_ptr ErrorCallback,
				png_error_ptr WarningCallback,
				png_malloc_ptr MallocCallback,
				png_free_ptr FreeCallback,
				png_rw_ptr ReadDataCallback
			) 
			{
				ReadPtr = png_create_read_struct_2(
					PNG_LIBPNG_VER_STRING,
					Context,
					ErrorCallback,
					WarningCallback,
					nullptr,
					MallocCallback,
					FreeCallback
				);
				InfoPtr = png_create_info_struct(ReadPtr);
				png_set_read_fn(ReadPtr, Context, ReadDataCallback);
			}

			// Destructor.
			~FPngReadGuard()
			{
				png_destroy_read_struct(&ReadPtr, &InfoPtr, nullptr);
			}

			// Accessor and validation.
			png_structp GetReadPtr() const { return ReadPtr; }
			png_infop GetInfoPtr() const { return InfoPtr; }
			bool IsValid() const { return (ReadPtr != nullptr && InfoPtr != nullptr); }

		private:
			// Pointers used to access png files with libpng.
			png_structp ReadPtr;
			png_infop InfoPtr;
		};

		class FPngWriteGuard : public FNoncopyable
		{
		public:
			// Constructor.
			FPngWriteGuard(
				png_voidp Context,
				png_error_ptr ErrorCallback,
				png_error_ptr WarningCallback,
				png_rw_ptr WriteDataCallback,
				png_flush_ptr OutputFlushCallback
			)
			{
				WritePtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, Context, ErrorCallback, WarningCallback);
				InfoPtr = png_create_info_struct(WritePtr);
				png_set_write_fn(WritePtr, Context, WriteDataCallback, OutputFlushCallback);
			}

			// Destructor.
			~FPngWriteGuard()
			{
				png_destroy_write_struct(&WritePtr, &InfoPtr);
			}

			// Accessor and validation.
			png_structp GetWritePtr() const { return WritePtr; }
			png_infop GetInfoPtr() const { return InfoPtr; }
			bool IsValid() const { return (WritePtr != nullptr && InfoPtr != nullptr); }

		private:
			// Pointers used to access png files with libpng.
			png_structp WritePtr;
			png_infop InfoPtr;
		};

		// Copy compression levels, IHDR chunks, row pointers, channels.
		void CopyPngInfoStruct(
			png_structp DestinationPngPtr,
			png_infop DestinationInfoPtr,
			png_structp SourcePngPtr,
			png_infop SourceInfoPtr
		)
		{
			check(DestinationPngPtr != nullptr && DestinationInfoPtr != nullptr);
			check(SourcePngPtr != nullptr && SourceInfoPtr != nullptr);

			// Set compression level.
			{
				png_set_compression_level(DestinationPngPtr, Z_BEST_SPEED);
			}
			// Copy IHDR.
			{
				png_uint_32 Width;
				png_uint_32 Height;
				int32 BitDepth;
				int32 ColorType;
				int32 InterlaceMethod;
				int32 CompressionMethod;
				int32 FilterMethod;
				png_get_IHDR(SourcePngPtr, SourceInfoPtr, &Width, &Height, &BitDepth, &ColorType, &InterlaceMethod, &CompressionMethod, &FilterMethod);
				png_set_IHDR(DestinationPngPtr, DestinationInfoPtr, Width, Height, BitDepth, ColorType, InterlaceMethod, CompressionMethod, FilterMethod);
			}
			// Copy row pointers.
			{
				const png_bytepp RowPointers = png_get_rows(SourcePngPtr, SourceInfoPtr);
				png_set_rows(DestinationPngPtr, DestinationInfoPtr, RowPointers);
			}
			// Copy channels.
			{
				DestinationInfoPtr->channels = SourceInfoPtr->channels;
			}
		}

		// Check if either the key or value is empty and if the key and value string contains \0.
		bool ValidateMap(const TMap<FString, FString>& Map)
		{
			auto HasEscapeSequence = [](const FString& String) -> bool
			{
				for (const auto& Char : String)
				{
					if (Char == TEXT('\0'))
					{
						return true;
					}
				}

				return false;
			};

			bool bIsValid = true;

			for (const auto& Pair : Map)
			{
				if (Pair.Key.IsEmpty() || 
					Pair.Value.IsEmpty() ||
					HasEscapeSequence(Pair.Key) || 
					HasEscapeSequence(Pair.Value)
				)
				{
					bIsValid = false;
					break;
				}
			}

			return bIsValid;
		}

		// Converts a string in the state of TArray<uint8> to FString.
		FString ConvertByteArrayToString(const TArray<uint8>& ByteArray)
		{
			FString ConvertedString = TEXT("");
			for (const auto& Byte : ByteArray)
			{
				ConvertedString += FString::Printf(TEXT("%c"), Byte);
			}
			return ConvertedString;
		}

		// Define the name of the text chunk in the png file.
		static const FString TextChunkName = TEXT("tEXt");

		// Gets the position of the next text chunk closest to the StartPosition.
		// If not found, INDEX_NONE is returned.
		int32 GetTextChunkPosition(const TArray<uint8>& CompressedData, int32 StartPosition = 0)
		{
			int32 FoundPosition = INDEX_NONE;
			const int32 ChunkNameLength = TextChunkName.Len();
			for (int32 Index = StartPosition; Index < CompressedData.Num() - ChunkNameLength; Index++)
			{
				bool bIsMatch = true;
				for (int32 Offset = 0; Offset < ChunkNameLength; Offset++)
				{
					if (CompressedData[Index + Offset] != TextChunkName[Offset])
					{
						bIsMatch = false;
					}
				}

				if (bIsMatch)
				{
					FoundPosition = Index + ChunkNameLength;
					break;
				}
			}

			return FoundPosition;
		}

		// Only allow one thread to use libpng at a time.
		static FCriticalSection PngTextChunkHelperSection;

		// Determine if this file is in Png format from the Png signature size.
		static constexpr int32 PngSignatureSize = sizeof(png_size_t);
	}

	FPngTextChunk::FPngTextChunk()
		: Filename(TEXT(""))
		, ReadOffset(0)
	{
	}

	bool FPngTextChunk::Write(const TMap<FString, FString>& MapToWrite)
	{
		check(IsPng());

		// Determine if the map is useable.
		if (!PngTextChunkInternal::ValidateMap(MapToWrite))
		{
			UE_LOG(LogGraphPrinter, Warning, TEXT("Writing to a text chunk is not possible because either the key or value is empty, or the key and value string contains \0."));
			return false;
		}

		// Only allow one thread to use libpng at a time.
		FScopeLock PngLock(&PngTextChunkInternal::PngTextChunkHelperSection);
	
		// Reset to the beginning of file so we can use png_read_png(), which expects to start at the beginning.
		ReadOffset = 0;

		// Read the png_info of the original image file.
		const PngTextChunkInternal::FPngReadGuard ReadGuard(
			this,
			FPngTextChunk::UserError,
			FPngTextChunk::UserWarning,
			FPngTextChunk::UserMalloc,
			FPngTextChunk::UserFree,
			FPngTextChunk::UserReadCompressed
		);
		if (!ReadGuard.IsValid())
		{
			return false;
		}

		if (setjmp(png_jmpbuf(ReadGuard.GetReadPtr())) != 0)
		{
			return false;
		}

		png_read_png(ReadGuard.GetReadPtr(), ReadGuard.GetInfoPtr(), PNG_TRANSFORM_IDENTITY, nullptr);

		// Prepare png_struct etc. for writing.
		const PngTextChunkInternal::FPngWriteGuard WriteGuard(
			this,
			FPngTextChunk::UserError,
			FPngTextChunk::UserWarning,
			FPngTextChunk::UserWriteCompressed,
			FPngTextChunk::UserFlushData
		);
		if (!WriteGuard.IsValid())
		{
			return false;
		}

		if (setjmp(png_jmpbuf(WriteGuard.GetWritePtr())) != 0)
		{
			return false;
		}

		// Copy the read original png_info to png_info used for write.
		PngTextChunkInternal::CopyPngInfoStruct(
			WriteGuard.GetWritePtr(), WriteGuard.GetInfoPtr(),
			ReadGuard.GetReadPtr(), ReadGuard.GetInfoPtr()
		);
	
		// Create text chunk data and set it to png.
		// When converting from FString to png_charp, most of the time the escape sequences are mixed first.
		const int32 NumText = MapToWrite.Num();
		TArray<png_text> TextPtr;
		TextPtr.Reserve(NumText);
		for (const auto& Data : MapToWrite)
		{
			png_text Text;
			FMemory::Memzero(Text);

			auto Key = StringCast<ANSICHAR>(*Data.Key);
			const int32 KeyLength = TCString<ANSICHAR>::Strlen(Key.Get());
			if (KeyLength > 0)
			{
				auto* const KeyBuffer = static_cast<ANSICHAR*>(FMemory::Malloc(KeyLength));
				TCString<ANSICHAR>::Strcpy(KeyBuffer, KeyLength, Key.Get());
				Text.key = KeyBuffer;
			}
		
			auto Value = StringCast<ANSICHAR>(*Data.Value);
			const int32 ValueLength = TCString<ANSICHAR>::Strlen(Value.Get());
			if (ValueLength > 0)
			{
				auto* const ValueBuffer = static_cast<ANSICHAR*>(FMemory::Malloc(ValueLength));
				TCString<ANSICHAR>::Strcpy(ValueBuffer, ValueLength, Value.Get());
				Text.text = ValueBuffer;
			}

			Text.text_length = ValueLength;
			Text.itxt_length = 0;
			Text.compression = PNG_TEXT_COMPRESSION_NONE;

			TextPtr.Add(Text);
		}
		png_set_text(WriteGuard.GetWritePtr(), WriteGuard.GetInfoPtr(), TextPtr.GetData(), NumText);

		// Write the data prepared so far to the png file.
		CompressedData.Empty();
		png_write_png(WriteGuard.GetWritePtr(), WriteGuard.GetInfoPtr(), PNG_TRANSFORM_IDENTITY, nullptr);

		return FFileHelper::SaveArrayToFile(CompressedData, *Filename);
	}

	bool FPngTextChunk::Read(TMap<FString, FString>& MapToRead)
	{
		check(IsPng());

		// Only allow one thread to use libpng at a time.
		FScopeLock PngLock(&PngTextChunkInternal::PngTextChunkHelperSection);

		// Reset to the beginning of file so we can use png_read_png(), which expects to start at the beginning.
		ReadOffset = 0;

		// Read png_info.
		const PngTextChunkInternal::FPngReadGuard ReadGuard(
			this, 
			FPngTextChunk::UserError, 
			FPngTextChunk::UserWarning, 
			FPngTextChunk::UserMalloc, 
			FPngTextChunk::UserFree,
			FPngTextChunk::UserReadCompressed
		);
		if (!ReadGuard.IsValid())
		{
			return false;
		}

		if (setjmp(png_jmpbuf(ReadGuard.GetReadPtr())) != 0)
		{
			return false;
		}

		png_read_info(ReadGuard.GetReadPtr(), ReadGuard.GetInfoPtr());

		// Transfer the text chunk data to the argument map.
		/**
		 * png_textp TextPtr;
		 * int32 NumText;
		 * if (!png_get_text(ReadGuard.GetReadPtr(), ReadGuard.GetInfoPtr(), &TextPtr, &NumText))
		 * {
		 *     return false;
		 * }
		 *
		 * for (int32 Index = 0; Index < NumText; Index++)
		 * {
		 *     const auto Key = FString(ANSI_TO_TCHAR(TextPtr[Index].key));
		 *     const auto Value = FString(ANSI_TO_TCHAR(TextPtr[Index].text));
		 *     MapToRead.Add(Key, Value);
		 * }
		 */
		// Hopefully wanted to do it the above way, but using png_get_text results in a zero-terminated C string, 
		// so if an escape sequence is mixed in during writing, the data will be lost.
		// Therefore, the data in the text chuck is taken out in the following roundabout way.

		// Find out the number of texts.
		png_textp UnusedTextPtr;
		int32 NumText;
		if (!png_get_text(ReadGuard.GetReadPtr(), ReadGuard.GetInfoPtr(), &UnusedTextPtr, &NumText))
		{
			return false;
		}

		// Since there are as many text chunks as there are NumText, split the data by \0 by the amount of NumText.
		TArray<TArray<uint8>> SplitTextChunks;
		int32 SearchStartPosition = 0;
		for (int32 Count = 0; Count < NumText; Count++)
		{
			const int32 TextChunkStartPosition = PngTextChunkInternal::GetTextChunkPosition(CompressedData, SearchStartPosition);
			if (TextChunkStartPosition == INDEX_NONE)
			{
				break;
			}

			int32 SectionStart = TextChunkStartPosition;
			int32 SectionEnd = TextChunkStartPosition;
			for (int32 Index = TextChunkStartPosition; Index < CompressedData.Num(); Index++)
			{
				if (CompressedData[Index] == TEXT('\0'))
				{
					SectionEnd = Index - 1;
					if (SectionEnd - SectionStart > 0)
					{
						TArray<uint8> SplitTextChunk;
						for (int32 CopyIndex = SectionStart; CopyIndex <= SectionEnd; CopyIndex++)
						{
							SplitTextChunk.Add(CompressedData[CopyIndex]);
						}
						SplitTextChunks.Add(SplitTextChunk);
					}

					SectionStart = Index + 1;
				}

				if (SplitTextChunks.Num() == (Count + 1) * 2)
				{
					break;
				}
			}

			SearchStartPosition = SectionEnd;
		}

		// Fail if the number of carved data is not the number of NumText x2.
		if (SplitTextChunks.Num() != NumText * 2)
		{
			UE_LOG(LogGraphPrinter, Error, TEXT("------ The text could not be read correctly ------"));
			UE_LOG(LogGraphPrinter, Error, TEXT("Number of texts retrieved from the library : %d"), NumText * 2);
			UE_LOG(LogGraphPrinter, Error, TEXT("Number of texts actually retrieved         : %d"), SplitTextChunks.Num());
			UE_LOG(LogGraphPrinter, Error, TEXT("--------------------------------------------------"));
			return false;
		}

		// Convert from a byte array to a string map.
		for (int32 Index = 0; Index < SplitTextChunks.Num(); Index += 2)
		{
			check(SplitTextChunks.IsValidIndex(Index) && SplitTextChunks.IsValidIndex(Index + 1));

			const FString& Key = PngTextChunkInternal::ConvertByteArrayToString(SplitTextChunks[Index]);
			const FString& Value = PngTextChunkInternal::ConvertByteArrayToString(SplitTextChunks[Index + 1]);

			MapToRead.Add(Key, Value);
		}

		return PngTextChunkInternal::ValidateMap(MapToRead);
	}

	bool FPngTextChunk::Initialize(const FString& InFilename, const void* InCompressedData, int64 InCompressedSize)
	{
		FText FailedReason;
		if (!FPaths::ValidatePath(InFilename, &FailedReason))
		{
			UE_LOG(LogGraphPrinter, Warning, TEXT("%s"), *FailedReason.ToString());
			return false;
		}

		check(InCompressedData != nullptr && InCompressedSize > 0);

		Filename = InFilename;
		CompressedData.Empty(InCompressedSize);
		CompressedData.AddUninitialized(InCompressedSize);
		FMemory::Memcpy(CompressedData.GetData(), InCompressedData, InCompressedSize);

		return IsPng();
	}

	bool FPngTextChunk::IsPng() const
	{
		if (CompressedData.Num() > PngTextChunkInternal::PngSignatureSize)
		{
			png_size_t PngSignature = *reinterpret_cast<const png_size_t*>(CompressedData.GetData());
			return (0 == png_sig_cmp(reinterpret_cast<png_bytep>(&PngSignature), 0, PngTextChunkInternal::PngSignatureSize));
		}

		UE_LOG(LogGraphPrinter, Error, TEXT("This file is not a png file."));
		return false;
	}

	void FPngTextChunk::UserReadCompressed(png_structp PngPtr, png_bytep Data, png_size_t Length)
	{
		FPngTextChunk* Context = static_cast<FPngTextChunk*>(png_get_io_ptr(PngPtr));
		if (Context == nullptr)
		{
			UE_LOG(LogGraphPrinter, Fatal, TEXT("[%s] Context is invalid."), GET_FUNCTION_NAME_STRING_CHECKED(FPngTextChunk, UserReadCompressed));
		}

		if (Context->ReadOffset + static_cast<int64>(Length) <= Context->CompressedData.Num())
		{
			FMemory::Memcpy(Data, &Context->CompressedData[Context->ReadOffset], Length);
			Context->ReadOffset += Length;
		}
		else
		{
			UE_LOG(LogGraphPrinter, Error, TEXT("Invalid read position for CompressedData."));
		}
	}

	void FPngTextChunk::UserWriteCompressed(png_structp PngPtr, png_bytep Data, png_size_t Length)
	{
		FPngTextChunk* Context = static_cast<FPngTextChunk*>(png_get_io_ptr(PngPtr));
		if (Context == nullptr)
		{
			UE_LOG(LogGraphPrinter, Fatal, TEXT("[%s] Context is invalid."), GET_FUNCTION_NAME_STRING_CHECKED(FPngTextChunk, UserWriteCompressed));
		}

		const int64 Offset = Context->CompressedData.AddUninitialized(Length);
		FMemory::Memcpy(&Context->CompressedData[Offset], Data, Length);
	}

	void FPngTextChunk::UserFlushData(png_structp PngPtr)
	{
	}

	void FPngTextChunk::UserError(png_structp PngPtr, png_const_charp ErrorMessage)
	{
		UE_LOG(LogGraphPrinter, Error, TEXT("libpng Error : %s"), ANSI_TO_TCHAR(ErrorMessage));
	}

	void FPngTextChunk::UserWarning(png_structp PngPtr, png_const_charp WarningMessage)
	{
		UE_LOG(LogGraphPrinter, Warning, TEXT("libpng Warning : %s"), ANSI_TO_TCHAR(WarningMessage));
	}

	void* FPngTextChunk::UserMalloc(png_structp PngPtr, png_size_t Size)
	{
		check(Size > 0);
		return FMemory::Malloc(Size);
	}

	void FPngTextChunk::UserFree(png_structp PngPtr, png_voidp StructPtr)
	{
		check(StructPtr);
		FMemory::Free(StructPtr);
	}
}

// Renable warning "interaction between '_setjmp' and C++ object destruction is non-portable".
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif
