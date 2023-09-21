
#pragma once

#include "CoreMinimal.h"
#include "GameCharacter.h"
#include "PlayerCharacter.generated.h"

class AWeapon;
class IInteractable;
class UCameraComponent;
class UHUDWidget;
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
	// -- Non-Gameplay Members -- //
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter")
	TSubclassOf<UHUDWidget> HUDWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "PlayerCharacter")
	UHUDWidget* HUDWidget;

	TScriptInterface<IInteractable> TargetInteractable;

	UPROPERTY(EditDefaultsOnly)
	float InteractionDistance;



	// -- Gameplay Members -- // 
	UPROPERTY(VisibleAnywhere) 
	AWeapon* EquippedWeapon;
};
