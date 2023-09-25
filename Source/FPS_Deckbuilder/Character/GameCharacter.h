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

	virtual void ReceiveDamage(FDamageStruct& DamageStruct);

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void NotifyOfDamageDealt(FDamageStruct& DamageStruct) PURE_VIRTUAL(AGameCharacter::NotifyOfDamageDealt);

	
private:
	// Kills the game character and initiates cleanup effects on the game (effects can be delayed, in the case of Enemies)
	virtual void Die() PURE_VIRTUAL(AGameCharacter::Die, );


private:
	UPROPERTY(VisibleAnywhere)
	float Health;
	
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;

	ULazyHealthBar* LazyHealthBar;


// -- Getters & Setters -- //
protected:
	FORCEINLINE void SetLazyHealthBar(ULazyHealthBar* _LazyHealthBar) { LazyHealthBar = _LazyHealthBar; }
};
