// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FPS_Deckbuilder/Card/StackObject.h"
#include "FPS_Deckbuilder/UI/TrayStackWidget.h"

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

	/* Set's the widget for this stack & initializes that widget using it's update function
	* This method should only be called once per a player life
	*/
	void SetWidget(UTrayStackWidget* _Widget);


	/* Set's SelectedCard (if possible). Updates the Stack's Widget */
	void ResetTrayStack();


	// -- Stack Modification -- //
	/* 
	*  There can sometimes be cases where multiple open slots exist. This method is more complicated than remove for this reason
	*/
	UFUNCTION(BlueprintCallable)
	bool SetCardInSlot(UStackSlot* Slot, int SlotChildIndex, UCard* Card);

	/* This method doesn't appropriately check for this but this method should only ever be called on the leading non-empty slot */
	UFUNCTION(BlueprintCallable)
	bool RemoveCard(UCard* Card);

	/* Finds the index of the outer most Slot, containing the passed card, in the stack*/
	UFUNCTION(BlueprintCallable)
	int Find(UCard* Card);

private:
	/* The card being seen by the player */
	UCard* SelectedCard;
	
	// The outer most Stack Object is ALWAYS a slot
	UPROPERTY(VisibleAnywhere)
	TArray <UStackSlot*> BackingArray;

	int ActiveSlotIndex;

	UTrayStackWidget* Widget;

public:
	FORCEINLINE const TArray<UStackSlot*> GetBackingArray() { return BackingArray; }

	/* returns the length in elements of the backing array */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int Num() { return BackingArray.Num(); }
};
