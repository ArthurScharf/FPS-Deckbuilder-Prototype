// Fill out your copyright notice in the Description page of Project Settings.


#include "StackEditorWidget.h"

#include "FPS_Deckbuilder/Card/Card.h"
#include "FPS_Deckbuilder/Card/StackSlot.h"
#include "FPS_Deckbuilder/Card/TrayStack.h"



void UStackEditorWidget::Update(TArray<UTrayStack*> Stacks, TArray<UCard*> Inventory)
{
	// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, "UStackEditorWidget::Update");


	InventoryTileView->ClearListItems();
	for (UCard* Card : Inventory)
	{
		InventoryTileView->AddItem(Card);
	}

	StackView->ClearListItems();
	// The outer most StackObject is Always a StackSlot
	for (UStackSlot* StackSlot : Stacks[ActiveStackIndex]->GetBackingArray())
	{
		StackView->AddItem(StackSlot);
	}
}



void UStackEditorWidget::UpdateStackView(int NewActiveStackIndex) 
{
	// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, "UStackEditorWidget::UpdateStackView");

	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("UStackEditorWidget::UpdateStackView -- !PlayerCharacter. Probably failed to intialize it after widget construction"));
		return;
	}


	ActiveStackIndex = NewActiveStackIndex;
	UTrayStack* Stack = PlayerCharacter->GetTrayStack(ActiveStackIndex);
	StackView->ClearListItems();
	// The outer most StackObject is Always a StackSlot

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString::Printf(TEXT("Stack: %i"), ActiveStackIndex));
	UCard* Card;
	for (UStackSlot* StackSlot : Stack->GetBackingArray())
	{
		Card = StackSlot->ReturnCard();
		if (Card) {
			// StackView->AddItem(Card);
			// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString::Printf(TEXT("  %s"), *Card->GetName()));
		}
		StackView->AddItem(StackSlot);
	}
}