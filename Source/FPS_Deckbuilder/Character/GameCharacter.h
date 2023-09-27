#pragma once

#include "CoreMinimal.h"

#include "FPS_Deckbuilder/CommonHeaders/DamagePackage.h"
#include "FPS_Deckbuilder/UI/LazyHealthBar.h"

#include "Engine/EngineTypes.h"
#include "GameFramework/Character.h"
#include "GameCharacter.generated.h"


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

	UFUNCTION(BlueprintCallable)
	virtual void ReceiveDamage(UPARAM(ref) FDamageStruct& DamageStruct);

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
	UPROPERTY(VisibleAnywhere)
	float Health;
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;
	ULazyHealthBar* LazyHealthBar;


// -- Getters & Setters -- //
protected:
	FORCEINLINE float GetHealth() { return Health; }

	FORCEINLINE void SetLazyHealthBar(ULazyHealthBar* _LazyHealthBar) { LazyHealthBar = _LazyHealthBar; }
};
