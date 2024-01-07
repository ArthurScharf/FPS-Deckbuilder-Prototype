// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TileView.h"
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
	void Init(TArray<UTrayStack*> Stacks, TArray<UCard*> Inventory) const;


public:
	/* View for the player character's inventory and stacks */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTileView * InventoryTileView;
};
