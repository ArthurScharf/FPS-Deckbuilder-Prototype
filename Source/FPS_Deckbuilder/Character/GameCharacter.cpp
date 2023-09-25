
#include "GameCharacter.h"





AGameCharacter::AGameCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AGameCharacter::BeginPlay()
{	
	if (MaxHealth <= 0) MaxHealth = 1; // Avoids divide by zero errors
	
	Health = MaxHealth;
	
	Super::BeginPlay();
}



void AGameCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogTemp, Warning, TEXT("AGameCharacter::EndPlay"));
}





void AGameCharacter::ReceiveDamage(FDamageStruct& DamageStruct)
{
	UE_LOG(LogTemp, Warning, TEXT("AGameCharacter::ReceiveDamage -- Damage: %f"), DamageStruct.Damage); // TEMP

	if (!LazyHealthBar) { UE_LOG(LogTemp, Error, TEXT("AGameCharacter::ReceiveDamage -- !LazyHealthBar")); return; }

	// TODO: HandleSpecialDamageConditions (ie hit certain bones for crits, or other effects)

	Health -= DamageStruct.Damage;

	if (Health <= 0.f)
	{
		DamageStruct.bWasLethal = true;

		if (DamageStruct.DamageCauser) DamageStruct.DamageCauser->NotifyOfDamageDealt(DamageStruct);
		Die(); 
	}

	if (DamageStruct.DamageCauser) DamageStruct.DamageCauser->NotifyOfDamageDealt(DamageStruct);
	LazyHealthBar->SetPercent(Health / MaxHealth);
}