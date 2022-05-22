// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/DetailCustomizations/GraphPrinterSubSettingsArrayDetail.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSubSettings.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"

namespace GraphPrinter
{
	namespace GraphPrinterSubSettingsArrayDetailInternal
	{
		static FPropertyEditorModule& GetPropertyEditorModule()
		{
			return FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		}
	}
	
	void FGraphPrinterSubSettingsArrayDetail::Register()
	{
		FPropertyEditorModule& PropertyEditorModule = GraphPrinterSubSettingsArrayDetailInternal::GetPropertyEditorModule();
		PropertyEditorModule.RegisterCustomPropertyTypeLayout(
			*GetNameSafe(FGraphPrinterSubSettingsArray::StaticStruct()),
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FGraphPrinterSubSettingsArrayDetail::MakeInstance)
		);
	}

	void FGraphPrinterSubSettingsArrayDetail::Unregister()
	{
		FPropertyEditorModule& PropertyEditorModule = GraphPrinterSubSettingsArrayDetailInternal::GetPropertyEditorModule();
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout(
			*GetNameSafe(FGraphPrinterSubSettingsArray::StaticStruct())
		);
	}

	TSharedRef<IPropertyTypeCustomization> FGraphPrinterSubSettingsArrayDetail::MakeInstance()
	{
		return MakeShared<FGraphPrinterSubSettingsArrayDetail>();
	}
	
	void FGraphPrinterSubSettingsArrayDetail::CustomizeHeader(
		TSharedRef<IPropertyHandle> InStructPropertyHandle,
		FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils
	)
	{
		uint32 NumChildren;
		InStructPropertyHandle->GetNumChildren(NumChildren);

		for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
		{
			const TSharedPtr<IPropertyHandle> ChildHandle = InStructPropertyHandle->GetChildHandle(ChildIndex);
			if (!ChildHandle.IsValid())
			{
				continue;
			}
			
			if (ChildHandle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED(FGraphPrinterSubSettingsArray, SubSettingsClasses))
			{
				PropertyHandle = ChildHandle->AsArray();
			}
		}
		check(PropertyHandle.IsValid());
		
		uint32 NumOfElements;
		if (PropertyHandle->GetNumElements(NumOfElements) == FPropertyAccess::Fail)
		{
			return;
		}

		const TSharedRef<SVerticalBox> SubSettingsList = SNew(SVerticalBox);
		for (uint32 Index = 0; Index < NumOfElements; Index++)
		{
			const TSharedRef<IPropertyHandle> ElementHandle = PropertyHandle->GetElement(Index);
			UObject* ElementValue;
			if (ElementHandle->GetValue(ElementValue) == FPropertyAccess::Fail)
			{
				continue;
			}

			const auto* Class = Cast<UClass>(ElementValue);
			if (!IsValid(Class))
			{
				continue;
			}
			
			if (UObject* DefaultObject = Class->GetDefaultObject())
			{
				FDetailsViewArgs DetailsViewArgs;
				DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
				DetailsViewArgs.bAllowSearch = true;
				DetailsViewArgs.bHideSelectionTip = false;
				DetailsViewArgs.bShowOptions = false;
				DetailsViewArgs.bShowPropertyMatrixButton = false;
				DetailsViewArgs.bShowScrollBar = false;
				
				FPropertyEditorModule& PropertyEditorModule = GraphPrinterSubSettingsArrayDetailInternal::GetPropertyEditorModule();
				TSharedRef<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
				DetailsView->SetObject(DefaultObject);
				
				SubSettingsList->AddSlot()
				.AutoHeight()
				[
					SNew(SVerticalBox)
					//+ SVerticalBox::Slot()
					//.MaxHeight(50.f)
					//.VAlign(VAlign_Top)
					//[
					//	SNew(SBorder)
					//	.BorderImage(FAppStyle::Get().GetBrush(TEXT("Brushes.Panel")))
					//	.VAlign(VAlign_Center)
					//	[
					//		SNew(STextBlock)
					//		.Text(FText::FromString(GetNameSafe(Class)))
					//		.Font(FAppStyle::Get().GetFontStyle(TEXT("DetailsView.CategoryFontStyle")))
					//		.TextStyle(FAppStyle::Get(), TEXT("DetailsView.CategoryTextStyle"))
					//	]
					//]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.VAlign(VAlign_Fill)
					[
						DetailsView
					]
				];
			}
		}
		
		HeaderRow
			.ValueContent()
			.HAlign(HAlign_Fill)
			[
				SubSettingsList
			];
	}

	void FGraphPrinterSubSettingsArrayDetail::CustomizeChildren(
		TSharedRef<IPropertyHandle> InStructPropertyHandle,
		IDetailChildrenBuilder& StructBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils
	)
	{
	}
}
