#include "Grammar_Slums.h"

#include "FPS_Deckbuilder/ProceduralGeneration/GameLevel.h"
#include "FPS_Deckbuilder/ProceduralGeneration/Shape.h"

void UGrammar_Slums::Init(TMultiMap<FString, UShape*>& Shapes)
{
	UE_LOG(LogTemp, Warning, TEXT("UGrammar_Slums::Init()"));


	ShapeRules.Add(
		"start",
		[&](UShape* Shape) 
		{
			Shape->SetScale(50000); // Making the cylinder very large

			FFace* Face;
			UShape* NewShape;

			/* Finding cylinder wall faces and constructing a new shape as a platform extending from the wall of the cylinder */
			int Num = 6;
			for (int i = 0; i < Num; i++)
			{
				// -- Finding Cylinder wall face, and changing it's label so it isn't used more than once -- //
				Face = Shape->FindFaceByLabel("cylinder_wall");
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



				// -- Creating new Face -- //
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


				// -- Creating new shape & Adding NewFace to NewShape -- //
				NewShape = NewObject<UShape>(GameLevel);
				NewShape->Faces.Add(NewFace);
				NewShape->Label = "encounter"; // So we can use a rule on it
				MigrateShape("encounter", NewShape, Shapes);
				

				// -- Modifying Grammar specific meta data -- //
				Rooms.Add(NewShape);
			}

			MigrateShape("cylinder", Shape, Shapes);
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

			// -- Constructing a room -- //

			// - Choosing Room Size - // 
			FVector2D Dimensions = Face->GetDimensions();
			float ExtentX = Dimensions.X * 0.05 + FMath::FRand() * 50;
			float ExtentY = Dimensions.Y * 0.05 + FMath::FRand() * 50;
			FVector Extent(ExtentX, ExtentY, 500);

			float PositionX = (FMath::RandBool() ? ExtentX : Dimensions.X - ExtentX);
			float PositionY = (FMath::RandBool() ? ExtentY : Dimensions.Y - ExtentY);

			// - Finding Center position for room - //
			FVector FaceLocation; // Remember that these are local on the face
			FVector2D RelativePosition(PositionX, PositionY);
			Face->GetPositionOnFace(RelativePosition, FaceLocation);
			FaceLocation.Z += 500.f;

			DrawDebugBox(GameLevel->GetWorld(), FaceLocation, Extent, FColor::Red, true);

			UShape* NewShape = UShape::CreateRectangle(FaceLocation, FVector(500000));
			// NewShape->SetScale(10000);
			MigrateShape("room", NewShape, Shapes);
		}
	);
}