// SegmentationHandler.cpp
#include "SegmentationHandler.h"
#include "UnrealcvServer.h"
#include "WorldController.h"
#include "Controller/ObjectAnnotator.h"

void FSegmentationHandler::RegisterCommands()
{
    CommandDispatcher->BindCommand(
        "vset /segmentation/mode [str]",
        FDispatcherDelegate::CreateRaw(this, &FSegmentationHandler::SetSegMode),
        "Set segmentation mode: part or object");

    CommandDispatcher->BindCommand(
        "vget /segmentation/mode",
        FDispatcherDelegate::CreateRaw(this, &FSegmentationHandler::GetSegMode),
        "Get current segmentation mode");
}

FExecStatus FSegmentationHandler::SetSegMode(const TArray<FString>& Args)
{
    if (Args.Num() != 1) return FExecStatus::InvalidArgument;
    UWorld* World = FUnrealcvServer::Get().GetWorld();
    auto* Ctl = FUnrealcvServer::Get().WorldController.Get();
    if (!Ctl || !World) return FExecStatus::Error(TEXT("WorldController invalid"));
    FString Mode = Args[0].ToLower();
    if (Mode == "part")
        Ctl->ObjectAnnotator.SetSegmentationMode(FObjectAnnotator::ESegmentationMode::Part, World);
    else if (Mode == "object")
        Ctl->ObjectAnnotator.SetSegmentationMode(FObjectAnnotator::ESegmentationMode::Object, World);
    else
        return FExecStatus::Error(TEXT("Mode must be part or object"));
    return FExecStatus::OK();
}

FExecStatus FSegmentationHandler::GetSegMode(const TArray<FString>& Args)
{
    auto* Ctl = FUnrealcvServer::Get().WorldController.Get();
    if (!Ctl) return FExecStatus::Error(TEXT("WorldController invalid"));
    auto Mode = Ctl->ObjectAnnotator.GetCurrentSegmentationMode();
    FString ModeStr = (Mode == FObjectAnnotator::ESegmentationMode::Part) ? TEXT("part") : TEXT("object");
    return FExecStatus::OK(ModeStr);
}
