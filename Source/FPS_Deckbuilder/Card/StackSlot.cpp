


#include "StackSlot.h"

#include "FPS_Deckbuilder/Card/Card.h"



UStackSlot::UStackSlot()
{
	// Slots always start with a single empty position by default
	Children.Add(nullptr); 
}


UCard* UStackSlot::ReturnCard()
{
	// Use unique search if card slot has been modified
	if (ReturnCardDelegate)
	{
		UCard* CardToReturn = nullptr;
		ReturnCardDelegate->Execute(CardToReturn);
		return CardToReturn;
	}


	/* NOTE
	*  While nullptrs are used to indicate an empty position in a slot, 
	*  these positions are not legal stack configurations outside of the stack editor.
	*  ReturnCard is only called during gameplay when these illegal stack configurations shouldn't exist.
	*  Thus, no checking for nullptr is done.
	*/
	for (IStackObject* Child : Children)
	{
		// The child is a slot or a card
		return Child->ReturnCard();
	}

	// This should never happen 
	UE_LOG(LogTemp, Error, TEXT("UStackSlot::ReturnCard / %s -- Returned nullptr"), *GetName());
	return nullptr;
}


bool UStackSlot::SetChild(IStackObject* Child, int Index)
{
	if (Index >= Children.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("UStackSlot::SetChild / %s -- Index exceeds length of array"), *GetName());
		return false;
	}

	Children[Index] = Child;
	return true;
}

