// Copyright 2020 Naotsun. All Rights Reserved.

#include "GraphEditorWrapper.h"
#include "GraphPrinterGlobals.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "HAL/PlatformApplicationMisc.h"
#include "EdGraphUtilities.h"

// Graph types.
#include "EdGraph/EdGraph.h"
#include "MaterialGraph/MaterialGraph.h"
#include "AIGraph.h"
#include "AnimationGraph.h"
#include "Graph/ControlRigGraph.h"
#include "SoundClassGraph/SoundClassGraph.h"
#include "SoundCueGraph/SoundCueGraph.h"
#include "SoundSubmixGraph/SoundSubmixGraph.h"
#include "ReferenceViewer/EdGraph_ReferenceViewer.h"

// Editor types.
#include "BlueprintEditor.h"
#include "MaterialEditor.h"
#include "BehaviorTreeEditor.h"
#include "EnvironmentQueryEditor.h"
#include "IControlRigEditor.h"
#include "SoundCueEditor.h"

// Asset types.
#include "Engine/Blueprint.h"
#include "Materials/Material.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "Animation/AnimBlueprint.h"
#include "ControlRigBlueprint.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundSubmix.h"

namespace GraphEditorWrapperInternal
{
	/**
	 * Define EdGraph and Editor types for each graph editor type.
	 */
	class FInvalidType {};

	template<EGraphEditorType GraphEditorType>
	struct TGraphEditorMap 
	{ 
		using GraphType = void; 
		using EditorType = void; 
		using AssetType = void;
	};

	template<>
	struct TGraphEditorMap<EGraphEditorType::Blueprint> 
	{ 
		using GraphType = UEdGraph; 
		using EditorType = FBlueprintEditor; 
		using AssetType = UBlueprint;
	};

	template<>
	struct TGraphEditorMap<EGraphEditorType::Material> 
	{ 
		using GraphType = UMaterialGraph; 
		using EditorType = FMaterialEditor; 
		using AssetType = UMaterial;
	};

	template<>
	struct TGraphEditorMap<EGraphEditorType::BehaviorTree> 
	{ 
		using GraphType = UAIGraph; 
		using EditorType = FBehaviorTreeEditor; 
		using AssetType = UBehaviorTree;
	};

	template<>
	struct TGraphEditorMap<EGraphEditorType::EnvironmentQuery>
	{ 
		using GraphType = UAIGraph; 
		using EditorType = FEnvironmentQueryEditor; 
		using AssetType = UEnvQuery;
	};

	template<>
	struct TGraphEditorMap<EGraphEditorType::Animation> 
	{ 
		using GraphType = UAnimationGraph; 
		using EditorType = FInvalidType;
		using AssetType = UAnimBlueprint;
	};

	template<>
	struct TGraphEditorMap<EGraphEditorType::ControlRig> 
	{ 
		using GraphType = UControlRigGraph; 
		using EditorType = IControlRigEditor; 
		using AssetType = UControlRigBlueprint;
	};

	template<>
	struct TGraphEditorMap<EGraphEditorType::SoundClass> 
	{ 
		using GraphType = USoundClassGraph; 
		using EditorType = FInvalidType;
		using AssetType = USoundClass;
	};

	template<>
	struct TGraphEditorMap<EGraphEditorType::SoundCue> 
	{ 
		using GraphType = USoundCueGraph; 
		using EditorType = FSoundCueEditor;
		using AssetType = USoundCue;
	};

	template<>
	struct TGraphEditorMap<EGraphEditorType::SoundSubmix> 
	{ 
		using GraphType = USoundSubmixGraph; 
		using EditorType = FInvalidType;
		using AssetType = USoundSubmix;
	};

	template<>
	struct TGraphEditorMap<EGraphEditorType::ReferenceViewer> 
	{ 
		using GraphType = UEdGraph_ReferenceViewer; 
		using EditorType = FInvalidType;
		using AssetType = UObject;
	};

	template<EGraphEditorType GraphEditorType>
	using TGraphType = typename TGraphEditorMap<GraphEditorType>::GraphType;

	template<EGraphEditorType GraphEditorType>
	using TEditorType = typename TGraphEditorMap<GraphEditorType>::EditorType;

	template<EGraphEditorType GraphEditorType>
	using TAssetType = typename TGraphEditorMap<GraphEditorType>::AssetType;
}

/**
 * Implementation class that defines detailed processing contents for each asset.
 */
template<EGraphEditorType InGraphEditorType>
class TGraphEditorWrapperImpl : public FGraphEditorWrapper
{
public:
	static constexpr EGraphEditorType Type = InGraphEditorType;
	using GraphType = GraphEditorWrapperInternal::TGraphType<Type>;
	using EditorType = GraphEditorWrapperInternal::TEditorType<Type>;
	using AssetType = GraphEditorWrapperInternal::TAssetType<Type>;

	// Constructor.
	TGraphEditorWrapperImpl(TSharedPtr<SGraphEditor> InGraphEditor) : FGraphEditorWrapper(Type, InGraphEditor)
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = nullptr;
		if (IsValid(GEditor))
		{
			AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		}
		if (!IsValid(AssetEditorSubsystem))
		{
			UE_LOG(LogGraphPrinter, Fatal, TEXT("Failed to get AssetEditorSubsystem."));
		}
		
		UObject* AssetBeingEdited = nullptr;
		if (InGraphEditor.IsValid())
		{
			if (GraphType* Graph = Cast<GraphType>(InGraphEditor->GetCurrentGraph()))
			{
				AssetBeingEdited = Graph->GetOuter();
			}
		}
		if (!IsValid(AssetBeingEdited))
		{
			UE_LOG(LogGraphPrinter, Fatal, TEXT("Failed to get asset being edited."));
		}

		AssetEditor = MakeShareable(static_cast<EditorType*>(AssetEditorSubsystem->FindEditorForAsset(AssetBeingEdited, false)));
	}

	// Destructor.
	virtual ~TGraphEditorWrapperImpl()
	{

	}

	// Check if the editor types match.
	static bool DoesEditorTypeMatch(TSharedPtr<SGraphEditor> GraphEditor)
	{
		if (GraphEditor.IsValid())
		{
			if (auto* Graph = Cast<GraphType>(GraphEditor->GetCurrentGraph()))
			{
				if (auto* Outer = Cast<AssetType>(Graph->GetOuter()))
				{
					return true;
				}
			}
		}

		return false;
	}

	// FGraphEditorWrapper interface.
	virtual bool ExportNodesToText(FString& ExportedText)
	{
		// Since the node information is output to the clipboard, the current information is temporarily set aside.
		FString CurrentClipboard;
		FPlatformApplicationMisc::ClipboardPaste(CurrentClipboard);

		// Get node information via clipboard.
		AssetEditor->CopySelectedNodes();
		FPlatformApplicationMisc::ClipboardPaste(ExportedText);

		// Restore the contents of the clipboard.
		FPlatformApplicationMisc::ClipboardCopy(*CurrentClipboard);

		if (!FEdGraphUtilities::CanImportNodesFromText(GraphEditorWidget->GetCurrentGraph(), ExportedText))
		{
			return false;
		}

		return true;
	}

	virtual bool ImportNodesFromText(const FString& TextToImport)
	{
		if (!FEdGraphUtilities::CanImportNodesFromText(GraphEditorWidget->GetCurrentGraph(), TextToImport))
		{
			return false;
		}

		// Since the node information is output to the clipboard, the current information is temporarily set aside.
		FString CurrentClipboard;
		FPlatformApplicationMisc::ClipboardPaste(CurrentClipboard);

		// Import node information via clipboard.
		FPlatformApplicationMisc::ClipboardCopy(*TextToImport);
		AssetEditor->PasteNodes();

		// Restore the contents of the clipboard.
		FPlatformApplicationMisc::ClipboardCopy(*CurrentClipboard);

		return true;
	}
	// End of FGraphEditorWrapper interface.

protected:
	// Unique graph editor for each asset.
	TSharedPtr<EditorType> AssetEditor;
};

//////////////////////////////////////////////////////
// FGraphEditorWrapper.

TSharedPtr<FGraphEditorWrapper> FGraphEditorWrapper::GenerateWrapper(TSharedPtr<SGraphEditor> GraphEditor)
{
	if (TGraphEditorWrapperImpl<EGraphEditorType::Material>::DoesEditorTypeMatch(GraphEditor))
	{
		return MakeShareable(new TGraphEditorWrapperImpl<EGraphEditorType::Material>(GraphEditor));
	}
	else if (TGraphEditorWrapperImpl<EGraphEditorType::BehaviorTree>::DoesEditorTypeMatch(GraphEditor))
	{
		return MakeShareable(new TGraphEditorWrapperImpl<EGraphEditorType::BehaviorTree>(GraphEditor));
	}
	else if (TGraphEditorWrapperImpl<EGraphEditorType::EnvironmentQuery>::DoesEditorTypeMatch(GraphEditor))
	{
		return MakeShareable(new TGraphEditorWrapperImpl<EGraphEditorType::EnvironmentQuery>(GraphEditor));
	}
	else if (TGraphEditorWrapperImpl<EGraphEditorType::Animation>::DoesEditorTypeMatch(GraphEditor))
	{
		return MakeShareable(new FGraphEditorWrapper(EGraphEditorType::Animation, GraphEditor));
	}
	else if (TGraphEditorWrapperImpl<EGraphEditorType::ControlRig>::DoesEditorTypeMatch(GraphEditor))
	{
		return MakeShareable(new TGraphEditorWrapperImpl<EGraphEditorType::ControlRig>(GraphEditor));
	}
	else if (TGraphEditorWrapperImpl<EGraphEditorType::SoundClass>::DoesEditorTypeMatch(GraphEditor))
	{
		return MakeShareable(new FGraphEditorWrapper(EGraphEditorType::SoundClass, GraphEditor));
	}
	else if (TGraphEditorWrapperImpl<EGraphEditorType::SoundCue>::DoesEditorTypeMatch(GraphEditor))
	{
		return MakeShareable(new TGraphEditorWrapperImpl<EGraphEditorType::SoundCue>(GraphEditor));
	}
	else if (TGraphEditorWrapperImpl<EGraphEditorType::SoundSubmix>::DoesEditorTypeMatch(GraphEditor))
	{
		return MakeShareable(new FGraphEditorWrapper(EGraphEditorType::SoundSubmix, GraphEditor));
	}
	else if (TGraphEditorWrapperImpl<EGraphEditorType::ReferenceViewer>::DoesEditorTypeMatch(GraphEditor))
	{
		return MakeShareable(new FGraphEditorWrapper(EGraphEditorType::ReferenceViewer, GraphEditor));
	}
	// In the case of Blueprint, both UEdGraph and UBlueprint are parent classes of many assets, so it is judged last.
	else if (TGraphEditorWrapperImpl<EGraphEditorType::Blueprint>::DoesEditorTypeMatch(GraphEditor))
	{
		return MakeShareable(new TGraphEditorWrapperImpl<EGraphEditorType::Blueprint>(GraphEditor));
	}
	else
	{
		return MakeShareable(new FGraphEditorWrapper(EGraphEditorType::Other, GraphEditor));
	}
}

FGraphEditorWrapper::FGraphEditorWrapper(EGraphEditorType InGraphEditorType, TSharedPtr<SGraphEditor> InGraphEditor)
	: GraphEditorType(InGraphEditorType), GraphEditorWidget(InGraphEditor)
{
}

FGraphEditorWrapper::~FGraphEditorWrapper()
{
}

FString FGraphEditorWrapper::GetGraphTitle() const
{
	if (GraphEditorWidget.IsValid())
	{
		// For ReferenceViewer, replace the name because the outer is a transiet object.
		if (GraphEditorType == EGraphEditorType::ReferenceViewer)
		{
			return TEXT("ReferenceViewer");
		}
		else if (UEdGraph* Graph = GraphEditorWidget->GetCurrentGraph())
		{
			if (UObject* Outer = Graph->GetOuter())
			{
				return FString::Printf(TEXT("%s-%s"), *Outer->GetName(), *Graph->GetName());
			}
		}
	}

	return TEXT("Invalid GraphEditor");
}

int32 FGraphEditorWrapper::GetNumberOfSelectedNodes() const
{
	if (GraphEditorWidget.IsValid())
	{
#if BEFORE_UE_4_23
		// Special support is implemented because SGraphEditor::GetNumberOfSelectedNodes before UE4.23 always returns 0.
		const TSet<UObject*>& SelectedNodes = GraphEditorWidget->GetSelectedNodes();
		return SelectedNodes.Num();
#else
		return GraphEditorWidget->GetNumberOfSelectedNodes();
#endif
	}

	return 0;
}

bool FGraphEditorWrapper::ExportNodesToText(FString& ExportedText)
{
	const FGraphPanelSelectionSet& SelectedNodes = GraphEditorWidget->GetSelectedNodes();
	if (SelectedNodes.Num() == 0)
	{
		return false;
	}

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			Node->PrepareForCopying();
		}
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);

	if (!FEdGraphUtilities::CanImportNodesFromText(GraphEditorWidget->GetCurrentGraph(), ExportedText))
	{
		return false;
	}

	return true;
}

bool FGraphEditorWrapper::ImportNodesFromText(const FString& TextToImport)
{
	if (!FEdGraphUtilities::CanImportNodesFromText(GraphEditorWidget->GetCurrentGraph(), TextToImport))
	{
		return false;
	}

	// Restore the node from the information read from the png file.
	TSet<UEdGraphNode*> ImportedNodeSet;
	FEdGraphUtilities::ImportNodesFromText(GraphEditorWidget->GetCurrentGraph(), TextToImport, ImportedNodeSet);

	// Move the nodes closer to the camera.
	FVector2D ViewLocation;
	float ZoomAmount;
	GraphEditorWidget->GetViewLocation(ViewLocation, ZoomAmount);

	const FIntRect& Boundaries = FEdGraphUtilities::CalculateApproximateNodeBoundaries(ImportedNodeSet.Array());
	const FVector2D& NodeOffset = ViewLocation - FVector2D(Boundaries.Min.X, Boundaries.Min.Y) + FVector2D(100.f);
	for (const auto& ImportedNode : ImportedNodeSet)
	{
		if (IsValid(ImportedNode))
		{
			ImportedNode->NodePosX += NodeOffset.X;
			ImportedNode->NodePosY += NodeOffset.Y;
		}
	}
}
