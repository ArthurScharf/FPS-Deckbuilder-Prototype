#pragma once

#include "CoreMinimal.h"



static int NextNodeID = 0;


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
	int ID;

	// UPROPERTY()
	FName TypeName;

	FNode()
	{
		ID = NextNodeID++;
		Location = FVector(0);
		Width = 0.f;
		TypeName = FName("");
	}
};