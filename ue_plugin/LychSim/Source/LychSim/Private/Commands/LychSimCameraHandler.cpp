#include "LychSimCameraHandler.h"
#include "CameraHandler.h"
#include "FusionCamSensor.h"
#include "ImageUtil.h"
#include "SensorBPLib.h"
#include "Serialization.h"
#include "Utils/DataUtil.h"
#include "Utils/StrFormatter.h"
#include "UnrealcvLog.h"
#include "Editor.h"
#include "ScopedTransaction.h"

void FLychSimCameraHandler::RegisterCommands() {
    CommandDispatcher->BindCommand(
        "lych cam get_loc [uint]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimCameraHandler::GetCameraLocation),
		"Get camera location in world space"
    );

	CommandDispatcher->BindCommand(
        "lych cam set_loc [uint] [float] [float] [float]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimCameraHandler::SetCameraLocation),
		"Set camera location in world space"
    );

    CommandDispatcher->BindCommand(
        "lych cam get_rot [uint]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimCameraHandler::GetCameraRotation),
		"Get camera location in world space"
    );

	CommandDispatcher->BindCommand(
        "lych cam set_rot [uint] [float] [float] [float]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimCameraHandler::SetCameraRotation),
		"Set camera rotation in world space"
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

	CommandDispatcher->BindCommand(
		"lych cam get_lit [uint] [str]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimCameraHandler::GetCameraLit),
		"Get png rendering data from lit sensor"
	);

	CommandDispatcher->BindCommand(
		"lych cam get_seg [uint] [str]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimCameraHandler::GetCameraSeg),
		"Get png segmentation data from annotation sensor"
	);

	CommandDispatcher->BindCommand(
		"lych cam get_depth [uint] [str]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimCameraHandler::GetCameraDepth),
		"Get png depth data from annotation sensor"
	);

	CommandDispatcher->BindCommand(
		"lych cam get_annots [uint]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimCameraHandler::GetCameraAnnotations),
		"Get png annotations data from annotation sensor"
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

FExecStatus FLychSimCameraHandler::GetCameraLit(const TArray<FString>& Args)
{
	FExecStatus ExecStatus = FExecStatus::OK();
	UFusionCamSensor* FusionCamSensor = GetCamera(Args, ExecStatus);
	if (!IsValid(FusionCamSensor)) return ExecStatus;

	TArray<FColor> Data;
	int Width, Height;
	FusionCamSensor->GetLit(Data, Width, Height);
	LychSim::SaveData(Data, Width, Height, Args, ExecStatus);
	return ExecStatus;
}

FExecStatus FLychSimCameraHandler::GetCameraSeg(const TArray<FString>& Args)
{
	FExecStatus ExecStatus = FExecStatus::OK();
	UFusionCamSensor* FusionCamSensor = GetCamera(Args, ExecStatus);
	if (!IsValid(FusionCamSensor)) return ExecStatus;

	TArray<FColor> Data;
	int Width, Height;
	FusionCamSensor->GetSeg(Data, Width, Height);

	LychSim::SaveData(Data, Width, Height, Args, ExecStatus);
	return ExecStatus;
}

FExecStatus FLychSimCameraHandler::GetCameraDepth(const TArray<FString>& Args)
{
	FExecStatus ExecStatus = FExecStatus::OK();
	UFusionCamSensor* FusionCamSensor = GetCamera(Args, ExecStatus);
	if (!IsValid(FusionCamSensor)) return ExecStatus;

	TArray<float> Data;
	int Width, Height;
	FusionCamSensor->GetDepth(Data, Width, Height);

	LychSim::SaveData(Data, Width, Height, Args, ExecStatus);
	return ExecStatus;
}

FExecStatus FLychSimCameraHandler::SetCameraLocation(const TArray<FString>& Args)
{
	FExecStatus Status = FExecStatus::OK();
	UFusionCamSensor* FusionCamSensor = GetCamera(Args, Status);
	if (!IsValid(FusionCamSensor))
	{
		UE_LOG(LogUnrealCV, Warning, TEXT("The camera is invalid when setting location."));
		return FExecStatus::Error();
	}

	// Should I set the component loction or the actor location?
	if (Args.Num() != 4) return FExecStatus::InvalidArgument; // ID, X, Y, Z

	float X = FCString::Atof(*Args[1]), Y = FCString::Atof(*Args[2]), Z = FCString::Atof(*Args[3]);
	FVector Location = FVector(X, Y, Z);

	FusionCamSensor->SetSensorLocation(Location);

	return FExecStatus::OK();
}

FExecStatus FLychSimCameraHandler::SetCameraRotation(const TArray<FString>& Args)
{
	FExecStatus Status = FExecStatus::OK();
	UFusionCamSensor* FusionCamSensor = GetCamera(Args, Status);
	if (!IsValid(FusionCamSensor)) return Status;

	if (Args.Num() != 4) return FExecStatus::InvalidArgument; // ID, X, Y, Z
	float Pitch = FCString::Atof(*Args[1]), Yaw = FCString::Atof(*Args[2]), Roll = FCString::Atof(*Args[3]);
	FRotator Rotator = FRotator(Pitch, Yaw, Roll);

	FusionCamSensor->SetSensorRotation(Rotator);

	return FExecStatus::OK();
}

FExecStatus FLychSimCameraHandler::SetCameraFOV(const TArray<FString>& Args)
{
	FExecStatus Status = FExecStatus::OK();
	UFusionCamSensor* FusionCamSensor = GetCamera(Args, Status);
	if (!IsValid(FusionCamSensor)) return Status;

	if (Args.Num() != 2) return FExecStatus::InvalidArgument; // ID, FOV

	float FOV = FCString::Atof(*Args[1]);
	FusionCamSensor->SetSensorFOV(FOV);

	return FExecStatus::OK();
}

FExecStatus FLychSimCameraHandler::GetCameraAnnotations(const TArray<FString>& Args)
{
	FString Out;
    TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&Out);
	Writer->WriteObjectStart();

	FExecStatus Status = FExecStatus::OK();
	UFusionCamSensor* FusionCamSensor = GetCamera(Args, Status);
	if (!IsValid(FusionCamSensor))
	{
		Writer->WriteValue(TEXT("status"), TEXT("invalid_camera"));
		Writer->WriteObjectEnd();
		Writer->Close();
		return FExecStatus::OK(MoveTemp(Out));
	}

	Writer->WriteValue(TEXT("status"), TEXT("ok"));
	Writer->WriteObjectStart(TEXT("outputs"));

	FVector Location = FusionCamSensor->GetSensorLocation();
	Writer->WriteArrayStart(TEXT("location"));
	Writer->WriteValue(Location.X); Writer->WriteValue(Location.Y); Writer->WriteValue(Location.Z);
	Writer->WriteArrayEnd();

	FRotator Rotation = FusionCamSensor->GetSensorRotation();
	Writer->WriteArrayStart(TEXT("rotation"));
	Writer->WriteValue(Rotation.Pitch); Writer->WriteValue(Rotation.Yaw); Writer->WriteValue(Rotation.Roll);
	Writer->WriteArrayEnd();

	float FOV = FusionCamSensor->GetSensorFOV();
	Writer->WriteValue(TEXT("fov"), FOV);

	FTransform CameraTransform = FusionCamSensor->GetComponentTransform();
	FMatrix C2W = CameraTransform.ToMatrixWithScale();
	Writer->WriteArrayStart(TEXT("c2w"));
	Writer->WriteArrayStart();
	Writer->WriteValue(C2W.M[0][0]); Writer->WriteValue(C2W.M[0][1]); Writer->WriteValue(C2W.M[0][2]); Writer->WriteValue(C2W.M[0][3]);
	Writer->WriteArrayEnd();
	Writer->WriteArrayStart();
	Writer->WriteValue(C2W.M[1][0]); Writer->WriteValue(C2W.M[1][1]); Writer->WriteValue(C2W.M[1][2]); Writer->WriteValue(C2W.M[1][3]);
	Writer->WriteArrayEnd();
	Writer->WriteArrayStart();
	Writer->WriteValue(C2W.M[2][0]); Writer->WriteValue(C2W.M[2][1]); Writer->WriteValue(C2W.M[2][2]); Writer->WriteValue(C2W.M[2][3]);
	Writer->WriteArrayEnd();
	Writer->WriteArrayStart();
	Writer->WriteValue(C2W.M[3][0]); Writer->WriteValue(C2W.M[3][1]); Writer->WriteValue(C2W.M[3][2]); Writer->WriteValue(C2W.M[3][3]);
	Writer->WriteArrayEnd();
	Writer->WriteArrayEnd();

	int Width = FusionCamSensor->GetFilmWidth();
	int Height = FusionCamSensor->GetFilmHeight();
	Writer->WriteValue(TEXT("width"), Width);
	Writer->WriteValue(TEXT("height"), Height);

	Writer->WriteObjectEnd();
	Writer->WriteObjectEnd();
	Writer->Close();
	return FExecStatus::OK(MoveTemp(Out));
}
