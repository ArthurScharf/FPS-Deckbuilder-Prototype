

#include "TrayStack.h"
#include "FPS_Deckbuilder/Card/Card.h"
#include "FPS_Deckbuilder/Card/StackSlot.h"



UTrayStack::UTrayStack()
{
	BackingArray.Add(CreateDefaultSubobject<UStackSlot>("Default Slot")); // Creating a leading slot
}


UCard* UTrayStack::Rotate()
{
	// TODO: Allow card that's about to rotate out a chance to cleanup
	
	// Checking to see if we cycled the stack
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



bool UTrayStack::SetCardInSlot(UStackSlot* Slot, int SlotChildIndex, UCard* Card)
{
	if (!Card->ModifyStack(this)) return false; // Visitor Pattern
	Slot->SetChild(Card, SlotChildIndex); // Set's the child of this slot, to the passed card
	BackingArray.Add(NewObject<UStackSlot>(this)); // Creating a leading slot
	return true;
}