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

	// -- Stack Use -- //
	/*
	* Sets Card.
	* Moves to the next outer slot in the backing array and sets the active card this way.
	* Let's the card being rotated out have a chance to do gameplay things.
	* Let's the card being rotated in have a chance to do gameplay things.
	* Handles Stack functionality like setting the stacks Warmup timer or cooldown timer.
	*/
	UCard* Rotate();

	/* Uses the Card currently shown to the player then rotates the stack */
	void UseSelectedCard(); 


	// -- Stack Modification -- //
	/* 
	*  Used by the view to set the value of the child in a slot to the passed card.
	*  Also updates stack properties like maintaining the existence of an empty leading slot in most cases
	*/
	UFUNCTION(BlueprintCallable)
	bool SetCardInSlot(UStackSlot* Slot, int SlotChildIndex, UCard* Card);

	

private:
	/* The card being seen by the player */
	UCard* SelectedCard;
	
	// The outer most Stack Object is ALWAYS a slot
	UPROPERTY(VisibleAnywhere)
	TArray <UStackSlot*> BackingArray;

	int ActiveSlotIndex;

public:
	FORCEINLINE const TArray<UStackSlot*> GetBackingArray() { return BackingArray; }
};
