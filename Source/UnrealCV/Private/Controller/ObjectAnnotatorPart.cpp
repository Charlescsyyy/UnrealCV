// ObjectAnnotatorPart.cpp - Component-level (part mode) annotation implementation
#include "Controller/ObjectAnnotator.h"
#include "Component/AnnotationComponent.h"
#include "UnrealcvLog.h"
#include "UObject/UObjectIterator.h"

/*
 * Strategy:
 *   1. Reuse original AnnotateWorld to attach AnnotationComponent only to those
 *      MeshComponents that UnrealCV originally considered "annotable" (this
 *      purposefully excludes SkySphere / background geometry).
 *   2. Iterate over all existing AnnotationComponents in the current GameWorld
 *      and re-assign a unique color to each MeshComponent, giving per-component
 *      masks while keeping background pure white.
 */

void FObjectAnnotator::AnnotateMeshComponents(UWorld* World)
{
    if (!IsValid(World))
    {
        UE_LOG(LogUnrealCV, Warning, TEXT("AnnotateMeshComponents: World is invalid"));
        return;
    }

    /******** Step-1: attach AnnotationComponents as original UnrealCV does ********/
    AnnotateWorld(World);   // Actor-level attachment (excludes sky background)

    /******** Step-2: recolor every existing AnnotationComponent ********/
    int32 ColorIdx = 0;
    int32 Recolored = 0;

    for (TObjectIterator<UAnnotationComponent> It; It; ++It)
    {
        UAnnotationComponent* Anno = *It;
        if (!IsValid(Anno))                continue;
        if (Anno->GetWorld() != World)    continue;   // ensure same world

        // 保留白色背景：若当前颜色已是白色则不修改
        FColor CurrColor = Anno->GetAnnotationColor();
        if (CurrColor == FColor::White) continue;

        FColor NewColor = ColorGenerator.GetColorFromColorMap(++ColorIdx); // skip 0
        Anno->SetAnnotationColor(NewColor);
        Anno->MarkRenderStateDirty();
        // update color map per Actor so /object/<actor>/color works
        if (AActor* Owner = Anno->GetOwner())
        {
            AnnotationColors.Emplace(Owner->GetName(), NewColor);
        }
        ++Recolored;
    }

    UE_LOG(LogUnrealCV, Log, TEXT("AnnotateMeshComponents: recolored %d components"), Recolored);
}
