// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

class FDetailWidgetRow;
class IDetailChildrenBuilder;
class IPropertyHandleArray;

namespace GraphPrinter
{
	/**
	 * A class that extends the detail panel of the default object of the subsetting class side by side.
	 */
	class FGraphPrinterSubSettingsArrayDetail : public IPropertyTypeCustomization
	{
	public:
		// Register-Unregister and instantiate this customization.
		static void Register();
		static void Unregister();
		static TSharedRef<IPropertyTypeCustomization> MakeInstance();

		// IPropertyTypeCustomization interface.
		virtual void CustomizeHeader(
			TSharedRef<IPropertyHandle> InStructPropertyHandle,
			FDetailWidgetRow& HeaderRow,
			IPropertyTypeCustomizationUtils& StructCustomizationUtils
		) override;
		virtual void CustomizeChildren(
			TSharedRef<IPropertyHandle> InStructPropertyHandle,
			IDetailChildrenBuilder& StructBuilder,
			IPropertyTypeCustomizationUtils& StructCustomizationUtils
		) override;
		// End of IPropertyTypeCustomization interface.

	private:
		// Handle for accessing FGraphPrinterSubSettingsArray::SubSettingsClasses.
		TSharedPtr<IPropertyHandleArray> PropertyHandle;
	};
}
