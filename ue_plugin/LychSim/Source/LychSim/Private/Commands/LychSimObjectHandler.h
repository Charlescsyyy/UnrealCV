#pragma once

#include "CommandDispatcher.h"
#include "CommandHandler.h"

class FLychSimObjectHandler : public FCommandHandler
{
public:
	void RegisterCommands();

private:
	FExecStatus ListObjects(const TArray<FString>& Args);

	FExecStatus GetObjectLocation(const TArray<FString>& Args);
	FExecStatus GetObjectLocation(const TArray<FString>& Pos, const TMap<FString,FString>& Kw, const TSet<FString>& Flags);
	FExecStatus GetObjectAABB(const TArray<FString>& Args);
	FExecStatus GetObjectOBB(const TArray<FString>& Args);

	FExecStatus AddObject(const TArray<FString>& Args);

	FExecStatus SetObjectMaterial(const TArray<FString>& Args);
};
