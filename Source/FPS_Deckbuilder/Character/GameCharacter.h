#pragma once

#include "CoreMinimal.h"


#include "FPS_Deckbuilder/CommonHeaders/DamagePackage.h"
#include "FPS_Deckbuilder/UI/LazyHealthBar.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Character.h"
#include "GameCharacter.generated.h"


class AProjectile;
class UStatusEffect;


/* An abstract class used by all characters in the game 
 * 
 * All subclasses MUST pass their healthbar widget up to GameCharacter in BeginPlay
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class FPS_DECKBUILDER_API AGameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGameCharacter();
	// AGameCharacter(const FObjectInitializer& ObjectInitializer); // Needed so child classes can use member initialization syntax to init type of movement component used

	UFUNCTION(BlueprintCallable)
	virtual void ReceiveDamage(UPARAM(ref) FDamageStruct& DamageStruct); 

	/* Adds a status effect to the list of those attached to this character.
	 * Will either add a new effect, or increase the stack value of one if a stackable instance of Class is already on the character 
	 */
	UFUNCTION(BlueprintCallable)
	void InstantiateStatusEffect(TSubclassOf<UStatusEffect> Class);

	UFUNCTION(BlueprintCallable)
	void RemoveStatusEffect(UStatusEffect* StatusEffect);
	
	/*
	* A workaround. Can't call SpawnActorDeferred in bluerprints. also can't pass function pointers to a "BlueprintCallable" function.
	* Therefor, must Spawn projectile without collision
	*
	* TFunction's cannot be passed as parameters to BlueprintCallable methods
	* The same is true of TFunctionRef
	*/
	UFUNCTION(BlueprintCallable)
	AProjectile* SpawnProjectileWithoutCollision(TSubclassOf<AProjectile> ProjectileClass, FTransform Transform);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void NotifyOfDamageDealt(FDamageStruct& DamageStruct) PURE_VIRTUAL(AGameCharacter::NotifyOfDamageDealt);

	// Kills the game character and initiates cleanup effects on the game (effects can be delayed, in the case of Enemies)
	//virtual void Die() PURE_VIRTUAL(AGameCharacter::Die, );
	virtual void Die();

	UFUNCTION(BlueprintCallable)
	void Dash();

private:
	/* See DependentActors */
	void AttemptDestroy();


protected:
	UPROPERTY(EditDefaultsOnly)
	int32 Money;


	// -- Movement -- //
	UPROPERTY(EditDefaultsOnly)
	float DashDistance;

	UPROPERTY(EditDefaultsOnly)
	float DashSpeed;

	UPROPERTY(BlueprintReadOnly)
	float DashSeconds;

	UPROPERTY(BlueprintReadWrite);
	FVector DashDirection;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDashing;	// Time constraints dictate a hackey solution to dashing instead of a custom character movement class



private:
	// Private because health is managed by calls to superclass methods like ReceiveDamage(). NOTE: This feels needlessly restrictive
	UPROPERTY(VisibleAnywhere, Transient)
	float Health;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;
	
	ULazyHealthBar* LazyHealthBar;

	UPROPERTY(VisibleAnywhere)
	TArray<UStatusEffect*> StatusEffects;

	/*
	* The system I've designed often has game character's spawning actors, then binding it's events to the delegates for those spawned actors.
	* Destroying a game character seems to prevent this delegate methods from being called (Makes sense, since these delegates would usually need properties that belong to the destroyed actor),
	* Thus, we maintain an array of actors we've spawned and bound to. The actor is only destroyed once they are dead, and this list is empty
	*/
	TArray<AActor*> DependentActors;

// -- Getters & Setters -- //
protected:
	FORCEINLINE float GetHealth() { return Health; }

	FORCEINLINE void SetLazyHealthBar(ULazyHealthBar* _LazyHealthBar) { LazyHealthBar = _LazyHealthBar; }

	UFUNCTION(BlueprintCallable)
	void AddDependentActor(AActor* Actor) { DependentActors.Add(Actor); }
};


