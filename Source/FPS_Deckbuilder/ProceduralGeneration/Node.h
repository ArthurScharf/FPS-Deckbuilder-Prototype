#pragma once

#include "CoreMinimal.h"




struct FNode
{
	FVector Location;

	TArray<FNode*> Adjacent;

	// TODO: This should be a function, not a single value
	float Width;

	FString ID;
};