#pragma once

#include "CoreMinimal.h"
#include "DamagePackage.generated.h"

class AGameCharacter;
class AProjectile;

UENUM(BlueprintType)
enum EDamageType
{
	EDT_Physical   UMETA(DisplayName = "Physical"),
	EDT_Fire       UMETA(DisplayName = "Fire"),
	EDT_Electrical UMETA(DisplayName = "Electrical"),
	EDT_Shadow     UMETA(DisplayName = "Shadow"),
	EDT_MAX        UMETA(DisplayName = "MAX")
};


USTRUCT(BlueprintType)
struct FDamageStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	float Damage;

	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EDamageType> DamageType;

	// DEPRECATED. Use Locations stored within HitResult
	UPROPERTY(BlueprintReadWrite)
	FVector DamageLocation;

	UPROPERTY(BlueprintReadWrite)
	AGameCharacter* DamageCauser;
	
	UPROPERTY(BlueprintReadWrite)
	AGameCharacter* DamageReceiver;

	UPROPERTY(BlueprintReadWrite)
	FName HitBoneName;

	UPROPERTY(BlueprintReadWrite)
	bool bWasLethal = false;

	UPROPERTY(BlueprintReadWrite)
	bool bTriggersStatusEffects;

	UPROPERTY(BlueprintReadWrite)
	FHitResult HitResult;

	/*
	* We don't always want to treat each individual projectile as it's own attack (Shotguns are an example of this).
	* Projectiles have an AttackID properties that AWeapon::ApplyDamage can use to determine wich Enemies were hit by a single 
	* multi-projectile or multi-hitscan attack.
	* There are also potentially other ways to interact with a projectile that's been spawned from another source (Enemy or Card).
	* As such, it's necassary to store a pointer to a projectile.
	*/
	UPROPERTY(BlueprintReadWrite)
	AProjectile* DamagingProjectile;

	UPROPERTY(BlueprintReadWrite)
	bool bWasPostureDamage = false;
};



//public:
//	FString ToString()
//	{
//		return FString::Printf(
//			TEXT("Damage: %f, Type: %s, DamageCauser: %s, DamageReceiver: %s, HitBoneName: %s"),
//			Damage,
//			*UEnum::GetValueAsString(DamageType),
//			DamageCauser ? *DamageCauser->GetName() : *FString("None"),
//			DamageReceiver ? *DamageReceiver->GetName() : *FString("None"),
//			*HitBoneName.ToString(),
//			bWasLethal ? FString("True") : FString("False")
//		);
//	}