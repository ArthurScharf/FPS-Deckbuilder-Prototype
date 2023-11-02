

#include "Card.h"
#include "FPS_Deckbuilder/Weapon/Projectile.h"



AActor* UCard::SpawnActor(TSubclassOf<AActor> Class, const FRotator Rotation, const FVector Location)
{
	if (!PlayerCharacter) { UE_LOG(LogTemp, Error, TEXT("UCard::SpawnActor -- !PlayerCharacter")); return nullptr; } // I'd need to check for nullptr whenever I spawn an actor with a card. Card's aren't spawning super fast so maybe this is ok?

	FActorSpawnParameters SpawnParams;
	return PlayerCharacter->GetWorld()->SpawnActor(Class, &Location, &Rotation, SpawnParams);
}



AProjectile* UCard::SpawnProjectile(TSubclassOf<AProjectile> ProjectileClass, const FRotator Rotation, const FVector Location)
{
	if (!PlayerCharacter) { UE_LOG(LogTemp, Error, TEXT("UCard::SpawnProjectile -- !PlayerCharacter")); return nullptr; } // I'd need to check for nullptr whenever I spawn an actor with a card. Card's aren't spawning super fast so maybe this is ok?

	FTransform Transform = FTransform(Rotation, Location);
	AProjectile* Projectile = PlayerCharacter->GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass, Transform);
	// Projectile->OnBeginOverlapNotifyEvent.AddUObject(this, &UCard::SpawnedActorCallback);
	Projectile->FinishSpawning(Transform);
	return Projectile;
}

