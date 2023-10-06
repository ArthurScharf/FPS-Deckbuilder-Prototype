#pragma once

#include "CoreMinimal.h"



// static int NodeID = 4;


//USTRUCT()
struct FNode
{
	// GENERATED_BODY()

	// UPROPERTY()
	FVector Location;

	// UPROPERTY()
	TArray<FNode*> Adjacent;

	// TODO: This should be a function, not a single value
	// UPROPERTY()
	float Width;

	// UPROPERTY()
	FString ID;

	// UPROPERTY()
	FName TypeName;

	FNode()
	{
		Location = FVector(0);
		Width = 0.f;
		ID = FString("-1");
		TypeName = FName("");
	}
};