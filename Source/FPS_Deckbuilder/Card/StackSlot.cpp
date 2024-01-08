


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
	
	// Default search
	for (int i = 0; i < Children.Num(); i++)
	{
		if (Children[i]) return Children[i]->ReturnCard();
	}

	// Empty Slot 
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

