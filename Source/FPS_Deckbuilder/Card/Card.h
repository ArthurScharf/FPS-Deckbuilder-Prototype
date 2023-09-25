
#pragma once

#include "CoreMinimal.h"

#include "FPS_Deckbuilder/Character/PlayerCharacter.h"

#include "UObject/NoExportTypes.h"
#include "Card.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class FPS_DECKBUILDER_API UCard : public UObject
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable) // BlueprintImplementableEvent
	void Use();

private:
	// PlayerCharacter this is held by
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Card")
	FName Name;

public:
	// FORCEINLINE FName GetName() { return Name; }

	FORCEINLINE void SetPlayerCharacter(APlayerCharacter* _PlayerCharacter) { PlayerCharacter = _PlayerCharacter; }
};
