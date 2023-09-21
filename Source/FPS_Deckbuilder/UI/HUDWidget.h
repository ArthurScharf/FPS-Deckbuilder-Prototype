#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"


class UCanvasPanel;
class UCanvasPanelSlot;
class UNamedSlot;


/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class FPS_DECKBUILDER_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeConstruct() override;
	
	void UpdateCrosshairsSpread(float Spread);

private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UCanvasPanel* CanvasPanel;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UNamedSlot* CrosshairTop;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UNamedSlot* CrosshairBottom;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UNamedSlot* CrosshairLeft;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UNamedSlot* CrosshairRight;

	// The position relative to the center of the screen that each crosshair when no other factor is included in the crosshairs position
	UPROPERTY(EditDefaultsOnly)
	float BaseCrosshairOffset = 20;


	UCanvasPanelSlot* SlotTop;
	UCanvasPanelSlot* SlotBottom;
	UCanvasPanelSlot* SlotLeft;
	UCanvasPanelSlot* SlotRight;

};
