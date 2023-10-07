// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Shape.generated.h"




// 
USTRUCT()
struct FVertex
{
	GENERATED_BODY()

	// Location is always relative to the origin of the shape.
	FVector Location;

	// Adjacent vertices. Order doesn't matter. TODO: Convert to Set
	TArray<FVertex*> Adjacent;

	// Index for finding this vertex within the procedural mesh component
	int ProceduralMeshIndex;

	FVertex()
	{
		ProceduralMeshIndex = -1;
	}
};





/* A face on a shape*/
USTRUCT()
struct FFace
{
	GENERATED_BODY()

	/* Ordered using the right hand rule on the normal
	*  { Upper left, bottom left, bottom right, upper right}
	*/
	TArray<FVertex*> Vertices;
	
	FVector Normal;

	FFace()
	{}
};


enum EFaceAxis
{
	X,
	Y
};





/**
 * 
 */
UCLASS()
class FPS_DECKBUILDER_API UShape : public UObject
{
	GENERATED_BODY()


public:
	void MoveFace(FFace* Face, const FVector& DeltaLocation);


	 
	/* Face : the face to be subdivided
	*  Axis : will subdivide on the X or the Y or the Z in the local coordinates of the shape 
	*  Percent: How far along the shape the subdivision takes place
	*  NOTE: only subdivides into rectangles right now
	*/
	void SubdivideFace(FFace* Face, EFaceAxis Axis, float Percent, FFace* OutFace1, FFace* OutFace2);

public:
	TArray<FFace*> Faces;
};
