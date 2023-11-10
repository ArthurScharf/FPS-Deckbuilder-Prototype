// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_LineOfSight.generated.h"

/**
 * https://www.youtube.com/watch?v=rZ6hsf3wccM&t=189s
 */
UCLASS()
class FPS_DECKBUILDER_API UEnvQueryTest_LineOfSight : public UEnvQueryTest
{
	GENERATED_BODY()

	UEnvQueryTest_LineOfSight(const FObjectInitializer& ObjectInitializer);

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
	
	virtual FText GetDescriptionDetails() const override;
};
