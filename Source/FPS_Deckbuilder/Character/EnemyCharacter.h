#pragma once

#include "CoreMinimal.h"
#include "FPS_Deckbuilder/Character/AI/EnemyAIController.h"
#include "FPS_Deckbuilder/Character/GameCharacter.h"
#include "EnemyCharacter.generated.h"


class AProjectile;
class UAnimMontage;
class UBehaviorTree;
class UEnemyAnimInstance;
class UMaterial;
class UMaterialInstanceDynamic;
class UWidgetComponent;



UCLASS()
class FPS_DECKBUILDER_API AEnemyCharacter : public AGameCharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();



	virtual void ReceiveDamage(FDamageStruct& DamageStruct, bool bTriggersStatusEffects = true) override;

	//virtual void NotifyOfDamageDealt(FDamageStruct& DamageStruct) override;

	/* Handles bones being hit or anything else unique to how a blueprint subclass takes damage 
	* 
	* NOTE: I can't seem to have pass-by-reference variables used in this way change once the blueprint implementation is popped
	* from the stack
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void HandleSpecialDamageConditions(UPARAM(ref) FDamageStruct& DamageStruct);

	/* Replaces previous stun timer each timer this is called */
	void Stun(float StunSeconds) override;


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

	UFUNCTION()
	virtual void Tick(float DeltaTime) override;

private:
	virtual void Die();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FRotator CalculateAimAtRotation(const FVector& TargetLocation);

public:
	// -- Editor Only -- //
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Enemy|Debugging")
	bool bDrawAim;
#endif


protected:
	/* The distance above the location of the actor that aiming is done from. Usually between the EyeHeight and the half height of the capsule */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	float AimOffset;

private:
	/* Lower LOD copy of the Character mesh. Applied material gives appearance of outline to Character mesh*/
	UPROPERTY(EditDefaultsOnly, Category = "EnemyCharacter")
	USkeletalMeshComponent* ShellMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "EnemyCharacter|Shell")
	UMaterial* ShellMat;

	UPROPERTY(VisibleAnywhere, Category = "EnemyCharacter|Shell")
	UMaterialInstanceDynamic* ShellMatInstance;

	UPROPERTY(EditDefaultsOnly, Category = "EnemyCharacter|Shell")
	float ShellOpacityDecayRate;

	UPROPERTY(EditDefaultsOnly, Category = "EnemyCharacter|Shell")
	float ShellOpacityDecayAmount;

	/* Index for Alpha the parameter for ShellMatInstance. Stored for more efficient accessing since the mat is modified frequently */
	UPROPERTY(VisibleAnywhere, Category = "EnemyCharacter|Shell")
	float ShellOpacity;

	FTimerHandle ShellOpacityHandle;

	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* BehaviorTree;

	UEnemyAnimInstance* EnemyAnimInstance;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AEnemyAIController* EnemyAIController;

	/* Name of the bone last hit by an attack.
	* Only EnemyCharacter must keep track of which bone has been hit with an attack.
	* Therefore, adding hit bone to the DamageStruct complicates it. 
	* Thus, Enemy Characters set which bone is hit when an damage instigating overlap occurs
	*/
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	FName HitBoneName;

	/* Used to halt tick for a short delay to simulate hitstun */
	bool bPausedTick;

	FTimerHandle StunnedHandle;

	UPROPERTY(VisibleAnywhere, Category = "EnemyCharacter")
	FRotator StoredRotationRate; // Rotation rate is set to 0 when stunned. Stored to return to normal state once stun is complete


	// -- Posture System -- //
	UPROPERTY(EditDefaultsOnly, Category = "EnemyCharacter|Posture")
	float MaxPosture;

	UPROPERTY(VisibleAnywhere, Category = "EnemyCharacter|Posture")
	float Posture;
	
	UPROPERTY(EditDefaultsOnly, Category = "EnemyCharacter|Posture")
	float PostureBreakSeconds;

	//UPROPERTY(EditDefaultsOnly, Category = "EnemyCharacter|Posture")
	//USoundCue* PostureBreakSound;

	FTimerHandle PostureBreakRecoveryHandle;


public:
	// Too much casting. This has been a reccurant issue throughout the code base
	FORCEINLINE APlayerCharacter* GetTargetPlayerCharacter() { return Cast<APlayerCharacter>(EnemyAIController->GetBlackboardComponent()->GetValueAsObject("TargetPlayerCharacter")); }
	
	FORCEINLINE void SetHitBoneName(const FName _HitBoneName) { HitBoneName = _HitBoneName; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPostureBroken() { return Posture >= MaxPosture; }

};
