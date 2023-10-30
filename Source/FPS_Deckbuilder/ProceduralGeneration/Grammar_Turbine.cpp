// Fill out your copyright notice in the Description page of Project Settings.


#include "Grammar_Turbine.h"

#include "FPS_Deckbuilder/ProceduralGeneration/GameLevel.h"
#include "FPS_Deckbuilder/ProceduralGeneration/Shape.h"








void UGrammar_Turbine::Init(TMultiMap<FString, UShape*>& Shapes)
{
	//float Scale = 100; // We're working in meters

	// -- Creating the starting plate -- //
	UShape* StartingShape = NewObject<UShape>(GameLevel);
	FFace* StartingFace = new FFace();
	FVertex* sv1 = new FVertex(FVector(-25, -25, 0));
	FVertex* sv2 = new FVertex(FVector(-25, 25, 0));
	FVertex* sv3 = new FVertex(FVector( 25,  25, 0));
	FVertex* sv4 = new FVertex(FVector( 25, -25, 0));
	StartingFace->Vertices.Append({ sv1, sv2, sv3, sv4 });
	StartingFace->Normal = FVector(0, 0, 1); //InitNormal isn't working properly
	StartingShape->Faces.Add(StartingFace);
	MigrateShape("platform", StartingShape, Shapes);
	MapFacesToMaterialLabel("foor", { StartingFace });





	// Creates the start platform, the arena platforms, and the walkways between them
	ShapeRules.Add(
		"platform",
		[&](UShape* PlatformShape)
		{
			UShape* WalkwayShape = NewObject<UShape>(GameLevel);
			FFace* WalkwayFace = new FFace();
			WalkwayShape->Faces.Add(WalkwayFace);

			float CenterToEdge = (PlatformShape->Faces[0]->GetDimensions().X) / 2.f;
			WalkwayFace->Vertices.Append(
				{
					new FVertex(FVector(CenterToEdge + 50, -10, 0)),
					new FVertex(FVector(CenterToEdge,      -10, 0)),
					new FVertex(FVector(CenterToEdge,       10, 0)),
					new FVertex(FVector(CenterToEdge + 50,  10, 0)),
				}
			);
			WalkwayFace->Normal = FVector(0, 0, 1);
			WalkwayFace->SetAdjacency();

			MapFacesToMaterialLabel("floor", { WalkwayFace });
			MigrateShape("walkway", WalkwayShape, Shapes);
			MigrateShape("", PlatformShape, Shapes);
		}
	);

}