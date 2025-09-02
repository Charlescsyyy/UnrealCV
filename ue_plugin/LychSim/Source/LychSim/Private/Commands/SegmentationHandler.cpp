// SegmentationHandler.cpp (LychSim)
//file added for part segmentation
#include "Commands/SegmentationHandler.h"
#include "Server/UnrealcvServer.h"
#include "WorldController.h"
#include "Controller/ObjectAnnotator.h"
#include "UnrealcvLog.h"

void FSegmentationHandler::RegisterCommands()
{
    CommandDispatcher->BindCommand(
        "vset /segmentation/mode [str]",
        FDispatcherDelegate::CreateRaw(this, &FSegmentationHandler::SetMode),
        "Set segmentation mode (part | object)"
    );

    CommandDispatcher->BindCommand(
        "vget /segmentation/mode",
        FDispatcherDelegate::CreateRaw(this, &FSegmentationHandler::GetMode),
        "Get current segmentation mode"
    );
}

FExecStatus FSegmentationHandler::SetMode(const TArray<FString>& Args)
{
    if (Args.Num() != 1)
    {
        return FExecStatus::Error(TEXT("Expected exactly one argument: part | object"));
    }

    const FString Mode = Args[0].ToLower();
    if (Mode != TEXT("part") && Mode != TEXT("object"))
    {
        return FExecStatus::Error(TEXT("Unsupported mode. Supported modes are: part | object"));
    }

    CurrentMode = Mode;
    ReannotateWorld(CurrentMode);
    return FExecStatus::OK();
}

FExecStatus FSegmentationHandler::GetMode(const TArray<FString>& Args)
{
    return FExecStatus::OK(CurrentMode);
}

void FSegmentationHandler::ReannotateWorld(const FString& Mode)
{
    AUnrealcvWorldController* WorldController = FUnrealcvServer::Get().WorldController.Get();
    if (!WorldController)
    {
        UE_LOG(LogUnrealCV, Warning, TEXT("WorldController is not valid when setting segmentation mode"));
        return;
    }

    UWorld* World = WorldController->GetWorld();
    if (!World)
    {
        UE_LOG(LogUnrealCV, Warning, TEXT("World is invalid when setting segmentation mode"));
        return;
    }

    // 先清空再重标注
    WorldController->ObjectAnnotator.ClearAnnotations(World);

    if (Mode == TEXT("object"))
    {
        // 先按 Actor 级附加组件，再按组统一着色
        WorldController->ObjectAnnotator.AnnotateWorld(World);
        WorldController->ObjectAnnotator.AnnotateGroupedActors(World);
    }
    else // part（默认）：Actor 级颜色
    {
        WorldController->ObjectAnnotator.AnnotateWorld(World);
    }
}
