#include "Grammar_Slums.h"

#include "Algo/Reverse.h" 
#include "FPS_Deckbuilder/Character/EnemyCharacter.h"
#include "FPS_Deckbuilder/ProceduralGeneration/GameLevel.h"
#include "FPS_Deckbuilder/ProceduralGeneration/Shape.h"
#include "NavMesh/NavMeshBoundsVolume.h"




/*
* Only map faces to their material label once they're finished with mutation
 */





void UGrammar_Slums::Init(TMultiMap<FString, UShape*>& Shapes)
{
	UE_LOG(LogTemp, Warning, TEXT("UGrammar_Slums::Init()"));

	// -- Initializing Meta Data -- //
	FenceHeight = FVector(0, 0, 400);


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
				NewFace->Label = "base_plate";
				FacesToMaterialMap.Add(NewFace);


				// -- Creating new shape & Adding NewFace to NewShape -- //
				NewShape = NewObject<UShape>(GameLevel);
				NewShape->Faces.Add(NewFace);
				NewShape->Label = "base_plate_1"; // So we can use a rule on it
				MigrateShape("base_plate_1", NewShape, Shapes);
			}

			MigrateShape("cylinder", Shape, Shapes);
			MapFacesToMaterialLabel("ground", FacesToMaterialMap); // TODO: Change to appropriate material
		}
	);


	/*
	* Modifies building to have a door and (soon to come) windows
	*/
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
		"fence_gate",
		[&](UShape* GateShape)
		{
			FFace* GateFace = GateShape->Faces[0];

			// -- Finding the a random position on the face where the gate should be -- //
			FVector2D Dimensions = GateFace->GetDimensions();
			FVector GateWorldLocation;
			FVector2D FacePosition((Dimensions.X/2.f) + 50.f, FMath::RandRange(500.f, Dimensions.Y - 500));
			GateFace->GetPositionOnFace( FacePosition, GateWorldLocation );

			// -- Constructing new face -- //
			FFace* NewFace = new FFace();
			GateShape->InsetFace(*GateFace, 1, NewFace, { "wall", "wall", "delete", "wall" }); // We're making a door. Don't need bottom face
			GateShape->Faces.Remove(GateShape->FindFaceByLabel("delete")); // Deleting Bottom Face
			NewFace->SetDimensions(FVector2D(200, 300));
			NewFace->SetWorldLocation(GateWorldLocation);
			GateShape->Faces.Remove(NewFace);	// Cutting door face out to make room for door actor

			// -- Migrating -- //
			MigrateShape("", GateShape, Shapes);
			MapFacesToMaterialLabel("fence", GateShape->Faces);
		}
	);




	/*
	* Makes building and fence shapes
	*/
	ShapeRules.Add(
		"base_plate_1",
		[&](UShape* BasePlateShape)
		{
			FFace* Face = BasePlateShape->FindFaceByLabel("base_plate");
			if (!Face)
			{
				UE_LOG(LogTemp, Error, TEXT("UGrammar_Slums::Init -- RULE: encounter --  !Face"));
				return;
			}
			Face->Label = ""; // Don't want to use this face more than once 


// -- Creating Fence -- //
			UShape* FenceShape; // Reused later
			FFace* FenceFace; // Reused later

			// - Creating FenceShape - //
			FenceShape = NewObject<UShape>(GameLevel);
			FenceFace = new FFace();
			FenceShape->Faces.Add(FenceFace);

			// - Finding start and end positions of FenceFace - //
			FVector FenceOrigin;
			FVector FenceTermination;
			BasePlateShape->Faces[0]->GetPositionOnFace(0, 0.5, FenceOrigin);
			BasePlateShape->Faces[0]->GetPositionOnFace(1, 0.5, FenceTermination);
			
			
			// - Initializing FenceFace - //
			// FVector FenceHeight(0, 0, 500);
			FenceFace->Vertices.Append(
				{
					new FVertex(FenceOrigin + FenceHeight), // 300 is fence height
					new FVertex(FenceOrigin),
					new FVertex(FenceTermination),
					new FVertex(FenceTermination + FenceHeight)
				}
			);
			FenceFace->InitNormal();
			MigrateShape("", FenceShape, Shapes); // Never mutated
			MapFacesToMaterialLabel("fence", FenceShape->Faces);
			// FenceFace->SetAdjacency(); TODO: Implement

		
			// - Creating additional fence faces - //
			int NumSubdivisions = 2 + FMath::Rand() % 2; // [2,4] obstacles
			float Percent;
			for (int i = 0; i < NumSubdivisions; i++)
			{
				FenceFace = new FFace();
				Percent = (i + 1) / (float)NumSubdivisions;
				BasePlateShape->Faces[0]->GetPositionOnFace(Percent, 0.5, FenceOrigin);
				BasePlateShape->Faces[0]->GetPositionOnFace(Percent, 0, FenceTermination);
				FenceFace->Vertices.Append
				(
					{
						new FVertex(FenceOrigin + FenceHeight),
						new FVertex(FenceOrigin),
						new FVertex(FenceTermination),
						new FVertex(FenceTermination + FenceHeight)
					}
				);
				FenceFace->InitNormal();
				FenceShape = NewObject<UShape>(GameLevel);
				FenceShape->Label = "fence_gate";
				FenceShape->Faces.Add(FenceFace);
				MigrateShape("fence_gate", FenceShape, Shapes);
			}
			


// -- Creating Building -- //		
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

			
			// - Constructing Room BasePlateShape - //
			UShape* BuildingShape = UShape::CreateRectangle(
				Location + FVector(0,0,1), 
				Rotation, 
				Extent 
			);
			FFace* FloorFace = BuildingShape->FindFaceByLabel("floor");
			Algo::Reverse(FloorFace->Vertices); 


			// - Migrating Shapes - //
			MigrateShape("building", BuildingShape, Shapes);
			MigrateShape("", BasePlateShape, Shapes);


			// - Material Mapping Faces - //
			TMap<FString, TArray<FFace*>> FacesToMaterialMap;
			for (FFace* f : BuildingShape->Faces)
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


