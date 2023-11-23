

#include "EnemyAnimInstance.h"
#include "GameFramework/Pawn.h"


//void UEnemyAnimInstance::NativeInitializeAnimation()
//{
//	APawn* Owner = TryGetPawnOwner();
//	if (!Owner)
//	{
//		UE_LOG(LogTemp, Error, TEXT("UEnemyAnimInstance::NativeInitializeAnimation / %s -- !TryGetPawnOwner()"), *GetName());
//		return;
//	}
//	EnemyCharacter = Cast<AEnemyCharacter>(Owner);
//	if (!EnemyCharacter)
//	{
//		UE_LOG(LogTemp, Error, TEXT("UEnemyAnimInstance::NativeInitializeAnimation / %s -- Failed to cast"), *GetName());
//		return;
//	}
//}



//void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTime)
//{
//	//Speed = EnemyCharacter->GetVelocity().Size();
//	//Direction = CalculateDirection(EnemyCharacter->GetVelocity(), EnemyCharacter->GetActorRotation());
//}



void UEnemyAnimInstance::PlayHitReactMontage()
{
	if (HitReactMontages.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyAnimInstance::PlayHitReactMontage -- HitReactMontages == 0"));
		return;
	}
	
	Montage_Play(HitReactMontages[FMath::Rand() % HitReactMontages.Num()]);
}