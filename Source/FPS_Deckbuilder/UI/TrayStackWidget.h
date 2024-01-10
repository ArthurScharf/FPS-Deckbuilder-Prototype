// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Image.h"
#include "Engine/Texture2D.h"

#include "TrayStackWidget.generated.h"

class UCard;



/*
* 1. Rotation
* 2. Reset
* 
* slot specific visuals?
*  for the future
*/

/**
 * 
 */
UCLASS()
class FPS_DECKBUILDER_API UTrayStackWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/* Called by TrayStack to update the widget upon card rotation 
	* bRotated used to manage widget animations 
	*/
	void Update(UCard* Card, bool bRotated = true);


private:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess="true"))
	UImage* Image;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UTexture2D* DefaultImageTexture;
	

public:
	FORCEINLINE void SetImage(UImage* _Image) { Image = _Image; }
};
