// Fill out your copyright notice in the Description page of Project Settings.


#include "StackEditorWidget.h"

#include "FPS_Deckbuilder/Card/Card.h"
#include "FPS_Deckbuilder/Card/TrayStack.h"


void UStackEditorWidget::Init(TArray<UTrayStack*> Stacks, TArray<UCard*> Inventory) const
{
	for (UCard* Card : Inventory)
	{
		InventoryTileView->AddItem(Card);
	}

}