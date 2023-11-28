

#include "Card.h"
#include "FPS_Deckbuilder/Weapon/Projectile.h"
#include "Kismet/KismetSystemLibrary.h"





AActor* UCard::SpawnActor(TSubclassOf<AActor> Class, const FRotator Rotation, const FVector Location)
{
	if (!PlayerCharacter) { UE_LOG(LogTemp, Error, TEXT("UCard::SpawnActor -- !PlayerCharacter")); return nullptr; } // I'd need to check for nullptr whenever I spawn an actor with a card. Card's aren't spawning super fast so maybe this is ok?

	FActorSpawnParameters SpawnParams;
	return PlayerCharacter->GetWorld()->SpawnActor(Class, &Location, &Rotation, SpawnParams);
}


// DEPRECATED //
AProjectile* UCard::SpawnProjectile(TSubclassOf<AProjectile> ProjectileClass, const FRotator Rotation, const FVector Location)
{
	// DEPRECATED //
	if (!PlayerCharacter) { UE_LOG(LogTemp, Error, TEXT("UCard::SpawnProjectile -- !PlayerCharacter")); return nullptr; } // I'd need to check for nullptr whenever I spawn an actor with a card. Card's aren't spawning super fast so maybe this is ok?

	FTransform Transform = FTransform(Rotation, Location);
	AProjectile* Projectile = PlayerCharacter->GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass, Transform);
	//Projectile->OnBeginOverlapNotify.AddDynamic(this, &UCard::SpawnedActorCallback);
	Projectile->FinishSpawning(Transform);
	return Projectile;
}



void UCard::LineTraceSingleByChannel(FHitResult& OutHit, const FVector& Start, const FVector& End, ECollisionChannel TraceChannel, TArray<AActor*> ActorsToIgnore, bool bIgnoreUserPlayerCharacter) const
{
	FCollisionQueryParams Params;
	Params.AddIgnoredActors(ActorsToIgnore);
	if (bIgnoreUserPlayerCharacter) Params.AddIgnoredActor(PlayerCharacter);
	FCollisionResponseParams ResponseParams;
	PlayerCharacter->GetWorld()->LineTraceSingleByChannel
	(
		OutHit,
		Start,
		End,
		TraceChannel,
		Params,
		ResponseParams
	);
}




bool UCard::SphereOverlapActors(const FVector Location, float Radius, const TArray< TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, UClass* ActorClassFilter, const TArray<AActor*>& ActorsToIgnore, bool bIgnoreSelf, TArray<AActor*>& OutActors)
{
	return UKismetSystemLibrary::SphereOverlapActors(
		PlayerCharacter->GetWorld(),
		Location,
		Radius,
		ObjectTypes,
		ActorClassFilter,
		ActorsToIgnore,
		OutActors
	);
}