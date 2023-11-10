#pragma once

#include "CoreMinimal.h"


#include "FPS_Deckbuilder/CommonHeaders/DamagePackage.h"
#include "FPS_Deckbuilder/UI/LazyHealthBar.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Character.h"
#include "GameCharacter.generated.h"

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


protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void NotifyOfDamageDealt(FDamageStruct& DamageStruct) PURE_VIRTUAL(AGameCharacter::NotifyOfDamageDealt);

	
private:
	// Kills the game character and initiates cleanup effects on the game (effects can be delayed, in the case of Enemies)
	virtual void Die() PURE_VIRTUAL(AGameCharacter::Die, );


protected:
	UPROPERTY(EditDefaultsOnly)
	int32 Money;



private:
	// Private because health is managed by calls to superclass methods like ReceiveDamage(). NOTE: This feels needlessly restrictive
	UPROPERTY(VisibleAnywhere, Transient)
	float Health;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;
	
	ULazyHealthBar* LazyHealthBar;

	UPROPERTY(VisibleAnywhere)
	TArray<UStatusEffect*> StatusEffects;


// -- Getters & Setters -- //
protected:
	FORCEINLINE float GetHealth() { return Health; }

	FORCEINLINE void SetLazyHealthBar(ULazyHealthBar* _LazyHealthBar) { LazyHealthBar = _LazyHealthBar; }
};


