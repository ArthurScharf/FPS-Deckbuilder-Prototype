

#include "TrayStack.h"
#include "FPS_Deckbuilder/Card/Card.h"
#include "FPS_Deckbuilder/Card/StackSlot.h"



UTrayStack::UTrayStack()
{
	// UStackSlot* Slot = NewObject<UStackSlot>(this);
	BackingArray.Add(CreateDefaultSubobject<UStackSlot>("Default Slot"));
}




bool UTrayStack::SetCard(UStackSlot* Slot, int Index, UCard* Card)
{
	//if (!Card->ModifyStack(this)) return false; // Visitor Pattern
	// Slot->SetChild(Card, Index);
	return true;
}