#pragma once

#include "CoreMinimal.h"

#include "FPS_Deckbuilder/CommonHeaders/DamagePackage.h"

#include "UObject/NoExportTypes.h"
#include "StatusEffect.generated.h"


class AGameCharacter;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class FPS_DECKBUILDER_API UStatusEffect : public UObject
{
	GENERATED_BODY()

// #if WITH_EDITOR
	/* https://benui.ca/unreal/uproperty-edit-condition-can-edit-change/
	*  I could do this a simpler way, but I'm doing it this was as practice
	*  This approach gives us the most control, and puts the parameter edit variables in one place
	*/
	// virtual bool CanEditChange(const FProperty* InProperty) const override;
//#endif
//#if WITH_EDITOR
//bool UStatusEffect::CanEditChange(const FProperty* InProperty) const
//{
//	const bool Value = Super::CanEditChange(InProperty);
//
//	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UStatusEffect, FrequencySeconds))
//	{
//		return FrequencySeconds == 0.f;
//	}
//
//	return Value;
//}
//#endif



public:
	// The business logic of the StatusEffect. Meant to be overridden in blueprints
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Trigger(AGameCharacter* _GameCharacter);

	// Specific enough that a unique trigger is required here. Must less time expensive than writing another class
	// Should only be called by Trigger
	UFUNCTION(BlueprintImplementableEvent)
	void Trigger_OnDamageDealt(FDamageStruct& DamageStruct);
	

	/* 1. (Native) : Adds icon to the GameCharacter's Widget..
	 * 2. (BPs)    : Binds Trigger to the appropriate event in the GameCharacter
	 * 3. (BPs)    : Uses GameCharacter to Init EffectTimer.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Init(AGameCharacter* _GameCharacter);

	

	/* -- NOTE -- 
	* Cleanup process must begin in blueprints, but is being called in C++.
	* Since BlueprintNativeEvent's cannot be virtual, two seperate functions must exist
	*/

	/* 1. Unbinds Trigger from the GameCharacterEvent;
	*  2. Does anything effect specific 
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void Cleanup();

	/* 1. Removes icon to the GameCharacter's Widget.
	 * 2. Removes reference to GameCharacter so instance is flagged for garbage collection
	 */
	UFUNCTION(BlueprintCallable)
	void Cleanup_Native();


protected:
	// The character this effect is affecting
	UPROPERTY(BlueprintReadOnly)
	AGameCharacter* GameCharacter;

	/* Timer set with this does one of two things.
	*  1. Handles the lifetime of the effect, killing it once the timer has expired
	*  2. Handles the recurrent timer that applies effects at an interval. In this case, the lifetime of the effect is EffectFrequency * MaxTriggers
	*/ 
	
	
	FTimerHandle EffectHandle;

	/* Either a lifetime, or the frequency between triggers */
	UPROPERTY(EditDefaultsOnly, meta = (UIMin = "0"))
	float Seconds;

	/* Numbers of triggers before the effect is complete and destroys itself.*/
	UPROPERTY(EditDefaultsOnly, meta = (UIMin = "0"))
	int NumTriggers;

	UPROPERTY(EditDefaultsOnly, meta = (UIMin = "0", EditCondition = "NumTriggers != 0", EditConditionHides))
	float FirstDelaySeconds;
};