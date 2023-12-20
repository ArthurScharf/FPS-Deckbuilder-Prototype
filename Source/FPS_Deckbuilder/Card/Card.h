
#pragma once

#include "CoreMinimal.h"

#include "FPS_Deckbuilder/Card/StackObject.h"
#include "FPS_Deckbuilder/Character/GameCharacter.h"
#include "FPS_Deckbuilder/Character/PlayerCharacter.h"
#include "FPS_Deckbuilder/CommonHeaders/CostPackage.h"

#include "UObject/NoExportTypes.h"
#include "Card.generated.h"


class AProjectile;
class UTrayStack;


/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class FPS_DECKBUILDER_API UCard : public UObject, public IStackObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Use();

	/* DEPRECATED. TO BE DELETED?
	To be bound to spawned actors when those actors instigate the card's effect. */
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnedActorCallback(AGameCharacter* GameCharacter, FVector Location);

	/* 
	* Card subscribes to events on the PlayerCharacter for the purpose of tracking progression. 
	* WARNING: Cannot be private. NEVER Call this directly except in SetPlayerCharacter
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void Subscribe();

	UFUNCTION(BlueprintImplementableEvent)
	void Unsubscribe();


	// -- Slot/Stack Methods -- //
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ModifyStack(UTrayStack* Stack);

	/* Undoes whatever modify stack did */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void RevertStackModification();

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
	UCard* ReturnCard() override { return this; };

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




private:
	// -- Book keeping properties -- //
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) // Same as having a blueprint only getter
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Card")
	FName Name;

	UPROPERTY(EditDefaultsOnly) // Same as blueprint specific getter
	UTexture2D* Texture;

	// -- Gameplay Properties -- //
	// Unlike PlayerCharacter::Resources, Cost is 5 long. This is so the card can expect to spend money, or health
	UPROPERTY(EditAnywhere, Category = "Card")
	FCost Cost;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) // Same as blueprint specific getter & setter
	float Progress;


	// -- Slot/Stack Properties -- //
	// Classes instantiated to populate the NextCards array. Empty array entries indicate an empty position in the array.
	// Empty positions are to be used by the player to build custom stacks 
	UPROPERTY(EditDefaultsOnly, Category = "Card|Stack Properties")
	TArray<TSubclassOf<UCard>> NextClasses; 

	UPROPERTY(VisibleAnywhere, Category = "Card|Stack Properties")
	TArray<UCard*> NextCards;

	// Seconds upon card being drawn for the stack to reset. 0 --> no timer reset
	UPROPERTY(EditDefaultsOnly, Category = "Card|Stack Properties")
	float ResetSeconds;

	// Cooldown applied to the slot upon reset
	UPROPERTY(EditDefaultsOnly, Category = "Card|Stack Properties")
	float ResetCooldownSeconds;
		
	// float WarmupSeconds;




public:
	FORCEINLINE FCost GetCost() { return Cost; }

	FORCEINLINE UTexture2D* GetTexture() { return Texture; }

	/* Sets player character and gives the card a chance to subscribe to any information the it would need for passive progression*/
	FORCEINLINE void SetPlayerCharacter(APlayerCharacter* _PlayerCharacter) 
	{ 
		PlayerCharacter = _PlayerCharacter; 
		Subscribe();
	}
};
