#pragma once

#include "CoreMinimal.h"
#include "FPS_Deckbuilder/Character/GameCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameCharacterMovementComponent.generated.h"


class AGameCharacter;


UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None		UMETA(Hidden),
	CMOVE_Dash		UMETA(DisplayName = "Dash"),
	CMOVE_MAX		UMETA(Hidden)
};


/*
* https://www.youtube.com/watch?v=-iaw-ifiUok&t=290s
*/
UCLASS()
class FPS_DECKBUILDER_API UGameCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()


protected:
	// Parameters
	UPROPERTY(EditDefaultsOnly) float Dash_MaxWalkSpeed;

	// Transient
	UPROPERTY(Transient) AGameCharacter* GameCharacterOwner;
	bool Safe_bWantsToDash;


public:
	UGameCharacterMovementComponent();

protected:
	virtual void InitializeComponent() override;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

public:
	UFUNCTION(BlueprintCallable) void DashPressed();
	
	UFUNCTION(BlueprintPure) bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
};
