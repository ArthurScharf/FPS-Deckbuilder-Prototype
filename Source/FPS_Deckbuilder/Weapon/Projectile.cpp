#include "Projectile.h"

#include "FPS_Deckbuilder/Character/EnemyCharacter.h"
#include "FPS_Deckbuilder/CommonHeaders/DamagePackage.h"
#include "FPS_Deckbuilder/CommonHeaders/TraceChannelDefinitions.h"
#include "Components/SphereComponent.h"
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
	UE_LOG(LogTemp, Warning, TEXT("AProjectile::OnSphereComponentBeginOverlap -- %s"), *(OtherActor->GetName()));

	
	// NOTE: Shouldn't be doing multiple casts like this
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OtherActor);
	if (EnemyCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("AProjectile::OnSphereComponentBeginOverlap -- Hit an Enemy"));
		EnemyCharacter->SetHitBoneName(SweepResult.BoneName); // Setting hit bone name so EnemyCharacter can handle bone specific hit conditions
		OnBeginOverlapNotify.Broadcast(EnemyCharacter, SweepResult);
	}
	APlayerCharacter* PlayerCharacter = Cast <APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("AProjectile::OnSphereComponentBeginOverlap -- Hit an Player"));
		OnBeginOverlapNotify.Broadcast(PlayerCharacter, SweepResult);
	}


	// UE_LOG(LogTemp, Warning, TEXT("AProjectile::OnSphereComponentBeginOverlap"));
	Destroy();
}
