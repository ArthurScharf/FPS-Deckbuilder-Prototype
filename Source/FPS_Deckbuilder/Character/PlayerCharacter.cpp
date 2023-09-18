
#include "PlayerCharacter.h"

#include "FPS_Deckbuilder/Interactable.h"
#include "FPS_Deckbuilder/Weapon/Weapon.h"



void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
}


void APlayerCharacter::Tick(float DeltaTime)
{
	FHitResult HitResult;
	FVector Start = GetActorLocation() + FVector(0.f, 0.f, BaseEyeHeight);
	FVector End = Start + GetController()->GetDesiredRotation().Vector() * InteractionDistance;

	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_GameTraceChannel1
	);

	// NOTE: Alternative solution using object types. Not used to keep number of object types down. Each interactable might want to be their own object type
	//FCollisionObjectQueryParams QueryParams;
	//QueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);
	//GetWorld()->LineTraceSingleByObjectType( HitResult, Start, End, QueryParams);


	/* The default setting for ECCGameTraceChannel1 (ie Interactable) is NoCollision.
	 * No check is done because the user must intentionally introduce a bug by setting
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
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::RightMouseButton_Pressed"));
}

void APlayerCharacter::RightMouseButton_Released()
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::RightMouseButton_Released"));
}

void APlayerCharacter::InteractButton_Pressed()
{
	if (TargetInteractable) TargetInteractable->Interact(this);
}


void APlayerCharacter::EquipWeapon(AWeapon* Weapon)
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);

	if (EquippedWeapon)
	{ 
		EquippedWeapon->StopFire(); 
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules(AttachmentRules, false));
		EquippedWeapon->SetActorLocation(Weapon->GetActorLocation());
		EquippedWeapon->SetActorRotation(FRotator(0.f));
		EquippedWeapon->SetActorEnableCollision(true);
	}

	Weapon->SetActorEnableCollision(false);
	Weapon->SetActorLocation(
		GetActorLocation() + \
		GetActorRightVector() * 50.f + \
		GetActorForwardVector() * 50.f
	);
	Weapon->AttachToActor(this, AttachmentRules);
	EquippedWeapon = Weapon;
}
