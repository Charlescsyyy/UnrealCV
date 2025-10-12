#pragma once

#include "CommandDispatcher.h"
#include "CommandHandler.h"

class FLychSimDataHandler : public FCommandHandler
{
public:
	void RegisterCommands();

private:
	FExecStatus CollectInfo(const TArray<FString>& Pos, const TMap<FString,FString>& Kw, const TSet<FString>& Flags);

	FExecStatus LSDrawDebugLine(const TArray<FString>& Pos, const TMap<FString,FString>& Kw, const TSet<FString>& Flags);
};
