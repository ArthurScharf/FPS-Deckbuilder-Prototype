// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatusEffectBox.generated.h"


class UHorizontalBox;


/**
 * 
 */
UCLASS()
class FPS_DECKBUILDER_API UStatusEffectBox : public UUserWidget
{
	GENERATED_BODY()


public:
	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* Box;
	
};
