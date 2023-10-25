#include "Grammar.h"
#include "GameLevel.h"
#include "Shape.h"

void UGrammar::Init(TMultiMap<FString, UShape*>& Shapes)
{
	UE_LOG(LogTemp, Warning, TEXT("UGrammar::Init"));
}



void UGrammar::Mutate(TMultiMap<FString, UShape*>& Shapes)
{
	UE_LOG(LogTemp, Warning, TEXT("UGrammar::Mutate"));

	TArray<FString> Keys;
	ShapeRules.GetKeys(Keys);
	UShape* Shape = nullptr;
	TFunction<void (UShape* Shape)> Rule;
	for (FString Key : Keys)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGrammar::Mutate -- Key: %s"), *Key);
		if (!Shapes.Contains(Key)) { continue; }

		UE_LOG(LogTemp, Warning, TEXT("UGrammar::Mutate -- FOUND"));
		Shape = *Shapes.Find(Key);
		Rule = *ShapeRules.Find(Key);
		Rule(Shape);
		break;
	}
	return;
}


void UGrammar::MigrateShape(FString NewLabel, UShape* Shape, TMultiMap<FString, UShape*>& Shapes)
{
	Shapes.Remove(Shape->Label, Shape);
	Shape->Label = NewLabel;
	Shapes.Add(NewLabel, Shape);
}


void UGrammar::MapFacesToMaterialLabel(FString Label, TArray<FFace*> Faces)
{
	UShape* Shape; 
	if (!MaterialShapeMap.Contains(Label)) 
	{ 
		Shape = NewObject<UShape>(GameLevel);
		Shape->Faces.Append(Faces);
		MaterialShapeMap.Add(Label, Shape);
		return;
	}

	MaterialShapeMap[Label]->Faces.Append(Faces);
}