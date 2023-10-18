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

			TArray<FFace*> RoomFaces;
			FFace* Face;
			UShape* NewShape;
			int Num = Shape->Faces.Num(); // Changes during loop so we must save this before
			for (int i = 0; i < Num; i++)
			{
				Face = Shape->FindFaceByLabel("cylinder_wall");

				if (!Face) 
				{
					UE_LOG(LogTemp, Error, TEXT("UGrammar_Slums::Init -- RULE: start -- !Face"));
					return;
				}

				// -- Creating new shape -- //
				NewShape = NewObject<UShape>(GameLevel);
				NewShape->Label = "room"; // So we can use a rule on it
				Shapes.Add("room", NewShape);

				// -- Migrating Face to new shape -- //
				NewShape->Faces.Add(Face);
				Shape->Faces.Remove(Face);

				// -- Insetting and creating face for room to begin from -- //
				FFace* InsetFace = new FFace(); // Destructive, so no need to set Face->Label to ""
				NewShape->InsetFace(*Face, 0.5, InsetFace);
				InsetFace->Label = "start"; // first face for room
				InsetFace->SetScale(FVector(1, 1, 0.1));
				NewShape->MoveFace(*InsetFace, FVector(0, 0, 30000));

				// -- Modifying Grammar specific meta data -- //
				Rooms.Add(NewShape);
			}

			MigrateShape("cylinder", Shape, Shapes);
		}
	);


	/// <summary>
	/// Generates a room on the side of the cylinder 
	/// </summary>
	/// <param name="Shapes"></param>
	ShapeRules.Add(
		"room",
		[&](UShape* Shape)
		{
			FFace* Face = Shape->FindFaceByLabel("start");

			if (!Face)
			{
				UE_LOG(LogTemp, Error, TEXT("UGrammar_Slums::Init -- RULE: room --  !Face"));
				return;
			}

			Shape->ExtrudeFace(*Face, 10000, new FFace());
		}
	);
}