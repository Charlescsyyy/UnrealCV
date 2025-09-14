#pragma once

#include "CommandHandler.h"

class FLychSimCameraHandler : public FCommandHandler
{
public:
    void RegisterCommands();

private:
    class UFusionCamSensor* GetCamera(const TArray<FString>& Args, FExecStatus& Status);

    FExecStatus GetCameraLocation(const TArray<FString>& Args);
    FExecStatus GetCameraRotation(const TArray<FString>& Args);
    FExecStatus GetCameraFOV(const TArray<FString>& Args);
    FExecStatus GetCameraC2W(const TArray<FString>& Args);
};
