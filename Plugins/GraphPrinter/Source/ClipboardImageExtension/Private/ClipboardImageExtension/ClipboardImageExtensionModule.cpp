// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

namespace ClipboardImageExtension
{
	class FClipboardImageExtensionModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};

	void FClipboardImageExtensionModule::StartupModule()
	{
	}

	void FClipboardImageExtensionModule::ShutdownModule()
	{
	}
}

IMPLEMENT_MODULE(ClipboardImageExtension::FClipboardImageExtensionModule, ClipboardImageExtension)
