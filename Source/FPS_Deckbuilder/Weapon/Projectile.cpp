#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "FPS_Deckbuilder/Character/EnemyCharacter.h"
#include "FPS_Deckbuilder/CommonHeaders/DamagePackage.h"

#include "FPS_Deckbuilder/CommonHeaders/TraceChannelDefinitions.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"



AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;


	// NOTE: to avoid a bug that I'm not understanding, collision profile is set in blueprint
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	//SphereComponent->SetCollisionObjectType(ECC_PlayerAttack);
	//SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//SphereComponent->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap);
	//SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereComponentBeginOverlap);
	SphereComponent->SetupAttachment(RootComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
}

void AProjectile::OnSphereComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("AProjectile::OnSphereComponentBeginOverlap / %s -- %s"), *GetName(), *(OtherActor->GetName()));
	
	// A Grotesque hack so blueprint only entities like those spawned from cards can receive hits
	if (SweepResult.bBlockingHit)
	{
		FHitResult UnusedHitResult;
		SweepResult.Actor->ReceiveHit(nullptr, nullptr, nullptr, false, FVector(0, 0, 0), FVector(0, 0, 0), FVector(0, 0, 0), UnusedHitResult);
	}

	// NOTE: Shouldn't be doing multiple casts like this
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OtherActor);
	if (EnemyCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("AProjectile::OnSphereComponentBeginOverlap -- Hit an Enemy"));
		EnemyCharacter->SetHitBoneName(SweepResult.BoneName); // Setting hit bone name so EnemyCharacter can handle bone specific hit conditions
		OnBeginOverlapNotify.Broadcast(EnemyCharacter, SweepResult);
	}
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("AProjectile::OnSphereComponentBeginOverlap -- Hit an Player"));
		OnBeginOverlapNotify.Broadcast(PlayerCharacter, SweepResult);
	}

	// -- FX -- //
	if (OtherActor != nullptr && OtherActor->Tags.Num() > 0 && ImpactPackageMap.Contains(OtherActor->Tags[0]))
	{
		FImpactPackage ImpactPackage = ImpactPackageMap[OtherActor->Tags[0]];
		if (ImpactPackage.IsValid())
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactPackage.ImpactCue, FVector(SweepResult.ImpactPoint));
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				this,
				ImpactPackage.ImpactSystem,
				SweepResult.ImpactPoint,
				SweepResult.ImpactNormal.Rotation()
			);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AProjectile::OnSphereComponentBeginOverlap / %s -- Invalid ImpactPackage"), *GetName());
		}
	}
	else
	{	// - Default ImpactPackage - //
		
		if (DefaultImpactPackage.IsValid())
		{
			UGameplayStatics::PlaySoundAtLocation(this, DefaultImpactPackage.ImpactCue, FVector(SweepResult.ImpactPoint));
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				this,
				DefaultImpactPackage.ImpactSystem,
				SweepResult.ImpactPoint,
				SweepResult.ImpactNormal.Rotation()
			);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AProjectile::OnSphereComponentBeginOverlap / %s -- Invalid DefaultImpactPackage"), *GetName());
		}
	}

	// UE_LOG(LogTemp, Warning, TEXT("AProjectile::OnSphereComponentBeginOverlap"));
	Destroy();
}
