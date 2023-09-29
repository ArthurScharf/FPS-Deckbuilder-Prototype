
#pragma once

#include "CoreMinimal.h"
#include "FPS_Deckbuilder/Character/AI/EnemyAIController.h"
#include "FPS_Deckbuilder/Character/GameCharacter.h"
#include "EnemyCharacter.generated.h"


class UAnimMontage;
class UBehaviorTree;
class UEnemyAnimInstance;
class UWidgetComponent;



UCLASS()
class FPS_DECKBUILDER_API AEnemyCharacter : public AGameCharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	virtual void ReceiveDamage(FDamageStruct& DamageStruct) override;

	virtual void NotifyOfDamageDealt(FDamageStruct& DamageStruct) override;

	/* overridden to force sight cone facing to align with the character's facing 
	 * NOTE: I wasn't able to understand where this method was being called in the engine code such that overriding it does this, but it works. Wouldn't mind understanding better 
	 */
	virtual FRotator GetViewRotation() const override;


	/*
	* the intention is that each enemy can implement their own attack behavior's in a switch statement.
	* Each attack can be handled by this statement
	*/
	// I'd rather this be able to be protected or private
	UFUNCTION(BlueprintCallable)
	void Attack(UAnimMontage* AttackMontage);

	/* These are written here, in this way because they should be overridden in the blueprints. Easier to understand all the attack behaviors at a glance if they're in one place */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnAttackDamageEvent1();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnAttackDamageEvent2();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnAttackDamageEvent3();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnAttackDamageEvent4();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnAttackDamageEvent5();


protected:
	virtual void BeginPlay() override;

private:
	virtual void Die();

private:
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* BehaviorTree;

	UEnemyAnimInstance* EnemyAnimInstance;

	AEnemyAIController* EnemyAIController;
};
