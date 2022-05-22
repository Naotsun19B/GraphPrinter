// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterGlobals/Utilities/GraphPrinterSubSettings.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "ISettingsEditorModule.h"

void UGraphPrinterSubSettings::PostInitProperties()
{
	Super::PostInitProperties();

	if (IsTemplate())
	{
		if (UClass* Class = GetClass())
		{
			if (!Class->HasAnyClassFlags(CLASS_Abstract))
			{
				UGraphPrinterSettings::FSubSettingsRegister Register;
				Register.Register(Class);
			}
		}
	}
}

void UGraphPrinterSubSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

#if BEFORE_UE_4_24
	const UProperty* Property =
#else
	const FProperty* Property =
#endif
		PropertyAccessUtil::FindPropertyByName(
			PropertyChangedEvent.MemberProperty->GetFName(),
			GetClass()
		);
	if (Property == nullptr)
	{
		return;
	}

	// Properties are not saved because they are not directly registered in the editor preferences.
	// Therefore, save it in the config file here.
	if (Property->HasAnyPropertyFlags(CPF_Config))
	{
		UpdateSinglePropertyInConfigFile(Property, GetConfigFilename(this));
	}

	// The meta specifier ConfigRestartRequired cannot be used because it is not directly registered in the editor preferences.
	// So a notification prompting you to restart is displayed from here instead.
	if (Property->GetBoolMetaData(TEXT("ConfigRestartRequired")))
	{
		if (auto* SettingsEditorModule = FModuleManager::GetModulePtr<ISettingsEditorModule>(TEXT("SettingsEditor")))
        {
        	SettingsEditorModule->OnApplicationRestartRequired();
        }
	}
}
