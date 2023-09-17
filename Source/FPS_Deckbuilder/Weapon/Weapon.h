
#pragma once

#include "CoreMinimal.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"

#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class FPS_DECKBUILDER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();


public:
	virtual void BeginPlay() override;

	void Fire();

	void StopFire();

	UFUNCTION()
	void OnSphereComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);





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
