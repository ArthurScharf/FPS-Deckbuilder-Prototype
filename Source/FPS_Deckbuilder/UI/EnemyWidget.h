// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyWidget.generated.h"

class ULazyHealthBar;
class UHorizontalBox;

/**
 * 
 */
UCLASS()
class FPS_DECKBUILDER_API UEnemyWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	ULazyHealthBar* LazyHealthBar;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UHorizontalBox* StatusEffectsHorizontalBox;
};
