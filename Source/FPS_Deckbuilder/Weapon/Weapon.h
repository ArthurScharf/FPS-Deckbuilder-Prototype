
#pragma once

#include "CoreMinimal.h"

#include "FPS_Deckbuilder/Interactable.h"
#include "FPS_Deckbuilder/Character/GameCharacter.h"

#include "GameFramework/Actor.h"
#include "Weapon.generated.h"


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

	virtual void Tick(float DeltaTime) override;

	void Fire();

	void StopFire();

	/* Equips the Weapon */
	void Interact(APlayerCharacter* PlayerCharacter);


private:
	void ApplyDamage(AGameCharacter* DamagerReceiver);

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon")
	USkeletalMeshComponent* SkeletalMeshComponent;

	/* Collision used to check if player can interact with this Actor. In this case, equip it.*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	bool bIsAutomatic;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TEnumAsByte<EDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing Properties")
	float RateOfFireSeconds;

	bool bIsFiring;

	// -- Spread -- //
	// Spread is the angle between the line being aimed down and the cone whose center is colinear to that line

	// The amount of spread the weapon always has
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing Properties")
	float BaseSpread;

	// The maximum value of BaseSpread + AccumulatedSpread
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing Properties")
	float MaxSpread;

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
	int MagazineCapacity;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Firing Properties")
	int CurrentAmmo;

	FTimerHandle WeaponHandle;

	APlayerCharacter* EquippedPlayerCharacter;	// PlayerCharacter the weapon is equipped to


public:
	FORCEINLINE float GetSpread() { return BaseSpread + AccumulatedSpread; }

	FORCEINLINE void SetEquippedPlayerCharacter(APlayerCharacter* _EquippedPlayerCharacter) 
	{
		EquippedPlayerCharacter = _EquippedPlayerCharacter;
		SetActorTickEnabled(true);
	}
};
