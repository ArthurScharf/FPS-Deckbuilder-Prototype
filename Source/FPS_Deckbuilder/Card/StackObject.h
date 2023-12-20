// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StackObject.generated.h"

class UCard;


UINTERFACE(MinimalAPI, BlueprintType)
class UStackObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implemented by UCard & UStackSlot.
 * UStack uses these objects as primitives to create Slot Tree's for each entry in an instance of UStack
 */
class FPS_DECKBUILDER_API IStackObject
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UCard* ReturnCard() { return nullptr; }


private:
	IStackObject* Outer;


public:
	FORCEINLINE IStackObject* GetOuter() { return Outer; }

	FORCEINLINE void SetOuter(IStackObject* _Outer) { Outer = _Outer; }
};
