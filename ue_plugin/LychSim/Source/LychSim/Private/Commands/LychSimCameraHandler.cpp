#include "LychSimCameraHandler.h"
#include "FusionCamSensor.h"
#include "SensorBPLib.h"
#include "Utils/StrFormatter.h"

void FLychSimCameraHandler::RegisterCommands() {
    CommandDispatcher->BindCommand(
        "lych cam get_loc [uint]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimCameraHandler::GetCameraLocation),
		"Get camera location in world space"
    );

    CommandDispatcher->BindCommand(
        "lych cam get_rot [uint]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimCameraHandler::GetCameraLocation),
		"Get camera location in world space"
    );

    CommandDispatcher->BindCommand(
        "lych cam get_fov [uint]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimCameraHandler::GetCameraFOV),
		"Get camera FOV"
    );

    CommandDispatcher->BindCommand(
        "lych cam get_c2w [uint]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimCameraHandler::GetCameraC2W),
		"Get camera C2W transform"
    );

	CommandDispatcher->BindCommand(
		"lych cam set_film_size [uint] [uint] [uint]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimCameraHandler::SetFilmSize),
		"Set Camera Film Size"
	);
}

UFusionCamSensor* FLychSimCameraHandler::GetCamera(const TArray<FString>& Args, FExecStatus& Status)
{
	if (Args.Num() < 1)
	{
		FString Msg = TEXT("No sensor id is available");
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
		Status = FExecStatus::Error(Msg);
		return nullptr;
	}
	int SensorId = FCString::Atoi(*Args[0]);
	UFusionCamSensor* FusionSensor = USensorBPLib::GetSensorById(SensorId);
	if (!IsValid(FusionSensor))
	{
		FString Msg = TEXT("Invalid sensor id");
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
		Status = FExecStatus::Error(Msg);
		return nullptr;
	}
	return FusionSensor;
}

FExecStatus FLychSimCameraHandler::GetCameraLocation(const TArray<FString>& Args)
{
    FExecStatus Status = FExecStatus::OK();
	UFusionCamSensor* FusionCamSensor = GetCamera(Args, Status);
	if (!IsValid(FusionCamSensor)) return Status;
	FStrFormatter Ar;
	FVector Location = FusionCamSensor->GetSensorLocation();
	Ar << Location;
    return FExecStatus::OK(Ar.ToString());
}

FExecStatus FLychSimCameraHandler::GetCameraRotation(const TArray<FString>& Args)
{
    FExecStatus Status = FExecStatus::OK();
	UFusionCamSensor* FusionCamSensor = GetCamera(Args, Status);
	if (!IsValid(FusionCamSensor)) return Status;
	FRotator Rotation = FusionCamSensor->GetSensorRotation();
	FStrFormatter Ar;
	Ar << Rotation;
	return FExecStatus::OK(Ar.ToString());
}

FExecStatus FLychSimCameraHandler::GetCameraFOV(const TArray<FString>& Args)
{
	FExecStatus Status = FExecStatus::InvalidArgument;
	UFusionCamSensor* FusionCamSensor = GetCamera(Args, Status);
	if (!IsValid(FusionCamSensor)) return FExecStatus::InvalidArgument;
	float FOV = FusionCamSensor->GetSensorFOV();
	FString Res = FString::Printf(TEXT("%f"), FOV);
	return FExecStatus::OK(Res);
}

FExecStatus FLychSimCameraHandler::GetCameraC2W(const TArray<FString>& Args)
{
    FExecStatus Status = FExecStatus::OK();
	UFusionCamSensor* FusionCamSensor = GetCamera(Args, Status);
	if (!IsValid(FusionCamSensor)) return Status;
	FTransform CameraTransform = FusionCamSensor->GetComponentTransform();
    FMatrix C2W = CameraTransform.ToMatrixWithScale();
    FStrFormatter Ar;
	Ar << C2W;
	return FExecStatus::OK(Ar.ToString());
}

FExecStatus FLychSimCameraHandler::SetFilmSize(const TArray<FString>& Args)
{
	FExecStatus Status = FExecStatus::InvalidArgument;
	UFusionCamSensor* FusionCamSensor = GetCamera(Args, Status);
	if (!IsValid(FusionCamSensor)) return FExecStatus::InvalidArgument;

	if (Args.Num() != 3) return FExecStatus::InvalidArgument; // ID, Width, Height

	int Width = FCString::Atof(*Args[1]);
	int Height = FCString::Atof(*Args[2]);
	FusionCamSensor->SetFilmSize(Width, Height);
	return FExecStatus::OK();
}
