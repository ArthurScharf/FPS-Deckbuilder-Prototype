// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Grammar.generated.h"


class AGameLevel;
class UShape;
struct FNode;


/**
 * Efficiently query graph structure for matching patterns.
 * Replace those patterns with other patterns
 * NOTE: Certains labels are always potentially findable. These are
 * Shape labels: {start, }
 * Face labels : {floor, ceiling, forward, backward, left, right}
 */
UCLASS()
class FPS_DECKBUILDER_API UGrammar : public UObject
{
	GENERATED_BODY()

public:
	virtual void Init(TMultiMap<FString, UShape*>& Shapes);

	void Mutate(TMultiMap<FString, UShape*>& Shapes);

protected:
	/* Helper method for avoiding programming mistakes */
	void MigrateShape(FString NewLabel, UShape* Shape, TMultiMap<FString, UShape*>& Shapes);

public:
	AGameLevel* GameLevel; // Needed to set the outer on new shapes being created

protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSubclassOf<AActor>> Actors;


	TMap<FString, TFunction<void (UShape* Shape)>> ShapeRules;
};
