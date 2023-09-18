
#pragma once

#include "CoreMinimal.h"
#include "GameCharacter.h"
#include "PlayerCharacter.generated.h"

class AWeapon;
class IInteractable;
class UCameraComponent;
class USpringArmComponent;


/**
 * Character controlled by the player
 */
UCLASS()
class FPS_DECKBUILDER_API APlayerCharacter : public AGameCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();


public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	// -- Gameplay Methods -- //
	void EquipWeapon(AWeapon* Weapon);

private:
	// -- Axis Bindngs & Action Bindings -- //
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookUp(float AxisValue);
	void LookRight(float AxisValue);

	void LeftMouseButton_Pressed();
	void LeftMouseButton_Released();
	void RightMouseButton_Pressed();
	void RightMouseButton_Released();
	void InteractButton_Pressed();


private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	/* Set it Tick
	 * Should always store something implementing IInteractable interface
	 */


	TScriptInterface<IInteractable> TargetInteractable;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float InteractionDistance;
};
