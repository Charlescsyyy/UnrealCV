#include "LychSimDataHandler.h"

#include "Controller/ActorController.h"
#include "CameraHandler.h"
#include "FusionCamSensor.h"
#include "ImageUtil.h"
#include "SensorBPLib.h"
#include "Serialization.h"
#include "Utils/DataUtil.h"
#include "Utils/StrFormatter.h"
#include "Utils/UObjectUtils.h"
#include "UnrealcvLog.h"
#include "Editor.h"
#include "ScopedTransaction.h"

#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

#include "Editor.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"
#include "Selection.h"
#include "GameFramework/Actor.h"

#include "DrawDebugHelpers.h"

void FLychSimDataHandler::RegisterCommands() {
	CommandDispatcher->BindCommandUE(
		"lych data info",
		FDispatcherDelegateUE::CreateRaw(this, &FLychSimDataHandler::CollectInfo),
		"Get info about selected object."
	);

	CommandDispatcher->BindCommandUE(
		"lych data debug_line",
		FDispatcherDelegateUE::CreateRaw(this, &FLychSimDataHandler::LSDrawDebugLine),
		"Draw a debug line connecting the center of a list of objects."
	);
}

FExecStatus FLychSimDataHandler::CollectInfo(const TArray<FString>& Pos, const TMap<FString,FString>& Kw, const TSet<FString>& Flags)
{
	FString Out;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&Out);

	Writer->WriteObjectStart();

#if WITH_EDITOR
	if (!GEditor)
	{
		Writer->WriteValue(TEXT("status"), TEXT("Editor not found"));
		Writer->WriteObjectEnd();
		Writer->Close();
		return FExecStatus::OK(MoveTemp(Out));
	}

	USelection* SelectedActors = GEditor->GetSelectedActors();
    if (!SelectedActors || SelectedActors->Num() == 0)
    {
        Writer->WriteValue(TEXT("status"), TEXT("No actor selected in editor"));
		Writer->WriteObjectEnd();
		Writer->Close();
		return FExecStatus::OK(MoveTemp(Out));
    }

	Writer->WriteValue(TEXT("status"), TEXT("ok"));
	Writer->WriteArrayStart(TEXT("outputs"));

	for (FSelectionIterator It(*SelectedActors); It; ++It)
	{
		if (AActor* Actor = Cast<AActor>(*It))
		{
			Writer->WriteObjectStart();
			Writer->WriteValue(TEXT("object_id"), *Actor->GetName());

			Writer->WriteValue(TEXT("status"), TEXT("ok"));

			FGuid Guid = Actor->GetActorGuid();
			if (Guid.IsValid())
			{
				Writer->WriteValue(TEXT("guid"), *Guid.ToString());
			}
			else
			{
				Writer->WriteValue(TEXT("guid"), TEXT("NO_GUID"));
			}

			FActorController Controller(Actor);

			FBox AABB = Controller.GetAxisAlignedBoundingBox();
			Writer->WriteObjectStart(TEXT("aabb"));
			Writer->WriteArrayStart(TEXT("center"));
        	Writer->WriteValue(AABB.GetCenter().X);
        	Writer->WriteValue(AABB.GetCenter().Y);
        	Writer->WriteValue(AABB.GetCenter().Z);
        	Writer->WriteArrayEnd();
			Writer->WriteArrayStart(TEXT("extent"));
			Writer->WriteValue(AABB.GetExtent().X);
			Writer->WriteValue(AABB.GetExtent().Y);
			Writer->WriteValue(AABB.GetExtent().Z);
			Writer->WriteArrayEnd();
			Writer->WriteObjectEnd();

			FVector Center;
			FVector Extent;
			Actor->GetActorBounds(false, Center, Extent);
			FRotator Rotator = Actor->GetActorRotation();
			Writer->WriteObjectStart(TEXT("obb"));
			Writer->WriteArrayStart(TEXT("center"));
        	Writer->WriteValue(Center.X);
        	Writer->WriteValue(Center.Y);
        	Writer->WriteValue(Center.Z);
        	Writer->WriteArrayEnd();
			Writer->WriteArrayStart(TEXT("extent"));
			Writer->WriteValue(Extent.X);
			Writer->WriteValue(Extent.Y);
			Writer->WriteValue(Extent.Z);
			Writer->WriteArrayEnd();
			Writer->WriteArrayStart(TEXT("rotation"));
			Writer->WriteValue(Rotator.Pitch);
			Writer->WriteValue(Rotator.Yaw);
			Writer->WriteValue(Rotator.Roll);
			Writer->WriteArrayEnd();
			Writer->WriteObjectEnd();

			bool bOnlyCollidingComponents = false;
			Actor->GetActorBounds(bOnlyCollidingComponents, Center, Extent);
			Writer->WriteObjectStart(TEXT("bounds"));
			Writer->WriteArrayStart(TEXT("center"));
			Writer->WriteValue(Center.X);
			Writer->WriteValue(Center.Y);
			Writer->WriteValue(Center.Z);
			Writer->WriteArrayEnd();
			Writer->WriteArrayStart(TEXT("extent"));
			Writer->WriteValue(Extent.X);
			Writer->WriteValue(Extent.Y);
			Writer->WriteValue(Extent.Z);
			Writer->WriteArrayEnd();
			Writer->WriteObjectEnd();

			FVector Location = Controller.GetLocation();
			Writer->WriteArrayStart(TEXT("location"));
			Writer->WriteValue(Location.X); Writer->WriteValue(Location.Y); Writer->WriteValue(Location.Z);
			Writer->WriteArrayEnd();

			FRotator Rotation = Controller.GetRotation();
			Writer->WriteArrayStart(TEXT("rotation"));
			Writer->WriteValue(Rotation.Pitch); Writer->WriteValue(Rotation.Yaw); Writer->WriteValue(Rotation.Roll);
			Writer->WriteArrayEnd();

			FVector Scale = Actor->GetActorScale3D();
			Writer->WriteArrayStart(TEXT("scale"));
			Writer->WriteValue(Scale.X); Writer->WriteValue(Scale.Y); Writer->WriteValue(Scale.Z);
			Writer->WriteArrayEnd();

			FColor AnnotationColor;
			Controller.GetAnnotationColor(AnnotationColor);
			Writer->WriteArrayStart(TEXT("color"));
        	Writer->WriteValue(AnnotationColor.R); Writer->WriteValue(AnnotationColor.G);
			Writer->WriteValue(AnnotationColor.B); Writer->WriteValue(AnnotationColor.A);
        	Writer->WriteArrayEnd();

			Writer->WriteObjectEnd();
		}
	}

	Writer->WriteArrayEnd();
	Writer->WriteObjectEnd();
	Writer->Close();
	return FExecStatus::OK(MoveTemp(Out));
#else
	Writer->WriteValue(TEXT("status"), TEXT("Running CollectInfo from outside editor"))
	Writer->WriteObjectEnd();
	Writer->Close();
	return FExecStatus::OK(MoveTemp(Out));
#endif
}

FExecStatus FLychSimDataHandler::LSDrawDebugLine(const TArray<FString>& Pos, const TMap<FString,FString>& Kw, const TSet<FString>& Flags)
{
	FString Out;
    TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&Out);
	Writer->WriteObjectStart();

	TArray<AActor*> ActorList;
	for (const FString& ActorId : Pos)
	{
		AActor* Actor = GetActorById(FUnrealcvServer::Get().GetWorld(), ActorId);
		if (!IsValid(Actor))
		{
			Writer->WriteValue(TEXT("status"), TEXT("Actor not valid: ") + ActorId);
			Writer->WriteObjectEnd();
			Writer->Close();
			return FExecStatus::OK(MoveTemp(Out));
		}
		ActorList.Add(Actor);
	}

#if WITH_EDITOR
	if (ActorList.Num() < 2)
	{
		Writer->WriteValue(TEXT("status"), TEXT("Need at least two actors to draw a line"));
		Writer->WriteObjectEnd();
		Writer->Close();
		return FExecStatus::OK(MoveTemp(Out));
	}

	UWorld* World = ActorList[0]->GetWorld();

	TArray<FVector> Centers;
    Centers.Reserve(ActorList.Num());

	for (AActor* Actor : ActorList)
	{
		FActorController Controller(Actor);
		FBox AABB = Controller.GetAxisAlignedBoundingBox();
		Centers.Add(AABB.GetCenter());
	}

	for (const FVector& C : Centers)
    {
        DrawDebugSphere(
            World, C,
            /*Radius=*/8.f,
            /*Segments=*/12,
            FColor::Cyan,
            /*bPersistentLines=*/true,
            /*LifeTime=*/-1.f,
            /*DepthPriority=*/0,
            /*Thickness=*/1.5f
        );
    }

	for (int32 i = 0; i + 1 < Centers.Num(); ++i)
    {
        DrawDebugDirectionalArrow(
            World,
            Centers[i],
            Centers[i + 1],
			/*ArrowSize=*/30.0f,
            FColor::Green,
            /*bPersistentLines=*/true,
            /*LifeTime=*/-1.f,
            /*DepthPriority=*/0,
            /*Thickness=*/2.0f
        );
    }

	Writer->WriteValue(TEXT("status"), TEXT("ok"));
	Writer->WriteObjectEnd();
	Writer->Close();
	return FExecStatus::OK(MoveTemp(Out));
#else
	Writer->WriteValue(TEXT("status"), TEXT("Running DrawDebugLine from outside editor"));
	Writer->WriteObjectEnd();
	Writer->Close();
	return FExecStatus::OK(MoveTemp(Out));
#endif
}
