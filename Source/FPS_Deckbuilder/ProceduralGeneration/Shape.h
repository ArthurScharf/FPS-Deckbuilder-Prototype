// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Shape.generated.h"



// Should this be here?
static int NextVertexID = 0;

// 
USTRUCT()
struct FVertex
{
	GENERATED_BODY()


public:
	// Location is always relative to the origin of the shape.
	FVector Location;

	// Adjacent vertices. Order doesn't matter. TODO: Convert to Set
	TArray<FVertex*> Adjacent;

	// Index for finding this vertex within the procedural mesh component
	int ProceduralMeshIndex;

private:
	uint32 ID;

	FString test;

public:
	FVertex()
	{
		ID = NextVertexID++;
		UE_LOG(LogTemp, Warning, TEXT("FFace::FFace -- FVertex_ID: %d"), ID);
	}

	inline uint32 GetID() const { return ID; }
};

// -- Enables usage with TSet -- //
FORCEINLINE uint32 GetTypeHash(const FVertex& Vertex) { return Vertex.GetID(); }
FORCEINLINE bool operator==(const FVertex& lhs, const FVertex& rhs) { return lhs.GetID() == rhs.GetID(); }



// TODO: This is code-smelly and should be removed
enum EFaceAxis
{
	X,
	Y
};




static int NextFaceID = 0;

/* A face on a shape*/
USTRUCT()
struct FFace
{
	GENERATED_BODY()

public:
	/* Ordered using the right hand rule on the normal
	*  { Upper left, bottom left, bottom right, upper right}
	*/
	TArray<FVertex*> Vertices;

	FVector Normal;

private:
	uint32 ID;

public:
	FFace()
	{
		ID = NextFaceID++;
		UE_LOG(LogTemp, Warning, TEXT("FFace::FFace -- FFace_ID: %d"), ID);
	}

	void SetAdjacency()
	{
		int N = Vertices.Num();
		for (int i = 0; i < N; i++)
		{			
			// the ternaries are checking for the need to loop. Can't use C++ `%` bacause negative left-hand values return negative numbers, unlike in true math
			Vertices[i]->Adjacent.Add(Vertices[(i == N-1) ? 0 : i]); // Vertex immediately clockwise
			Vertices[i]->Adjacent.Add(Vertices[(i == 0  ) ? N-1 : i]); // Vertex immediately counter clockwise
		}
	}

	// Enables Usage with TSet
	//inline const uint32 GetID() { return ID; }
	//static uint32 GetTypeHash(const FFace& Face) { return Face.GetID(); }
	//inline bool operator==(const FFace& rhs) { return ID == rhs.ID; }
};







/**
 * 
 */
UCLASS()
class FPS_DECKBUILDER_API UShape : public UObject
{
	GENERATED_BODY()



public:
	static void InitCube(UShape* Shape);

	// -- Modifier Methods -- //
	void SetScale(float Scale);

	void MoveFace(FFace& Face, const FVector& DeltaLocation);
 
	/* Face : the face to be subdivided
	*  Axis : will subdivide on the X or the Y or the Z in the local coordinates of the shape 
	*  Percent: How far along the shape the subdivision takes place
	*  NOTE: only subdivides into rectangles right now
	*/
	void SubdivideFace(FFace& Face, EFaceAxis Axis, float Percent, FFace* OutFace1, FFace* OutFace2);

	void ExtrudeFace(FFace& Face, float Distance, FFace* OutFace);

public:
	TArray<FFace*> Faces;

};
