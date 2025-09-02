// Weichao Qiu @ 2017
#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"

/**************  FColorGenerator  *****************/
// Generate a pseudo-random unique color for each index (deterministic)
class FColorGenerator
{
public:
    FColor GetColorFromColorMap(int32 ObjectIndex);

private:
    int32 GetChannelValue(uint32 Index);
    void  GetColors(int32 MaxVal, bool Fix1, bool Fix2, bool Fix3, TArray<FColor>& ColorMap);
};

/**************  FObjectAnnotator  *****************/
/**
 * 负责为场景中的对象分配 AnnotationComponent 并写入颜色。
 * 提供三种着色策略：
 *   1. AnnotateWorld           —— 原始 Actor 级（游戏启动默认）
 *   2. AnnotateMeshComponents  —— StaticMeshComponent 级（part 模式）
 *   3. AnnotateGroupedActors   —— AttachmentRootActor 级（object 模式）
 * 切换模式前可调用 ClearAnnotations 清空现有标注。
 */
class UNREALCV_API FObjectAnnotator
{
public:
    FObjectAnnotator();

    /* ---------- 批量着色入口 ---------- */
    void AnnotateWorld(UWorld* World);              // 每个 Actor 一个颜色（原始实现）
    void AnnotateMeshComponents(UWorld* World);     // 每个 MeshComponent 一个颜色（part）
    void AnnotateGroupedActors(UWorld* World);      // Actor 组共用颜色（object）

    /* ---------- 清理 ---------- */
    void ClearAnnotations(UWorld* World);           // 删除所有现有 AnnotationComponent

    /* ---------- 单个 Actor 操作 ---------- */
    void SetAnnotationColor(AActor* Actor, const FColor& AnnotationColor);
    void GetAnnotationColor(AActor* Actor, FColor& AnnotationColor);

    TMap<FString, FColor> GetAnnotationColors() { return AnnotationColors; }

private:
    /**** 工具函数 ****/
    void GetAnnotableActors(UWorld* World, TArray<AActor*>& ActorArray);

    void CreateAnnotationComponent(AActor* Actor, const FColor& AnnotationColor);
    void UpdateAnnotationComponent(AActor* Actor, const FColor& AnnotationColor);

    FColor GetDefaultColor(AActor* Actor);

private:
    FColorGenerator              ColorGenerator;
    TMap<FString, FColor>        AnnotationColors;   // ActorName → Color
};
