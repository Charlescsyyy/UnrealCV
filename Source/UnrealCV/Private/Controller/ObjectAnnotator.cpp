// ObjectAnnotator.cpp  (restored full implementation)
#include "Controller/ObjectAnnotator.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Component/AnnotationComponent.h"
#include "UnrealcvLog.h"
#include "Components/MeshComponent.h"
#include "Components/SceneComponent.h"
#include "UObject/UObjectIterator.h"
#include "Math/UnrealMathUtility.h"

/*********************  FObjectAnnotator  ************************/ 
FObjectAnnotator::FObjectAnnotator() {}

/******** 1. 原始 Actor 级着色 ********/ 
void FObjectAnnotator::AnnotateWorld(UWorld* World)
{
    if(!IsValid(World)) { UE_LOG(LogUnrealCV,Warning,TEXT("World invalid in AnnotateWorld")); return; }
    TArray<AActor*> Actors;  GetAnnotableActors(World,Actors);
    for(AActor* Actor:Actors)
    {
        if(!IsValid(Actor)) continue;
        FColor C = GetDefaultColor(Actor);
        SetAnnotationColor(Actor,C);
    }
    UE_LOG(LogUnrealCV,Log,TEXT("AnnotateWorld: %d actors"),Actors.Num());
}

/******** 2. Actor 组着色 (object 模式) ********/ 
void FObjectAnnotator::AnnotateGroupedActors(UWorld* World)
{
    if(!IsValid(World)) return;
    TMap<AActor*,FColor> RootColor;
    for(TActorIterator<AActor> It(World);It;++It)
    {
        AActor* Actor=*It; if(!IsValid(Actor)) continue;
        // find top attachment root
        AActor* Root=Actor; while(Root->GetAttachParentActor()) Root=Root->GetAttachParentActor();
        FColor* Existing=RootColor.Find(Root);
        FColor Col;
        if(Existing) Col=*Existing; else { Col=GetDefaultColor(Root); RootColor.Add(Root,Col);}        
        SetAnnotationColor(Actor,Col);
        AnnotationColors.Emplace(Actor->GetName(), Col);
    }
    UE_LOG(LogUnrealCV,Log,TEXT("AnnotateGroupedActors: processed %d root groups"),RootColor.Num());
}

/******** 3. 清空现有 AnnotationComponent ********/ 
void FObjectAnnotator::ClearAnnotations(UWorld* World)
{
    if(!IsValid(World)) return;
    AnnotationColors.Empty();
    int32 Count=0;
    for(TObjectIterator<UAnnotationComponent> It;It;++It)
    {
        UAnnotationComponent* Comp=*It; if(!IsValid(Comp)) continue;
        if(Comp->GetWorld()!=World) continue;
        Comp->DestroyComponent(); Count++;
    }
    UE_LOG(LogUnrealCV,Log,TEXT("ClearAnnotations: removed %d comps"),Count);
}

/****************  单 Actor 接口  ****************/ 
void FObjectAnnotator::SetAnnotationColor(AActor* Actor,const FColor& Color)
{
    if(!IsValid(Actor)) return;
    TArray<UActorComponent*> Anno=Actor->K2_GetComponentsByClass(UAnnotationComponent::StaticClass());
    if(Anno.Num()==0) CreateAnnotationComponent(Actor,Color); else UpdateAnnotationComponent(Actor,Color);
    AnnotationColors.Emplace(Actor->GetName(),Color);
}

void FObjectAnnotator::GetAnnotationColor(AActor* Actor,FColor& Out)
{
    if(!IsValid(Actor)) return;
    TArray<UActorComponent*> Anno=Actor->K2_GetComponentsByClass(UAnnotationComponent::StaticClass());
    if(Anno.Num()==0) return;
    UAnnotationComponent* AC=Cast<UAnnotationComponent>(Anno[0]);
    Out=AC->GetAnnotationColor();
}

/****************  内部工具  ****************/ 
void FObjectAnnotator::GetAnnotableActors(UWorld* World,TArray<AActor*>& Out)
{
    for(TActorIterator<AActor> It(World);It;++It) Out.Add(*It);
}

void FObjectAnnotator::CreateAnnotationComponent(AActor* Actor,const FColor& Color)
{
    TArray<UActorComponent*> MeshComps=Actor->K2_GetComponentsByClass(UMeshComponent::StaticClass());
    for(UActorComponent* C:MeshComps)
    {
        UMeshComponent* MC=Cast<UMeshComponent>(C); if(!MC) continue;
        UAnnotationComponent* AC=NewObject<UAnnotationComponent>(MC);
        AC->SetupAttachment(MC);
        AC->RegisterComponent();
        AC->SetAnnotationColor(Color);
        AC->MarkRenderStateDirty();
    }
}

void FObjectAnnotator::UpdateAnnotationComponent(AActor* Actor,const FColor& Color)
{
    TArray<UActorComponent*> Anno=Actor->K2_GetComponentsByClass(UAnnotationComponent::StaticClass());
    for(UActorComponent* C:Anno)
    {
        UAnnotationComponent* AC=Cast<UAnnotationComponent>(C);
        AC->SetAnnotationColor(Color);
        AC->MarkRenderStateDirty();
    }
}

FColor FObjectAnnotator::GetDefaultColor(AActor* Actor)
{
    FString Name=Actor->GetName();
    if(AnnotationColors.Contains(Name)) return AnnotationColors[Name];
    FColor Col=ColorGenerator.GetColorFromColorMap(AnnotationColors.Num());
    return Col;
}

/****************  FColorGenerator  ****************/
static int32 ChannelValues[256]={0}; static bool bInit=false;
int32 FColorGenerator::GetChannelValue(uint32 Index)
{
    if(!bInit){ float Step=256; uint32 Iter=0; ChannelValues[0]=0; while(Step>=1){ for(uint32 V=Step-1;V<=256;V+=Step*2){ChannelValues[++Iter]=V;} Step/=2;} bInit=true; }
    return ChannelValues[Index];
}

void FColorGenerator::GetColors(int32 MaxVal,bool F1,bool F2,bool F3,TArray<FColor>& Map)
{
    for(int32 I=0;I<=(F1?0:MaxVal-1);I++)
        for(int32 J=0;J<=(F2?0:MaxVal-1);J++)
            for(int32 K=0;K<=(F3?0:MaxVal-1);K++)
                Map.Add(FColor(GetChannelValue(F1?MaxVal:I),GetChannelValue(F2?MaxVal:J),GetChannelValue(F3?MaxVal:K),255));
}

FColor FColorGenerator::GetColorFromColorMap(int32 Index)
{
    static TArray<FColor> Map; const int32 N=32; if(Map.Num()==0){ for(int32 M=0;M<N;M++){ GetColors(M,false,false,true,Map); GetColors(M,false,true,false,Map); GetColors(M,false,true,true,Map); GetColors(M,true,false,false,Map); GetColors(M,true,false,true,Map); GetColors(M,true,true,false,Map); GetColors(M,true,true,true,Map);} }
    Index= Index % Map.Num(); return Map[Index];
}
