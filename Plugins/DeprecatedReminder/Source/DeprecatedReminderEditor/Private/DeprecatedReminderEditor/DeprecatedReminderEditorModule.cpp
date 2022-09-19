// Copyright 2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "DeprecatedReminderEditor/Utilities/DeprecatedReminderSettings.h"

namespace DeprecatedReminder
{
	class FDeprecatedReminderModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};
	
	void FDeprecatedReminderModule::StartupModule()
	{
		// Register settings.
		UDeprecatedReminderSettings::Register();
	}

	void FDeprecatedReminderModule::ShutdownModule()
	{
		// Unregister settings.
		UDeprecatedReminderSettings::Unregister();
	}	
}

IMPLEMENT_MODULE(DeprecatedReminder::FDeprecatedReminderModule, DeprecatedReminderEditor)