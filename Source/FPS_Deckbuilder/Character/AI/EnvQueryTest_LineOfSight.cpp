

#include "EnvQueryTest_LineOfSight.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "FPS_Deckbuilder/Character/EnemyCharacter.h"
#include "FPS_Deckbuilder/Character/PlayerCharacter.h"


#include "DrawDebugHelpers.h"


/* NOTE: What is any of this doing?*/
UEnvQueryTest_LineOfSight::UEnvQueryTest_LineOfSight(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Cost = EEnvTestCost::High;
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	SetWorkOnFloatValues(false);
}


void UEnvQueryTest_LineOfSight::RunTest(FEnvQueryInstance& QueryInstance) const
{
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		const FVector ItemLocation = GetItemLocation(QueryInstance, It.GetIndex());
		
		// -- Line traces from each point to context to see if each point has line of sight -- //
		AEnemyCharacter* QueryContext = Cast<AEnemyCharacter>(QueryInstance.Owner.Get());
		UE_LOG(LogTemp, Warning, TEXT("UEnvQueryTest_LineOfSight::RunTest -- Owner.Name: %s"), *QueryContext->GetName());

		FVector TargetPlayerCharacterLocation = QueryContext->GetTargetPlayerCharacter()->GetActorLocation();

		FHitResult HitResult;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(QueryContext);
		GetWorld()->LineTraceSingleByChannel(
			HitResult,
			ItemLocation + FVector(0, 0, 200), // TODO: Hardcoded. Should be using the QueryContext eye height
			TargetPlayerCharacterLocation,
			ECollisionChannel::ECC_Visibility
		);

		//DrawDebugLine(
		//	GetWorld(),
		//	ItemLocation + FVector(0, 0, 200),
		//	TargetPlayerCharacterLocation,
		//	HitResult.bBlockingHit ? FColor::Red : FColor::Green,
		//	false,
		//	3.f
		//);

		It.SetScore(TestPurpose, FilterType, HitResult.bBlockingHit, false); // last param decides what a true value is considered?

	}
}

FText UEnvQueryTest_LineOfSight::GetDescriptionDetails() const
{
	return DescribeBoolTestParams("line of sight");
}
