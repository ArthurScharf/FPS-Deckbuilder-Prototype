
#include "GameCharacter.h"
#include "FPS_Deckbuilder/Character/GameCharacterMovementComponent.h"
#include "FPS_Deckbuilder/Character/StatusEffect.h"

AGameCharacter::AGameCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

//AGameCharacter::AGameCharacter(const FObjectInitializer& ObjectInitializer)
//{
//	PrimaryActorTick.bCanEverTick = true;
//}


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




void AGameCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("%s / AGameCharacter::Die"), *GetName());

	AttemptDestroy();

	FTimerHandle DeathHandle;
	GetWorldTimerManager().SetTimer(
		DeathHandle,
		this,
		&ThisClass::AttemptDestroy,
		5.f,
		true
	);
}


void AGameCharacter::AttemptDestroy()
{
	bool ToDestroy = true;
	for (int i = 0; i < DependentActors.Num(); i++)
	{
		if (DependentActors[i] != nullptr) // BUG && IsValid(DependentActors[i])
		{
			ToDestroy = false;
		};
	}
	if (ToDestroy)
	{
		UE_LOG(LogTemp, Warning, TEXT("AGameCharacter::ToBeDestroyed / %s -- Destroying"), *GetName());
		Destroy();
	}
	else
	{
		FTimerHandle DeathHandle;
		GetWorldTimerManager().SetTimer(
			DeathHandle,
			this,
			&ThisClass::AttemptDestroy,
			5.f,
			false
		);
	}
}



void AGameCharacter::ReceiveDamage(FDamageStruct& DamageStruct)
{
	UE_LOG(LogTemp, Warning, TEXT("AGameCharacter::ReceiveDamage -- Damage: %f"), DamageStruct.Damage); // TEMP

	if (!LazyHealthBar) { UE_LOG(LogTemp, Error, TEXT("AGameCharacter::ReceiveDamage -- !LazyHealthBar")); return; }

	DamageStruct.DamageReceiver = this;


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




void AGameCharacter::InstantiateStatusEffect(TSubclassOf<UStatusEffect> Class)
{
	// Searching for stackable duplicate
	for (UStatusEffect* Effect : StatusEffects)
	{
		if (Effect->GetClass() == Class && Effect->IsStackable())
		{
			Effect->IncrementNumInstances();
			return;
		}
	}

	// No Stackable instance. Creating and adding new instance of of type Class
	UStatusEffect* Effect = NewObject<UStatusEffect>(this, Class);
	StatusEffects.Add(Effect);
	Effect->Init(this);
}



void AGameCharacter::RemoveStatusEffect(UStatusEffect* StatusEffect)
{
	int NumRemoved = StatusEffects.Remove(StatusEffect);
}