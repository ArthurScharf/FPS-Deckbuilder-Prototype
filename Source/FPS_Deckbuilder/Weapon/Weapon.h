
#pragma once

#include "CoreMinimal.h"

#include "FPS_Deckbuilder/Interactable.h"

#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USkeletalMeshComponent;
class USphereComponent;


UCLASS()
class FPS_DECKBUILDER_API AWeapon : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AWeapon();


public:
	virtual void BeginPlay() override;

	void Fire();

	void StopFire();

	/* Equips the Weapon */
	void Interact(APlayerCharacter* PlayerCharacter);




private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMeshComponent;
	/* Collision used to check if player can interact with this Actor. In this case, equip it.*/
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereComponent;

	//UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	//TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float RateOfFireSeconds;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsAutomatic;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	int MagazineCapacity;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	int CurrentAmmo;


	FTimerHandle WeaponHandle;
};
