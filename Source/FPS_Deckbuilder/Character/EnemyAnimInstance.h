// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FPS_Deckbuilder/Character/EnemyCharacter.h"

#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackAnimationCompleteSignature);


/**
 * Handles variables like enemy velocity
 */
UCLASS()
class FPS_DECKBUILDER_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	


protected:
	UFUNCTION(BlueprintCallable)
	void BroadcastOnAttackAnimationCompleteDelegate() { OnAttackAnimationCompleteDelegate.Broadcast(); }

protected:
	UPROPERTY(BlueprintReadWrite)
	AEnemyCharacter* EnemyCharacter;


	/* Used to call FinishExecute on the Attack behavior tree node */
	UPROPERTY(BlueprintAssignable)
	FOnAttackAnimationCompleteSignature OnAttackAnimationCompleteDelegate;
};

























