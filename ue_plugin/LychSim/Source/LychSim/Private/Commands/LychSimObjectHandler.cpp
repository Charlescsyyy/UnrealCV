#include "LychSimObjectHandler.h"

#include "Controller/ActorController.h"
#include "Utils/StrFormatter.h"
#include "Utils/UObjectUtils.h"
#include "UnrealcvLog.h"
#include "VisionBPLib.h"
#include "EngineUtils.h"
#include "Actor/LychSimBasicActor.h"

void FLychSimObjectHandler::RegisterCommands()
{
	CommandDispatcher->BindCommand(
		"lych object list",
		FDispatcherDelegate::CreateRaw(this, &FLychSimObjectHandler::ListObjects),
		"Get a list of all objects."
	);
	
	CommandDispatcher->BindCommand(
		"lych object get_loc [str]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimObjectHandler::GetObjectLocation),
		"Get object location [x, y, z]."
	);

	CommandDispatcher->BindCommand(
		"lych object get_aabb [str]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimObjectHandler::GetObjectAABB),
		"Get object axis-aligned bounding box [center(3), extent(3)]."
	);

	CommandDispatcher->BindCommand(
		"lych object add [str] [str] [str] [str] [str] [str] [str] [str]",
		FDispatcherDelegate::CreateRaw(this, &FLychSimObjectHandler::AddObject),
		"Add object to the scene."
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

FExecStatus FLychSimObjectHandler::GetObjectLocation(const TArray<FString>& Args)
{
	AActor* Actor = LychSimGetActor(Args);
	if (!Actor) return FExecStatus::Error("Object not found");

	FActorController Controller(Actor);
	FVector Location = Controller.GetLocation();

	FStrFormatter Ar;
	Ar << Location;

	return FExecStatus::OK(Ar.ToString());
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
