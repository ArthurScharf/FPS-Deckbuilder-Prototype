
#include "Weapon.h"

#include "FPS_Deckbuilder/Character/PlayerCharacter.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	RootComponent = SkeletalMeshComponent;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetSphereRadius(50.f);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereComponentBeginOverlap);
	SphereComponent->SetupAttachment(RootComponent);
}


void AWeapon::BeginPlay()
{
	CurrentAmmo = MagazineCapacity;
}


void AWeapon::OnSphereComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (!PlayerCharacter) return; 

	UE_LOG(LogTemp, Warning, TEXT("AWeapon::OnSphereComponentBeginOverlap"));

	PlayerCharacter->EquipWeapon(this);
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

