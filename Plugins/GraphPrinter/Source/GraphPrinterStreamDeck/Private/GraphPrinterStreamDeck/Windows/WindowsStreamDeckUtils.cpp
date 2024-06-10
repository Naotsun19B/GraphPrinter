// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "GraphPrinterStreamDeck/Windows/WindowsStreamDeckUtils.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Interfaces/IPluginManager.h"
#include "HAL/PlatformMisc.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
THIRD_PARTY_INCLUDES_START
#include <winreg.h>
#include <stdlib.h>
THIRD_PARTY_INCLUDES_END
#include "Windows/HideWindowsPlatformTypes.h"
#endif

namespace GraphPrinter
{
	namespace WindowsStreamDeckDefine
	{
		// The display name of the installed Stream Deck application.
		static const FString StreamDeckDisplayName = TEXT("Elgato Stream Deck");
		
		// The environment variable key of the folder where the Stream Deck data is located.
		static const FString AppDataEnvironmentVariableKey = TEXT("APPDATA");
		
		// The path to the folder where the Stream Deck plugin is located under the App Data folder.
		static const FString StreamDeckPluginRelativePath = FPaths::Combine(TEXT("Elgato"), TEXT("StreamDeck"), TEXT("Plugins"));

		// The name of the Stream Deck plugin's installed folder.
		static const FString StreamDeckPluginName = TEXT("com.naotsun.graphprinter.sdPlugin");
	}

#if PLATFORM_WINDOWS
	namespace WinReg
	{
		static constexpr int MaxKeyNameLength = 255;
		static const FString WindowsUninstallKeyPath = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");

		struct FScopedKey
		{
		public:
			explicit FScopedKey(const HKEY Root, const LPCWSTR SubKey, const REGSAM Desired = KEY_READ)
			{
				const LSTATUS Result = RegOpenKeyExW(
                	Root,
                	SubKey,
                	0,
                	Desired,
                	&Key
                );
				bIsValid = (Result == ERROR_SUCCESS);
			}
			
			~FScopedKey()
			{
				if (IsValid())
				{
					RegCloseKey(Key);
				}
			}

			bool IsValid() const
			{
				return bIsValid;
			}

			HKEY& operator*()
			{
				return Key;
			}

			bool GetNumOfSubKeys(DWORD& NumOfSubKeys) const
			{
				FILETIME LastWriteTime;
				const LSTATUS Result = RegQueryInfoKeyW(
					Key,
					nullptr,
					nullptr,
					nullptr,
					&NumOfSubKeys,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					&LastWriteTime
				);

				return (Result == ERROR_SUCCESS);
			}

			bool GetSubKeyName(const DWORD Index, FString& SubKeyName) const
			{
				TCHAR KeyNameBuffer[MaxKeyNameLength];
				DWORD KeyNameLength = MaxKeyNameLength;

				FILETIME LastWriteTime;
				const LSTATUS Result = RegEnumKeyExW(
					Key,
					Index,
					KeyNameBuffer,
					&KeyNameLength,
					nullptr,
					nullptr,
					nullptr,
					&LastWriteTime
				);

				if (Result == ERROR_SUCCESS)
				{
					SubKeyName = FString(KeyNameLength, KeyNameBuffer);
					return true;
				}

				return false;
			}

			bool GetStringValue(const LPCWSTR PropertyName, FString& Value) const
			{
				DWORD DataSize = 0;
				LSTATUS Result = RegGetValueW(
					Key,
					nullptr,
					PropertyName,
					RRF_RT_REG_SZ,
					nullptr,
					nullptr,
					&DataSize
				);
				if (Result != ERROR_SUCCESS)
				{
					return false;
				}
				
				auto& ValueBuffer = Value.GetCharArray();
				ValueBuffer.SetNumUninitialized(DataSize / sizeof(wchar_t));
				Result = RegGetValueW(
					Key,
					nullptr,
					PropertyName,
					RRF_RT_REG_SZ,
					nullptr,
					ValueBuffer.GetData(),
					&DataSize
				);
				
				return (Result == ERROR_SUCCESS);
			}

		private:
			HKEY Key;
			bool bIsValid;
		};
	}
#endif

	bool FWindowsStreamDeckUtils::IsStreamDeckInstalled()
	{
#if PLATFORM_WINDOWS
		const WinReg::FScopedKey WindowsUninstallKey(HKEY_LOCAL_MACHINE, *WinReg::WindowsUninstallKeyPath);

		DWORD NumOfSubKeys;
		if (!WindowsUninstallKey.GetNumOfSubKeys(NumOfSubKeys))
		{
			return false;
		}

		for (DWORD Index = 0; Index < NumOfSubKeys; Index++)
		{
			FString SubKeyName;
			if (!WindowsUninstallKey.GetSubKeyName(Index, SubKeyName))
			{
				continue;
			}

			const WinReg::FScopedKey ApplicationKey(HKEY_LOCAL_MACHINE, *(WinReg::WindowsUninstallKeyPath + TEXT("\\") + SubKeyName));
			FString ApplicationName;
			if (!ApplicationKey.GetStringValue(TEXT("DisplayName"), ApplicationName))
			{
				continue;
			}
			
			if (ApplicationName.Contains(WindowsStreamDeckDefine::StreamDeckDisplayName))
			{
				return true;
			}
		}

		return false;
#endif
	}

	void FWindowsStreamDeckUtils::InstallStreamDeckPlugin()
	{
#if PLATFORM_WINDOWS
		const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(Global::PluginName.ToString());
		check(Plugin.IsValid());
		const FString StreamDeckPluginFilePath = FPaths::ConvertRelativePathToFull(
			Plugin->GetBaseDir() / TEXT("Resources") / TEXT("StreamDeck") / TEXT("com.naotsun.graphprinter.streamDeckPlugin")
		);

		system(TCHAR_TO_ANSI(*FString::Printf(TEXT("\"%s\""), *StreamDeckPluginFilePath)));
#endif
	}

	bool FWindowsStreamDeckUtils::IsInstalledStreamDeckPlugin()
	{
		const FString AppDataPath = FPlatformMisc::GetEnvironmentVariable(*WindowsStreamDeckDefine::AppDataEnvironmentVariableKey);
		const FString StreamDeckPluginPath = FPaths::Combine(
			FPaths::ConvertRelativePathToFull(AppDataPath),
			WindowsStreamDeckDefine::StreamDeckPluginRelativePath,
			WindowsStreamDeckDefine::StreamDeckPluginName
		);
		
		return IFileManager::Get().DirectoryExists(*StreamDeckPluginPath);
	}
}
