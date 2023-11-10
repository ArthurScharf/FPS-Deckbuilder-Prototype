#include "GameCharacterMovementComponent.h"

#include "GameFramework/CharacterMovementComponent.h"


UGameCharacterMovementComponent::UGameCharacterMovementComponent()
{

}


void UGameCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent(); 
	GameCharacterOwner = Cast<AGameCharacter>(GetOwner());
}



void UGameCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{

}


void UGameCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{

}



#pragma region Input


void UGameCharacterMovementComponent::DashPressed()
{

}


bool UGameCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const 
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

#pragma endregion