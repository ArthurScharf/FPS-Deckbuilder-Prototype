#include "Grammar.h"
#include "Shape.h"
#include "GameLevel.h"

void UGrammar::Mutate(TMultiMap<FString, UShape*>& Shapes)
{
	Rules.Add(
		"start",
		[&](UShape* Shape)
		{
			FFace* Face = Shape->FindFaceByLabel("forward");
			if (!Face) { return; }

			UE_LOG(LogTemp, Warning, TEXT("UGrammar::Mutate -- start --> hallway"));

			// -- Creating new shape -- //
			UShape* NewShape = NewObject<UShape>(GameLevel);
			NewShape->Faces.Add(Face);
			NewShape->Label = "hallway";
			NewShape->ExtrudeFace(*Face, 1 * GameLevel->GetScale(), new FFace());
			Shape->Faces.Remove(Face);
			Shapes.Add("hallway", NewShape);
			
			// -- Removing "start" shape from options -- //
			UE_LOG(LogTemp, Warning, TEXT("%d"), Shapes.RemoveSingle("start", Shape));
			Shape->Label = "NULL";
			Shapes.Add("NULL", Shape);
		}
	);

	Rules.Add(
		"hallway",
		[&](UShape* Shape)
		{
			FFace* Face = Shape->FindFaceByLabel("forward");
			if (!Face) { return; }

			UE_LOG(LogTemp, Warning, TEXT("UGrammar::Mutate -- hallway --> hallway"));
			FFace* NewFace = new FFace();
			Shape->ExtrudeFace(*Face, 1 * GameLevel->GetScale(), NewFace);
			NewFace->SetScale(0.9 + (FMath::FRand() * 0.2)); // 0.9, 1.1

			// -- Setting new Label and moving to correct bucket -- //
			//UE_LOG(LogTemp, Warning, TEXT("%d"), Shapes.RemoveSingle("hallway", Shape));
			//Shape->Label = "hallway";
			//Shapes.Add("hallway", Shape);
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