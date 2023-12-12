
#include "Weapon.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Curves/CurveFloat.h" // TODO: Implement or remove
#include "FPS_Deckbuilder/Character/EnemyCharacter.h"
#include "FPS_Deckbuilder/CommonHeaders/DamagePackage.h"
#include "FPS_Deckbuilder/CommonHeaders/TraceChannelDefinitions.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Projectile.h"

#include "Sound/SoundCue.h"



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
	FVector End;
	FRotator EyeRotation;
	FRotator Rotation;
	EquippedPlayerCharacter->GetCameraViewPoint(Start, EyeRotation);

	// Firing projectile, or performing line trace & applying damage
	// TODO: Clean this up
	if (ProjectileClass)
	{
		End = Start + EyeRotation.Vector() * 100000.f; // A large enough number that a player couldn't pheasibly aim at something at that distance
		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);

		// FVector Location = GetActorLocation() + GetActorForwardVector() * 50.f; // TODO: MilitaryWeaponsSilver and Dark have weapons misaligned and can't reimport to fix. Reimplement once we have out own weapon meshes
		FVector Location = SkeletalMeshComponent->GetSocketLocation(FName("MuzzleFlash"));
		if (HitResult.bBlockingHit) { Rotation = (HitResult.ImpactPoint - Location).Rotation(); }
		else						{ Rotation = (End				    - Location).Rotation(); }

		float Spread = GetSpread();
		// TODO: Change spread to a circle instead of a square
		Rotation = Rotation + FRotator(FMath::RandRange(-Spread, Spread), FMath::RandRange(-Spread, Spread), 0.f);
		FTransform Transform = FTransform(Rotation, Location);
		AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass, Transform);
		Projectile->OnBeginOverlapNotify.AddDynamic(this, &AWeapon::ApplyDamage);
		Projectile->SetImpactPackageMap(ImpactPackageMap);
		UGameplayStatics::FinishSpawningActor(Projectile, Transform);
	}//~ Projectile
	else
	{
		float Spread = GetSpread();
		// TODO: Change spread to a circle instead of a square
		Rotation = FRotator(FMath::RandRange(-Spread, Spread), FMath::RandRange(-Spread, Spread), 0.f);
		End = Start + (Rotation + EyeRotation).Vector() * 100000.f;
		
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(EquippedPlayerCharacter);
		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

		// A Grotesque hack so blueprint only entities like those spawned from cards can receive hits
		if (HitResult.bBlockingHit)
		{
			FHitResult UnusedHitResult;
			HitResult.Actor->ReceiveHit(nullptr, nullptr, nullptr, false, FVector(0, 0, 0), FVector(0, 0, 0), FVector(0, 0, 0), HitResult);
		}
		
		// -- Applying Damage to hit EnemyCharacter -- //
		AEnemyCharacter* HitEnemyCharacter = Cast<AEnemyCharacter>(HitResult.Actor);
		if (HitEnemyCharacter)
		{
			HitEnemyCharacter->SetHitBoneName(HitResult.BoneName);
			ApplyDamage(HitEnemyCharacter, HitResult);
		}
		else if (HitResult.Actor != nullptr && HitResult.Actor->Tags.Num() > 0)
		{
			// -- FXs -- //
			if (ImpactPackageMap.Contains(HitResult.Actor->Tags[0]))
			{
				FImpactPackage ImpactPackage = ImpactPackageMap[HitResult.Actor->Tags[0]];
				if (ImpactPackage.IsValid())
				{
					UGameplayStatics::PlaySoundAtLocation(this, ImpactPackage.ImpactCue, FVector(HitResult.ImpactPoint));
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						this,
						ImpactPackage.ImpactSystem,
						HitResult.ImpactPoint,
						HitResult.ImpactNormal.Rotation()
					);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("AWeapon::Fire -- !ImpactSystem || !ImpactCue"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("AWeapon::Fire / %s -- Don't have mapped value for hit surface"), *GetName());
			}
		}
		// - Tracer Effect - //
		if (BulletTracerSystem)
		{
			FVector MuzzleLocation = SkeletalMeshComponent->GetSocketLocation(FName("MuzzleFlash"));

			UNiagaraComponent* Tracer = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTracerSystem, MuzzleLocation);

			FVector EndLocation;
			if (HitResult.bBlockingHit)
			{
				EndLocation = (HitResult.Location - MuzzleLocation);
				Tracer->SetVariableVec3(FName("Direction"), EndLocation.GetSafeNormal());
				Tracer->SetVariableVec3(FName("EndLocation"), EndLocation);
			}
			else
			{
				EndLocation = (End - MuzzleLocation);
				Tracer->SetVariableVec3(FName("Direction"), EndLocation.GetSafeNormal());
				Tracer->SetVariableVec3(FName("EndLocation"), EndLocation);
			}
		}
	}//~ HitScan

	// -- Sound & Animation -- //
	// NOTE: Sounds are notifies on the animation.
	SkeletalMeshComponent->Stop();
	SkeletalMeshComponent->Play(false);
	if (FireShakeClass) { EquippedPlayerCharacter->ShakeCamera(FireShakeClass); }
	


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




void AWeapon::ApplyDamage(AGameCharacter* HitGameCharacter, const FHitResult& HitResult)
{
	FDamageStruct DamageStruct;
	DamageStruct.Damage = this->Damage;
	DamageStruct.DamageType = this->DamageType;
	DamageStruct.DamageCauser = this->EquippedPlayerCharacter;
	DamageStruct.DamageLocation = HitResult.ImpactPoint;
	DamageStruct.HitResult = HitResult;

	// UE_LOG(LogTemp, Warning, TEXT("BEFORE: %f"), DamageStruct.Damage);
	// -- Notifying Observers -- //
	if (EquippedPlayerCharacter)
	{
		for (FOnApplyDamageDelegate Delegate : EquippedPlayerCharacter->Observers_OnApplyDamage)
		{
			if (Delegate.IsBound()) { Delegate.Execute(DamageStruct); }
			else { UE_LOG(LogTemp, Error, TEXT("APlayerCharacter::NotifyOfDamageDealt -- Delegate Not Bound")); }

		}
	}
	// UE_LOG(LogTemp, Warning, TEXT("After : %f"), DamageStruct.Damage);


	HitGameCharacter->ReceiveDamage(DamageStruct);
}
