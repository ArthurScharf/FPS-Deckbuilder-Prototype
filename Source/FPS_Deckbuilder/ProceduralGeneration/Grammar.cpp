#include "Grammar.h"
#include "Shape.h"
#include "GameLevel.h"

void UGrammar::Mutate(TMultiMap<FString, UShape*>& Shapes)
{

	// start --> hallway
	Rules.Add(
		"start",
		[&](UShape* Shape)
		{
			FFace* Face = Shape->FindFaceByLabel("forward");
			if (!Face) { return; }

			UE_LOG(LogTemp, Warning, TEXT("UGrammar::Mutate -- start --> hallway"));

			// -- Creating new shape -- //
			UShape* NewShape = NewObject<UShape>(GameLevel);

			// -- Creating new shape and removing face to be modified from the old shape -- //
			NewShape->Faces.Add(Face);
			Shape->Faces.Remove(Face);

			// -- Modifying new shape & Adding it to ShapeMap -- //
			FFace* InsetFace = new FFace();
			NewShape->InsetFace(*Face, 0.5, InsetFace); // Removes `Face` from `NewShape`
			NewShape->MoveFace(*InsetFace, (FVector(0,0,-0.1) + InsetFace->Normal) * 0.5 * GameLevel->GetScale()); // Putting it close to the ground
			NewShape->Label = "hallway";
			Shapes.Add("hallway", NewShape);

			// -- Removing "start" shape from options -- //
			UE_LOG(LogTemp, Warning, TEXT("%d"), Shapes.RemoveSingle("start", Shape));
			Shape->Label = "_";
			Shapes.Add("_", Shape);
		}
	);

	// hallway --> hallway
	Rules.Add(
		"hallway",
		[&](UShape* Shape)
		{
			TArray<FString> Labels = { "forward" }; // , forward "right", "left" 
			FFace* Face = Shape->FindFaceByLabel( Labels[FMath::Rand() % Labels.Num()] );
			if (!Face) { return; }

			UE_LOG(LogTemp, Warning, TEXT("UGrammar::Mutate -- hallway --> hallway"));
			FFace* NewFace = new FFace();
			Shape->ExtrudeFace(*Face, 0.5 * GameLevel->GetScale(), NewFace);
			// NewFace->SetScale(0.9 + (FMath::FRand() * 0.2)); // 0.9, 1.1

			// -- Probability of requiring a turn in the hallway -- //
			if (FMath::FRand() >= 0.7) 
			{
				UE_LOG(LogTemp, Warning, TEXT("%d"), Shapes.RemoveSingle("hallway", Shape));
				Shape->Label = "turn";
				Shapes.Add("turn", Shape);
			}
		}
	);

	Rules.Add(
		"turn",
		[&](UShape* Shape)
		{
			TArray<FString> Labels = { "left", "right"};  
			FFace* Face = Shape->FindFaceByLabel(Labels[FMath::Rand() % Labels.Num()]);
			if (!Face) { return; }

			UE_LOG(LogTemp, Warning, TEXT("UGrammar::Mutate -- hallway --> hallway"));
			FFace* NewFace = new FFace();
			Shape->ExtrudeFace(*Face, 1 * GameLevel->GetScale(), NewFace);
			// NewFace->SetScale(0.9 + (FMath::FRand() * 0.2)); // 0.9, 1.1

			// -- Setting new Label and moving to correct bucket -- //
			UE_LOG(LogTemp, Warning, TEXT("%d"), Shapes.RemoveSingle("turn", Shape));
			Shape->Label = "hallway";
			Shapes.Add("hallway", Shape);
		}
	);







	TArray<FString> Keys;
	Rules.GetKeys(Keys);
	// Keys.Sort([&](const FString& s1, const FString& s2) { FMath::RandBool() ? s1 : s2;}); // Place holder for unique rule probability
	UShape* Shape = nullptr;
	TFunction<void (UShape* Shape)> Rule;
	for (FString Key : Keys)
	{
		// if (Key != "start" && FMath::Rand() % 10 > 4) continue; // Test for randomizing which features are chosen.

		UE_LOG(LogTemp, Warning, TEXT("UGrammar::Mutate -- Key: %s"), *Key);
		if (!Shapes.Contains(Key)) { continue; }

		UE_LOG(LogTemp, Warning, TEXT("UGrammar::Mutate -- FOUND"));
		Shape = *Shapes.Find(Key);
		Rule = *Rules.Find(Key);
		Rule(Shape);
		break;
	}
	return;
}