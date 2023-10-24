#pragma once

#include "CoreMinimal.h"
#include "Grammar.h"
#include "Grammar_Slums.generated.h"



class UShape;



// -- Testing Rules -- // 
/*

	// start --> hallway
	ShapeRules.Add(
		"start",
		[&](UShape* Shape)
		{
			FFace* Face = Shape->FindFaceByLabel(FString("forward"));
			if (!Face) { return; }

			UE_LOG(LogTemp, Warning, TEXT("UGrammar::Mutate -- start --> hallway"));

			// -- Creating new shape -- //
			UShape* NewShape = NewObject<UShape>(GameLevel);

			// -- Creating new face and removing face to be modified from the old shape -- //
			NewShape->Faces.Add(Face);
			Shape->Faces.Remove(Face);

			// -- Modifying new shape & Adding it to ShapeMap -- //
			FFace* InsetFace = new FFace();
			NewShape->InsetFace(*Face, 0.5, InsetFace); // Removes `Face` from `NewShape`
			NewShape->MoveFace(*InsetFace, (FVector(0, 0, -0.1) + InsetFace->Normal) * 0.5 * GameLevel->GetScale()); // Putting it close to the ground
			// MigrateShape("hallway", NewShape, Shapes);
			Shapes.Add("hallway", NewShape);

			// -- Removing "start" shape from options -- //
			MigrateShape("_", Shape, Shapes);
		}
	);

	// turn --> hallway { choose face({"left", "right"}) --> Extrude }
	ShapeRules.Add(
		"turn",
		[&](UShape* Shape)
		{
			TArray<FString> Labels = { FString("left"), FString("right") };
			FFace* Face = Shape->FindFaceByLabel(Labels[FMath::Rand() % Labels.Num()]);
			if (!Face) { return; }
			UE_LOG(LogTemp, Warning, TEXT("UGrammar::Mutate -- hallway --> hallway"));
			FFace* NewFace = new FFace();
			if (FMath::FRand() > 0.5)
			{
				Shape->InsetFace(*Face, 0.6, NewFace);
				Shape->ExtrudeFace(*NewFace, 0.5 * GameLevel->GetScale(), new FFace());
			}
			else
			{
				Shape->ExtrudeFace(*Face, 1 * GameLevel->GetScale(), NewFace);
			}
			// -- Setting new Label and moving to correct bucket -- //
			MigrateShape("hallway", Shape, Shapes);
		}
	);

	// hallway|obstacle --> hallway
	ShapeRules.Add(
		"hallway|obstacle",
		[&](UShape* Shape)
		{
			FFace* Face = Shape->FindFaceByLabel(FString("floor"));
			if (!Face) { UE_LOG(LogTemp, Error, TEXT("UGrammar::Mutate -- hallway|obstacle : FAILURE")); return; }
			UE_LOG(LogTemp, Warning, TEXT("UGrammar::Mutate -- hallway|obstacle --> _"));

			FVector2D Dimensions = Face->GetDimensions();

			// -- Choosing the a random position for the box in the hallway -- //
			FVector2D LocationOnFace = FVector2D(
				FMath::FRandRange(85.f, Dimensions.X - 85.f),
				FMath::FRandRange(85.f, Dimensions.Y - 85.f)
			);
			FVector FeatureLocation;
			Face->GetPositionOnFace(LocationOnFace, FeatureLocation);
			FeatureLocation += GameLevel->GetActorLocation() + FVector(0, 0, 80);

			// --  Alternate Approach -- //
			//FVector FeatureLocation;
			//Face->GetPositionOnFace(0.25, 0.25, FeatureLocation);
			//FeatureLocation += GameLevel->GetActorLocation();

			DrawDebugBox(GameLevel->GetWorld(), FeatureLocation, FVector(80, 80, 80), FColor::Orange, true);

			MigrateShape(FString("hallway"), Shape, Shapes);
		}
	);


	// hallway --> hallway
	ShapeRules.Add(
		"hallway",
		[&](UShape* Shape)
		{
			FFace* Face = Shape->FindFaceByLabel(FString("forward"));
			if (!Face) { return; }
			UE_LOG(LogTemp, Warning, TEXT("UGrammar::Mutate -- hallway --> hallway"));

			// -- Creating new shape -- //
			UShape* NewShape = NewObject<UShape>(GameLevel);

			// -- Creating new face and removing face to be modified from the old shape -- //
			NewShape->Faces.Add(Face);
			Shape->Faces.Remove(Face);

			// -- Extruding face into hallway segment and adding it to `Shapes` -- //
			FFace* NewFace = new FFace();
			NewShape->ExtrudeFace(*Face, 0.5 * GameLevel->GetScale(), NewFace);

			// -- Setting label for NewShape and adding it to the ShapesMap -- //
			float Choice = FMath::FRand();
			if (Choice >= 0.7)
			{
				NewShape->Label = FString("hallway|obstacle");
			}
			else if (Choice >= 0.55)
			{
				NewShape->Label = FString("turn");
			}
			else
			{
				NewShape->Label = FString("hallway");
			}
			Shapes.Add(NewShape->Label, NewShape);
		}
	);



*/












/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class FPS_DECKBUILDER_API UGrammar_Slums : public UGrammar
{
	GENERATED_BODY()

public:
	void Init(TMultiMap<FString, UShape*>& Shapes) override;


private:
	TArray<UShape*> Rooms;
};
