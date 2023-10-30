#include "PlayerCharacter.h"


#include "GameFramework/SpringArmComponent.h"
#include "FPS_Deckbuilder/Card/Card.h"
#include "FPS_Deckbuilder/CommonHeaders/TraceChannelDefinitions.h"
#include "FPS_Deckbuilder/Interactable.h"
#include "FPS_Deckbuilder/UI/HUDWidget.h"
#include "FPS_Deckbuilder/Weapon/Weapon.h"



APlayerCharacter::APlayerCharacter()
{
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(RootComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComponent->SetupAttachment(CameraComponent);
}


void APlayerCharacter::BeginPlay()
{
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

	// -- Cards -- //
	for (int i = 0; i < StartingDeck.Num(); i++) { Deck.Add(NewObject<UCard>(this, StartingDeck[i])); }
	ShuffleDeck();
	for (int i = 0; i < TraySize; i++) 
	{  // Initializing the Tray and corresponding tray's slots
		Tray.Add(nullptr);
		HUDWidget->AddTraySlot();
	} 
	for (int i = 0; i < TraySize; i++) { Tray[i] = DrawCard();  }
	Resources = { 0, 0, 0 };

	Super::BeginPlay(); // Calls SetupPlayerInputComponent(...)
}

void APlayerCharacter::Tick(float DeltaTime)
{
	// -- Target Interactable -- // 
	FHitResult HitResult;
	FVector Start = GetActorLocation() + FVector(0.f, 0.f, BaseEyeHeight);
	FVector End = Start + GetController()->GetDesiredRotation().Vector() * InteractionDistance;
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
		return;
	}
	TargetInteractable = nullptr;

	// -- Weapon Spread -- //
	HUDWidget->UpdateCrosshairsSpread(EquippedWeapon ? EquippedWeapon->GetSpread() : 0.f);
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
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("Interact"), IE_Pressed, this, &ThisClass::InteractButton_Pressed);

	// Binding tray select actions. Handsize is calculated at runtime so this needs to be done dynamically
	FName ActionName;
	for (int i = 0; i < TraySize; i++)
	{
		ActionName = FName(*(FString("UseCardInTray") + FString::FromInt(i + 1)));

		UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::SetupPlayerInputComponent -- %s"), *(ActionName.ToString()));

		PlayerInputComponent->BindAction<FInputUseCardInTrayDelegate>(ActionName, IE_Pressed, this, &ThisClass::UseCardInTray, i);
	}
}

void APlayerCharacter::ReceiveDamage(FDamageStruct& DamageStruct)
{
	// if (OnDamageReceived.IsBound()) 

	OnDamageReceived.Broadcast(DamageStruct);
	AGameCharacter::ReceiveDamage(DamageStruct);
}


void APlayerCharacter::NotifyOfDamageDealt(FDamageStruct& DamageStruct)
{
	// if (OnDamageDealt.IsBound()) 
	OnDamageDealt.Broadcast(DamageStruct);
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::NotifyOfDamageDealt"));
}


void APlayerCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector(), AxisValue);
}

void APlayerCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector(), AxisValue);
}

void APlayerCharacter::LookUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}

void APlayerCharacter::LookRight(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

void APlayerCharacter::LeftMouseButton_Pressed()
{
	if (EquippedWeapon) EquippedWeapon->Fire();
}

void APlayerCharacter::LeftMouseButton_Released()
{
	if (EquippedWeapon) EquippedWeapon->StopFire();
}

void APlayerCharacter::RightMouseButton_Pressed()
{
	HUDWidget->RemoveTraySlot();
}

void APlayerCharacter::RightMouseButton_Released()
{

}

void APlayerCharacter::InteractButton_Pressed()
{
	if (TargetInteractable) TargetInteractable->Interact(this);
}


void APlayerCharacter::Die()
{

	Destroy();
}


void APlayerCharacter::EquipWeapon(AWeapon* Weapon)
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::SnapToTarget, true);

	if (EquippedWeapon)
	{ 
		EquippedWeapon->StopFire(); 
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules(AttachmentRules, false));
		EquippedWeapon->SetActorLocation(Weapon->GetActorLocation());
		EquippedWeapon->SetActorRotation(FRotator(0.f));
		EquippedWeapon->SetActorEnableCollision(true);
		EquippedWeapon->SetEquippedPlayerCharacter(nullptr);
		EquippedWeapon->SetAmmoTextBlock(nullptr);
	}

	Weapon->SetActorEnableCollision(false);
	Weapon->SetActorRelativeLocation(FVector(0.f));
	Weapon->SetActorRelativeRotation(FRotator(0.f, -90.f, 0.f)); // NOTE: Test weapon meshes are rotated funny. Hard set to handle this. Import August's weapons properly and change this
	Weapon->AttachToComponent(SpringArmComponent, AttachmentRules);
	Weapon->SetEquippedPlayerCharacter(this);
	Weapon->SetAmmoTextBlock(HUDWidget->GetCurrentAmmoText());
	EquippedWeapon = Weapon;
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
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::UseCardInTray -- Index: %i"), Index);
	if (Tray[Index] == nullptr) { UE_LOG(LogTemp, Error, TEXT("APlayerCharacter::UseCardInTray -- !Tray[Index]")); return; }
	
	FCost Cost = Tray[Index]->GetCost();

	// Keeping this check here, instead of in the card allows the use of card effects that can use cards for free
	if (Resources.X < Cost.Resource_X ||
		Resources.Y < Cost.Resource_Y ||
		Resources.Z < Cost.Resource_Z ||
		GetHealth() <= Cost.Health ||
		Money < Cost.Money)
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::UseCardInTray -- Insufficient Resources to use card"));
		return;
	}

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
	
	// TODO: not appropriate for DrawCard() to sometimes return nullptr;
	// Using Card, updating tray & updating tray slot
	Tray[Index]->Use();
	DiscardPile.Add(Tray[Index]);
	Tray[Index] = DrawCard();
	HUDWidget->SetCardForSlotAtIndex(Index, Tray[Index]);
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

