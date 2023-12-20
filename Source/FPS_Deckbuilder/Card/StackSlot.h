
#pragma once

#include "CoreMinimal.h"
#include "FPS_Deckbuilder/Card/StackObject.h"
#include "UObject/NoExportTypes.h"
#include "StackSlot.generated.h"



class UCard;


DECLARE_DYNAMIC_DELEGATE_OneParam(FReturnCardDelegate, UCard*, ReturnCard); // For modifying the way the slot chooses which of it's children is chosen


/**
 * A container class that, when combined with StackObject, allows for nested
 * containers used for representing sequences of stacks of cards
 */
UCLASS(BlueprintType)
class FPS_DECKBUILDER_API UStackSlot : public UObject, public IStackObject
{
	GENERATED_BODY()


public:
	UStackSlot();

	UCard* ReturnCard();

	bool SetChild(IStackObject* Child, int Index);


private:
	TArray<IStackObject*> Children;

	FReturnCardDelegate* ReturnCardDelegate;
	
public:
	// UFUNCTION(BlueprintCallable) // We'll need this to set the stack editor view 
	const TArray<IStackObject*> GetChildren() { return Children; } 

	UFUNCTION(BlueprintCallable)
	void SetReturnCardBehavior(const FReturnCardDelegate& Delegate) { *ReturnCardDelegate = Delegate; }
	
	UFUNCTION(BlueprintCallable)
	void ClearReturnCardBehavior() { ReturnCardDelegate = nullptr; }
};
