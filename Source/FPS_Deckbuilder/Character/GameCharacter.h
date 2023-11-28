#pragma once

#include "CoreMinimal.h"


#include "FPS_Deckbuilder/CommonHeaders/DamagePackage.h"
#include "FPS_Deckbuilder/UI/LazyHealthBar.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Character.h"
#include "GameCharacter.generated.h"


class AProjectile;
class UStatusEffect;



// -- Delegate Declarations -- //
// Some of these don't make sense here. See custom delegates declarations for more info
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnReceiveDamageDelegate, UPARAM(ref) FDamageStruct&, DamageStruct);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnApplyDamageDelegate, UPARAM(ref) FDamageStruct&, DamageStruct);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDamageDealtDelegate, UPARAM(ref) FDamageStruct&, DamageStruct);
DECLARE_DYNAMIC_DELEGATE(FOnReloadDelegate);
DECLARE_DYNAMIC_DELEGATE(FOnAttackDelegate);


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
	virtual void ReceiveDamage(UPARAM(ref) FDamageStruct& DamageStruct, bool bTriggersStatusEffects = true); 

	UFUNCTION(BlueprintCallable)
	virtual void Stun(float StunSeconds) PURE_VIRTUAL(&AGameCharacter::Stun, );

	/* Adds a status effect to the list of those attached to this character.
	 * Will either add a new effect, or increase the stack value of one if a stackable instance of Class is already on the character 
	 */
	UFUNCTION(BlueprintCallable)
	void InstantiateStatusEffect(TSubclassOf<UStatusEffect> Class, AGameCharacter* InstigatingGameCharacter = nullptr);

	UFUNCTION(BlueprintCallable)
	void RemoveStatusEffect(UStatusEffect* StatusEffect);
	
	/*
	* A workaround. Can't call SpawnActorDeferred in bluerprints. also can't pass function pointers to a "BlueprintCallable" function.
	* Therefor, must Spawn projectile without collision
	*
	* TFunction's cannot be passed as parameters to BlueprintCallable methods
	* The same is true of TFunctionRef
	* 
	* NOTE: This might need to be templated
	*/
	UFUNCTION(BlueprintCallable)
	AProjectile* SpawnProjectileWithoutCollision(TSubclassOf<AProjectile> ProjectileClass, FTransform Transform);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void NotifyOfDamageDealt(FDamageStruct& DamageStruct); //PURE_VIRTUAL(AGameCharacter::NotifyOfDamageDealt);

	// Kills the game character and initiates cleanup effects on the game (effects can be delayed, in the case of Enemies)
	//virtual void Die() PURE_VIRTUAL(AGameCharacter::Die, );
	virtual void Die();

	UFUNCTION(BlueprintCallable)
	bool Dash();

private:
	/* See DependentActors */
	void AttemptDestroy();



public:
	/* -- Custom Delegates --
	* Possible for multiple effects to need to act on data structure in a sequence.
	* Dynamic multicast delegates don't support non-const references.
	* Therefore, a custom solution is necessary
	* 
	* WARNING: Some of these are only used by PlayerCharacter. This is a code smell and would make working with this system confusing in the future. Should fix
	*
	* WARNING: Delegates bound in blueprints MUST be with the Create Event node using a function.
	*/
	UPROPERTY(VisibleAnywhere, Category = "GameCharacter|Observers")
	TArray<FOnReceiveDamageDelegate> Observers_OnReceiveDamage; // AGameCharacter::ReceiveDamage
	
	UPROPERTY(VisibleAnywhere, Category = "GameCharacter|Observers")
	TArray<FOnApplyDamageDelegate> Observers_OnApplyDamage; // AWeapon::ApplyDamage. Pre-notify damage instigator. Used for damage modifiers. Only used by APlayerCharacter
	
	UPROPERTY(VisibleAnywhere, Category = "GameCharacter|Observers")
	TArray<FOnDamageDealtDelegate> Observers_OnDamageDealt; // AGameCharacter::NotifyOfDamageDealt. During notify of damage instigator. Used for damage checkers (was lethal, final damage amount etc)

	UPROPERTY(VisibleAnywhere, Category = "GameCharacter|Observers")
	TArray<FOnReloadDelegate> Observers_OnReload;

	UPROPERTY(VisibleAnywhere, Category = "GameCharacter|Observers")
	TArray<FOnAttackDelegate> Observers_OnAttack;

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
	bool bIsDashing;	// Time constraints dictate a hackey solution to dashing instead of a custom character movement class. See Second timer set in dash method for context

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	bool bIsStunned;	// Maintained so effects can verify if a character is stunned


private:
	// -- Health & StatusEffects -- //
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

	
public:
	// NOTE: Blueprints don't recognize the signature for each type of observer without these. Silly that I need these
	// -- Add & Remove Methods for Observers -- //
	UFUNCTION(BlueprintCallable)
	void AddObserver_OnDamageReceived(const FOnReceiveDamageDelegate& Delegate) { Observers_OnReceiveDamage.Add(Delegate); }
	UFUNCTION(BlueprintCallable)
	void RemoveObserver_OnDamageReceived(const FOnReceiveDamageDelegate& Delegate) 
	{
		if (Observers_OnReceiveDamage.Remove(Delegate) > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("AGameCharecter.h::RemoveObserver_OnDamageReceived"));
		}
	}

	UFUNCTION(BlueprintCallable)
	void AddObserver_OnApplyDamage(const FOnApplyDamageDelegate& Delegate) { Observers_OnApplyDamage.Add(Delegate); }
	UFUNCTION(BlueprintCallable)
	void RemoveObserver_OnApplyDamage(const FOnApplyDamageDelegate& Delegate)
	{
		if (Observers_OnApplyDamage.Remove(Delegate) > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("AGameCharecter.h::RemoveObserver_OnApplyDamage"));
		}
	}

	UFUNCTION(BlueprintCallable)
	void AddObserver_OnDamageDealt(const FOnDamageDealtDelegate& Delegate) { Observers_OnDamageDealt.Add(Delegate); }
	UFUNCTION(BlueprintCallable)
	void RemoveObserver_OnDamageDealt(const FOnDamageDealtDelegate& Delegate)
	{
		if (Observers_OnDamageDealt.Remove(Delegate) > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("AGameCharecter.h::RemoveObserver_OnDamageDealt"));
		}
	}

	UFUNCTION(BlueprintCallable)
	void AddObserver_OnReload(const FOnReloadDelegate& Delegate) { Observers_OnReload.Add(Delegate); }
	UFUNCTION(BlueprintCallable)
	void RemoveObserver_OnReload(const FOnReloadDelegate& Delegate) { Observers_OnReload.Remove(Delegate); }

	UFUNCTION(BlueprintCallable)
	void AddObserver_OnAttack(const FOnAttackDelegate& Delegate) { Observers_OnAttack.Add(Delegate); }
	UFUNCTION(BlueprintCallable)
	void RemoveObserver_OnAttack(const FOnAttackDelegate& Delegate) { Observers_OnAttack.Remove(Delegate); }


// -- Getters & Setters -- //
protected:
	FORCEINLINE float GetHealth() { return Health; }

	FORCEINLINE void SetLazyHealthBar(ULazyHealthBar* _LazyHealthBar) { LazyHealthBar = _LazyHealthBar; }

	UFUNCTION(BlueprintCallable)
	void AddDependentActor(AActor* Actor) { DependentActors.Add(Actor); }
};


