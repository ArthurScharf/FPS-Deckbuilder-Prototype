// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//#include "FPS_Deckbuilder/Character/PlayerCharacter.h"
#include "Components/SphereComponent.h"

#include "UObject/Interface.h"
#include "Interactable.generated.h"


class APlayerCharacter;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};


/**
 * 
 */
class FPS_DECKBUILDER_API IInteractable
{
	GENERATED_BODY()

public:
	virtual void Interact(APlayerCharacter* Character) PURE_VIRTUAL(IInteractable::Interact,);
};
