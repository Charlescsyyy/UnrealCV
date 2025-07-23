// SegmentationHandler.h
#pragma once
#include "CommandHandler.h"

class FSegmentationHandler : public FCommandHandler
{
public:
    void RegisterCommands() override;

private:
    FExecStatus SetSegMode(const TArray<FString>& Args);
    FExecStatus GetSegMode(const TArray<FString>& Args);
};
