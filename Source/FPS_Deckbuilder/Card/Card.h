
#pragma once

#include "CoreMinimal.h"

#include "FPS_Deckbuilder/Character/PlayerCharacter.h"

#include "UObject/NoExportTypes.h"
#include "Card.generated.h"




/*
* Struct because costs aren't all the same data type
*/
USTRUCT(BlueprintType)
struct FCost
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Resource_X = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Resource_Y = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Resource_Z = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Money;
};






/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class FPS_DECKBUILDER_API UCard : public UObject
{
	GENERATED_BODY()

public:
	UCard();

	UFUNCTION(BlueprintCallable) // BlueprintImplementableEvent
	void Use();

private:
	// PlayerCharacter this is held by
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Card")
	FName Name;

	// Unlike PlayerCharacter::Resources, Cost is 5 long. This is so the card can expect to spend money, or health
	UPROPERTY(EditAnywhere, Category = "Card")
	FCost Cost;

public:
	FORCEINLINE FCost GetCost() { return Cost; }

	FORCEINLINE void SetPlayerCharacter(APlayerCharacter* _PlayerCharacter) { PlayerCharacter = _PlayerCharacter; }
};
