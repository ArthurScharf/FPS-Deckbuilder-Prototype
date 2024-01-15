
#pragma once

#include "CoreMinimal.h"

#include "FPS_Deckbuilder/Card/StackObject.h"
#include "FPS_Deckbuilder/Card/StackSlot.h"
#include "FPS_Deckbuilder/Character/GameCharacter.h"
#include "FPS_Deckbuilder/Character/PlayerCharacter.h"
#include "FPS_Deckbuilder/CommonHeaders/CostPackage.h"



#include "UObject/NoExportTypes.h"
#include "Card.generated.h"


class AProjectile;
class UTrayStack;



/*
* Normal  : Cards found in world. Usable and Stack modifiable
* Created : Cards created and placed on a stack by normal cards when they're placed on the stack themselves. CANNOT be directly placed or removed from a stack
* Block : Created cards that aren't usable 
*/
UENUM(BlueprintType)
enum class ECardType : uint8 {
	CT_Normal		UMETA(DisplayName = "Normal"),
	CT_Created		UMETA(DisplayName = "Created"),
	CT_Block		UMETA(DisplayName = "Block")
};


/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class FPS_DECKBUILDER_API UCard : public UObject, public IStackObject
{
	GENERATED_BODY()

public:

	virtual void BeginDestroy() override;


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool Use();
	/* Called when attempting to use a card. The card checks for valid state of the player character, itself, or other stack slots
	* The Native method checks for features common to all cards such as the cost in resources required to play this card
	* 
	* NOTE: There is a bug. For now, functionality that belongs in here will go into Use
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanUse();

	/* DEPRECATED. TO BE DELETED?
	To be bound to spawned actors when those actors instigate the card's effect. */
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnedActorCallback(AGameCharacter* GameCharacter, FVector Location);

	/* 
	* Card subscribes to events on PlayerCharacter for the purpose of tracking progression. 
	* WARNING: Cannot be private. NEVER Call this directly except in SetPlayerCharacter
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void Subscribe();

	UFUNCTION(BlueprintImplementableEvent)
	void Unsubscribe();


	// -- Slot/Stack Methods -- //
	
	/* ALWAYS implement RevertModifyStack if this is also implemented */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ModifyStack(UTrayStack* Stack);

	/* ALWAYS implement ModifyStack if this is also implemented */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool RevertModifyStack(UTrayStack* Stack);


	/* Pre-rotate functionality can be folded into Use*/
	UFUNCTION(BlueprintImplementableEvent)
	void PreRotate();
	UFUNCTION(BlueprintImplementableEvent)
	void PostRotate();

	/* Called before reset is applied to slot. Can implement unique effects here*/
	UFUNCTION(BlueprintImplementableEvent)
	void PreReset();
	UFUNCTION(BlueprintImplementableEvent)
	void PostReset();

	// -- Stack Object Interface -- //
	UFUNCTION()
	UCard* ReturnCard() override;

private:
	/* Blueprints cant access GetWorld() on PlayerCharacter, while C++ can.
	   This is a workaround to allow the code that card uses to spawn actors to belong to 
	   the card class, rather than the player, or some other actor
	   */
	UFUNCTION(BlueprintCallable)
	AActor* SpawnActor(TSubclassOf<AActor> Class, const FRotator Rotation, const FVector Location);
	
	/* WARNING: MUST have given a blueprint definition to `SpawnedActorCallback` to use this method */
	UFUNCTION(BlueprintCallable)
	AProjectile* SpawnProjectile(TSubclassOf<AProjectile> ProjectileClass, const FRotator Rotation, const FVector Location);

	UFUNCTION(BlueprintCallable)
	void LineTraceSingleByChannel(
		FHitResult& OutHit,
		const FVector& Start,
		const FVector& End,
		ECollisionChannel TraceChannel,
		TArray<AActor*> ActorsToIgnore,
		bool bIgnoreUserPlayerCharacter
	) const;


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




public:
	/* The stack slot the card is contained within */
	UPROPERTY(BlueprintReadOnly)
	UStackSlot* ContainingStackSlot;


private:
	// -- Book keeping properties -- //

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) // Same as having a blueprint only getter
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Card")
	FName Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) // Same as blueprint specific getter
	UTexture2D* Texture;

	/* See Enum Definition for details */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Card|Stack Properties")
	ECardType Type;

	// -- Gameplay Properties -- //
	
	// Unlike PlayerCharacter::Resources, Cost is 5 long. This is so the card can expect to spend money, or health
	UPROPERTY(EditAnywhere, Category = "Card")
	FCost Cost;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) // Same as blueprint specific getter & setter
	float Progress;



	// -- Slot/Stack Properties -- //

	/* Seconds until the slot can use it's selected card upon this card becoming said slot's selected card */
	UPROPERTY(EditDefaultsOnly, Category = "Card|Stack Properties")
	float CooldownSeconds;

	/* Seconds upon card being drawn for the stack to reset. 0 --> no timer reset */
	UPROPERTY(EditDefaultsOnly, Category = "Card|Stack Properties")
	float ToResetSeconds;

	/* Cooldown applied to the slot upon reset */
	UPROPERTY(EditDefaultsOnly, Category = "Card|Stack Properties")
	float ResetCooldownSeconds;
		


public:
	FORCEINLINE FCost GetCost() { return Cost; }

	FORCEINLINE UTexture2D* GetTexture() { return Texture; }

	/* Sets player character and gives the card a chance to subscribe to any information the it would need for passive progression*/
	FORCEINLINE void SetPlayerCharacter(APlayerCharacter* _PlayerCharacter) 
	{ 
		PlayerCharacter = _PlayerCharacter; 
		Subscribe();
	}

	FORCEINLINE ECardType GetCardType() { return Type; }

	FORCEINLINE float GetCooldownSeconds() { return CooldownSeconds; }

	FORCEINLINE float GetToResetSeconds() { return ToResetSeconds; }

	FORCEINLINE float GetResetCooldownSeconds() { return ResetCooldownSeconds; }
};
