

#include "Card.h"




void UCard::Use()
{
	UE_LOG(LogTemp, Warning, TEXT("UCard::Use -- %s"), *(Name.ToString()));

	if (!PlayerCharacter) { UE_LOG(LogTemp, Error, TEXT("UCard::Use -- !PlayerCharacter")); }
	// PlayerCharacter->GetWorld()->SpawnActor(); 
}