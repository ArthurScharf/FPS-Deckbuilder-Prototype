
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
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon")
	USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon|Gameplay Properties")
	bool bIsAutomatic;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon|Gameplay Properties")
	float Damage;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon|Gameplay Properties")
	TEnumAsByte<EDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon|Gameplay Properties")
	float RateOfFireSeconds;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon|Gameplay Properties")
	int MagazineCapacity;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Weapon|Gameplay Properties")
	int CurrentAmmo;

	FTimerHandle WeaponHandle;

	APlayerCharacter* EquippedPlayerCharacter;	// PlayerCharacter the weapon is equipped to
};
