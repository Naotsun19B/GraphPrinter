// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace TextChunkHelper
{
	/**
	 * An interface for manipulating text chunks in image files.
	 */
	class TEXTCHUNKHELPER_API ITextChunk
	{
	public:
		// Destructor.
		virtual ~ITextChunk() = default;
		
		// Write to text chunk of image file.
		virtual bool Write(const TMap<FString, FString>& MapToWrite) = 0;

		// Read into text chunks in image files.
		virtual bool Read(TMap<FString, FString>& MapToRead) = 0;

	protected:
		// Copy loaded data to member variables.
		virtual bool Initialize(const FString& InFilename, const void* InCompressedData, int64 InCompressedSize) = 0;

	public:
		// Partial friend for calling initialization from the factory class of the module class.
		struct FInitializer
		{
		private:
			friend class FTextChunkHelperModule;
			ITextChunk* This = nullptr;

			explicit FInitializer(ITextChunk* InThis) : This(InThis) { check(This != nullptr); }
			bool Initialize(const FString& InFilename, const void* InCompressedData, int64 InCompressedSize)
			{
				return This->Initialize(InFilename, InCompressedData, InCompressedSize);
			}
		};
	};
}
