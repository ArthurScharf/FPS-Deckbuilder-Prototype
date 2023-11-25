#pragma once

#include "CoreMinimal.h"

#include "Engine/World.h"
#include "FPS_Deckbuilder/CommonHeaders/DamagePackage.h"

#include "UObject/NoExportTypes.h"
#include "StatusEffect.generated.h"

class AGameCharacter;
class UTexture2D;

/**
 * NumTriggers == 0 --> Effect is in listening mode
 * NumTriggers  > 0  --> Effect is in triggered mode
 * 
 * WARNING: Must always call Cleanup_Native in blueprint.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
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
	/* NEVER call this directly. It is called in AGameCharacter::InstantiateEffect
	 * 1. (Native) : Adds icon to the GameCharacter's Widget..
	 * 2. (BPs)    : Binds Trigger to the appropriate event in the GameCharacter
	 * 3. (BPs)    : Uses GameCharacter to Init EffectTimer.
	 * 
	 * TODO: Prevent user from ever calling this directly
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Init(AGameCharacter* _GameCharacter,  AGameCharacter* _InstigatingGameCharacter);


	/* -- NOTE -- 
	* Cleanup process must begin in blueprints, but is being called in C++.
	* Since BlueprintNativeEvent's cannot be virtual, two seperate functions must exist
	*/

	/* 1. Unbinds Trigger from the GameCharacterEvent;
	*  2. Does anything effect specific 
	* 
	*  IMPORTANT: Must call Cleanup_Native
	*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Cleanup();

	/* WARNING: Only call this within the BP implementation of `Cleanup`
	 * 1. Removes icon to the GameCharacter's Widget.
	 * 2. Removes reference to GameCharacter so instance is flagged for garbage collection
	 */
	UFUNCTION(BlueprintCallable)
	void Cleanup_Native();

	UFUNCTION(BlueprintCallable)
	void DecrementNumTriggers();

	/* Remember when implementing to decrement NumTriggers */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Trigger(AGameCharacter* _GameCharacter);

	/* Specific enough that a unique trigger is required here.Must less time expensive than writing another class
	* Should be called instead of trigger when needed
	*/
	UFUNCTION(BlueprintNativeEvent)
	void Trigger_OnDamageDealt(UPARAM(ref) FDamageStruct& DamageStruct);

	void IncrementNumInstances();

	// -- Trace functions -- //
	UFUNCTION(BlueprintCallable)
	bool MultiSphereTraceForObjects(
		FVector Start, 
		FVector End, 
		float Radius,
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes,
		bool bIgnoreSelf,
		TArray<FHitResult>& OutHits
	);

	UFUNCTION(BlueprintCallable)
	bool SphereOverlapActors(
		const FVector Location,
		float Radius,
		const TArray< TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
		UClass* ActorClassFilter,
		const TArray<AActor*>& ActorsToIgnore,
		bool bIgnoreSelf,
		TArray<AActor*>& OutActors
	);
	
	
private:
	/* Sets or resets lifetime timer, including whether or not timer does*/
	void SetLifetimeTimer();


protected:
	/* The character this effect is affecting */
	UPROPERTY(BlueprintReadOnly)
	AGameCharacter* GameCharacter;

	/* The Character that instigated the effect */
	UPROPERTY(BlueprintReadOnly)
	AGameCharacter* InstigatingGameCharacter;

	UPROPERTY(BlueprintReadOnly)
	UWorld* World; // StatusEffects will sometimes need to interact with the world


	/* NOTE: Where is this comment meant to be?
	Timer set with this does one of two things.
	*  1. Handles the lifetime of the effect, killing it once the timer has expired
	*  2. Handles the recurrent timer that applies effects at an interval. In this case, the lifetime of the effect is EffectFrequency * MaxTriggers
	*/ 
	

	/* if True:
	*	Will trigger indefinitely if in trigger mode, or will never remove itself if it listening mode
	*/
	UPROPERTY(EditDefaultsOnly)
	bool bSelfCallsCleanup = true;

	/* Decides whether or not repeated attempts to add instances to a game character result in multiplied strength of effect or several unique stacks of an effect */
	UPROPERTY(EditDefaultsOnly)
	bool bIsStackable;

	UPROPERTY(BlueprintReadOnly)
	int NumInstances;

	UPROPERTY(EditDefaultsOnly, meta=(EditCondition = "bIsStackable"))
	bool bResetLifetimeOnStack;

	FTimerHandle EffectHandle;

	/* Either a lifetime, or the frequency between triggers */
	UPROPERTY(EditDefaultsOnly, meta = (UIMin = "0"))
	float Seconds;

	/* Numbers of triggers before the effect is complete and destroys itself.*/
	UPROPERTY(EditDefaultsOnly, meta = (UIMin = "0"))
	int NumTriggers;

	// Stored so NumTriggers can be reset if bResetLifetimeOnStack == true
	int InitialNumTriggers;

	UPROPERTY(EditDefaultsOnly, meta = (UIMin = "0", EditCondition = "NumTriggers != 0", EditConditionHides))
	float FirstDelaySeconds;

	UTexture2D* Texture;


public:
	FORCEINLINE bool IsStackable() { return bIsStackable; }

	//UFUNCTION(BlueprintCallable)
	//void SetInstigatingGameCharacter(AGameCharacter* _InstigatingGameCharacter) { InstigatingGameCharacter = _InstigatingGameCharacter; }
};