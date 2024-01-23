#include "PlayerCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "FPS_Deckbuilder/Card/Card.h"
#include "FPS_Deckbuilder/Card/TrayStack.h"
#include "FPS_Deckbuilder/CommonHeaders/TraceChannelDefinitions.h"
#include "FPS_Deckbuilder/Interactable.h"
#include "FPS_Deckbuilder/UI/HUDWidget.h"
#include "FPS_Deckbuilder/UI/StackEditorWidget.h"
#include "FPS_Deckbuilder/Weapon/Weapon.h"

#include "DrawDebugHelpers.h"



#include "FPS_Deckbuilder/Card/StackSlot.h"


APlayerCharacter::APlayerCharacter()
{
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(RootComponent);
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComponent->SetupAttachment(CameraComponent);
	
	AttackCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Enemy Attack Detection"));
	AttackCollision->SetupAttachment(RootComponent);
}


//APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
//{
//	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
//	CameraComponent->SetupAttachment(RootComponent);
//	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
//	SpringArmComponent->SetupAttachment(CameraComponent);
//}



void APlayerCharacter::BeginPlay()
{
	// Don't want to mess with the constructor. This is should be there though
	CameraComponent->bUsePawnControlRotation = false;
	RecoilInterpolationSpeed = 25.f; // Set by the weapon upon pickup
	TargetInterpolatedRecoil = 0.8f; // Set by the weapon upon pickup 
	bUseControllerRotationYaw = false;

	// -- UI -- // 
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UHUDWidget>(Cast<APlayerController>(GetController()), HUDWidgetClass);
		HUDWidget->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCharacter::BeginPlay -- !HUDWidgetClass"));
	}

	SetLazyHealthBar(HUDWidget->GetLazyHealthBar()); // Setting GameCharacter->LazyHealthBar
	SetStatusEffectHorizontalBox(HUDWidget->GetStatusEffectHorizontalBox());

	// -- Gameplay Properties -- //
	Resources = { 0, 0, 0 };
	DashCharges = MaxDashCharges;

	UTrayStack* TrayStack;
	for (int i = 0; i < TraySize; i++)
	{
		TrayStack = NewObject<UTrayStack>(this);
		Tray.Add(TrayStack);
		HUDWidget->CreateAndLinkTrayStackWidget(TrayStack);
	}







	// -- Testing -- //
	UCard* Card;
	for (TSubclassOf<UCard> CardClass : InitialInventory)
	{
		Card = NewObject<UCard>(this, CardClass);
		Card->SetPlayerCharacter(this);
		Inventory.Add(Card);
	}


	// -- Trying to find bug that keeps crashing when using cards in tray -- //
	//UStackSlot* LeadingSlot = Tray[0]->GetLeadingEmptySlot();
	//Tray[0]->SetCardInSlot(LeadingSlot, 0, Inventory[0]);
	//GetWorldTimerManager().SetTimer(
	//	TestTimer,
	//	[&]()
	//	{
	//		UseCardInTray(0);
	//	},
	//	0.2f,
	//	true
	//);




	Super::BeginPlay(); // Calls SetupPlayerInputComponent(...)
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// -- Recoil -- //
	CameraComponent->SetRelativeRotation(FRotator(PlayerRotation.Pitch, 0, 0)); // Up and down is camera rotation
	GetCapsuleComponent()->SetWorldRotation(FRotator(0, PlayerRotation.Yaw, 0)); // Player turn is the entire actor. -90 to set facing to be the same as the actors forward vector  
	if (bIsInterpolatingRecoil)
	{
		//InterpolatedRecoil = FMath::FInterpTo(InterpolatedRecoil, TargetInterpolatedRecoil, DeltaTime, RecoilInterpolationSpeed);
		InterpolatedRecoil = FMath::InterpEaseIn(InterpolatedRecoil, TargetInterpolatedRecoil, DeltaTime, RecoilInterpolationSpeed);
		if (InterpolatedRecoil >= TargetInterpolatedRecoil * 0.98) //- Completed interpolation
		{
			UE_LOG(LogTemp, Warning, TEXT("Completed interpolation"));

			//- Accumulating & Updating for frame
			AccumulatedRecoil_Pitch += TargetInterpolatedRecoil;
			InterpolatedRecoil = 0;
			bIsInterpolatingRecoil = false;
			CameraComponent->AddRelativeRotation(FRotator(AccumulatedRecoil_Pitch, 0, 0));
		}
		else //- Incomplete interpolation
		{
			UE_LOG(LogTemp, Warning, TEXT("Incomplete interpolation"));

			//- Updating for frame
			CameraComponent->AddRelativeRotation(FRotator(AccumulatedRecoil_Pitch + InterpolatedRecoil, 0, 0));
		}
	}
	else if ((AccumulatedRecoil_Pitch + PlayerRotation.Pitch) <= StoredPitch) //- Player has pulled down further than where firing started
	{
		UE_LOG(LogTemp, Warning, TEXT("Pulled down far enough"));

		//- Decay is stopped and Player rotation is adjusted
		PlayerRotation.Pitch += AccumulatedRecoil_Pitch;
		CameraComponent->AddRelativeRotation(FRotator(AccumulatedRecoil_Pitch, 0, 0)); // Needed for this frame
		AccumulatedRecoil_Pitch = 0;
	}
	else //- Decaying AccumulatedRecoil
	{
		AccumulatedRecoil_Pitch -= RecoilResetSpeed;
		UE_LOG(LogTemp, Warning, TEXT("Decaying: %f"), RecoilResetSpeed);
		CameraComponent->AddRelativeRotation(FRotator(AccumulatedRecoil_Pitch, 0, 0)); // Needed for this frame
	}





	// -- Weapon Spread -- //
	HUDWidget->UpdateCrosshairsSpread(EquippedWeapon ? EquippedWeapon->GetSpread() : 0.f);
	/* -- OLD implementation of recoil -- */
	//float DeltaPitch = (EquippedWeapon ? EquippedWeapon->GetRecoilResetSpeed() : 10.f) * DeltaTime;
	//// Will still reset without an equipped weapon 
	//// UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::Tick -- %f"), AccumulatedRecoil_Pitch);
	//if (AccumulatedRecoil_Pitch > 0.f && !bIsFiring)
	//{
	//	AccumulatedRecoil_Pitch -= DeltaPitch;
	//	AddControllerPitchInput(DeltaPitch);

	//	if (AccumulatedRecoil_Pitch < 0)
	//	{
	//		DeltaPitch = DeltaPitch - AccumulatedRecoil_Pitch;
	//		AccumulatedRecoil_Pitch = 0;
	//	}
	//}






	// -- Target Interactable -- // 
	FHitResult HitResult;
	FVector Start = GetActorLocation() + FVector(0.f, 0.f, BaseEyeHeight);
	FVector End = Start + CameraComponent->GetForwardVector() * InteractionDistance;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Interactable
	);
	/* The default setting for ECC_Interactable is NoCollision.
	 * No check is done for correct type of actor because the user must intentionally introduce a bug by setting
	 * the trace response on a component or actor to something other than no collision. 
	 * This should only be done intentionally, for objects that implemenent IInteractable
	 */
	if (HitResult.bBlockingHit)
	{
		if (!TargetInteractable || TargetInteractable != HitResult.Actor.Get()) UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::Tick -- Interactable")); // NOTE: We use get because there is no overridden `!=` operator for TScriptInterface and a WeakPtr	
		TargetInteractable = HitResult.Actor.Get(); //NOTE: `.Get()` dereferences the WeakPtr, allowing us to pass a traditional C++ ptr to the `=` operator. There is no conversion to Interface using WeakPtr
	}
	else
	{
		TargetInteractable = nullptr;
	}



	// -- Dash Recharge -- //
	if (DashCharges < MaxDashCharges)
	{
		SecondsSinceLastDash += DeltaTime;
		if (SecondsSinceLastDash >= DashRechargeSeconds)
		{
			SecondsSinceLastDash = 0.f;
			++DashCharges;
			// UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter:Tick -- Adding Dash Charge"));
		}
	}


}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ThisClass::MoveRight);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ThisClass::LookUp);
	PlayerInputComponent->BindAxis(FName("LookRight"), this, &ThisClass::LookRight);

	PlayerInputComponent->BindAction(FName("LeftMouseButton"), IE_Pressed, this, &ThisClass::LeftMouseButton_Pressed);
	PlayerInputComponent->BindAction(FName("LeftMouseButton"), IE_Released, this, &ThisClass::LeftMouseButton_Released);
	PlayerInputComponent->BindAction(FName("RightMouseButton"), IE_Pressed, this, &ThisClass::RightMouseButton_Pressed);
	PlayerInputComponent->BindAction(FName("RightMouseButton"), IE_Released, this, &ThisClass::RightMouseButton_Released);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ThisClass::JumpButton_Pressed);
	PlayerInputComponent->BindAction(FName("Crouch"), IE_Pressed, this, &APlayerCharacter::CrouchButton_Pressed);
	PlayerInputComponent->BindAction(FName("Crouch"), IE_Released, this, &APlayerCharacter::CrouchButton_Released);
	PlayerInputComponent->BindAction(FName("Interact"), IE_Pressed, this, &ThisClass::InteractButton_Pressed);
	PlayerInputComponent->BindAction(FName("Reload"), IE_Pressed, this, &ThisClass::ReloadButton_Pressed);
	PlayerInputComponent->BindAction(FName("Dash"), IE_Pressed, this, &ThisClass::DashButton_Pressed);
	PlayerInputComponent->BindAction(FName("OpenStackEditor"), IE_Pressed, this, &ThisClass::OpenStackEditorButton_Pressed);

	// Binding tray select actions. Handsize is calculated at runtime so this needs to be done dynamically
	FName ActionName;
	for (int i = 0; i < TraySize; i++)
	{
		ActionName = FName(*(FString("UseCardInTray") + FString::FromInt(i + 1)));

		UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::SetupPlayerInputComponent -- %s"), *(ActionName.ToString()));

		PlayerInputComponent->BindAction<FInputUseCardInTrayDelegate>(ActionName, IE_Pressed, this, &ThisClass::UseCardInTray, i);
	}
}

void APlayerCharacter::ReceiveDamage(FDamageStruct& DamageStruct, bool bTriggersStatusEffects)
{
	/* Allows the player a small window to react late to an attack
	*
	* The player doesn't get a delayed damage if they're not moving. Helps to avoid situations where the player
	* See's that it's obvious they should have taken damage immediately. Also encourages moving
	*/
	if (GetCharacterMovement()->Velocity.Size() > 10.f) // Arbitrary number. Might need tweeking
	{
		FTimerHandle DamageHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &APlayerCharacter::HandleReceiveDamage, DamageStruct, bTriggersStatusEffects);
		GetWorldTimerManager().SetTimer(
			DamageHandle,
			TimerDelegate,
			DamageDelaySeconds,
			false
		);
		return;
	}
	HandleReceiveDamage(DamageStruct, bTriggersStatusEffects);
}

void APlayerCharacter::HandleReceiveDamage(FDamageStruct DamageStruct, bool bTriggersStatusEffects)
{
	Super::ReceiveDamage(DamageStruct, bTriggersStatusEffects);

	// -- FX -- //
	if (HitReactShakeClass) ShakeCamera(HitReactShakeClass);
}

//void APlayerCharacter::NotifyOfDamageDealt(FDamageStruct& DamageStruct)
//{
//	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::NotifyOfDamageDealt"));
//}

void APlayerCharacter::MoveForward(float AxisValue)
{
	FVector ForwardVector = GetActorForwardVector();
	DashDirection +=  ForwardVector * AxisValue;
	AddMovementInput(ForwardVector, AxisValue);
}

void APlayerCharacter::MoveRight(float AxisValue)
{
	FVector RightVector = GetActorRightVector();
	DashDirection += RightVector * AxisValue;
	AddMovementInput(GetActorRightVector(), AxisValue);
}

void APlayerCharacter::LookUp(float AxisValue)
{
	// AxisValue < 0 --> Looking up
	float DeltaPitch = AxisValue * MouseSensitivity;
	if (DeltaPitch < 0) StoredPitch -= DeltaPitch;
	
	PlayerRotation.Pitch -= DeltaPitch;
}

void APlayerCharacter::LookRight(float AxisValue)
{
	PlayerRotation.Yaw += AxisValue * MouseSensitivity;
}

void APlayerCharacter::LeftMouseButton_Pressed()
{
	// Fire Weapon exists seperately from this method to allow other things to force fire the weapon for this character. Probably just let this method be the method called in those cases and make the body of FireWeapon the body of this weapon
	FireWeapon(); // Attempts to set bIsFiring;
}

void APlayerCharacter::LeftMouseButton_Released()
{
	if (EquippedWeapon) bIsFiring = EquippedWeapon->StopFire();
}

void APlayerCharacter::RightMouseButton_Pressed()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, FString::Printf(TEXT("DashCharges: %i, MaxDashCharges: %i"), DashCharges, MaxDashCharges));
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, FString::Printf(TEXT("Observers_OnDamageDealt: %i"), Observers_OnDamageDealt.Num()));
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, FString::Printf(TEXT("Observers_OnApplyDamage: %i"), Observers_OnApplyDamage.Num()));
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, FString::Printf(TEXT("Observers_OnReload: %i"), Observers_OnReload.Num()));


	//int i = 0;
	//for (UTrayStack* Stack : Tray)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, FString::Printf(TEXT("Stack: %i"), i));
	//	UCard* Card;
	//	for (UStackSlot* Slot : Stack->GetBackingArray())
	//	{
	//		Card = Slot->ReturnCard();
	//		if (Card) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, FString::Printf(TEXT("  %s"), *Card->GetName()));
	//	}
	//	i++;
	//}

	// GetWorldTimerManager().ClearTimer(TestTimer);
}

void APlayerCharacter::RightMouseButton_Released()
{
	UseCardInTray(2);
}

void APlayerCharacter::InteractButton_Pressed()
{
	if (TargetInteractable) TargetInteractable->Interact(this);
}

void APlayerCharacter::JumpButton_Pressed()
{
	if (!bIsDashing)
	{
		ACharacter::Jump();
	}
}

void APlayerCharacter::CrouchButton_Pressed()
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::CrouchButton_Pressed"));
	// GetCharacterMovement()->Crouch();
}

void APlayerCharacter::CrouchButton_Released()
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::CrouchButton_Released"));
	// GetCharacterMovement()->UnCrouch();
}

void APlayerCharacter::ReloadButton_Pressed()
{
	if (EquippedWeapon) 
	{
		TArray<FOnReloadDelegate> LocalObservers(Observers_OnReload);
		for (FOnReloadDelegate Delegate : LocalObservers)
		{
			if (Delegate.IsBound()) { Delegate.Execute(); }
		}

		EquippedWeapon->Reload(); 
	}
}

void APlayerCharacter::DashButton_Pressed()
{
	if (DashCharges == 0 || !Dash()) return;

	// -- I-Frames -- //
	FTimerManager& TimerManager = GetWorldTimerManager();
	TimerManager.ClearAllTimersForObject(this);
	AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FTimerHandle NoCollisionHandle;
	GetWorldTimerManager().SetTimer(
		NoCollisionHandle,
		[&]() 
		{
			AttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		},
		DashSeconds,
		false
	);
	--DashCharges;
}

void APlayerCharacter::OpenStackEditorButton_Pressed()
{
	APlayerController* Player = Cast<APlayerController>(GetController());
	FInputModeUIOnly Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	Player->SetInputMode(Mode);
	Player->bShowMouseCursor = true;
	HUDWidget->RemoveFromViewport();
	StackEditorWidget = CreateWidget<UStackEditorWidget>(Cast<APlayerController>(GetController()), StackEditorWidgetClass);
	StackEditorWidget->PlayerCharacter = this;
	StackEditorWidget->Update(Tray, Inventory);
	StackEditorWidget->AddToViewport();
	StackEditorWidget->SetKeyboardFocus();
}


void APlayerCharacter::CloseStackEditor()
{
	APlayerController* Player = StackEditorWidget->GetOwningPlayer();
	FInputModeGameOnly Mode;
	Player->SetInputMode(Mode);
	Player->bShowMouseCursor = false;
	StackEditorWidget->RemoveFromViewport();
	HUDWidget->AddToViewport();
	for (UTrayStack* TrayStack : Tray) TrayStack->ResetTrayStack(); 
}

void APlayerCharacter::UpdateStackEditor()
{
	if (StackEditorWidget) StackEditorWidget->Update(Tray, Inventory);
}


void APlayerCharacter::Die()
{
	// TODO: Many things


	Super::Die();
}


void APlayerCharacter::EquipWeapon(AWeapon* Weapon)
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::SnapToTarget, true);
	TargetInterpolatedRecoil = 2.5;
	RecoilResetSpeed = 0.3;
	RecoilInterpolationSpeed = 25;
	if (EquippedWeapon)
	{ 

		// GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, "TEST");


		EquippedWeapon->StopFire(); 
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules(AttachmentRules, false));
		EquippedWeapon->SetActorLocation(Weapon->GetActorLocation());
		EquippedWeapon->SetActorRotation(FRotator(0.f));
		EquippedWeapon->SetActorEnableCollision(true);
		EquippedWeapon->SetEquippedPlayerCharacter(nullptr);
		EquippedWeapon->SetAmmoTextBlock(nullptr);
		TargetInterpolatedRecoil = 0;
		RecoilResetSpeed = 0.3;
		RecoilInterpolationSpeed = 25;
		// GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, FString::Printf(TEXT("%f, %f, %f"), TargetInterpolatedRecoil, RecoilResetSpeed, RecoilInterpolationSpeed));
	}

	if (!Weapon) return;

	Weapon->SetActorEnableCollision(false);
	Weapon->SetActorRelativeLocation(FVector(0.f));
	Weapon->SetActorRelativeRotation(FRotator(0.f, -90.f, 0.f)); // NOTE: Test weapon meshes are rotated funny. Hard set to handle this. Import August's weapons properly and change this
	Weapon->AttachToComponent(SpringArmComponent, AttachmentRules);
	Weapon->SetEquippedPlayerCharacter(this);
	Weapon->SetAmmoTextBlock(HUDWidget->GetCurrentAmmoText());

	UE_LOG(LogTemp, Error, TEXT("Target Interpolated Recoil: %f"), Weapon->GetRecoilPitch());
	TargetInterpolatedRecoil = Weapon->GetRecoilPitch();
	
	UE_LOG(LogTemp, Error, TEXT("Recoil Reset Speed: %f"), Weapon->GetRecoilResetSpeed());
	RecoilResetSpeed = Weapon->GetRecoilResetSpeed();
	
	UE_LOG(LogTemp, Error, TEXT("Recoil Interpolation Speed: %f"), Weapon->GetRecoilInterpolationSpeed());
	RecoilInterpolationSpeed = Weapon->GetRecoilInterpolationSpeed();
	
	EquippedWeapon = Weapon;
}


void APlayerCharacter::AddWeaponRecoil()
{
	AccumulatedRecoil_Pitch += InterpolatedRecoil; // Interpolated Recoil MUST be set from the weapon
	InterpolatedRecoil = 0;
	bIsInterpolatingRecoil = true;
}


void APlayerCharacter::Stun(float StunSeconds)
{
	bIsStunned = true;
	FTimerHandle StunnedHandle;
	GetWorldTimerManager().SetTimer(
		StunnedHandle,
		[&]()
		{
			bIsStunned = false;
		},
		StunSeconds,
		false
	);
}


UCard* APlayerCharacter::DrawCard()
{
	// Checking if deck needs reshuffling
	if (Deck.Num() == 0)
	{
		Deck = DiscardPile;
		ShuffleDeck();
		DiscardPile.RemoveAll([this](const UCard* c) {return c;});
	}
	// If deck is still empty then there are no cards in the deck
	if (Deck.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCharacter::DrawCard -- No Cards in Deck"));
		return nullptr;
	}

	// Drawing card 
	UCard* Card = Deck.Pop();
	Card->SetPlayerCharacter(this);
	return Card;
}


void APlayerCharacter::UseCardInTray(int Index)
{
	if (!Tray[Index]) 
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCharacter::UseCardInTray -- !Tray[Index]"));
		return;
	}
	// return;
	Tray[Index]->UseSelectedCard();
	return;


	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::UseCardInTray -- Index: %i"), Index);
	if (Tray[Index] == nullptr) { UE_LOG(LogTemp, Error, TEXT("APlayerCharacter::UseCardInTray -- !Tray[Index]")); return; }
	
	// FCost Cost = Tray[Index]->GetCost();

	// Keeping this check here, instead of in the card allows the use of card effects that can use cards for free
	//if (Resources.X < Cost.Resource_X ||
	//	Resources.Y < Cost.Resource_Y ||
	//	Resources.Z < Cost.Resource_Z ||
	//	GetHealth() <= Cost.Health ||
	//	Money < Cost.Money)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::UseCardInTray -- Insufficient Resources to use card"));
	//	return;
	//}

	// -- Spending Resources -- //
	// DoTransaction(Cost);
	
	// -- Using Card, updating tray & updating tray slot -- //
	// TODO: not appropriate for DrawCard() to sometimes return nullptr;
	//Tray[Index]->Use();
	//Tray[Index]->Unsubscribe();
	//DiscardPile.Add(Tray[Index]);
	//Tray[Index] = DrawCard();
	//HUDWidget->SetCardForSlotAtIndex(Index, Tray[Index]);
}


void APlayerCharacter::ShuffleDeck()
{
	for (int32 i = Deck.Num() - 1; i > 0; i--) {
		int32 j = FMath::FloorToInt(FMath::SRand() * (i + 1)) % Deck.Num();
		UCard* temp = Deck[i];
		Deck[i] = Deck[j];
		Deck[j] = temp;
	}
}


void APlayerCharacter::ShakeCamera(TSubclassOf<UMatineeCameraShake> CameraShakeClass)
{
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(CameraShakeClass);
}


void APlayerCharacter::FireWeapon(bool bTriggersStatusEffects)
{
	if (!EquippedWeapon || !bWeaponEnabled) return;
	
	if (bTriggersStatusEffects)
	{
		for (FOnAttackDelegate Delegate : Observers_OnAttack)
		{
			if (Delegate.IsBound()) { Delegate.Execute(); }
		}
	}

	bIsFiring = EquippedWeapon->Fire();
	if (AccumulatedRecoil_Pitch == 0) StoredPitch = PlayerRotation.Pitch;
}


void APlayerCharacter::DoTransaction(FCost Cost)
{
	// Spending resources
	Resources -= FIntVector(Cost.Resource_X, Cost.Resource_Y, Cost.Resource_Z);
	HUDWidget->SetResourceText(Resources);
	Money -= Cost.Money;
	HUDWidget->SetMoneyText(Money);
	if (Cost.Health != 0.f)
	{
		FDamageStruct DamageStruct;
		DamageStruct.Damage = Cost.Health;
		DamageStruct.DamageType = EDT_MAX; // TODO: Consider implementing self damage type, or that cards should specify how damage is dealt
		ReceiveDamage(DamageStruct);
	}
}


UTrayStack* APlayerCharacter::GetTrayStack(int Index)
{
	if (Index < 0 || Tray.Num() <= Index)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCharacter::GetTrayStack -- Index exceeds array bounds"));
		return nullptr;
	}

	return Tray[Index];
}


void APlayerCharacter::AddCardToInventory(UCard* Card)
{
	Inventory.Add(Card);
	if (StackEditorWidget)
	{
		StackEditorWidget->Update(Tray, Inventory);
	}
}


bool APlayerCharacter::RemoveCardFromInventory(UCard* Card)
{
	bool Output = (Inventory.Remove(Card) == 1);

	if (StackEditorWidget) StackEditorWidget->Update(Tray, Inventory);

	return Output;
}


bool APlayerCharacter::PlaceCardInSlot(int StackIndex, UStackSlot* Slot, UCard* Card)
{
	if (StackIndex >= TraySize)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCharacter::PlaceCardInSlot -- Index exceeds array size"));
		return false;
	}

	TrayCards.Add(Card); // See TrayCard definition for explanation
	bool bAdded = Tray[StackIndex]->SetCardInSlot(Slot, 0, Card); // Will allow the card a chance to modify the structure for it's needs

	if (StackEditorWidget) StackEditorWidget->Update(Tray, Inventory);

	return bAdded;
}


bool APlayerCharacter::RemoveCardFromStack(int StackIndex, UCard* Card)
{
	if (StackIndex >= TraySize)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCharacter::RemoveCardInSlot -- Index exceeds array size"));
		return false;
	}

	TrayCards.Remove(Card);
	bool bRemoved = Tray[StackIndex]->RemoveCard(Card) ; // Will allow the card a chance to modify the structure for it's needs

	if (StackEditorWidget) StackEditorWidget->Update(Tray, Inventory);

	return bRemoved;
}



UTrayStack* APlayerCharacter::GetStackWithSelectedCardOfClass(TSubclassOf<UCard> CardClass)
{
	const UCard* SelectedCard;
	for (UTrayStack* TrayStack : Tray)
	{
		SelectedCard = TrayStack->GetSelectedCard();
		if (!SelectedCard) continue;

		// GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, FString::Printf(TEXT("%s == , %s"), *SelectedCard->GetClass()->GetName(), *CardClass->GetName()));

		if (SelectedCard->GetClass() == CardClass) return TrayStack;
	}
	return nullptr;
}