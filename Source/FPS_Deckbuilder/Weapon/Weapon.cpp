
#include "Weapon.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "FPS_Deckbuilder/Character/PlayerCharacter.h"
#include "FPS_Deckbuilder/CommonHeaders/DamagePackage.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = SkeletalMeshComponent;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetSphereRadius(50.f);
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);\
	SphereComponent->SetupAttachment(RootComponent);
}


void AWeapon::BeginPlay()
{
	CurrentAmmo = MagazineCapacity;
}




void AWeapon::Fire()
{
	if (CurrentAmmo == 0)
	{
		// play sound and auto reload
		return;
	}

	CurrentAmmo--;


	if (ProjectileClass)
	{
		FVector Location = GetActorLocation() + GetActorForwardVector() * 50.f; // TODO: Use MuzzleLocation socket on skeletal mesh for spawn
		FRotator Rotation = GetActorRotation(); //  TODO: Set actor rotation such that it faces where the player character is facing
		FTransform Transform = FTransform(Rotation, Location);

		AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass, Transform);
		Projectile->OnBeginOverlapNotifyEvent.AddUObject(this, &AWeapon::ApplyDamage);
		UGameplayStatics::FinishSpawningActor(Projectile, Transform);
	}
	else // HitScan
	{

	}


	if (bIsAutomatic) GetWorldTimerManager().SetTimer(WeaponHandle, [&]() {Fire();}, RateOfFireSeconds, true);
}



void AWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(WeaponHandle);
}


void AWeapon::Interact(APlayerCharacter* PlayerCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Interact"));
	PlayerCharacter->EquipWeapon(this);
	EquippedPlayerCharacter = PlayerCharacter;
}




void AWeapon::ApplyDamage(AGameCharacter* DamageReceiver)
{
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::ApplyDamage"));

	FDamageStruct DamageStruct;
	DamageStruct.Damage = Damage;
	DamageStruct.DamageReceiver = DamageReceiver;
	DamageStruct.DamageType = DamageType;

	DamageReceiver->ReceiveDamage(DamageStruct);
}