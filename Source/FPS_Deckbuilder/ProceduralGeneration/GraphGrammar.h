// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphGrammar.generated.h"


class UGeomNode;

/**
 * 
 */
UCLASS(Abstract)
class FPS_DECKBUILDER_API UGraphGrammar : public UObject
{
	GENERATED_BODY()


public:
	/* For adding lambda rules for transforming the graph */
	virtual void Init() PURE_VIRTUAL(UGraphGrammar::Init, );

	void Mutate();

protected:
	/* 1. Removes Node from bucket hash of label if node is there
	*  2. Addes the Node to the correct bucket using it's new label as the hash */
	void PutNode(FString NewLabel, UGeomNode* Node);




protected:
	/* The number of engine units equally one unit in room space. 
	   Rules are written with sizes in terms of room space.
	   The true values*/
	float Scale;

	/* Used for two things
	*  1. Finding the dimensions of the geometry being generated
	*  2. Constructing the matrix used for constructing the geometry.
	*     All node positions will be shifted such that they rastorize onto a grid
	*     starting at (0,0,0). ie we're eliminating negative indices
	*/
	FVector LeastBounds;
	FVector GreatestBounds;


	/* Rules for transforming a node with a given label */
	TMap<FString, TFunction<void(UGeomNode* Node)>> Rules;

	TMultiMap<FString, UGeomNode*> Nodes;

	/* Stored to be returned when `GetGraph` is called*/
	UGeomNode* Head;

public:
	FORCEINLINE UGeomNode* GetGraph() { return Head; }

	FORCEINLINE void SetScale(float _Scale) { Scale = _Scale; }

	FORCEINLINE FVector GetLeastBounds() { return LeastBounds; }
	FORCEINLINE FVector GetGreatestBounds() { return GreatestBounds; }
};
