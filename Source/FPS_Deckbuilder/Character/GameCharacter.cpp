
#include "GameCharacter.h"


AGameCharacter::AGameCharacter()
{

	PrimaryActorTick.bCanEverTick = true;

}


void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void AGameCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogTemp, Warning, TEXT("AGameCharacter::EndPlay"));
}


void AGameCharacter::ReceiveDamage(FDamageStruct& DamageStruct)
{
	UE_LOG(LogTemp, Warning, TEXT("AGameCharacter::ReceiveDamage -- Damage: %f"), DamageStruct.Damage);
}