// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "StatusEffectWidget.generated.h"


// class USizeBox;
//class UImage;


/**
 * 
 */
UCLASS()
class FPS_DECKBUILDER_API UStatusEffectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UImage* Image;
};

