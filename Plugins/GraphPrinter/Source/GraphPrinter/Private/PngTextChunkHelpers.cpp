﻿// Copyright 2020 Naotsun. All Rights Reserved.

#include "PngTextChunkHelpers.h"

//////////////////////////////////////////////////////
// FPngTextChunkHelpers.

const int32 FPngTextChunkHelpers::SignatureLength = 8;
const int32 FPngTextChunkHelpers::ByteSize = 8;
const int32 FPngTextChunkHelpers::SizeWidth = 32;

FPngTextChunkHelpers::FPngTextChunkHelpers(const FString& InFilename)
	: bIsValid(true)
{
	// Check file path.
	ValidFilename = FPaths::ConvertRelativePathToFull(InFilename);
	if (!FPaths::ValidatePath(ValidFilename))
	{
		bIsValid = false;
		return;
	}

	// Check if the file opened.
	FILE* FilePtr;
	if (fopen_s(&FilePtr, TCHAR_TO_ANSI(*ValidFilename), "rb") != 0)
	{
		bIsValid = false;
		return;
	}

	// Reading process starts from here.
	unsigned char Header[ByteSize];
	fread(Header, 1, SignatureLength, FilePtr);

	// Check if it is a PNG.
	if (png_sig_cmp(Header, 0, SignatureLength))
	{
		fclose(FilePtr);
		bIsValid = false;
		return;
	}

	// Set up libPNG data structures and error handling.
	ReadPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (ReadPtr == NULL)
	{
		fclose(FilePtr);
		bIsValid = false;
		return;
	}

	InfoPtr = png_create_info_struct(ReadPtr);
	if (InfoPtr == NULL)
	{
		png_destroy_read_struct(&ReadPtr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(FilePtr);
		bIsValid = false;
		return;
	}

	png_infop EndInfo = png_create_info_struct(ReadPtr);
	if (EndInfo == NULL)
	{
		png_destroy_read_struct(&ReadPtr, &InfoPtr, (png_infopp)NULL);
		fclose(FilePtr);
		bIsValid = false;
		return;
	}
	// The reading process ends here.

	// Initialize IO for png.
	png_init_io(ReadPtr, FilePtr);

	// Alert libPNG that we read PNG_SIG_LENGTH bytes at the beginning.
	png_set_sig_bytes(ReadPtr, SignatureLength);

	// Read the entire png image into memory.
	png_read_png(ReadPtr, InfoPtr, PNG_TRANSFORM_IDENTITY, NULL);

	RowPointers = png_get_rows(ReadPtr, InfoPtr);

	// Make sure the bit depth is correct.
	if (ReadPtr->bit_depth != ByteSize)
	{
		bIsValid = false;
	}

	fclose(FilePtr);
}

FPngTextChunkHelpers::~FPngTextChunkHelpers()
{
	png_destroy_read_struct(&ReadPtr, &InfoPtr, NULL);
}

//////////////////////////////////////////////////////
// FPngTextChunkReader.

bool FPngTextChunkReader::ReadTextChunk(TMap<FString, FString>& TextChunk)
{
	if (!bIsValid)
	{
		return false;
	}

	if (InfoPtr == NULL)
	{
		return false;
	}

	png_textp TextPtr;
	int32 NumText;
	if (!png_get_text(ReadPtr, InfoPtr, &TextPtr, &NumText))
	{
		return false;
	}

	for (int32 Index = 0; Index < NumText; Index++)
	{
		TextChunk.Add(ANSI_TO_TCHAR(TextPtr[Index].key), ANSI_TO_TCHAR(TextPtr[Index].text));
	}

	return true;
}

//////////////////////////////////////////////////////
// FPngTextChunkWriter.

FPngTextChunkWriter::~FPngTextChunkWriter()
{
	if (WritePtr != NULL)
	{
		png_destroy_write_struct(&WritePtr, &InfoPtr);
	}
}

bool FPngTextChunkWriter::WriteTextChunk(const FString& Key, const FString& Value)
{
	if (!bIsValid)
	{
		return false;
	}

	FILE* FilePtr;
	if (fopen_s(&FilePtr, TCHAR_TO_ANSI(*ValidFilename), "wb") != 0)
	{
		return false;
	}

	WritePtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (WritePtr == NULL)
	{
		fclose(FilePtr);
		return false;
	}

	// Writing process starts from here.
	png_init_io(WritePtr, FilePtr);

	png_text text_ptr[1];

	text_ptr[0].key = TCHAR_TO_ANSI(*Key);
	text_ptr[0].text = TCHAR_TO_ANSI(*Value);
	text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;

	png_set_text(WritePtr, InfoPtr, text_ptr, 1);

	png_set_rows(WritePtr, InfoPtr, RowPointers);

	png_write_png(WritePtr, InfoPtr, PNG_TRANSFORM_IDENTITY, NULL);
	// The writing process ends here.

	fclose(FilePtr);

	return true;
}