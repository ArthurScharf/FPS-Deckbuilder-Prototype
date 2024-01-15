


#include "StackSlot.h"

#include "FPS_Deckbuilder/Card/Card.h"



UStackSlot::UStackSlot()
{
	// Slots always start with a single empty position by default
	Children.Add(nullptr); 
}


UCard* UStackSlot::ReturnCard()
{
	UE_LOG(LogTemp, Warning, TEXT("UStackSlot::ReturnCard / %s"), *GetName());
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
		// UE_LOG(LogTemp, Warning, TEXT("	%i / %s"), i, (Children[i] ? *Children[i]->GetName() : "nullptr"));
		//if (!IsValid(Cast<UCard>(Children[i]))) { UE_LOG(LogTemp, Error, TEXT("		Invalid Card")); continue; }
		//else if (!IsValid(Cast<UStackSlot>(Children[i]))) { UE_LOG(LogTemp, Error, TEXT("		Invalid StackSlot")); continue; }
		if (Children[i]) return Children[i]->ReturnCard();
	}

	// Empty Slot 
	return nullptr;
}


bool UStackSlot::SetChild(IStackObject* Child, int Index)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, FString::Printf(TEXT("Valid: %i"), IsValid(Children) ? 1 : 0 ));
	//return false;

	if (Index < 0 || Children.Num() <= Index)
	{
		UE_LOG(LogTemp, Error, TEXT("UStackSlot::SetChild / %s -- Index exceeds length of array"), *GetName());
		return false;
	}

	Children[Index] = Child;
	return true;
}


void UStackSlot::ClearChild(int Index)
{
	if (Index < 0 || Children.Num() <= Index)
	{
		UE_LOG(LogTemp, Error, TEXT("UStackSlot::ClearChild / %s -- Index exceeds length of array"), *GetName());
		return;
	}

	Children[Index] = nullptr;	
	return;
}


TArray<UCard*> UStackSlot::GetContainedCards()
{
	if (Children.Num() == 0) return {}; // Escape Condition

	TArray<UCard*> OutArray;
	UCard* Card;
	for (IStackObject* Child : Children) // Recursion
	{
		
		if (!Child) continue; // Skips nullptr entries reserved for unfilled slot positions
			
		Card = Cast<UCard>(Child);
		if (Card)
		{
			OutArray.Add(Card);
			continue;
		}

		OutArray.Append(Cast<UStackSlot>(Child)->GetContainedCards() );
	}

	return OutArray;
}



/* Is Card in my children, or my children's children, Recursively */
bool UStackSlot::Contains(IStackObject* StackObject, int& ChildIndex)
{
	// -- Escape Condition -- //
	for (int i = 0; i < Children.Num(); i++) // Iterating Children
	{
		if (Children[i] == StackObject) // Return Condition
		{

			ChildIndex = i;
			return true;
		}
		
		// -- Recursion -- //
		UStackSlot* Slot = Cast<UStackSlot>(Children[i]);
		if (Slot) // Continue condition
		{

			if (Slot->Contains(StackObject, ChildIndex)) return true;
		}
	}

	return false;
}


bool UStackSlot::Contains(IStackObject* StackObject)
{
	int temp;
	return Contains(StackObject, temp);
}



bool UStackSlot::IsEmpty()
{
	UStackSlot* Slot = nullptr;
	for (int i = 0; i < Children.Num(); i++) // Iterating Children
	{
		if (!Children[i]) continue; // Empty position in a slot

		if (Cast<UCard>(Children[i])) return false;

		return Cast<UStackSlot>(Children[i])->IsEmpty();
	}

	return true;
}