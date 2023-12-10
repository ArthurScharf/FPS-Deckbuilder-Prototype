
#include "GameCharacter.h"
#include "Components/HorizontalBox.h"
#include "FPS_Deckbuilder/Character/GameCharacterMovementComponent.h"
#include "FPS_Deckbuilder/Character/StatusEffect.h"
#include "FPS_Deckbuilder/UI/StatusEffectWidget.h"
#include "FPS_Deckbuilder/Weapon/Projectile.h"



AGameCharacter::AGameCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

//AGameCharacter::AGameCharacter(const FObjectInitializer& ObjectInitializer)
//{
//	PrimaryActorTick.bCanEverTick = true;
//
//	// -- Dashing -- //
//	DashDirection.Normalize();
//	if (bIsDashing)
//	{
//		GetCharacterMovement()->AddInputVector(DashDirection);
//	}
//}

void AGameCharacter::BeginPlay()
{	
	if (MaxHealth <= 0) MaxHealth = 1; // Avoids divide by zero errors
	
	Health = MaxHealth;
	
	// -- Dashing -- //
	bIsDashing = false;
	DashSeconds = DashDistance / DashSpeed;


	Super::BeginPlay();
}

void AGameCharacter::Tick(float DeltaTime)
{
	// -- Dashing -- //
	DashDirection.Normalize();
	if (bIsDashing)
	{
		//FTimerHandle DashHandle;
		//GetWorldTimerManager().GetTimerElapsed(DashHandle);

		GetCharacterMovement()->AddInputVector(DashDirection);
	}
}

void AGameCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogTemp, Warning, TEXT("AGameCharacter::EndPlay"));

}

void AGameCharacter::NotifyOfDamageDealt(FDamageStruct& DamageStruct)
{
	// UE_LOG(LogTemp, Warning, TEXT("AGameCharacter::NotifyOfDamageDealt"));
	
	// BUG: possible for Observers that should be no longer valid to still be called. This is also slow
	TArray<FOnDamageDealtDelegate> LocalObserverArray(Observers_OnDamageDealt); // Delegates can self remove. This avoids bugs associated with removing during iteration.

	for (FOnDamageDealtDelegate Delegate : LocalObserverArray)
	{
		if (Delegate.IsBound()) { Delegate.Execute(DamageStruct); }
		else { UE_LOG(LogTemp, Error, TEXT("AGameCharacter::NotifyOfDamageDealt / %s() -- Delegate not bound"), *Delegate.GetFunctionName().ToString()); }
	}
}

void AGameCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("%s / AGameCharacter::Die"), *GetName());

	// -- Cleanup Status Effects -- //
	TArray<UStatusEffect*> LocalEffects(StatusEffects);
	for (UStatusEffect* Effect : StatusEffects)
	{
		Effect->Cleanup();
	}

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

bool AGameCharacter::Dash()
{
	// TODO: Character speed is being hardcoded in this method. Should fetch dynamically
	UCharacterMovementComponent* CharMovement = GetCharacterMovement();

	if (!CharMovement || bIsDashing || DashDirection.Size() == 0) return false;

	bIsDashing = true;
	float Stored_MaxWalkSpeed = CharMovement->GetMaxSpeed();
	CharMovement->MaxWalkSpeed = DashSpeed;
	CharMovement->MaxAcceleration *= 20.f;
	CharMovement->bCanWalkOffLedges = false;
	// DashDirection = CharMovement->GetLastUpdateVelocity().GetSafeNormal();
	DashDirection.Z = 0;



	FTimerHandle DashHandle;
	GetWorldTimerManager().SetTimer(
		DashHandle,
		[&, CharMovement]()
		{
			if (CharMovement) // Can fail to exist if character dies during dash
			{
				UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::DashButton_Pressed -- Dash ending"));
				bIsDashing = false;
				CharMovement->MaxWalkSpeed = 600;
				CharMovement->MaxAcceleration /= 20.f;
				//CharMovement->StopMovementImmediately();
				CharMovement->StopActiveMovement();
				CharMovement->Velocity = CharMovement->Velocity.GetSafeNormal() * CharMovement->MaxWalkSpeed;
			}
		},
		DashSeconds,
		false
		);

	// A hack to prevent the character from launching themselves from edges
	FTimerHandle EdgeHandle;
	GetWorldTimerManager().SetTimer(
		EdgeHandle,
		[&, CharMovement]()
		{
			// UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::DashButton_Pressed -- Edge Timer Ending"), Stored_MaxWalkSpeed);

			CharMovement->bCanWalkOffLedges = true;
		},
		DashSeconds + 0.1f,
		false
		);
	return true;
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

void AGameCharacter::ReceiveDamage(FDamageStruct& DamageStruct, bool bTriggersStatusEffects)
{
	UE_LOG(LogTemp, Warning, TEXT("AGameCharacter::ReceiveDamage / %s -- Damage: %f, %d"), *GetName(), DamageStruct.Damage, DamageStruct.bWasPostureDamage);

	if (!LazyHealthBar) { UE_LOG(LogTemp, Error, TEXT("AGameCharacter::ReceiveDamage -- !LazyHealthBar")); return; }

	if (bTriggersStatusEffects)
	{
		// UE_LOG(LogTemp, Warning, TEXT("AGameCharacter::ReceiveDamage -- BEFORE: %f"), DamageStruct.Damage);
		TArray<FOnReceiveDamageDelegate> LocalObservers(Observers_OnReceiveDamage);
		for (FOnReceiveDamageDelegate Delegate : LocalObservers)
		{
			if (Delegate.IsBound()) { Delegate.Execute(DamageStruct); }
			else { UE_LOG(LogTemp, Error, TEXT("AGameCharacter::ReceiveDamage / %s -- Delegate Not Bound"), *Delegate.GetFunctionName().ToString()); }
		}
		// UE_LOG(LogTemp, Warning, TEXT("AGameCharacter::ReceiveDamage -- AFTER : %f"), DamageStruct.Damage);
	}
	
	if (!DamageStruct.bWasPostureDamage)
	{
		DamageStruct.DamageReceiver = this;
		Health -= DamageStruct.Damage;
		if (Health <= 0.f) { DamageStruct.bWasLethal = true; }
		else if (Health >= MaxHealth) { Health = MaxHealth; }

		if (DamageStruct.DamageCauser) DamageStruct.DamageCauser->NotifyOfDamageDealt(DamageStruct);
		LazyHealthBar->SetPercent(Health / MaxHealth);

		if (DamageStruct.bWasLethal) Die();
	}
}

UStatusEffect* AGameCharacter::InstantiateStatusEffect(TSubclassOf<UStatusEffect> Class, AGameCharacter* InstigatingGameCharacter)
{
	if (!Class)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameCharacter::InstantiateStatusEffect / %s -- !Class"), *GetName());
		return nullptr;
	}

	// -- Notifying Pre-Observers -- //
	TArray<FPreInstantiateStatusEffect> LocalPreObservers(Observers_PreInstantiateStatusEffect);
	for (FPreInstantiateStatusEffect Delegate : LocalPreObservers)
	{
		if (Delegate.IsBound()) { Delegate.Execute(Class); }
		else { UE_LOG(LogTemp, Error, TEXT("AGameCharacter::InstantiateStatusEffect / %s -- Pre-Delegate Not Bound"), *Delegate.GetFunctionName().ToString()); }
	}

	UStatusEffect* Effect = nullptr; // Both stack and non-stack control paths set this. Needed for notifying post-observers 

	bool bWasStacked = false;
	// -- Searching for stackable duplicate -- //
	for (UStatusEffect* ToStackEffect : StatusEffects)
	{
		if (ToStackEffect->GetClass() == Class && ToStackEffect->IsStackable())
		{
			ToStackEffect->IncrementNumInstances();
			Effect = ToStackEffect;
			bWasStacked = true;
			break;
		}
	}

	// -- Failed to stack. Instantiating new effect -- //
	if (!bWasStacked)
	{
		// -- No Stackable instance. Creating and adding new instance of of type Class -- //
		Effect = NewObject<UStatusEffect>(this, Class);
		UTexture2D* Texture = Effect->GetTexture();
		if (Texture)
		{
			if (StatusEffectWidgetClass)
			{
				UStatusEffectWidget* StatusEffectWidget = CreateWidget<UStatusEffectWidget>(StatusEffectHorizontalBox, StatusEffectWidgetClass);
				StatusEffectWidget->Image->SetBrushFromTexture(Texture);
				StatusEffectHorizontalBox->AddChildToHorizontalBox(StatusEffectWidget);
				Effect->SetWidget(StatusEffectWidget);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("AGameCharacter::InstantiateStatusEffect / %s -- !StatusEffectWidgetClass"), *GetName());
			}
		}
		StatusEffects.Add(Effect);
		Effect->Init(this, InstigatingGameCharacter);
	}

	// -- Notifying Post-Observers -- //
	TArray<FPostInstantiateStatusEffect> LocalPostObservers(Observers_PostInstantiateStatusEffect);
	for (FPostInstantiateStatusEffect Delegate : LocalPostObservers)
	{
		if (Delegate.IsBound()) { Delegate.Execute(Effect); }
		else { UE_LOG(LogTemp, Error, TEXT("AGameCharacter::InstantiateStatusEffect / %s -- Post-Delegate Not Bound"), *Delegate.GetFunctionName().ToString()); }
	}

	return Effect;
}

void AGameCharacter::RemoveStatusEffect(UStatusEffect* StatusEffect)
{
	UUserWidget* Widget = StatusEffect->GetWidget();
	if (Widget)
	{
		StatusEffectHorizontalBox->RemoveChild(Widget);
	}
	StatusEffects.Remove(StatusEffect);
}




AProjectile* AGameCharacter::SpawnProjectileWithoutCollision(TSubclassOf<AProjectile> ProjectileClass, FTransform Transform)
{
	AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass, Transform);
	Projectile->SetActorEnableCollision(false);
	Projectile->FinishSpawning(Transform);
	AddDependentActor(Projectile);
	return Projectile;
}