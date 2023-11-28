#pragma once

#include "CoreMinimal.h"
#include "CostPackage.generated.h"



USTRUCT(Blueprintable, BlueprintType)
struct FCost
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Resource_X = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Resource_Y = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Resource_Z = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Money;
};