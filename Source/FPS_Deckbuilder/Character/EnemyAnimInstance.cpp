

#include "EnemyAnimInstance.h"




void UEnemyAnimInstance::PlayHitReactMontage()
{
	if (HitReactMontages.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyAnimInstance::PlayHitReactMontage -- HitReactMontages == 0"));
		return;
	}
	
	Montage_Play(HitReactMontages[FMath::Rand() % HitReactMontages.Num()]);
}