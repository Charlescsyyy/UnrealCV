// Weichao Qiu @ 2017
#include "Controller/ObjectAnnotator.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Component/AnnotationComponent.h"
#include "UnrealcvLog.h"
//2025-7-23 part segmentation logic-Siyi Chen
#include "Runtime/Engine/Classes/Components/MeshComponent.h"
#include "Runtime/Engine/Classes/Components/SceneComponent.h"
#include "Runtime/Core/Public/Containers/Map.h"
#include "Runtime/Core/Public/Algo/ForEach.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"
#include "Math/UnrealMathUtility.h"
//2025-7-23 end
// For UE4 < 17
// check https://github.com/unrealcv/unrealcv/blob/1369a72be8428547318d8a52ae2d63e1eb57a001/Source/UnrealCV/Private/Controller/ObjectAnnotator.cpp#L1

FObjectAnnotator::FObjectAnnotator()
{
}

/** Annotate all static mesh in the world */
void FObjectAnnotator::AnnotateWorld(UWorld* World)
{
	//2025-7-23 part segmentation logic-Siyi Chen
	if (CurrentSegmentationMode == FObjectAnnotator::ESegmentationMode::Part)
	{
		AnnotateMeshComponents(World);
		return;
	}
	//2025-7-23 end

	if (!IsValid(World))
	{
		UE_LOG(LogUnrealCV, Warning, TEXT("Can not annotate world, the world is not valid"));
		return;
	}

	TArray<AActor*> ActorArray;
	GetAnnotableActors(World, ActorArray);

	for (AActor* Actor : ActorArray)
	{
		FColor AnnotationColor = GetDefaultColor(Actor);

		if (!IsValid(Actor))
		{
			UE_LOG(LogUnrealCV, Warning, TEXT("Found invalid actor in AnnotateWorld"));
			continue;
		}
		// Use VertexColor as annotation
		this->SetAnnotationColor(Actor, AnnotationColor);
	}
	UE_LOG(LogUnrealCV, Log, TEXT("Annotate mesh of the scene (%d)"), AnnotationColors.Num());
}


void FObjectAnnotator::SetAnnotationColor(AActor* Actor, const FColor& AnnotationColor)
{
	if (!IsValid(Actor))
	{
		return;
	}
	// CHECK: Add the annotation color regardless successful or not
	TArray<UActorComponent*> AnnotationComponents = Actor->K2_GetComponentsByClass(UAnnotationComponent::StaticClass());
	if (AnnotationComponents.Num() == 0)
	{
		CreateAnnotationComponent(Actor, AnnotationColor);
	}
	else
	{
		UpdateAnnotationComponent(Actor, AnnotationColor);
	}
	this->AnnotationColors.Emplace(Actor->GetName(), AnnotationColor);
	// TODO: Remote AnnotationColor Map!
}

void FObjectAnnotator::GetAnnotationColor(AActor* Actor, FColor& AnnotationColor)
{
	if (!IsValid(Actor))
	{
		UE_LOG(LogUnrealCV, Warning, TEXT("InActor is invalid in GetAnnotationColor"));
		return;
	}

	// FString ActorName = Actor->GetName();
	// if (!this->AnnotationColors.Contains(ActorName))
	// {
	// 	UE_LOG(LogUnrealCV, Warning, TEXT("Can not find actor %s in GetAnnotationColor"), *ActorName);
	// 	return;
	// }
	// AnnotationColor = this->AnnotationColors[ActorName];

	// Another way to get annotation color is directly read color from AnnotationComponent
	// TODO: Remove the first only leave the second method

	// Check its direct children, do not recursive, otherwise it is very easy to trigger the warning.
	TArray<UActorComponent*> AnnotationComponents = Actor->K2_GetComponentsByClass(UAnnotationComponent::StaticClass());
	TArray<UActorComponent*> MeshComponents = Actor->K2_GetComponentsByClass(UMeshComponent::StaticClass());
	// Note: Strange that the MeshComponents.Num() is twice the number of AnnotationComponents.Num()
	if (AnnotationComponents.Num() == 0) return;
	if (AnnotationComponents.Num() != MeshComponents.Num())
	{
		// UE_LOG(LogTemp, Warning, TEXT("More than one AnnotationComponent for MeshComponent."));
		UE_LOG(LogTemp, Warning, TEXT("In actor %s, the number of MeshComponent (%d) and AnnotationComponent (%d) is different."), *Actor->GetName(), MeshComponents.Num(), AnnotationComponents.Num());
		// for (UActorComponent* Component : MeshComponents)
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("%s"), *Component->GetName()); 
		// }
	}
	UAnnotationComponent* AnnotationComponent = Cast<UAnnotationComponent>(AnnotationComponents[0]);
	// check(AnnotationColor == AnnotationComponent->AnnotationColor);
	AnnotationColor = AnnotationComponent->GetAnnotationColor();
}

void FObjectAnnotator::GetAnnotableActors(UWorld* World, TArray<AActor*>& ActorArray)
{
	if (!IsValid(World))
	{
		UE_LOG(LogUnrealCV, Warning, TEXT("The world is invalid in GetAnnotableActors"));
		return;
	}

	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		AActor *Actor = *ActorItr;
		ActorArray.Add(Actor);
	}
}

/**
 * Debug tips:
 * AnnotationComponent->AnnotationColor = FColor::MakeRandomColor();
 */
void FObjectAnnotator::CreateAnnotationComponent(AActor* Actor, const FColor& AnnotationColor)
{
	// Two special type of actors
	// https://api.unrealengine.com/INT/API/Runtime/Landscape/ALandscape/index.html
	// https://api.unrealengine.com/INT/API/Runtime/Foliage/AInstancedFoliageActor/index.html
	if (!IsValid(Actor))
	{
		UE_LOG(LogUnrealCV, Warning, TEXT("Invalid actor in CreateAnnotationComponent"));
		return;
	}
	TArray<UActorComponent*> AnnotationComponents = Actor->K2_GetComponentsByClass(UAnnotationComponent::StaticClass());
	if (AnnotationComponents.Num() != 0)
	{
		UE_LOG(LogUnrealCV, Log, TEXT("Skip annotated actor %s"), *Actor->GetName());
		return;
	}

	TArray<UActorComponent*> MeshComponents = Actor->K2_GetComponentsByClass(UMeshComponent::StaticClass());
	if (MeshComponents.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Annotate actor %s (%s) with color %s"), *Actor->GetActorNameOrLabel(), *Actor->GetName(), *AnnotationColor.ToString());

		for (UActorComponent* Component : MeshComponents)
		{
			UMeshComponent* MeshComponent = Cast<UMeshComponent>(Component);

			UAnnotationComponent* AnnotationComponent = NewObject<UAnnotationComponent>(MeshComponent);
			// UE_LOG(LogTemp, Log, TEXT("Annotate %s with color %s"), *MeshComponent->GetName(), *AnnotationColor.ToString());
			AnnotationComponent->SetupAttachment(MeshComponent);
			AnnotationComponent->RegisterComponent();
			// Set annotation color after the component is registered
			AnnotationComponent->SetAnnotationColor(AnnotationColor);
			AnnotationComponent->MarkRenderStateDirty();
		}
	}
}


void FObjectAnnotator::UpdateAnnotationComponent(AActor* Actor, const FColor& AnnotationColor)
{
	if (!IsValid(Actor))
	{
		UE_LOG(LogUnrealCV, Warning, TEXT("Invalid actor in CreateAnnotationComponent"));
		return;
	}
	TArray<UActorComponent*> AnnotationComponents = Actor->K2_GetComponentsByClass(UAnnotationComponent::StaticClass());
	for (UActorComponent* Component : AnnotationComponents)
	{
		UAnnotationComponent* AnnotationComponent = Cast<UAnnotationComponent>(Component);
		AnnotationComponent->SetAnnotationColor(AnnotationColor);
		AnnotationComponent->MarkRenderStateDirty();
	}
}

FColor FObjectAnnotator::GetDefaultColor(AActor* Actor)
{
	FString ActorName = Actor->GetName();
	if (AnnotationColors.Contains(ActorName))
	{
		// Already initialized
		return AnnotationColors[ActorName];
	}

	int ColorIndex = AnnotationColors.Num();
	FColor AnnotationColor = ColorGenerator.GetColorFromColorMap(ColorIndex);

	return AnnotationColor;
}



/**
void FObjectAnnotator::AnnotateMeshComponents(UWorld* World)
{
	if (!IsValid(World))
	{
		UE_LOG(LogUnrealCV, Warning, TEXT("Can not annotate world, the world is not valid"));
		return;
	}

	// List all MeshComponents in the scene
	TArray<UMeshComponent*> ComponentList;
	TArray<UObject*> UObjectList;
	bool bIncludeDerivedClasses = true;
	EObjectFlags ExclusionFlags = EObjectFlags::RF_ClassDefaultObject;
	EInternalObjectFlags ExclusionInternalFlags = EInternalObjectFlags::AllFlags;
	GetObjectsOfClass(UMeshComponent::StaticClass(), UObjectList, bIncludeDerivedClasses, ExclusionFlags, ExclusionInternalFlags);
	for (UObject* Object : UObjectList)
	{
		UMeshComponent* Component = Cast<UMeshComponent>(Object);

		if (Component->GetWorld() == World
		&& !ComponentList.Contains(Component))
		{
			ComponentList.Add(Component);
		}
	}

	for (int i = 0; i < ComponentList.Num(); i++)	
	// for (UMeshComponent* MeshComponent : ComponentList)
	{
		UMeshComponent* MeshComponent = ComponentList[i];
		if (!IsValid(MeshComponent))
		{
			UE_LOG(LogTemp, Warning, TEXT("MeshComponent is invalid."));
			continue;
		}

		UAnnotationComponent* AnnotationComponent = nullptr;
		TArray<USceneComponent*> AttachChildren = MeshComponent->GetAttachChildren();
		for (USceneComponent* Child : AttachChildren)
		{
			AnnotationComponent = Cast<UAnnotationComponent>(Child);
			if (IsValid(AnnotationComponent))
			{
				break;
			}
		}
		if (!IsValid(AnnotationComponent))
		{
			// Create a new one
			AnnotationComponent = NewObject<UAnnotationComponent>(MeshComponent);
			AnnotationComponent->SetupAttachment(MeshComponent);
			AnnotationComponent->RegisterComponent();
			AnnotationComponent->MarkRenderStateDirty();
		}
		// UE_LOG(LogTemp, Log, TEXT("Annotate %s with color %s"), *MeshComponent->GetName(), *AnnotationColor.ToString());
		// FColor AnnotationColor = FColor::MakeRandomColor();
		FColor AnnotationColor = ColorGenerator.GetColorFromColorMap(i);
		AnnotationComponent->SetAnnotationColor(AnnotationColor);
		// AnnotationComponent->AnnotationColor = FColor::MakeRandomColor(); // Debug
	}
}
*/
//2025-7-23 part segmentation logic-Siyi Chen
void FObjectAnnotator::AnnotateMeshComponents(UWorld* World)
{
	if (!IsValid(World)) return;

	TArray<UMeshComponent*> MeshComponents;
	TArray<UObject*> AllObjects;
	GetObjectsOfClass(UMeshComponent::StaticClass(), AllObjects, /*bIncludeDerivedClasses*/true);

	for (UObject* Obj : AllObjects)
	{
		UMeshComponent* Mesh = Cast<UMeshComponent>(Obj);
		if (Mesh && Mesh->GetWorld() == World)
		{
			MeshComponents.AddUnique(Mesh);
		}
	}

	for (int32 Idx = 0; Idx < MeshComponents.Num(); ++Idx)
	{
		UMeshComponent* Mesh = MeshComponents[Idx];
		if (!IsValid(Mesh)) continue;

		// 1. 保证有 AnnotationComponent
		UAnnotationComponent* AnnComp = nullptr;
		for (USceneComponent* Child : Mesh->GetAttachChildren())
		{
			AnnComp = Cast<UAnnotationComponent>(Child);
			if (AnnComp) break;
		}
		if (!AnnComp)
		{
			AnnComp = NewObject<UAnnotationComponent>(Mesh);
			AnnComp->SetupAttachment(Mesh);
			AnnComp->RegisterComponent();
		}

		// 2. 赋随机不重复颜色
		FColor Color = ColorGenerator.GetColorFromColorMap(Idx);
		AnnComp->SetAnnotationColor(Color);
		AnnComp->MarkRenderStateDirty();
	}
	UE_LOG(LogUnrealCV, Log, TEXT("AnnotateMeshComponents finished, %d components"), MeshComponents.Num());
}

/** 获取顶层父Actor（若无 AttachParent 则返回自身） */
static AActor* GetRootActor(AActor* Actor)
{
	AActor* Root = Actor;
	while (Root && Root->GetAttachParentActor())
	{
		Root = Root->GetAttachParentActor();
	}
	return Root;
}

void FObjectAnnotator::AnnotateByParent(UWorld* World)
{
	if (!IsValid(World)) return;

	TArray<AActor*> Actors;
	GetAnnotableActors(World, Actors);

	// 根Actor → 颜色
	TMap<AActor*, FColor> RootColorMap;
	int32 ColorIdx = 0;

	for (AActor* Actor : Actors)
	{
		if (!IsValid(Actor)) continue;
		AActor* Root = GetRootActor(Actor);
		if (!RootColorMap.Contains(Root))
		{
			RootColorMap.Add(Root, ColorGenerator.GetColorFromColorMap(ColorIdx++));
		}
	}

	for (AActor* Actor : Actors)
	{
		AActor* Root = GetRootActor(Actor);
		const FColor* ColorPtr = RootColorMap.Find(Root);
		if (ColorPtr) SetAnnotationColor(Actor, *ColorPtr);
	}

	UE_LOG(LogUnrealCV, Log, TEXT("AnnotateByParent finished, %d root groups"), RootColorMap.Num());
}

void FObjectAnnotator::SetSegmentationMode(FObjectAnnotator::ESegmentationMode Mode, UWorld* World)

{
	if (Mode == CurrentSegmentationMode || !IsValid(World)) return;

	CurrentSegmentationMode = Mode;

	// 先清空旧 AnnotationColor 记录，避免颜色重用冲突
	AnnotationColors.Empty();

	if (Mode == FObjectAnnotator::ESegmentationMode::Part)
	{
		AnnotateMeshComponents(World);
	}
	else // ESegmentationMode::Object
	{
		AnnotateByParent(World);
	}
	//2025-7-23 part segmentation logic-Siyi Chen
	for (TObjectIterator<UAnnotationComponent> It; It; ++It)
	{
    UAnnotationComponent* Ann = *It;
    Ann->MarkRenderStateDirty();
	}
	//2025-7-23 end
}

FObjectAnnotator::ESegmentationMode FObjectAnnotator::GetCurrentSegmentationMode() const
{
	return CurrentSegmentationMode;
}
//2025-7-23 end

/** Utility function to generate color map */
int32 FColorGenerator::GetChannelValue(uint32 Index)
{
	static int32 Values[256] = { 0 };
	static bool Init = false;
	if (!Init)
	{
		float Step = 256;
		uint32 Iter = 0;
		Values[0] = 0;
		while (Step >= 1)
		{
			for (uint32 Value = Step - 1; Value <= 256; Value += Step * 2)
			{
				Iter++;
				Values[Iter] = Value;
			}
			Step /= 2;
		}
		Init = true;
	}
	if (Index >= 0 && Index <= 255)
	{
		return Values[Index];
	}
	else
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Invalid channel index"));
		check(false);
		return -1;
	}
}

void FColorGenerator::GetColors(int32 MaxVal, bool Fix1, bool Fix2, bool Fix3, TArray<FColor>& ColorMap)
{
	for (int32 I = 0; I <= (Fix1 ? 0 : MaxVal - 1); I++)
	{
		for (int32 J = 0; J <= (Fix2 ? 0 : MaxVal - 1); J++)
		{
			for (int32 K = 0; K <= (Fix3 ? 0 : MaxVal - 1); K++)
			{
				uint8 R = (uint8)GetChannelValue(Fix1 ? MaxVal : I);
				uint8 G = (uint8)GetChannelValue(Fix2 ? MaxVal : J);
				uint8 B = (uint8)GetChannelValue(Fix3 ? MaxVal : K);
				FColor Color(R, G, B, 255);
				ColorMap.Add(Color);
			}
		}
	}
}

FColor FColorGenerator::GetColorFromColorMap(int32 ObjectIndex)
{
	static TArray<FColor> ColorMap;
	int NumPerChannel = 32;
	if (ColorMap.Num() == 0)
	{
		// 32 ^ 3
		for (int32 MaxChannelIndex = 0; MaxChannelIndex < NumPerChannel; MaxChannelIndex++) // Get color map for 1000 objects
		{
			// GetColors(MaxChannelIndex, false, false, false, ColorMap);
			GetColors(MaxChannelIndex, false, false, true, ColorMap);
			GetColors(MaxChannelIndex, false, true, false, ColorMap);
			GetColors(MaxChannelIndex, false, true, true, ColorMap);
			GetColors(MaxChannelIndex, true, false, false, ColorMap);
			GetColors(MaxChannelIndex, true, false, true, ColorMap);
			GetColors(MaxChannelIndex, true, true, false, ColorMap);
			GetColors(MaxChannelIndex, true, true, true, ColorMap);
		}
	}
	if (ObjectIndex < 0 || ObjectIndex >= FMath::pow(NumPerChannel, 3))
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Object index %d is out of the color map boundary [%d, %d]"), ObjectIndex, 0, (int) pow(NumPerChannel, 3));
	}
	return ColorMap[ObjectIndex];
}


