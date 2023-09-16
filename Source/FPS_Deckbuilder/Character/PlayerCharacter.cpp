
#include "PlayerCharacter.h"



void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
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
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::LeftMouseButton_Pressed"));
}

void APlayerCharacter::LeftMouseButton_Released()
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::LeftMouseButton_Released"));
}

void APlayerCharacter::RightMouseButton_Pressed()
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::RightMouseButton_Pressed"));
}

void APlayerCharacter::RightMouseButton_Released()
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::RightMouseButton_Released"));
}



