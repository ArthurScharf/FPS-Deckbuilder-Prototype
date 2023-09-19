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

	UPROPERTY(BlueprintReadWrite)
	AGameCharacter* DamageCauser;
	
	UPROPERTY(BlueprintReadWrite)
	AGameCharacter* DamageReceiver;

};