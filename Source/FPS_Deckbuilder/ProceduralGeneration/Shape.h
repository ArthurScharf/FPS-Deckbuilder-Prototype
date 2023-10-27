// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DrawDebugHelpers.h"

#include "UObject/NoExportTypes.h"
#include "Shape.generated.h"




// This should be a uint8
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

private:
	uint32 ID;

	FString test;

public:
	FVertex()
	{
		ID = NextVertexID++;
		// UE_LOG(LogTemp, Warning, TEXT("FVertex() -- ID: %d"), ID);
	}

	FVertex(const FVector& _Location)
	{
		Location = _Location; 
	}

	FVertex(const FVertex& Vertex)
	{
		ID = NextVertexID++;
		Location = Vertex.Location;
		// UE_LOG(LogTemp, Warning, TEXT("FVertex(COPY) -- ID: %d"), ID);
	}
	
	//inline uint32 GetID() const { return ID; }
};

// -- Enables usage with TSet -- //
//FORCEINLINE uint32 GetTypeHash(const FVertex& Vertex) { return Vertex.GetID(); }
//FORCEINLINE bool operator==(const FVertex& lhs, const FVertex& rhs) { return lhs.GetID() == rhs.GetID(); }



// TODO: This is code-smelly and should be removed
enum EFaceAxis
{
	X,
	Y
};




static int NextFaceID = 0;

// TODO: Have the face init it's normal automatically. The user shouldn't need to init it 
/* A face on a shape
*  Face labels : {floor, roof, forward, back, left, right}
*/
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

	/* Set by grammer and used by grammer to detect and modify/replace this face in a shape*/
	FString Label;

private:
	uint32 ID;

public:
	FFace()
	{
		ID = NextFaceID++;
		// UE_LOG(LogTemp, Warning, TEXT("FFace() -- ID: %d"), ID);
	}

	FFace(const FFace& Face)
	{
		ID = NextFaceID++;

		FVertex* NewVertex;
		for (FVertex* Vertex : Face.Vertices)
		{
			NewVertex = new FVertex(*Vertex);
			NewVertex->Adjacent.RemoveAll([&](const FVertex* v)->bool { return true; });
			Vertices.Add(NewVertex);
		}
		SetAdjacency();

		// UE_LOG(LogTemp, Warning, TEXT("FFace(COPY) -- ID: %d"), ID);
	}

	void InitNormal()
	{
		Normal = FVector::CrossProduct((Vertices[3]->Location - Vertices[0]->Location), (Vertices[1]->Location - Vertices[0]->Location));
	}

	FVector GetFaceCenter()
	{
		if (Vertices.Num() == 0) return FVector(0);

		// -- Calculating the face center -- // 
		float X = 0;
		float Y = 0;
		float Z = 0;
		int Num = Vertices.Num();
		for (FVertex* Vertex : Vertices)
		{
			X += Vertex->Location.X;
			Y += Vertex->Location.Y;
			Z += Vertex->Location.Z;
		}
		X /= Num;
		Y /= Num;
		Z /= Num;
		return FVector(X, Y, Z);
	}

	void SetScale(float Scale)
	{
		if (Vertices.Num() == 0) return;

		// -- Calculating the face center -- // 
		FVector FaceCenter = GetFaceCenter();

		// -- Centering on 0, and scaling, Then centering at original center -- //
		for (FVertex* Vertex : Vertices)
		{
			Vertex->Location -= FaceCenter;
			Vertex->Location *= Scale;
			Vertex->Location += FaceCenter;
		}
	}

	/* BUG: Scale isn't being changed relative to the alignment of the face
	*  BUG: Should be set using a 2D vector since
	*/
	void SetScale(FVector Scale)
	{
		if (Vertices.Num() == 0) return;

		// -- Calculating the face center -- // 
		FVector FaceCenter = GetFaceCenter();

		// -- Centering on 0, and scaling, Then centering at original center -- //
		for (FVertex* Vertex : Vertices)
		{
			Vertex->Location -= FaceCenter;
			Vertex->Location.X *= Scale.X;
			Vertex->Location.Y *= Scale.Y;
			Vertex->Location.Z *= Scale.Z;
			Vertex->Location += FaceCenter;
		}
	}


	FVector2D GetDimensions()
	{
		return FVector2D(
			(Vertices[1]->Location - Vertices[0]->Location).Size(),
			(Vertices[3]->Location - Vertices[0]->Location).Size()
		);
	}


	void SetDimensions(FVector2D Dimensions)
	{
		FVector Center = GetFaceCenter();

		// X in the basis of the face 
		FVector UnitX = (Vertices[3]->Location - Vertices[0]->Location).GetSafeNormal() / 2.f;
		FVector UnitY = (Vertices[1]->Location - Vertices[0]->Location).GetSafeNormal() / 2.f;
		
		/* Observe the differing signs in each calculation */
		Vertices[0]->Location = Center - (Dimensions.X * UnitX) - (Dimensions.Y * UnitY);
		Vertices[1]->Location = Center - (Dimensions.X * UnitX) + (Dimensions.Y * UnitY);
		Vertices[2]->Location = Center + (Dimensions.X * UnitX) + (Dimensions.Y * UnitY);
		Vertices[3]->Location = Center + (Dimensions.X * UnitX) - (Dimensions.Y * UnitY);
	}

	void SetAdjacency()
	{
		int N = Vertices.Num();
		for (int i = 0; i < N; i++)
		{
			// the ternaries are checking for the need to loop. Can't use C++ `%` bacause negative left-hand values return negative numbers, unlike in true math
			Vertices[i]->Adjacent.Add(Vertices[(i == N - 1) ? 0 : i]); // Vertex immediately clockwise
			Vertices[i]->Adjacent.Add(Vertices[(i == 0) ? N - 1 : i]); // Vertex immediately counter clockwise
		}
	}

	void MoveFace(FVector DeltaLocation)
	{
		for (FVertex* Vertex : Vertices)
		{
			Vertex->Location += DeltaLocation;
		}
	}


	/* Using UV coordinates, returns a location on the face */
	void GetPositionOnFace(float U, float V, FVector& OutLocalOffset)
	{
		OutLocalOffset = Vertices[0]->Location + 
			((Vertices[1]->Location - Vertices[0]->Location) * U) +
			((Vertices[3]->Location - Vertices[0]->Location) * V);
	}

	/*
	* RelativeOffset : Offset of position in units from Vertices[0].
	*	RelativeOffset.X : U direction
	*	RelativeOffset.Y : V direction
	* 
	* Can exceed bounds of face
	*/
	void GetPositionOnFace(FVector2D& RelativeOffset, FVector& OutLocalOffset)
	{
		FVector U_Direction = (Vertices[1]->Location- Vertices[0]->Location);
		U_Direction.Normalize();
		U_Direction *= RelativeOffset.X;
		FVector V_Direction = (Vertices[3]->Location - Vertices[0]->Location);
		V_Direction.Normalize();
		V_Direction *= RelativeOffset.Y;
		OutLocalOffset = U_Direction + V_Direction + Vertices[0]->Location;
	}

	void SetWorldLocation(FVector Location)
	{
		FVector FaceCenter = GetFaceCenter();

		for (FVertex* Vertex : Vertices)
		{
			Vertex->Location -= FaceCenter;
			Vertex->Location += Location;
		}
	}

	/* TEMP: debugging */
	void DrawLabel(AActor* WorldReference, bool bDrawVerts)
	{
		if (Vertices.Num() == 0) return;


		DrawDebugString(
			WorldReference->GetWorld(),
			GetFaceCenter() + WorldReference->GetActorLocation() + Normal * 10.f,
			Label,
			(AActor*)0,
			FColor::White,
			30.f,
			true,
			1.f
		);
		

		if (bDrawVerts)
		{
			for (int i = 0; i < Vertices.Num(); i++)
			{
				DrawDebugString(
					WorldReference->GetWorld(),
					Vertices[i]->Location + WorldReference->GetActorLocation() + Normal * 10.f,
					FString::FromInt(i),
					(AActor*)0,
					FColor::White,
					500.f,
					true,
					1.f
				);
			}
		}
		
	}

	// Enables Usage with TSet
	//inline const uint32 GetID() { return ID; }
	//static uint32 GetTypeHash(const FFace& Face) { return Face.GetID(); }
	//inline bool operator==(const FFace& rhs) { return ID == rhs.ID; }
};







//static int NextShapeID = 0;

/**
 * Holds and exposes methods for manipulating Faces and their vertices.
 * Each shape also has a `Label` string. `Label` is set by a grammar and used
 * by said grammar for replacement.
 * 
 * NOTE: A single shape can share vertices with another
 */
UCLASS()
class FPS_DECKBUILDER_API UShape : public UObject
{
	GENERATED_BODY()


public:
	static UShape* CreateRectangle(FVector Center, FRotator Rotation, FVector Extent);

	// TODO: Should have center settable
	static UShape* CreateCylinder(int NumFaces, int Height);

	// TODO: Complete
	static UShape* CreatePlane(FVector Origin, FVector HypotenuseVector);


	// -- Modifier Methods -- //
	void SetScale(float Scale);

	void SetScale(FVector Scale);

	void MoveFace(FFace& Face, const FVector& DeltaLocation);
 
	/* Face : the face to be subdivided
	*  Axis : will subdivide on the X or the Y or the Z in the local coordinates of the shape 
	*  Percent: How far along the shape the subdivision takes place
	*  NOTE: only subdivides into rectangles right now
	*/
	void SubdivideFace(FFace& Face, EFaceAxis Axis, float Percent, FFace* OutFace1, FFace* OutFace2);

	void ExtrudeFace(FFace& Face, float Distance, FFace* OutFace);

	/* Percent : percent of Face's area outfaces area covers 
	*  NOTE: Destroys the old face data, replacing it with the new faces being created
	*/
	void InsetFace(FFace& Face, float Percent, FFace* OutFace, TArray<FString> Labels);

	FFace* FindFaceByLabel(FString _Label);


public:
	FString Label;

	TArray<FFace*> Faces;
};

