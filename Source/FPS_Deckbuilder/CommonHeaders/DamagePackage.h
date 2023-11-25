#pragma once

#include "CoreMinimal.h"
#include "DamagePackage.generated.h"

class AGameCharacter;

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