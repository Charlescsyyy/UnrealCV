#include "LychSimObjectHandler.h"

#include "Controller/ActorController.h"
#include "Utils/StrFormatter.h"
#include "Utils/UObjectUtils.h"
#include "UnrealcvLog.h"
#include "VisionBPLib.h"

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
}

AActor* LychSimGetActor(const TArray<FString>& Args)
{
	FString ActorId = Args[0];
	AActor* Actor = GetActorById(FUnrealcvServer::Get().GetWorld(), ActorId);
	return Actor;
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
