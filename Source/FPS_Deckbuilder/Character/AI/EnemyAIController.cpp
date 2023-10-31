// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"


#include "Perception/AISenseConfig_Sight.h"


AEnemyAIController::AEnemyAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::HandleTargetPerceptionUpdate);
}


void AEnemyAIController::SetSearchTimer()
{
	GetWorldTimerManager().SetTimer(	/* Setting timer to lose player character and return to roaming state */
		SearchHandle,
		[&]() {
			UE_LOG(LogTemp, Warning, TEXT("AEnemyAIController::SetTimerToClearBlackboardTargetPlayer -- Returning to roaming behavior"));
			Blackboard->SetValueAsBool(FName("bCanSeePlayer"), false);
			Blackboard->SetValueAsObject(FName("TargetPlayerCharacter"), nullptr);
		},
		SecondsToLoseTargetPlayer,
		false
	);
}

void AEnemyAIController::ClearSearchTimer()
{
	GetWorldTimerManager().ClearTimer(SearchHandle);
}


/* NOTE: This method might be too complex */
void AEnemyAIController::HandleTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor->ActorHasTag(FName("PlayerCharacter"))) return; // Will only cast if seeing player character. improves performance
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);

	if (Blackboard->GetValueAsObject(FName("TargetPlayerCharacter")))	// Losing sight of the player OR regaining sight of the player
	{	
		if (Blackboard->GetValueAsBool(FName("bCanSeePlayer")))
		{ /* Losing sight of the player */
			UE_LOG(LogTemp, Warning, TEXT("AEnemyAIController::HandleTargetPerceptionUpdate -- Lost sight of player"));
			SetFocus(nullptr);
			Blackboard->SetValueAsBool(FName("bCanSeePlayer"), false);	// Behavior tree reacts
			Blackboard->SetValueAsVector(FName("TargetLocation"), Actor->GetActorLocation());
			SetSearchTimer();
		}
		else
		{ /* Regaining sight of the player */
			UE_LOG(LogTemp, Warning, TEXT("AEnemyAIController::HandleTargetPerceptionUpdate -- Regained sight of player"));
			SetFocus(PlayerCharacter);
			Blackboard->SetValueAsBool(FName("bCanSeePlayer"), true);	// Behavior tree reacts
			GetWorldTimerManager().ClearTimer(SearchHandle);
		}
	}
	else	
	{	/* Seeing player for the first time */
		UE_LOG(LogTemp, Warning, TEXT("AEnemyAIController::HandleTargetPerceptionUpdate -- First time detecting player"));
		SetFocus(PlayerCharacter);
		Blackboard->SetValueAsObject(FName("TargetPlayerCharacter"), PlayerCharacter);
		Blackboard->SetValueAsBool(FName("bCanSeePlayer"), true);
	}
}
