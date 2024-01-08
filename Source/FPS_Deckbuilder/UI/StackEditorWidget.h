// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "Components/TileView.h"
#include "FPS_Deckbuilder/Character/PlayerCharacter.h"
#include "StackEditorWidget.generated.h"

class UCard;
class UTrayStack;

/**
 * 
 */
UCLASS()
class FPS_DECKBUILDER_API UStackEditorWidget : public UUserWidget
{
	GENERATED_BODY()



public:
	/* Widget must have a few things initialized after the Initialize method is called.
	* This method is called to populate the list using data related to the character.
	* I'd prefer to overload Initialize, but I'm not sure how if this is possible, or how to force
	* the overloaded version to be called. 
	* Thus, we call this manually
	*/
	// UFUNCTION(BlueprintImplementableEvent)
	void Update(TArray<UTrayStack*> Stacks, TArray<UCard*> Inventory);

public:
	/* The stack view model used to modify stacks. Repopulated when modifying different stacks */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UListView* StackView; 

	/* View for the player character's inventory and stacks */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTileView* InventoryTileView;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int ActiveStackIndex; // Index of stack currently being edited 
};