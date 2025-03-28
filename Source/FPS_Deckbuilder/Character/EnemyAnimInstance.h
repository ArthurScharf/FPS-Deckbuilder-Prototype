// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FPS_Deckbuilder/Character/EnemyCharacter.h"

#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackAnimationCompleteSignature);


/**
 * Outside of handling the things that an AnimInstance would,
 * this class also handles logic related to attack animations specifically.
 * 
 * An Attack Node in a AEnemyCharacter behavior tree
 * Binds "Finish Execute" to EnemyAnimInstance's "OnAttackAnimationCompleteDelegate" then starts the Enemy's attack animation through the appropriate attack method
 * Attack animation triggers AttackAnimationComplete notify on the skeleton
 * AnimInstance blueprint receives notification and calls Broadcasts "OnAttackAnimationCompleteDelegate"
 * Behavior tree node receives FinishExecute and continues process
 * 
 * It might have made more sense for this code to belong to the EnemyCharacter, but since it's animation related, I've put it here
 */
UCLASS()
class FPS_DECKBUILDER_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	void PlayHitReactMontage();

	//virtual void NativeInitializeAnimation() override;

	//virtual void NativeUpdateAnimation(float DeltaTime) override;

protected:
	UFUNCTION(BlueprintCallable)
	void BroadcastOnAttackAnimationCompleteDelegate() { OnAttackAnimationCompleteDelegate.Broadcast(); }

protected:
	UPROPERTY(BlueprintReadWrite)
	AEnemyCharacter* EnemyCharacter;

	/* Used to call FinishExecute on the Attack behavior tree node */
	UPROPERTY(BlueprintAssignable)
	FOnAttackAnimationCompleteSignature OnAttackAnimationCompleteDelegate;

	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimMontage*> HitReactMontages;

	/* Speed the EnemyCharacter is moving at */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Speed;

	/* Direction the enemy Charcter is moving in. relative to it's forward vector. [-180, 180] */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Direction;

	UPROPERTY(BlueprintReadOnly)
	bool bIsStunned;

public:
	FORCEINLINE void SetIsStunned(bool _bIsStunned) { bIsStunned = _bIsStunned; } // Used exclusively by AEnemyCharacter::Stun to control animation state
};

























