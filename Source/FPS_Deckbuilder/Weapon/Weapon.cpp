
#include "Weapon.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "FPS_Deckbuilder/Character/PlayerCharacter.h"


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

	// TODO: Spawn Projectile;
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, __FUNCTION__);

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
}