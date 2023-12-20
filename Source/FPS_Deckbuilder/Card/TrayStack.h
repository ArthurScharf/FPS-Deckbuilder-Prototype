// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FPS_Deckbuilder/Card/StackObject.h"

#include "UObject/NoExportTypes.h"
#include "TrayStack.generated.h"


class UCard;
class UStackSlot;



/**
 * 
 */
UCLASS(BlueprintType)
class FPS_DECKBUILDER_API UTrayStack : public UObject
{
	GENERATED_BODY()

public:
	UTrayStack();

	/* Used by the view to set the value of the child in a slot to the passed card.
	*  Also updates stack properties like maintaining the existence of an empty leading slot in most cases
	*/
	bool SetCard(UStackSlot* Slot, int Index, UCard* Card);



private:
	TArray<IStackObject*> BackingArray;
	
};
