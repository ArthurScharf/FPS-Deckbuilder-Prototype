
#pragma once

#include "CoreMinimal.h"

#include "Camera/CameraComponent.h"

#include "GameFramework/CharacterMovementComponent.h" // I'm using this in a getter. Is there no other way to do this?

#include "GameCharacter.h"
#include "PlayerCharacter.generated.h"

class AWeapon;
class IInteractable;
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

	virtual void ReceiveDamage(FDamageStruct& DamageStruct) override;
	
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

	// -- Gameplay Methods -- // 
	virtual void Die();


private:

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

	UPROPERTY(VisibleAnywhere) 
	AWeapon* EquippedWeapon;

public:
	FORCEINLINE void GetCameraViewPoint(FVector& OutLocation, FRotator& OutRotation) { OutLocation = CameraComponent->GetComponentLocation(); OutRotation = CameraComponent->GetComponentRotation(); }

	// returns value [0, 1]
	FORCEINLINE float GetPercentOfMaxWalkingSpeed() 
	{
		UCharacterMovementComponent* CharMovement = GetCharacterMovement();
		return (CharMovement->Velocity.Size() / CharMovement->MaxWalkSpeed);
	}

};
