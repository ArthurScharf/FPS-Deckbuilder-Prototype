
#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "FPS_Deckbuilder/Interactable.h"
#include "FPS_Deckbuilder/Character/GameCharacter.h"
#include "FPS_Deckbuilder/Character/PlayerCharacter.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"


class UMatineeCameraShake;
class UCurveFloat; // TODO: implement or remove
class UNiagaraSystem;
class AProjectile;
class USkeletalMeshComponent;
class USphereComponent;


UCLASS()
class FPS_DECKBUILDER_API AWeapon : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	virtual void BeginPlay() override;

	// Decays AccumulatedSpread, sets it's movement innacuracy from it's character, and deactivates it's own tick when it's not needed
	virtual void Tick(float DeltaTime) override;

	void Fire();

	void StopFire();

	/* Equips the Weapon */
	void Interact(APlayerCharacter* PlayerCharacter);

	void ApplyDamage(AGameCharacter* HitGameCharacter, FVector Location);


private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon")
	USkeletalMeshComponent* SkeletalMeshComponent;

	/* Collision used to check if player can interact with this Actor. In this case, equip it.*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UNiagaraSystem* BulletTracerSystem;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UMatineeCameraShake> FireShakeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	bool bIsAutomatic;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TEnumAsByte<EDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing Properties")
	float RateOfFireSeconds;

	bool bIsFiring;

// -- Spread -- //	Spread is the angle between the line being aimed down and the cone whose center is colinear to that line 
	// The amount of spread the weapon always has
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing Properties")
	float BaseSpread;

	// The maximum value of BaseSpread + AccumulatedSpread
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing Properties")
	float MaxSpread;

	/* [EquippedPlayerCharacter->Velocity.Size(), EquippedPlayerCharacter->MaxWalkingSpeed] --> [0, MaxMovementSpread] */
	float MovementSpread;

	// Maximum amount of spread that can be incurred as a consequence of moving.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing Properties")
	float MaxMovementSpread;

	// Spread Applied while jumping
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing Properties")
	float JumpSpread;

	// Rate at which spread grows per a calling of Fire()
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing Properties")
	float SpreadGrowth;

	// degrees per second the weapon decays while firing
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing Properties")
	float FiringSpreadDecay;

	// degrees per second the weapon decays while NOT firing
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing Properties")
	float NotFiringSpreadDecay;

	// Amount of spread incurred from firing the weapon (ie not the BaseSpread);
	UPROPERTY(VisibleAnywhere, Category = "Weapon|Firing Properties")
	float AccumulatedSpread;



	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing Properties")
	UCurveFloat* SpreadGrowthCurve; // TODO: Implement or remove
	

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing Properties")
	int MagazineCapacity;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Firing Properties")
	int CurrentAmmo;

	FTimerHandle WeaponHandle;

	APlayerCharacter* EquippedPlayerCharacter;	// PlayerCharacter the weapon is equipped to

	UTextBlock* AmmoTextBlock; // Stored on weapon equip so weapon can update it's values
	

public:
	FORCEINLINE float GetSpread() { return BaseSpread + AccumulatedSpread + MovementSpread + (JumpSpread * EquippedPlayerCharacter->GetCharacterMovement()->IsFalling()); }

	FORCEINLINE void SetEquippedPlayerCharacter(APlayerCharacter* _EquippedPlayerCharacter) 
	{
		EquippedPlayerCharacter = _EquippedPlayerCharacter;
		SetActorTickEnabled(true);
	}

	FORCEINLINE void SetAmmoTextBlock(UTextBlock* _AmmoTextBlock) 
	{  
		AmmoTextBlock = _AmmoTextBlock; 
		if (AmmoTextBlock) AmmoTextBlock->SetText(FText::FromString(FString::FromInt(CurrentAmmo)));
	}

	/* Placeholder until we do a better reload system */
	FORCEINLINE void Reload() 
	{ 
		CurrentAmmo = MagazineCapacity; 
		if (AmmoTextBlock) AmmoTextBlock->SetText(FText::FromString(FString::FromInt(CurrentAmmo)));
	}
};
