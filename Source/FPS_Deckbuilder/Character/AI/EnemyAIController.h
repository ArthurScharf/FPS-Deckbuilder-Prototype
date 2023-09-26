

#pragma once

#include "CoreMinimal.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "FPS_Deckbuilder/Character/PlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"


#include "AIController.h"
#include "EnemyAIController.generated.h"


class UAIPerceptionComponent;
class UAISenseConfig_Sight;
	

/**
 * AI Controller for EnemyCharacters. 
 * NOTE: This controller is shared between all enemies. 
 *       Thus, each enemy is required to run their unique behavior tree in their blueprint's BeginPlay event
 */
UCLASS(BlueprintType)
class FPS_DECKBUILDER_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

private:
	/* controls the blackboard variables responsible for setting the Enemy AI's state. */
	UFUNCTION()
	void HandleTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus); 

private:
	UPROPERTY(EditDefaultsOnly)
	UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(EditDefaultsOnly)
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditDefaultsOnly)
	float SecondsToLoseTargetPlayer;

	FTimerHandle SearchHandle;

public:
	FORCEINLINE void SetBlackboardTargetPlayerCharacter(APlayerCharacter* PlayerCharacter) { Blackboard->SetValueAsObject(FName("TargetPlayerCharacter"), PlayerCharacter); }
	
};
