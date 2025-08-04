// SegmentationHandler.cpp
#include "Commands/SegmentationHandler.h"
#include "UnrealcvServer.h"
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

    FString Mode = Args[0].ToLower();
    if (Mode != TEXT("part") && Mode != TEXT("object"))
    {
        return FExecStatus::Error(TEXT("Unsupported mode. Supported modes are: part | object"));
    }

    // Store new mode
    CurrentMode = Mode;

    // Re-generate annotation based on mode
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

    // always clear previous annotation first
    WorldController->ObjectAnnotator.ClearAnnotations(World);

    if (Mode == TEXT("object"))
    {
        // First attach annotation components just like initial state
        WorldController->ObjectAnnotator.AnnotateWorld(World);
        // Then recolor by actor groups
        WorldController->ObjectAnnotator.AnnotateGroupedActors(World);
    }
    else // part -> original implementation (Actor-level colors)
    {
        WorldController->ObjectAnnotator.AnnotateWorld(World);
    }
}
