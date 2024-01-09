

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
	Card->ContainingStackSlot = Slot;
	BackingArray.Add(NewObject<UStackSlot>(this)); // Creating a leading slot
	return true;
}



bool UTrayStack::RemoveCard(UCard* Card)
{
	if (!Card->RevertModifyStack(this)) return false;

	// -- Finding & Removing the card from the Stack -- //
	UStackSlot** ContainingStackSlot = nullptr;
	int ChildIndex = -1;
	for (int i = 0; i < BackingArray.Num(); i++)
	{
		if (!BackingArray[i]->Contains(Card, ChildIndex)) continue;

		// -- Card is found. Removing and Modifying stack -- //
		Card->ContainingStackSlot->ClearChild(ChildIndex);
		Card->ContainingStackSlot = nullptr;
		BackingArray.RemoveAt(BackingArray.Num() - 1); // Removes the leading empty slot
		return true;
	}

	return false;
}



int UTrayStack::Find(UCard* Card)
{
	for (int i = 0; i < BackingArray.Num(); i++)
	{
		if (BackingArray[i]->Contains(Card)) return i;
	}
	return -1;
}