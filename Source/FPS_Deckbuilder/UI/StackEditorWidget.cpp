// Fill out your copyright notice in the Description page of Project Settings.


#include "StackEditorWidget.h"

#include "FPS_Deckbuilder/Card/Card.h"
#include "FPS_Deckbuilder/Card/StackSlot.h"
#include "FPS_Deckbuilder/Card/TrayStack.h"



void UStackEditorWidget::Update(TArray<UTrayStack*> Stacks, TArray<UCard*> Inventory)
{
	InventoryTileView->ClearListItems();
	for (UCard* Card : Inventory)
	{
		InventoryTileView->AddItem(Card);
	}

	StackView->ClearListItems();
	// The outer most StackObject is Always a StackSlot
	for (UStackSlot* StackSlot : Stacks[0]->GetBackingArray())
	{
		StackView->AddItem(StackSlot);
	}
}