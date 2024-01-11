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
	* 
	*  TODO: This method should only create a new leading slot IFF the slot being placed into is the current leading slot.
	*/
	UFUNCTION(BlueprintCallable)
	bool SetCardInSlot(UStackSlot* Slot, int SlotChildIndex, UCard* Card);

	/* This method doesn't appropriately check for this but this method should only ever be called on the leading non-empty slot */
	UFUNCTION(BlueprintCallable)
	bool RemoveCard(UCard* Card);


	/* Finds the index of the outer most Slot containing the passed card contained in the stack*/
	UFUNCTION(BlueprintCallable)
	int Find(UCard* Card);

	/* True --> Card is the first card, found while iterating from the bottom of the stack, which has card type normal
	*  False --> Card isn't in stack OR isn't in correct position
	*/
	UFUNCTION(BlueprintCallable)
	bool IsLeadingNormalCard(UCard* Card);


private:
	/* The stack can be left in a variety of states upon Card::RevertModifyStack is finished.
	*  This method is a catch all the makes sure their is only one leading-unmodified-empty-slot
	*/
	void Sanitize();


private:
	/* The card being seen by the player */
	UCard* SelectedCard;
	
	// The outer most Stack Object is ALWAYS a slot
	UPROPERTY(VisibleAnywhere)
	TArray <UStackSlot*> BackingArray;

	int ActiveSlotIndex;

	UTrayStackWidget* Widget;

public:
	/* Mostly used to get the slot needing to be modified during Card::Modify;
	* Exposed because card's will sometimes want to maintain a pointer to the slot they'ved modified.
	*/
	UFUNCTION(BlueprintCallable)
	UStackSlot* GetLeadingEmptySlot() { return BackingArray[BackingArray.Num() - 1]; }

	FORCEINLINE const TArray<UStackSlot*> GetBackingArray() { return BackingArray; }

	/* returns the length in elements of the backing array */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int Num() { return BackingArray.Num(); }
};
