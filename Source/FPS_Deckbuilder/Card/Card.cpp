

#include "Card.h"


UCard::UCard()
{
	UE_LOG(LogTemp, Warning, TEXT("UCard::UCard"));

}


AActor* UCard::SpawnActor(TSubclassOf<AActor> Class, const FRotator Rotation, const FVector Location)
{
	UE_LOG(LogTemp, Warning, TEXT("UCard::SpawnActor -- Spawning: %s"), *(Class->GetName()));

	if (!PlayerCharacter) { UE_LOG(LogTemp, Error, TEXT("UCard::SpawnActor -- !PlayerCharacter")); return nullptr; } // I'd need to check for nullptr whenever I spawn an actor with a card. Card's aren't spawning super fast so maybe this is ok?

	FActorSpawnParameters SpawnParams;
	return PlayerCharacter->GetWorld()->SpawnActor(Class, &Location, &Rotation, SpawnParams);
}

