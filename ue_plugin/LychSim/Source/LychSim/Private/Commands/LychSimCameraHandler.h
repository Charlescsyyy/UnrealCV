#pragma once

#include "CameraHandler.h"
#include "CommandHandler.h"

class FLychSimCameraHandler : public FCommandHandler
{
public:
    void RegisterCommands();

private:
    class UFusionCamSensor* GetCamera(const TArray<FString>& Args, FExecStatus& Status);

    FExecStatus GetCameraLocation(const TArray<FString>& Args);
    FExecStatus GetCameraRotation(const TArray<FString>& Args);
    FExecStatus SetCameraLocation(const TArray<FString>& Args);
    FExecStatus SetCameraRotation(const TArray<FString>& Args);
    FExecStatus GetCameraFOV(const TArray<FString>& Args);
    FExecStatus SetCameraFOV(const TArray<FString>& Args);
    FExecStatus GetCameraC2W(const TArray<FString>& Args);

    FExecStatus SetFilmSize(const TArray<FString>& Args);

    FExecStatus GetCameraLit(const TArray<FString>& Args);
    FExecStatus GetCameraSeg(const TArray<FString>& Args);
};
