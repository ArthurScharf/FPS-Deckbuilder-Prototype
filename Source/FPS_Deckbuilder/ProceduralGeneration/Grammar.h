// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Grammar.generated.h"


class AGameLevel;
class UShape;
struct FFace;
struct FNode;


/**
 * Efficiently query graph structure for matching patterns and 
 * replace those patterns with other patterns
 * 
 * NOTE: This entire process is too complicated. Since I'm subclassing to create each level type anyway,
 * I might as well simply write the process of generation using member functions and randomized data structures
 * like queues to achieve the same result. I'm leaving it this way for now, bit the current implementation
 * is needlessly opaque
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

	/*
	* Each faces must be associated with a material (by a label) for it to become real geometry.
	* This associates faces with materials in this way
	*/
	void MapFacesToMaterialLabel(FString Label, TArray<FFace*> Faces);

public:
	AGameLevel* GameLevel; // Needed to set the outer on new shapes being created

protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSubclassOf<AActor>> Actors;

	TMap<FString, TFunction<void (UShape* Shape)>> ShapeRules;

	/*
	* Once set of faces are no longer required for mutation, they are removed from Shapes
	* and added to this map, often breaking down a shape in Shapes and constructing to or adding to a shape in here.
	* Each Shape in this data structure should contain ALL faces that will receive the material associated with it's label.
	* This may mean a shape has many islands of faces that are disconnected from one another
	*/
	TMap<FString, UShape*> MaterialShapeMap;

public:
	FORCEINLINE TMap<FString, UShape*> GetMaterialShapeMap() { return MaterialShapeMap; }
};
