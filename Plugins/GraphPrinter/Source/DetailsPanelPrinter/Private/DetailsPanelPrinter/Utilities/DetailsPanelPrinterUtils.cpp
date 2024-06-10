// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/Utilities/DetailsPanelPrinterUtils.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterUtils.h"
#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SWindow.h"
#include "SDetailsView.h"
#include "SActorDetails.h"
#if UE_5_00_OR_LATER
#include "SSubobjectInstanceEditor.h"
#endif
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"

#define LOCTEXT_NAMESPACE "DetailsPanelPrinterUtils"

namespace GraphPrinter
{
	namespace JsonFieldNames
	{
		static const FString ClassFieldName			= TEXT("Class");
		static const FString PropertiesFieldName	= TEXT("Properties");
	}
	
	TSharedPtr<SDetailsView> FDetailsPanelPrinterUtils::FindNearestChildDetailsView(const TSharedPtr<SWidget>& SearchTarget)
	{
		TSharedPtr<SDetailsView> FoundDetailsView = nullptr;
		
		FWidgetPrinterUtils::EnumerateChildWidgets(
			SearchTarget,
			[&FoundDetailsView](const TSharedPtr<SWidget>& ChildWidget) -> bool
			{
				const TSharedPtr<SDetailsView> DetailsView = GP_CAST_SLATE_WIDGET(SDetailsView, ChildWidget);
				if (DetailsView.IsValid())
				{
					FoundDetailsView = DetailsView;
					return false;
				}

				return true;
			}
		);

		return FoundDetailsView;
	}

	TSharedPtr<SDetailsView> FDetailsPanelPrinterUtils::GetActiveDetailsView()
	{
		const TSharedRef<FGlobalTabmanager> GlobalTabManager = FGlobalTabmanager::Get();
		const TSharedPtr<SDockTab> ActiveTab = GlobalTabManager->GetActiveTab();
		if (ActiveTab.IsValid())
		{
			const TSharedPtr<SWidget> DockingTabStack = FWidgetPrinterUtils::FindNearestParentDockingTabStack(ActiveTab);
			if (DockingTabStack.IsValid())
			{
				TSharedPtr<SDetailsView> DetailsView = FindNearestChildDetailsView(DockingTabStack);
				if (DetailsView.IsValid())
				{
					return DetailsView;
				}
			}
		}

		const TSharedPtr<SWindow> ActiveWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
		if (ActiveWindow.IsValid())
		{
			return FindNearestChildDetailsView(ActiveWindow);
		}

		return nullptr;
	}

	TSharedPtr<SActorDetails> FDetailsPanelPrinterUtils::FindNearestChildActorDetailsView(const TSharedPtr<SWidget>& SearchTarget)
	{
		TSharedPtr<SActorDetails> FoundActorDetailsView = nullptr;
		
		FWidgetPrinterUtils::EnumerateChildWidgets(
			SearchTarget,
			[&FoundActorDetailsView](const TSharedPtr<SWidget>& ChildWidget) -> bool
			{
				const TSharedPtr<SActorDetails> ActorDetailsView = GP_CAST_SLATE_WIDGET(SActorDetails, ChildWidget);
				if (ActorDetailsView.IsValid())
				{
					FoundActorDetailsView = ActorDetailsView;
					return false;
				}

				return true;
			}
		);

		return FoundActorDetailsView;
	}

	TSharedPtr<SActorDetails> FDetailsPanelPrinterUtils::GetActiveActorDetailsView()
	{
		const TSharedRef<FGlobalTabmanager> GlobalTabManager = FGlobalTabmanager::Get();
		const TSharedPtr<SDockTab> ActiveTab = GlobalTabManager->GetActiveTab();
		if (ActiveTab.IsValid())
		{
			const TSharedPtr<SWidget> DockingTabStack = FWidgetPrinterUtils::FindNearestParentDockingTabStack(ActiveTab);
			if (DockingTabStack.IsValid())
			{
				TSharedPtr<SActorDetails> ActorDetailsView = FindNearestChildActorDetailsView(DockingTabStack);
				if (ActorDetailsView.IsValid())
				{
					return ActorDetailsView;
				}
			}
		}

		const TSharedPtr<SWindow> ActiveWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
		if (ActiveWindow.IsValid())
		{
			return FindNearestChildActorDetailsView(ActiveWindow);
		}

		return nullptr;
	}

	TSharedPtr<SWidget> FDetailsPanelPrinterUtils::FindNearestChildSubobjectInstanceEditor(const TSharedPtr<SActorDetails>& SearchTarget)
	{
		TSharedPtr<SWidget> FoundSubobjectInstanceEditor = nullptr;

#if UE_5_00_OR_LATER
		FWidgetPrinterUtils::EnumerateChildWidgets(
			SearchTarget,
			[&FoundSubobjectInstanceEditor](const TSharedPtr<SWidget>& ChildWidget) -> bool
			{
				const TSharedPtr<SSubobjectInstanceEditor> SubobjectInstanceEditor = GP_CAST_SLATE_WIDGET(SSubobjectInstanceEditor, ChildWidget);
				if (SubobjectInstanceEditor.IsValid())
				{
					FoundSubobjectInstanceEditor = SubobjectInstanceEditor;
					return false;
				}
		
				return true;
			}
		);
#endif

		return FoundSubobjectInstanceEditor;
	}

	FVector2D FDetailsPanelPrinterUtils::GetDifferenceBetweenWidgetLocalSizeAndDesiredSize(TSharedPtr<SWidget> Widget)
	{
		check(Widget.IsValid());
		
		const FVector2D DesiredSize = Widget->GetDesiredSize();
		
		const FGeometry& Geometry =
#if UE_4_24_OR_LATER
			Widget->GetTickSpaceGeometry();
#else
			Widget->GetCachedGeometry();
#endif
		const FVector2D LocalSize = Geometry.GetLocalSize();

#if UE_5_01_OR_LATER
		if (DesiredSize.ComponentwiseAllGreaterThan(LocalSize))
#else
		if (DesiredSize > LocalSize)
#endif
		{
			return (DesiredSize - LocalSize);
		}

		return FVector2D::ZeroVector;
	}

	bool FDetailsPanelPrinterUtils::ImportObjectPropertiesFromJsonString(UObject* Object, const FString& JsonString)
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
		const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
		if (!FJsonSerializer::Deserialize(JsonReader, JsonObject))
		{
			return false;
		}

		if (!IsValid(Object) || !JsonObject.IsValid())
		{
			return false;
		}

		if (JsonObjectToUObject(*Object, JsonObject.ToSharedRef()) == 0)
		{
			return false;
		}

		Object->Modify();
		Object->MarkPackageDirty();

		return true;
	}

	bool FDetailsPanelPrinterUtils::ExportObjectPropertiesAsJsonString(UObject* Object, FString& JsonString)
	{
		if (!IsValid(Object))
		{
			return false;
		}
		
		const TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
		if (UObjectToJsonObject(*Object, JsonObject, CPF_Edit | CPF_BlueprintVisible) == 0)
		{
			return false;
		}
		
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
		return FJsonSerializer::Serialize(JsonObject, Writer);
	}

	int32 FDetailsPanelPrinterUtils::JsonObjectToUObject(UObject& Object, const TSharedRef<FJsonObject>& JsonObject)
	{
		// Checks if the class recorded in Json and the class of the passed object match.
		bool bIsMatchesClass = false;
		FString ClassName;
		check((JsonObject->TryGetStringField(JsonFieldNames::ClassFieldName, ClassName)));

		FString ObjectClassName = TEXT("Unknown");
		UClass* Class = Object.GetClass();
		if (IsValid(Class))
		{
			ObjectClassName = Class->GetName();
		}
		if (ClassName.Equals(ObjectClassName))
		{
			bIsMatchesClass = true;
		}
			
		if (!bIsMatchesClass)
		{
			FGraphPrinterUtils::ShowNotification(
				FText::Format(
					LOCTEXT("IleagalClassErrorFormat", "The class of the loaded data ({0}) and the class of the target object ({1}) do not match."),
					FText::FromString(ClassName),
					FText::FromString(ObjectClassName)
				),
				FGraphPrinterUtils::CS_Fail,
				7.f
			);
			return 0;
		}
		
		const TSharedPtr<FJsonObject>* PropertiesJsonObjectPtr;
		check(JsonObject->TryGetObjectField(JsonFieldNames::PropertiesFieldName, PropertiesJsonObjectPtr));
		check(PropertiesJsonObjectPtr != nullptr);

		int32 NumOfProperties = 0;
		const TSharedRef<FJsonObject> PropertiesJsonObject = PropertiesJsonObjectPtr->ToSharedRef();
#if UE_4_25_OR_LATER
		for (auto* Property : TFieldRange<FProperty>(Class, EFieldIteratorFlags::IncludeSuper, EFieldIteratorFlags::ExcludeDeprecated))
#else
		for (auto* Property : TFieldRange<UProperty>(Class, EFieldIteratorFlags::IncludeSuper, EFieldIteratorFlags::ExcludeDeprecated))
#endif
		{
			if (Property == nullptr)
			{
				continue;
			}

#if UE_4_25_OR_LATER
			if (const auto* ObjectProperty = CastField<FObjectProperty>(Property))
#else
			if (const auto* ObjectProperty = Cast<UObjectProperty>(Property))
#endif
			{
				UObject** ChildObjectPtr = ObjectProperty->ContainerPtrToValuePtr<UObject*>(&Object);
				if (ChildObjectPtr == nullptr)
				{
					continue;
				}

				UObject* ChildObject = *ChildObjectPtr;
				if (!IsValid(ChildObject))
				{
					continue;
				}

				const TSharedPtr<FJsonObject>* ChildJsonObjectPtr;
				if (!PropertiesJsonObject->TryGetObjectField(ObjectProperty->GetName(), ChildJsonObjectPtr))
				{
					continue;
				}
				if (ChildJsonObjectPtr == nullptr || !ChildJsonObjectPtr->IsValid())
				{
					continue;
				}

				const TSharedRef<FJsonObject> ChildJsonObject = ChildJsonObjectPtr->ToSharedRef();
				if (JsonObjectToUObject(*ChildObject, ChildJsonObject) == 0)
				{
					return 0;
				}
			}
#if UE_4_25_OR_LATER
			else if (const auto* StructProperty = CastField<FStructProperty>(Property))
#else
			else if (const auto* StructProperty = Cast<UStructProperty>(Property))
#endif
			{
				const TSharedPtr<FJsonObject>* ChildJsonObjectPtr;
				if (!PropertiesJsonObject->TryGetObjectField(StructProperty->GetName(), ChildJsonObjectPtr))
				{
					continue;
				}
				if (ChildJsonObjectPtr == nullptr || !ChildJsonObjectPtr->IsValid())
				{
					continue;
				}

				const TSharedRef<FJsonObject> ChildJsonObject = ChildJsonObjectPtr->ToSharedRef();
				if (!FJsonObjectConverter::JsonObjectToUStruct(
					ChildJsonObject,
					StructProperty->Struct,
					StructProperty->ContainerPtrToValuePtr<void>(&Object)
				))
				{
					return 0;
				}
			}
			else
			{
				FString ValueStr;
				if (!PropertiesJsonObject->TryGetStringField(Property->GetName(), ValueStr))
				{
					continue;
				}

#if UE_5_01_OR_LATER
				Property->ImportText_Direct(
					*ValueStr,
					Property->ContainerPtrToValuePtr<uint8>(&Object),
					&Object, PPF_None
				);
#else
				Property->ImportText(
					*ValueStr,
					Property->ContainerPtrToValuePtr<uint8>(&Object),
					PPF_None, &Object
				);
#endif
			}

			NumOfProperties++;
		}

		return NumOfProperties;
	}

	int32 FDetailsPanelPrinterUtils::UObjectToJsonObject(
		UObject& Object,
		const TSharedRef<FJsonObject>& JsonObject,
		const int64 CheckFlags /* = 0 */,
		const int64 SkipFlags /* = 0 */
	)
	{
		UClass* Class = Object.GetClass();
		if (!IsValid(Class))
		{
			return 0;
		}

		JsonObject->SetStringField(JsonFieldNames::ClassFieldName, Class->GetName());

		int32 NumOfProperties = 0;
		const TSharedRef<FJsonObject> PropertiesJsonObject = MakeShared<FJsonObject>();
#if UE_4_25_OR_LATER
		for (auto* Property : TFieldRange<FProperty>(Class, EFieldIteratorFlags::IncludeSuper, EFieldIteratorFlags::ExcludeDeprecated))
#else
		for (auto* Property : TFieldRange<UProperty>(Class, EFieldIteratorFlags::IncludeSuper, EFieldIteratorFlags::ExcludeDeprecated))
#endif
		{
			if (Property == nullptr)
			{
				continue;
			}

			if (CheckFlags != 0 && !Property->HasAnyPropertyFlags(CheckFlags))
			{
				continue;
			}
			if (Property->HasAnyPropertyFlags(SkipFlags))
			{
				continue;
			}

#if UE_4_25_OR_LATER
			if (const auto* ObjectProperty = CastField<FObjectProperty>(Property))
#else
			if (const auto* ObjectProperty = Cast<UObjectProperty>(Property))
#endif
			{
				UObject** ChildObjectPtr = ObjectProperty->ContainerPtrToValuePtr<UObject*>(&Object);
				if (ChildObjectPtr == nullptr)
				{
					continue;
				}

				UObject* ChildObject = *ChildObjectPtr;
				if (!IsValid(ChildObject))
				{
					continue;
				}

				const TSharedRef<FJsonObject> ChildJsonObject = MakeShared<FJsonObject>();
				if (UObjectToJsonObject(*ChildObject, ChildJsonObject, CheckFlags, SkipFlags) == 0)
				{
					continue;
				}

				PropertiesJsonObject->SetObjectField(ObjectProperty->GetName(), ChildJsonObject);
			}
#if UE_4_25_OR_LATER
			else if (const auto* StructProperty = CastField<FStructProperty>(Property))
#else
			else if (const auto* StructProperty = Cast<UStructProperty>(Property))
#endif
			{
				TSharedRef<FJsonObject> ChildJsonObject = MakeShared<FJsonObject>();
				if (!FJsonObjectConverter::UStructToJsonObject(
					StructProperty->Struct,
					StructProperty->ContainerPtrToValuePtr<void>(&Object),
					ChildJsonObject,
					CheckFlags,
					SkipFlags
				))
				{
					continue;
				}
				
				PropertiesJsonObject->SetObjectField(StructProperty->GetName(), ChildJsonObject);
			}
			else
			{
				FString DefaultValueStr;
#if UE_5_01_OR_LATER
				Property->ExportTextItem_Direct
#else
				Property->ExportTextItem
#endif
				(
					DefaultValueStr,
					Property->ContainerPtrToValuePtr<uint8>(Class->GetDefaultObject()),
					nullptr, &Object, PPF_None
				);
	
				FString ValueStr;
#if UE_5_01_OR_LATER
				Property->ExportTextItem_Direct
#else
				Property->ExportTextItem
#endif
				(
					ValueStr,
					Property->ContainerPtrToValuePtr<uint8>(&Object),
					&DefaultValueStr, &Object, PPF_None
				);

				PropertiesJsonObject->SetStringField(Property->GetName(), ValueStr);
			}

			NumOfProperties++;
		}

		JsonObject->SetObjectField(JsonFieldNames::PropertiesFieldName, PropertiesJsonObject);
		
		return NumOfProperties;
	}
}

#undef LOCTEXT_NAMESPACE
