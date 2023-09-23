
#include "Weapon.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Curves/CurveFloat.h" // TODO: Implement or remove
#include "FPS_Deckbuilder/Character/EnemyCharacter.h"
#include "FPS_Deckbuilder/CommonHeaders/DamagePackage.h"
#include "FPS_Deckbuilder/CommonHeaders/TraceChannelDefinitions.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"

#include "DrawDebugHelpers.h"



AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = SkeletalMeshComponent;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetSphereRadius(50.f);
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	SphereComponent->SetupAttachment(RootComponent);
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmo = MagazineCapacity;
}


// I don't like how many checks are in this tick function
void AWeapon::Tick(float DeltaTime)
{
	if (bIsFiring)
	{
		AccumulatedSpread -= FiringSpreadDecay * DeltaTime;
	}
	else 
	{
		AccumulatedSpread -= NotFiringSpreadDecay * DeltaTime;
	}

	if (EquippedPlayerCharacter) 
	{ 
		MovementSpread = EquippedPlayerCharacter->GetPercentOfMaxWalkingSpeed() * MaxMovementSpread;
	}

	if (AccumulatedSpread <= 0)
	{
		AccumulatedSpread = 0;
		if (!EquippedPlayerCharacter) { UE_LOG(LogTemp, Warning, TEXT("AWeapon::Tick -- Disabling Tick")); SetActorTickEnabled(false); }// So weapons on the map don't continue to tick
	}
}



void AWeapon::Fire()
{
	bIsFiring = true;

	if (CurrentAmmo == 0)
	{
		// play sound and auto reload
		return;
	}
	CurrentAmmo--;
	if (AmmoTextBlock) AmmoTextBlock->SetText( FText::FromString(FString::FromInt(CurrentAmmo)) );

	FHitResult HitResult;
	FVector Start;
	FRotator EyeRotation;
	EquippedPlayerCharacter->GetCameraViewPoint(Start, EyeRotation);

	// Firing projectile, or performing line trace & applying damage
	// TODO: Clean this up
	if (ProjectileClass)
	{
		FVector End = Start + EyeRotation.Vector() * 100000.f; // A large enough number that a player couldn't pheasibly aim at something at that distance
		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);

		FVector Location = GetActorLocation() + GetActorForwardVector() * 50.f;
		FRotator Rotation;
		if (HitResult.bBlockingHit) { Rotation = (HitResult.ImpactPoint - Location).Rotation(); }
		else						{ Rotation = (End				    - Location).Rotation(); }


		float Spread = GetSpread();
		// TODO: Change spread to a circle instead of a square
		Rotation = Rotation + FRotator(FMath::RandRange(-Spread, Spread), FMath::RandRange(-Spread, Spread), 0.f);
		FTransform Transform = FTransform(Rotation, Location);
		AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass, Transform);
		Projectile->OnBeginOverlapNotifyEvent.AddUObject(this, &AWeapon::ApplyDamage);
		UGameplayStatics::FinishSpawningActor(Projectile, Transform);
	}
	else
	{
		float Spread = GetSpread();
		// TODO: Change spread to a circle instead of a square
		FRotator Rotation = FRotator(FMath::RandRange(-Spread, Spread), FMath::RandRange(-Spread, Spread), 0.f);
		FVector End = Start + (Rotation + EyeRotation).Vector() * 100000.f;
		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.5, (uint8)0U, 5.f);

		AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(HitResult.Actor);
		if (EnemyCharacter)
		{
			ApplyDamage(EnemyCharacter);
		}
		// TODO: Tracer Particle effect
	}

	// Accumulating Spread
	AccumulatedSpread += SpreadGrowth;
	if (AccumulatedSpread + BaseSpread >= MaxSpread) AccumulatedSpread = MaxSpread - BaseSpread;

	bIsAutomatic ? GetWorldTimerManager().SetTimer(WeaponHandle, [&]() {Fire();}, RateOfFireSeconds, true) : bIsFiring = false;
}



void AWeapon::StopFire()
{
	bIsFiring = false;
	GetWorldTimerManager().ClearTimer(WeaponHandle);
}



void AWeapon::Interact(APlayerCharacter* PlayerCharacter)
{
	PlayerCharacter->EquipWeapon(this);
	SetActorTickEnabled(true);
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