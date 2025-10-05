#include "LychSimObjectHandler.h"

#include "Controller/ActorController.h"
#include "Utils/StrFormatter.h"
#include "Utils/UObjectUtils.h"
#include "UnrealcvLog.h"
#include "VisionBPLib.h"
#include "EngineUtils.h"
#include "Actor/LychSimBasicActor.h"

#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

void FLychSimObjectHandler::RegisterCommands()
{
	CommandDispatcher->BindCommand(
		"lych obj list",
		FDispatcherDelegate::CreateRaw(this, &FLychSimObjectHandler::ListObjects),
		"Get a list of all objects."
	);

	CommandDispatcher->BindCommandUE(
		"lych obj get_loc",
		FDispatcherDelegateUE::CreateRaw(this, &FLychSimObjectHandler::GetObjectLocation),
		"Get object location [x, y, z]."
	);

	CommandDispatcher->BindCommand(
		"lych obj get_aabb [str]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimObjectHandler::GetObjectAABB),
		"Get object axis-aligned bounding box [center(3), extent(3)]."
	);

	CommandDispatcher->BindCommand(
		"lych obj get_obb [str]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimObjectHandler::GetObjectOBB),
		"Get object oriented bounding box [center(3), extent(3), rotation_matrix(9)]."
	);

	CommandDispatcher->BindCommandUE(
		"lych obj get_color",
		FDispatcherDelegateUE::CreateRaw(this, &FLychSimObjectHandler::GetObjectAnnotationColor),
		"Get object color [r, g, b, a]."
	);

	CommandDispatcher->BindCommand(
		"lych obj add [str] [str] [str] [str] [str] [str] [str] [str]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimObjectHandler::AddObject),
		"Add object to the scene."
	);

	CommandDispatcher->BindCommand(
		"lych obj set_mtl [str] [str] [str]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimObjectHandler::SetObjectMaterial),
		"Set object material."
	);
}

AActor* LychSimGetActor(const TArray<FString>& Args)
{
	FString ActorId = Args[0];
	AActor* Actor = GetActorById(FUnrealcvServer::Get().GetWorld(), ActorId);
	return Actor;
}

UWorld* GetPIEWorld()
{
	if (GEngine)
	{
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			if (Context.WorldType == EWorldType::PIE)
			{
				return Context.World();
			}
		}
	}
	return nullptr;
}

bool ExistsActor(UWorld* World, const FString& ActorName)
{
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (It->GetName() == ActorName)
		{
			return true;
		}
	}
	return false;
}

FExecStatus FLychSimObjectHandler::ListObjects(const TArray<FString>& Args)
{
	TArray<AActor*> ActorList;
	UVisionBPLib::GetActorList(ActorList);

	FString StrActorList;
	for (AActor* Actor : ActorList)
	{
		StrActorList += FString::Printf(TEXT("%s "), *Actor->GetName());
	}
	return FExecStatus::OK(StrActorList);
}

FExecStatus FLychSimObjectHandler::GetObjectLocation(
	const TArray<FString>& Pos,
    const TMap<FString,FString>& Kw,
    const TSet<FString>& Flags)
{
	TArray<AActor*> ActorList;
	if (Flags.Contains("all"))
	{
		UVisionBPLib::GetActorList(ActorList);
	}
	else
	{
		for (const FString& ActorId : Pos)
		{
			AActor* Actor = GetActorById(FUnrealcvServer::Get().GetWorld(), ActorId);
			ActorList.Add(Actor);
		}
	}

	FString Out;
    TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&Out);

	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("status"), TEXT("ok"));

	Writer->WriteArrayStart(TEXT("outputs"));

	for (AActor* Actor : ActorList)
	{
		Writer->WriteObjectStart();
		Writer->WriteValue(TEXT("object_id"), *Actor->GetName());

		if (!Actor)
        {
            Writer->WriteValue(TEXT("status"), TEXT("not_found"));
        }
		else
		{
			Writer->WriteValue(TEXT("status"), TEXT("ok"));

			FActorController Controller(Actor);
			FVector Location = Controller.GetLocation();

			Writer->WriteArrayStart(TEXT("center"));
        	Writer->WriteValue(Location.X); Writer->WriteValue(Location.Y); Writer->WriteValue(Location.Z);
        	Writer->WriteArrayEnd();
		}

		Writer->WriteObjectEnd();
	}

	Writer->WriteArrayEnd();
	Writer->WriteObjectEnd();
	Writer->Close();

	return FExecStatus::OK(MoveTemp(Out));
}

FExecStatus FLychSimObjectHandler::GetObjectAABB(const TArray<FString>& Args)
{
	AActor* Actor = LychSimGetActor(Args);
	if (!Actor) return FExecStatus::Error("Object not found");

	FActorController Controller(Actor);
	FBox AABB = Controller.GetAxisAlignedBoundingBox();

	FVector Center = AABB.GetCenter();
	FVector Extent = AABB.GetExtent();

	FStrFormatter Ar1;
	Ar1 << Center;

	FStrFormatter Ar2;
	Ar2 << Extent;

	return FExecStatus::OK(Ar1.ToString() + " " + Ar2.ToString());
}

FExecStatus FLychSimObjectHandler::GetObjectOBB(const TArray<FString>& Args)
{
	AActor* Actor = LychSimGetActor(Args);
	if (!Actor) return FExecStatus::Error("Object not found");

	FVector Center;
	FVector Extent;
	Actor->GetActorBounds(false, Center, Extent);

	FRotator Rotator = Actor->GetActorRotation();

	FStrFormatter Ar1;
	Ar1 << Center;

	FStrFormatter Ar2;
	Ar2 << Extent;

	FStrFormatter Ar3;
	Ar3 << Rotator;

	return FExecStatus::OK(Ar1.ToString() + " " + Ar2.ToString() + " " + Ar3.ToString());
}

FExecStatus FLychSimObjectHandler::GetObjectAnnotationColor(
	const TArray<FString>& Pos,
    const TMap<FString,FString>& Kw,
    const TSet<FString>& Flags)
{
	TArray<AActor*> ActorList;
	if (Flags.Contains("all"))
	{
		UVisionBPLib::GetActorList(ActorList);
	}
	else
	{
		for (const FString& ActorId : Pos)
		{
			AActor* Actor = GetActorById(FUnrealcvServer::Get().GetWorld(), ActorId);
			ActorList.Add(Actor);
		}
	}

	FString Out;
    TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&Out);

	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("status"), TEXT("ok"));

	Writer->WriteArrayStart(TEXT("outputs"));

	for (AActor* Actor : ActorList)
	{
		Writer->WriteObjectStart();
		Writer->WriteValue(TEXT("object_id"), *Actor->GetName());

		if (!Actor)
        {
            Writer->WriteValue(TEXT("status"), TEXT("not_found"));
        }
		else
		{
			Writer->WriteValue(TEXT("status"), TEXT("ok"));

			FColor AnnotationColor;
			FActorController Controller(Actor);
			Controller.GetAnnotationColor(AnnotationColor);

			Writer->WriteArrayStart(TEXT("center"));
        	Writer->WriteValue(AnnotationColor.R); Writer->WriteValue(AnnotationColor.G);
			Writer->WriteValue(AnnotationColor.B); Writer->WriteValue(AnnotationColor.A);
        	Writer->WriteArrayEnd();
		}

		Writer->WriteObjectEnd();
	}

	Writer->WriteArrayEnd();
	Writer->WriteObjectEnd();
	Writer->Close();

	return FExecStatus::OK(MoveTemp(Out));
}

FExecStatus FLychSimObjectHandler::AddObject(const TArray<FString>& Args)
{
	FString ObjectName, ObjectPath;
	float X = 0.0f, Y = 0.0f, Z = 0.0f;
	float Pitch = 0.0f, Yaw = 0.0f, Roll = 0.0f;

	if (Args.Num() == 2) { ObjectName = Args[0]; ObjectPath = Args[1]; }
	else if (Args.Num() == 5) {
		ObjectName = Args[0];
		ObjectPath = Args[1];
		X = FCString::Atof(*Args[2]);
		Y = FCString::Atof(*Args[3]);
		Z = FCString::Atof(*Args[4]);
	}
	else if (Args.Num() == 8) {
		ObjectName = Args[0];
		ObjectPath = Args[1];
		X = FCString::Atof(*Args[2]);
		Y = FCString::Atof(*Args[3]);
		Z = FCString::Atof(*Args[4]);
		Pitch = FCString::Atof(*Args[5]);
		Yaw = FCString::Atof(*Args[6]);
		Roll = FCString::Atof(*Args[7]);
	}
	else {
		return FExecStatus::Error("Usage: lych add_object [str] [str] ([str] [str] [str]) ([str] [str] [str])");
	}

	FVector Location(X, Y, Z);
	FRotator Rotation(Pitch, Yaw, Roll);

	UWorld* World = GetPIEWorld();
	if (!World) {
		return FExecStatus::Error("PIE world context not found");
	}

	if (ExistsActor(World, ObjectName))
	{
		return FExecStatus::Error("Object with the same name already exists");
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Name = FName(*ObjectName);
	ALychSimBasicActor* NewActor = World->SpawnActor<ALychSimBasicActor>(
		ALychSimBasicActor::StaticClass(),
		Location,
		Rotation,
		SpawnParams
	);
	if (!NewActor)
	{
		return FExecStatus::Error("Failed to spawn actor");
	}

	NewActor->InitializeMesh(ObjectPath);

	return FExecStatus::OK();
}

FExecStatus FLychSimObjectHandler::SetObjectMaterial(const TArray<FString>& Args)
{
	FString ObjectName, MaterialPath;
	int ElementIdx;

	if (Args.Num() == 3) {
		ObjectName = Args[0];
		MaterialPath = Args[1];
		ElementIdx = FCString::Atoi(*Args[2]);
	}
	else {
		return FExecStatus::Error("Usage: lych obj set_mtl [str] [str] [int]");
	}

	AActor* Actor = LychSimGetActor(Args);
	if (!Actor) return FExecStatus::Error("Object not found");

	UMeshComponent* MC = Actor->FindComponentByClass<UMeshComponent>();
	if (!MC) return FExecStatus::Error("No mesh component");

	UMaterialInterface* MI = Cast<UMaterialInterface>(
        StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *MaterialPath));
    if (!MI) return FExecStatus::Error("Material not found");

	MC->SetMaterial(ElementIdx, MI);
    return FExecStatus::OK("OK");
}
