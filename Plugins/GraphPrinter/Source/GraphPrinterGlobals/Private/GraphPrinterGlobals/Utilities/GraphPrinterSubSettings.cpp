// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterGlobals/Utilities/GraphPrinterSubSettings.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

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

	if (Property->HasAnyPropertyFlags(CPF_Config))
	{
		UpdateSinglePropertyInConfigFile(Property, GetConfigFilename(this));
	}
}
