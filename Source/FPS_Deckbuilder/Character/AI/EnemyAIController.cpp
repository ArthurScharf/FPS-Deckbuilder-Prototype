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


void AEnemyAIController::HandleTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	UE_LOG(LogTemp, Warning, TEXT("AEnemyAIController::OnTargetPerceptionUpdated"))

	if (!Actor->ActorHasTag(FName("PlayerCharacter"))) return; // Will only cast if seeing player character. improves performance
	
	UE_LOG(LogTemp, Warning, TEXT("AEnemyAIController::OnTargetPerceptionUpdated -- Player Seen"))

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
	SetFocus(PlayerCharacter); 
	Blackboard->SetValueAsObject(FName("TargetPlayerCharacter"), PlayerCharacter);
}
