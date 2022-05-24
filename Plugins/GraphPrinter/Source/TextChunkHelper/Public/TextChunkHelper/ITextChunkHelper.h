// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "ImageWriteTypes.h"
#include "TextChunkHelper/ITextChunk.h"

namespace TextChunkHelper
{
	// Define a delegate for the text chunk generation process.
	DECLARE_DELEGATE_RetVal(TSharedPtr<ITextChunk>, FOnCreateTextChunk);
	
	/**
	 * The public interface to the TextChunkHelper module.
	 */
	class ITextChunkHelper : public IModuleInterface
	{
	public:
		// The name of the module for this plugin.
		TEXTCHUNKHELPER_API static const FName PluginModuleName;
		
	public:
		// Returns singleton instance, loading the module on demand if needed.
		static ITextChunkHelper& Get()
		{
			return FModuleManager::LoadModuleChecked<ITextChunkHelper>(PluginModuleName);
		}

		// Returns whether the module is loaded and ready to use.
		static bool IsAvailable()
		{
			return FModuleManager::Get().IsModuleLoaded(PluginModuleName);
		}

		// Returns a class of text chunks depending on the type of image file.
		virtual TSharedPtr<ITextChunk> CreateTextChunk(const FString& InFilename) const = 0;

		// Returns whether text chunks in the specified image format is supported.
		virtual bool IsSupportedImageFormat(const EDesiredImageFormat ImageFormat) const = 0;
		
		// Adds text chunk generation processing for the specified image format.
		virtual void RegisterTextChunkGenerator(
			const EDesiredImageFormat ImageFormat,
			const FOnCreateTextChunk& GenerationProcess
		) = 0;

		// Deletes the text chunk generation process for the specified image format.
		virtual void UnregisterTextChunkGenerator(const EDesiredImageFormat ImageFormat) = 0;
	};
}
