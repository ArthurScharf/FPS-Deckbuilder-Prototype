// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FPS_Deckbuilder/CommonHeaders/DamagePackage.h"
#include "Damageable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FPS_DECKBUILDER_API IDamageable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
//public:
//	UFUNCTION(BlueprintCallable)
//	virtual void ReceiveDamage(UPARAM(ref) FDamageStruct& DamageStruct);
};
