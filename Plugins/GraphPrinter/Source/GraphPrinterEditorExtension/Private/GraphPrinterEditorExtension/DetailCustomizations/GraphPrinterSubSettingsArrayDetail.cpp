// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/DetailCustomizations/GraphPrinterSubSettingsArrayDetail.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSubSettings.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "DetailWidgetRow.h"
#include "Widgets/Layout/SExpandableArea.h"

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
	}

	void FGraphPrinterSubSettingsArrayDetail::CustomizeChildren(
		TSharedRef<IPropertyHandle> InStructPropertyHandle,
		IDetailChildrenBuilder& StructBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils
	)
	{
		uint32 NumOfElements;
		if (PropertyHandle->GetNumElements(NumOfElements) == FPropertyAccess::Fail)
		{
			return;
		}
		
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
			
			if (auto* DefaultObject = Cast<UGraphPrinterSubSettings>(Class->GetDefaultObject()))
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

				const FText DetailsTitle = DefaultObject->GetDetailsTitle();
				StructBuilder.AddGroup(
					DefaultObject->GetFName(),
					DetailsTitle
				)
				.HeaderRow()
					.WholeRowContent()
					.HAlign(HAlign_Fill)
					[
						SNew(SExpandableArea)
						.AreaTitle(DetailsTitle)
						.ToolTipText(Class->GetToolTipText())
						.BorderImage(FAppStyle::Get().GetBrush(TEXT("DetailsView.CategoryTop")))
						.Padding(FMargin(20.f, 0.f, 0.f, 0.f))
						.BodyContent()
						[
							DetailsView
						]
					];
			}
		}
	}
}
