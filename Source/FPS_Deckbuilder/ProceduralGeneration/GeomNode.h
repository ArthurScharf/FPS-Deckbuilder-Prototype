#pragma once

#include "CoreMinimal.h"
#include "GeomNode.generated.h"


static uint32 NextNodeID = 0;


/*
* A node in a graph.
* 
* - Abstract Explanation -
* Each node represents the position of the start or end of an imaginary brush-stroke in 3D space.
* The node contains data for setting the value of voxels in 3d space such as stroke width, height, angle, and
* type.
*/
USTRUCT()
struct FGeomNode
{
	GENERATED_BODY()


public:
	UPROPERTY()
	FString Label;

	UPROPERTY()
	FVector Location;

	TArray<FGeomNode*> Adjacent;

	// TODO: This should be a function, not a single value
	UPROPERTY()
	float Width;

private:
	UPROPERTY()
	uint32 ID;

public:
	FGeomNode()
	{
		ID = NextNodeID++;
		Location = FVector(0);
		Width = 0.f;
		Label = "";
	}

	inline uint32 GetID() const { return ID; }

};

// NOTE: These must be inline for some reason
//inline uint32 GetTypeHash(const FGeomNode& Node) { return Node.GetID(); }
//inline bool operator==(const FGeomNode& lhv, const FGeomNode& rhv) { return lhv.GetID() == rhv.GetID(); }