#pragma once

#include "CoreMinimal.h"
#include "GeomNode.generated.h"


static int NextNodeID = 0;


/*
* A node in a graph.
*
* - Abstract Explanation -
* Each node represents the position of the start or end of an imaginary brush-stroke in 3D space.
* The node contains data for setting the value of voxels in 3d space such as stroke width, height, angle, and
* type.
*/
UCLASS()
class UGeomNode : public UObject
{
	GENERATED_BODY()


public:
	UGeomNode();

	FString ToString();


public:
	UPROPERTY()
		FString Label;

	UPROPERTY()
		FVector Location;

	UPROPERTY()
		TArray<UGeomNode*> Adjacent;

	// TODO: This should be a function, not a single value
	UPROPERTY()
		uint32 Width;

private:
	UPROPERTY()
	uint32 ID;

public:
	FORCEINLINE uint32 GetID() const { return ID; }
};

// NOTE: These must be inline for some reason
//inline uint32 GetTypeHash(const UGeomNode& Node) { return Node.GetID(); }
//inli