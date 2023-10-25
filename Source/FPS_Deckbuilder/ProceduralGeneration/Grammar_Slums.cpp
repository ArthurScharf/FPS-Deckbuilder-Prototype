#include "Grammar_Slums.h"

#include "FPS_Deckbuilder/ProceduralGeneration/GameLevel.h"
#include "FPS_Deckbuilder/ProceduralGeneration/Shape.h"
#include "NavMesh/NavMeshBoundsVolume.h"

#include "FPS_Deckbuilder/Character/EnemyCharacter.h"

void UGrammar_Slums::Init(TMultiMap<FString, UShape*>& Shapes)
{
	UE_LOG(LogTemp, Warning, TEXT("UGrammar_Slums::Init()"));


	/*
	* Creates a cylinder and generates platforms which are used to generate encounters
	*/
	ShapeRules.Add(
		"start",
		[&](UShape* Shape) 
		{
			// NOTE: This is a code smell. Shouldn't be doing this here
			MapFacesToMaterialLabel("cylinder", Shape->Faces);
			
			Shape->SetScale(50000); // Making the cylinder very large

			FFace* Face;
			UShape* NewShape;
			TArray<FFace*> FacesToMaterialMap;

			/* Finding cylinder wall faces and constructing a new shape as a platform extending from the wall of the cylinder */
			int Num = 5;
			for (int i = 0; i < Num; i++)
			{
				// -- Finding Cylinder wall face, and changing it's label so it isn't used more than once -- //
				Face = Shape->FindFaceByLabel("cylinder");
				if (!Face)
				{
					UE_LOG(LogTemp, Error, TEXT("UGrammar_Slums::Init -- RULE: start -- !Face"));
					return;
				}
				Face->Label = "";


				// -- Constructing platform face based -- //
				FVector2D Dimensions = Face->GetDimensions();
				FVector FaceLocation;
				Face->GetPositionOnFace(.25 + (i * .01), .5, FaceLocation);

				// Need the tangent to find vertex locations on CylinderFace
				FVector Tangent = FRotator(0, -90, 0).RotateVector(Face->Normal);
				Tangent *= Dimensions.Y * 0.4;


				// -- Creating base -- //
				FFace* NewFace = new FFace();
				NewFace->Vertices.Append(
					{
						new FVertex(FaceLocation + Tangent),
						new FVertex((FaceLocation - (Face->Normal * Dimensions.Y * 0.8)) + Tangent),
						new FVertex((FaceLocation - (Face->Normal * Dimensions.Y * 0.8)) - Tangent),
						new FVertex(FaceLocation - Tangent)
					}
				);
				NewFace->Normal = FVector(0, 0, 1);
				NewFace->SetAdjacency();
				NewFace->Label = "base";
				FacesToMaterialMap.Add(NewFace);


				// -- Creating new shape & Adding NewFace to NewShape -- //
				NewShape = NewObject<UShape>(GameLevel);
				NewShape->Faces.Add(NewFace);
				NewShape->Label = "encounter"; // So we can use a rule on it
				MigrateShape("encounter", NewShape, Shapes);
				

				// -- Modifying Grammar specific meta data -- //
				Rooms.Add(NewShape);
			}

			MigrateShape("cylinder", Shape, Shapes);
			MapFacesToMaterialLabel("floor", FacesToMaterialMap); // TODO: Change to appropriate material
		}
	);


	ShapeRules.Add(
		"building",
		[&](UShape* Shape)
		{
			// -- Creating door -- //
			FFace* Face = Shape->FindFaceByLabel("forward");
			if (!Face)
			{
				UE_LOG(LogTemp, Error, TEXT("UGrammar_Slums::Init -- RULE: room --  !Face"));
				return;
			}
			Face->Label = ""; // Don't want to use this face more than once 

			// - Creating door - //
			FVector2D Dimensions = Face->GetDimensions();
			FFace* NewFace = new FFace();
			Shape->InsetFace(*Face, 1, NewFace, {"wall", "delete", "wall", "wall"}); // We're making a door. Don't need bottom face
			Shape->Faces.Remove(Shape->FindFaceByLabel("delete")); // Deleting Bottom Face
			NewFace->SetDimensions(FVector2D(300, 400));
			NewFace->MoveFace(FVector(
				0, 
				0, 
				-(Dimensions.Y/2.f) + 150.f
			));
			Shape->Faces.Remove(NewFace);	// Cutting door face out to make room for door actor
			
			// - Mapping faces to material labels - //
			TArray<FFace*> WallFaces = Shape->Faces.FilterByPredicate(
				[&](const FFace* f) { return f->Label == "wall"; }
			);
			MapFacesToMaterialLabel("wall", WallFaces);

			// - Migrating Shape - //
			MigrateShape("", Shape, Shapes); // Room is completed
		}
	);


	ShapeRules.Add(
		"encounter",
		[&](UShape* Shape)
		{
			FFace* Face = Shape->FindFaceByLabel("base");
			if (!Face)
			{
				UE_LOG(LogTemp, Error, TEXT("UGrammar_Slums::Init -- RULE: encounter --  !Face"));
				return;
			}
			Face->Label = ""; // Don't want to use this face more than once 


			// - Choosing Room Size - // 
			FVector2D Dimensions = Face->GetDimensions();
			float ExtentX = Dimensions.X * 0.2 + FMath::FRand() * 50;
			float ExtentY = Dimensions.Y * 0.2 + FMath::FRand() * 50;
			FVector Extent(ExtentX, ExtentY, 500);

			bool XLessThan0 = FMath::RandBool(); // Stored for use to calculate random rotation

			float PositionX = (XLessThan0 ? ExtentX + 500 : Dimensions.X - (ExtentX + 500));
			float PositionY = (FMath::RandBool() ? ExtentY + 500 : Dimensions.Y - (ExtentY + 500));

			// - Finding Center position for room - //
			FVector Location; // Remember that these are local on the face
			FVector2D RelativePosition(PositionX, PositionY);
			Face->GetPositionOnFace(RelativePosition, Location);
			Location.Z += 500.f; // 50 more than height so the room is slightly elevated 

			// - Finding Rotation including choosing randoming facing for door - //
			FRotator Rotation = (Face->Vertices[1]->Location - Face->Vertices[0]->Location).Rotation();
			Rotation.Yaw += (XLessThan0 ? -90.f : 90.f); 

			// - Constructing Room Shape - //
			UShape* NewShape = UShape::CreateRectangle(
				Location + FVector(0,0,1), 
				Rotation, 
				Extent 
			);

			// - Migrating Shapes - //
			MigrateShape("building", NewShape, Shapes);
			MigrateShape("", Shape, Shapes);


			// - Material Mapping Faces - //
			TMap<FString, TArray<FFace*>> FacesToMaterialMap;
			for (FFace* f : NewShape->Faces)
			{
				// I wish this was a switch statement
				if (f->Label == "left" || f->Label == "right" || f->Label == "backward")
				{
					// - Adding face to appropriate mapping - //
					if (!FacesToMaterialMap.Contains("wall")) { FacesToMaterialMap.Add("wall", { f }); }
					else { FacesToMaterialMap["wall"].Add(f); }
				}
				if (f->Label == "floor")
				{
					if (!FacesToMaterialMap.Contains("floor")) { FacesToMaterialMap.Add("floor", { f }); }
					else { FacesToMaterialMap["floor"].Add(f); }
				}
				else if (f->Label == "ceiling")
				{
					if (!FacesToMaterialMap.Contains("ceiling")) { FacesToMaterialMap.Add("ceiling", { f }); }
					else { FacesToMaterialMap["ceiling"].Add(f); }
				}
			}
			TArray<FString> Labels;
			FacesToMaterialMap.GetKeys(Labels);
			for (FString Label : Labels)
			{
				MapFacesToMaterialLabel(Label, FacesToMaterialMap[Label]);
			}


			// - Testing Spawning Enemies - //
			Location += FVector(0, 0, 300);
			FActorSpawnParameters SpawnParams;
			FTransform Transform;
			Transform.SetLocation(Location);
			Transform.SetRotation(FQuat(Rotation));
			GameLevel->GetWorld()->SpawnActor<AEnemyCharacter>(Actors["TestEnemy"], Transform, SpawnParams);
		}
	);
}