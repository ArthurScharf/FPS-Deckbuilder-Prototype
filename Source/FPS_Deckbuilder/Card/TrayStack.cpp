

#include "TrayStack.h"
#include "FPS_Deckbuilder/Card/Card.h"
#include "FPS_Deckbuilder/Card/StackSlot.h"



UTrayStack::UTrayStack()
{
	// UStackSlot* Slot = NewObject<UStackSlot>(this);
	BackingArray.Add(CreateDefaultSubobject<UStackSlot>("Default Slot"));
}


UCard* UTrayStack::Rotate()
{
	// TODO: Allow card that's about to rotate out a chance to cleanup
	// Did we cycle the entire stack ?
	if (ActiveSlotIndex == BackingArray.Num() - 1) { ActiveSlotIndex = -1; }
	++ActiveSlotIndex;
	SelectedCard = BackingArray[ActiveSlotIndex]->ReturnCard();
	// TODO: Allow card that just rotated in a chance to setup
	return SelectedCard;
}


void UTrayStack::UseSelectedCard()
{
	// TODO: Properly check for card requirements
	SelectedCard->Use();
	Rotate();
}



bool UTrayStack::SetCardInSlot(UStackSlot* Slot, int Index, UCard* Card)
{
	if (!Card->ModifyStack(this)) return false; // Visitor Pattern
	Slot->SetChild(Card, Index);
	return true;
}