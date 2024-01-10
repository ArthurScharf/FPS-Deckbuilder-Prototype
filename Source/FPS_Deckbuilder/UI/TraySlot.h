
#pragma once

#include "CoreMinimal.h"

#include "Components/Image.h"

#include "Blueprint/UserWidget.h"
#include "TraySlot.generated.h"


class UCanvasPanel;
class UCard;
class UOverlay;
class UProgressBar;
class USizeBox;


/**
 * DELETE THIS
 */
UCLASS()
class FPS_DECKBUILDER_API UTraySlot : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	UImage* Image;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) // Same as having a blueprint only getter. Allows binding of the card's variables
	UCard* Card;

public:
	// -- Getters and Setters -- // 
	FORCEINLINE void SetCard(UCard* _Card) { Card = _Card; }

	FORCEINLINE void SetTexture(UTexture2D* Texture) { Image->SetBrushFromTexture(Texture); }
};