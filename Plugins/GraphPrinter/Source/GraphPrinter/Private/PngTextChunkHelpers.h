﻿// Copyright 2020 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Include libPNG headers needed to work with png.
THIRD_PARTY_INCLUDES_START
#include "ThirdParty/libPNG/libPNG-1.5.2/png.h"
#include "ThirdParty/libPNG/libPNG-1.5.2/pnginfo.h"
#include "ThirdParty/libPNG/libPNG-1.5.2/pngstruct.h"
THIRD_PARTY_INCLUDES_END

/**
 * Helper base class for accessing text chunks in png files.
 * Since this class is a base class, it has no function.
 */
class FPngTextChunkHelpers
{
public:
	// The signature length for PNG.
	static const int32 SignatureLength;
	// Size of a byte.
	static const int32 ByteSize;
	// The number of bits used for storing the length of a file.
	static const int32 SizeWidth;

public:
	// Constructor.
	FPngTextChunkHelpers(const FString& InFilename);

	// Destructor.
	virtual ~FPngTextChunkHelpers();

	// If the read png file is invalid, it will be false.
	bool IsValid() const { return bIsValid; }
 
	// Get valid and absolute file path.
	const FString& GetFilename() const { return ValidFilename; }

protected:
	// Variables used by libPNG.
 	png_structp ReadPtr;
	png_infop InfoPtr;
	png_bytep* RowPointers;

	// If the read png file is invalid, it will be false.
	bool bIsValid;

	// Valid and absolute file path.
	FString ValidFilename;
};

/**
 * Helper class to read text chunks in png files.
 */
class FPngTextChunkReader : public FPngTextChunkHelpers
{
public:
	// Constructor.
	FPngTextChunkReader(const FString& InFilename) : FPngTextChunkHelpers(InFilename) {}

	// Try read text chunks.
	bool ReadTextChunk(TMap<FString, FString>& TextChunk);
};

/**
 * Helper class to write to text chunks of png files.
 */
class FPngTextChunkWriter : public FPngTextChunkHelpers
{
public:
	// Constructor.
	FPngTextChunkWriter(const FString& InFilename) : FPngTextChunkHelpers(InFilename) {}

	// Destructor.
	~FPngTextChunkWriter();

	// Try write text chunks.
	bool WriteTextChunk(const FString& Key, const FString& Value);

private:
	// Variable used by libPNG.
	png_structp WritePtr;
};