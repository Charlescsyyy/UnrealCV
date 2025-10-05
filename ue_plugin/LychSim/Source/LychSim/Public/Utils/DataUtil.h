#pragma once

#include "CoreMinimal.h"
#include "CommandHandler.h"

namespace LychSim
{
    enum class EFilenameType
    {
	    Png,
	    Npy,
	    Exr,
	    Bmp,
	    PngBinary,
	    NpyBinary,
	    BmpBinary,
	    Invalid, // Unrecognized filename type
    };

    LYCHSIM_API EFilenameType ParseFilenameType(const FString& Filename);
    LYCHSIM_API FExecStatus SerializeData(const TArray<FColor>& Data, int Width, int Height, const FString& Filename);
	LYCHSIM_API FExecStatus SerializeData(const TArray<FFloat16Color>& Data, int Width, int Height, const FString& Filename);
	LYCHSIM_API FExecStatus SerializeData(const TArray<float>& Data, int Width, int Height, const FString& Filename);

    template<class T>
	void SaveData(const TArray<T>& Data, int Width, int Height,
		const TArray<FString>& Args, FExecStatus& Status)
    {
        if (Args.Num() != 2)
	    {
		    Status = FExecStatus::Error("Filename can not be empty");
		    return;
	    }
	    FString Filename = Args[1];
	    if (Data.Num() == 0)
	    {
		    Status = FExecStatus::Error("Captured data is empty");
		    return;
	    }
	    Status = SerializeData(Data, Width, Height, Filename);
	    return;
    }
}
