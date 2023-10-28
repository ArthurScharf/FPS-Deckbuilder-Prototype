// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grammar.h"
#include "Grammar_Turbine.generated.h"

class UShape;

/**
 * 
 */
UCLASS()
class FPS_DECKBUILDER_API UGrammar_Turbine : public UGrammar
{
	GENERATED_BODY()

public:
	virtual void Init(TMultiMap<FString, UShape*>& Shapes) override;
	
};
