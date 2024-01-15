

#include "TrayStack.h"

#include "FPS_Deckbuilder/Card/StackSlot.h"



UTrayStack::UTrayStack()
{
	// SelectedCard = MakeShared<UCard>();
	BackingArray.Add(CreateDefaultSubobject<UStackSlot>("Default Slot")); // Creating a leading slot
	bOnCooldown = false;
}


UCard* UTrayStack::Rotate()
{
	// TODO: Allow card that's about to rotate out a chance to cleanup

	++ActiveSlotIndex;
	if (BackingArray[ActiveSlotIndex]->IsEmpty())
	{
		ResetTrayStack();
	}
	else
	{
		UWorld* World = GetWorld();

		// -- Setting Cooldown based on card that's about to be rotated out -- //
		if (SelectedCard->GetCooldownSeconds() != 0.f)
		{
			World->GetTimerManager().SetTimer(
				CooldownHandle,
				[&]() { bOnCooldown = false; },
				SelectedCard->GetCooldownSeconds(),
				false
			);
			bOnCooldown = true;
		}
		
		// -- Setting ResetTimer based on card that was just rotated in -- //
		SelectedCard = BackingArray[ActiveSlotIndex]->ReturnCard();
		//SelectedCard = TSharedPtr<UCard>(BackingArray[ActiveSlotIndex]->ReturnCard());
		if (SelectedCard->GetToResetSeconds())
		{
			World->GetTimerManager().SetTimer(
				CooldownHandle,
				[&]() {
					ResetTrayStack(); // will reset cooldown timer with new seconds
				},
				SelectedCard->GetToResetSeconds(),
				false
			);
		}

		Widget->Update(SelectedCard);
	}
	
	// TODO: Allow card that just rotated in a chance to setup
	return SelectedCard;
}


void UTrayStack::UseSelectedCard()
{
	UE_LOG(LogTemp, Warning, TEXT("UTrayStack::UseSelectedCard / %s"), *GetName());
	
	//UCard* Card = nullptr;
	//FString String;
	//for (int i = 0; i < BackingArray.Num(); i++)
	//{
	//	Card = BackingArray[i]->ReturnCard();
	//	
	//	String = (IsValid(Card) ? *Card->GetName() : FString("nullptr"));
	//	UE_LOG(LogTemp, Warning, TEXT("		%s"), *String);
	//}

	if (bOnCooldown)
	{
		UE_LOG(LogTemp, Warning, TEXT("		On Cooldown"));
		return;
	}

	// Remember that backing array has only StackSlots 
	// UCard* Card = BackingArray[ActiveSlotIndex]->ReturnCard();

	// TODO: Properly check for card requirements
	if (IsValid(SelectedCard))
	{
		// ECardType CardType = SelectedCard->GetCardType();
		// if (CardType == ECardType::CT_Block) return; 

		UE_LOG(LogTemp, Warning, TEXT("		%s"), *SelectedCard->GetName());

		SelectedCard->Use();
	}
}


void UTrayStack::SetWidget(UTrayStackWidget* _Widget)
{
	Widget = _Widget;

	Widget->Update(SelectedCard); //  Initialized the widget with the first card in the stack
}


void UTrayStack::ResetTrayStack()
{
	ActiveSlotIndex = 0;
	// Remember that a stack should always be maintaining a leading empty slot. No need to check Slot at 0 exists
	SelectedCard = BackingArray[0]->ReturnCard();
	Widget->Update(SelectedCard, true);
	if (!SelectedCard || SelectedCard->GetResetCooldownSeconds() == 0.f) return; // no sense resetting an empty slot
	bOnCooldown = true;
	GetWorld()->GetTimerManager().SetTimer(
		CooldownHandle, 
		[&]() { bOnCooldown = false; },
		SelectedCard->GetResetCooldownSeconds(), 
		false
	);	
}


bool UTrayStack::SetCardInSlot(UStackSlot* Slot, int SlotChildIndex, UCard* Card)
{
	if (!Card)
	{
		UE_LOG(LogTemp, Warning, TEXT("UTrayStack::SetCardInSlot -- !Card"));
		return false;
	}

	BackingArray.Add(NewObject<UStackSlot>(this)); // Creating a new leading slot so modify has a chance to modify a slot ahead, since the previous leading slot is now filled with Card
	if (!Card->ModifyStack(this)) // Visitor Pattern
	{
		BackingArray.Pop(true); // Failed to modify so old slot must remain leading empty slot
		return false;
	}

	Slot->SetChild(Card, SlotChildIndex); // Set's the child of this slot, to the passed card
	Card->ContainingStackSlot = Slot;
	return true;
}



bool UTrayStack::RemoveCard(UCard* Card)
{
	if (!Card)
	{
		UE_LOG(LogTemp, Warning, TEXT("UTrayStack::RemoveCard -- !Card"));
		return false;
	}

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
		Sanitize();
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


/* Possible that there are other normals cards in the same slot as the one we passed. 
*  This doesn't mean the result is false.
*/
bool UTrayStack::IsLeadingNormalCard(UCard* Card)
{
	UE_LOG(LogTemp, Warning, TEXT("UTrayStack::IsLeadingNormalCard"));

	if (Card->GetCardType() != ECardType::CT_Normal) return false;

	TArray<UCard*> ContainedCards;
	UStackSlot* StackSlot;
	for (int i = BackingArray.Num()-1; i >= 0; i--)
	{
		StackSlot = BackingArray[i];
		ContainedCards = StackSlot->GetContainedCards();

		// -- Searching Contained Cards for this outer stack slot -- // 
		bool bNormalFound = false;
		for (UCard* ContainedCard : ContainedCards)
		{
			UE_LOG(LogTemp, Warning, TEXT("		%s"), *ContainedCard->GetName());
			if (ContainedCard->GetCardType() == ECardType::CT_Normal)
			{
				UE_LOG(LogTemp, Warning, TEXT("			Normal"));
				bNormalFound = true; // Since a normal card is found, Card MUST be in ContainedCards for TRUE
				if (ContainedCard == Card)
				{
					return true;
					UE_LOG(LogTemp, Warning, TEXT("				Found"));
				}

			}
		}
		if (bNormalFound) return false; // First normals found didn't contain Card, thus Card isn't leading normal 
	}
	return false; // BackingArray.Num() == 0
}


UWorld* UTrayStack::GetWorld() const
{
	if (!Cast<APlayerCharacter>(GetOuter()))
	{
		UE_LOG(LogTemp, Error, TEXT("UTrayStack::GetWorld -- Outer not set to correct class"));
	}

	return GetOuter()->GetWorld();
}


void UTrayStack::Sanitize()
{
	/* Leading-unmodified-empty-slot cannot
	* 1. Single Child that is set to nullptr
	* 2. No ReturnCardDelegate
	*/
	TArray<int> ToRemoveIndices;

	UStackSlot* StackSlot;
	for (int i = BackingArray.Num() - 1; i >= 0; i--)
	{
		StackSlot = BackingArray[i];

		if (StackSlot->GetChildren().Num() == 1 && StackSlot->GetChildren()[0] == nullptr && !StackSlot->IsReturnCardDelegateAssigned())
		{
			ToRemoveIndices.Add(i);
		}
	}

	if (ToRemoveIndices.Num() >= 2) // More than one unmodified-empty-slot exists ahead of the legal stack 
	{
		for (int i = 0; i < ToRemoveIndices.Num() - 1; i++) BackingArray.Pop();
	}

	Widget->Update(SelectedCard);
}


