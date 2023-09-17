
#pragma once

#include "CoreMinimal.h"
#include "FPS_Deckbuilder/Weapon/Weapon.h"
#include "GameCharacter.h"
#include "PlayerCharacter.generated.h"

/**
 * Character controlled by the player
 */
UCLASS()
class FPS_DECKBUILDER_API APlayerCharacter : public AGameCharacter
{
	GENERATED_BODY()


public:
	virtual void BeginPlay() override;

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


	


	

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;
};
