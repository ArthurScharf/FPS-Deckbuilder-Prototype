#include "PlayerCharacter.h"


#include "GameFramework/SpringArmComponent.h"
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
}

void APlayerCharacter::ReceiveDamage(FDamageStruct& DamageStruct)
{
	OnDamageReceived.Broadcast(DamageStruct);

	AGameCharacter::ReceiveDamage(DamageStruct);
}


void APlayerCharacter::NotifyOfDamageDealt(FDamageStruct& DamageStruct)
{
	OnDamagDealt.Broadcast(DamageStruct);

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
	Weapon->AttachToComponent(SpringArmComponent, AttachmentRules);
	Weapon->SetEquippedPlayerCharacter(this);
	Weapon->SetAmmoTextBlock(HUDWidget->GetCurrentAmmoText());
	EquippedWeapon = Weapon;
}
