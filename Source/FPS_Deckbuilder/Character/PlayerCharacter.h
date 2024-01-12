
#pragma once

#include "CoreMinimal.h"

#include "Camera/CameraComponent.h"
//#include "FPS_Deckbuilder/Card/Card.h"
#include "FPS_Deckbuilder/CommonHeaders/CostPackage.h"
#include "GameFramework/CharacterMovementComponent.h" // I'm using this in a getter. Is there no other way to do this?
#include "GameCharacter.h"
#include "Math/IntVector.h"

#include "PlayerCharacter.generated.h"



class AWeapon;
class IInteractable;
class UCapsuleComponent;
class UCard;
class UHUDWidget;
class UStackEditorWidget;
class UStackSlot;
class USpringArmComponent;
class UTrayStack;



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
	//virtual void NotifyOfDamageDealt(FDamageStruct& DamageStruct) override;

	void EquipWeapon(AWeapon* Weapon);
	void ShakeCamera(TSubclassOf<UMatineeCameraShake> CameraShakeClass);
	virtual void Stun(float StunSeconds) override;

	// -- Stack Editor & Inventory Methods -- //
	
	/* Used by StackEditorWidget to read & modify the stacks */
	UFUNCTION(BlueprintCallable)
	UTrayStack* GetTrayStack(int Index);

	/* A hackey workaround to get the menu working */
	UFUNCTION(BlueprintCallable)
	void CloseStackEditor();

	/* TODO: Have card's that are being placed into slots do so through a PlayerCharacter method. This avoids the need for this method 
	* PROBLEM: Since Card::ModifyStack takes a stack as a parameter, while the automatic updating of the StackEditor widget only occurs
	* when using PlayerCharacter Inventory and stack methods (PlaceCardInSlot for example), card's modifying slots don't update the UI.
	* This method is an ugly workaround that can be removed by having cards use the player's method for updating UI.
	* Further improvement can come from having each stack manage communication with it's own widget, avoiding the need for player
	* driven methods entirely.
	* 
	* NOTE that indirection like this between objects with bidirectional communication is bad. Just have the ViewModel and View point to one another
	* directly 
	*/
	UFUNCTION(BlueprintCallable)
	void UpdateStackEditor();

	// Modifies Inventory & Updates Inventory view if available
	UFUNCTION(BlueprintCallable)
	void AddCardToInventory(UCard* Card);
	
	// Modifies Inventory & Updates Inventory view if available
	UFUNCTION(BlueprintCallable)
	bool RemoveCardFromInventory(UCard* Card);
	
	/* This class updates the view for the tray slots. This method acts as a wrapper for the TrayStack::SetCardInSlot
	* StackIndex : Exposed to allow the View to choose the stack it's modifying based on it's own state
	*/
	UFUNCTION(BlueprintCallable)
	bool PlaceCardInSlot(int StackIndex, UStackSlot* Slot, UCard* Card);

	UFUNCTION(BlueprintCallable)
	bool RemoveCardFromStack(int StackIndex, UCard* Card);

	UFUNCTION(BlueprintCallable)
	UTrayStack* GetStackWithSelectedCardOfClass(TSubclassOf<UCard> CardClass);



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
	void OpenStackEditorButton_Pressed();


	// -- Gameplay Methods -- // 
	virtual void Die();
	void HandleReceiveDamage(FDamageStruct DamageStruct, bool bTriggersStatusEffects);
	
	UFUNCTION(BlueprintCallable) 
	void FireWeapon(bool bTriggersStatusEffects = true); // Needed for cards & StatusEffects to interact with
	
	UFUNCTION(BlueprintCallable)
	void DoTransaction(FCost Cost); // Spends or gains resources, health, and money

	// -- Card Methods -- //
	UCard* DrawCard();
	void ShuffleDeck();
	void UseCardInTray(int Index);



private:
	// -- Components -- //
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Components")
	USpringArmComponent* SpringArmComponent;

	/* Collision for the single purpose of checking for damaging collisions from attacks */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "PlayerCharacter|Components")
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


	// -- Weapon -- //
	UPROPERTY(VisibleAnywhere) 
	AWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bWeaponEnabled = true;

	// -- Dashing -- //
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category="PlayerCharacter")
	float DamageDelaySeconds;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "PlayerCharacter")
	float DashRechargeSeconds;

	float SecondsSinceLastDash;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "PlayerCharacter")
	int MaxDashCharges;

	int DashCharges;


	// -- Card Members -- //
	// DEPRECATED
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Cards")
	TArray<TSubclassOf<UCard>> StartingDeck;

	// DEPRECATED
	UPROPERTY(VisibleAnywhere, Category = "PlayerCharacter|Cards")
	TArray<UCard*> Deck;

	// DEPRECATED
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Cards")
	int DeckSize;

	// DEPRECATED
	UPROPERTY(VisibleAnywhere, Category = "PlayerCharacter|Cards")
	TArray<UCard*> DiscardPile;


	// -- Cards -- // 
	UPROPERTY(EditAnywhere, Category = "PlayerCharacter|Cards|Tray")
	TArray<TSubclassOf<UCard>> InitialInventory; // TESTING 

	UPROPERTY(VisibleAnywhere, Category = "PlayerCharacter|Cards|Tray")
	TArray<UCard*> Inventory;

	UPROPERTY(VisibleAnywhere, Category = "PlayerCharacter|Cards|Tray")
	TArray<UTrayStack*> Tray;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Cards|Tray", meta=(UIMin=1, UIMax=9))
	int TraySize;

	int MaxTraySize = 9; // Hardcoded for the number of single numerical keys on the keyboard, discluding the '0' key

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Cards|SlotEditor")
	TSubclassOf<UUserWidget> StackEditorWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "PlayerCharacter|Cards|SlotEditor")
	UStackEditorWidget* StackEditorWidget;

	FIntVector Resources;


public:
	UFUNCTION(BlueprintCallable)
	void GetCameraViewPoint(FVector& OutLocation, FRotator& OutRotation) { OutLocation = CameraComponent->GetComponentLocation(); OutRotation = CameraComponent->GetComponentRotation(); }

	// returns values [0, 1]
	FORCEINLINE float GetPercentOfMaxWalkingSpeed() 
	{
		UCharacterMovementComponent* CharMovement = GetCharacterMovement();
		return (CharMovement->Velocity.Size() / CharMovement->MaxWalkSpeed);
	}

	FORCEINLINE void SetWeaponEnabled(bool _bWeaponEnabled) { bWeaponEnabled = _bWeaponEnabled; }
};
