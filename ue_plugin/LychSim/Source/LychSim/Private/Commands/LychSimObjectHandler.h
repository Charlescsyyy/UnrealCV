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
	FExecStatus GetObjectAABB(const TArray<FString>& Args);
};
