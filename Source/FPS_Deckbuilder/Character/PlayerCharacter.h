
#pragma once

#include "CoreMinimal.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h" // I'm using this in a getter. Is there no other way to do this?
#include "GameCharacter.h"
#include "Math/IntVector.h"
#include "PlayerCharacter.generated.h"



class AWeapon;
class IInteractable;
class UCapsuleComponent;
class UCard;
class UHUDWidget;
class USpringArmComponent;


// -- Events -- //
DECLARE_EVENT_OneParam(APlayerCharacter, FOnDamageDealtEvent, FDamageStruct&)
DECLARE_EVENT_OneParam(APlayerCharacter, FOnDamageReceivedEvent, FDamageStruct&)


/**
 * Character controlled by the player
 * 
 * NOTE: Given time constraints, Dashing is going to be solved by managing state within the player character class itself.
 * This is obviously hackey, and properly implementing a subclass of the CharacterMovementClass would be the correct way to do this
 */
UCLASS()
class FPS_DECKBUILDER_API APlayerCharacter : public AGameCharacter
{
	GENERATED_BODY()

	DECLARE_DELEGATE_OneParam(FInputUseCardInTrayDelegate, const int32) // For using card's in slots

public:
	APlayerCharacter();
	// APlayerCharacter(const FObjectInitializer& ObjectInitializer);


public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void ReceiveDamage(FDamageStruct& DamageStruct, bool bTriggersStatusEffects = true) override;
	virtual void NotifyOfDamageDealt(FDamageStruct& DamageStruct) override;

	void EquipWeapon(AWeapon* Weapon);
	void ShakeCamera(TSubclassOf<UMatineeCameraShake> CameraShakeClass);
	


	
	
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
	void JumpButton_Pressed();
	void CrouchButton_Pressed();
	void CrouchButton_Released();
	void InteractButton_Pressed();
	void ReloadButton_Pressed();
	void DashButton_Pressed();
	/*void DashButton_Released();*/


	// -- Gameplay Methods -- // 
	virtual void Die();
	void HandleReceiveDamage(FDamageStruct DamageStruct, bool bTriggersStatusEffects);
	UFUNCTION(BlueprintCallable) 
	void FireWeapon(bool bTriggersStatusEffects = true); // Needed for cards & StatusEffects to interact with

	// -- Card Methods -- //
	UCard* DrawCard();
	void ShuffleDeck();
	void UseCardInTray(int Index);

	



public:
	// NOTE: These events can be public because only this class can call them. Makes binding simple
	FOnDamageDealtEvent OnDamageDealt;
	FOnDamageReceivedEvent OnDamageReceived;

private:
	// -- Components -- //
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Components")
	USpringArmComponent* SpringArmComponent;

	//UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Components")
	//UCapsuleComponent* EnemyAttackDetection;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Components")
	UCapsuleComponent* AttackCollision;


	// -- Input & Interaction -- //
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter")
	TSubclassOf<UHUDWidget> HUDWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "PlayerCharacter")
	UHUDWidget* HUDWidget;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter")
	TSubclassOf<UMatineeCameraShake> HitReactShakeClass;

	TScriptInterface<IInteractable> TargetInteractable;

	UPROPERTY(EditDefaultsOnly)
	float InteractionDistance;

	UPROPERTY(EditAnywhere)
	float MouseSensitivity;


	// -- Gameplay -- //
	UPROPERTY(VisibleAnywhere) 
	AWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bWeaponEnabled = true;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category="PlayerCharacter")
	float DamageDelaySeconds;
	
	TArray<FTimerHandle*> DamageBuffer;


	// -- Card Members -- //
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Cards")
	TArray<TSubclassOf<UCard>> StartingDeck;

	UPROPERTY(VisibleAnywhere, Category = "PlayerCharacter|Cards")
	TArray<UCard*> Deck;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Cards")
	int DeckSize;

	UPROPERTY(VisibleAnywhere, Category = "PlayerCharacter|Cards")
	TArray<UCard*> DiscardPile;

	UPROPERTY(VisibleAnywhere, Category = "PlayerCharacter|Cards")
	TArray<UCard*> Tray;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Cards", meta=(UIMin=1, UIMax=9))
	int TraySize;

	int MaxTraySize = 9; // Hardcoded for the number of single numerical keys on the keyboard, discluding 1

	/* The count of each type of resource. Resources are spent on cards to use them */
	FIntVector Resources;


public:
	FORCEINLINE void GetCameraViewPoint(FVector& OutLocation, FRotator& OutRotation) { OutLocation = CameraComponent->GetComponentLocation(); OutRotation = CameraComponent->GetComponentRotation(); }

	// returns value [0, 1]
	FORCEINLINE float GetPercentOfMaxWalkingSpeed() 
	{
		UCharacterMovementComponent* CharMovement = GetCharacterMovement();
		return (CharMovement->Velocity.Size() / CharMovement->MaxWalkSpeed);
	}

	///* Used by cards and status effects for various things */
	//UFUNCTION(BlueprintCallable)
	//FVector GetWeaponMuzzleLocation()
	//{
	//	if (EquippedWeapon)
	//	{
	//		EquippedWeapon->
	//	}
	//}

	FORCEINLINE void SetWeaponEnabled(bool _bWeaponEnabled) { bWeaponEnabled = _bWeaponEnabled; }


};
