#pragma once

#include "CoreMinimal.h"
#include "FPS_Deckbuilder/UI/LazyHealthBar.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"


class UCanvasPanel;
class UCanvasPanelSlot;
class UNamedSlot;
class UTextBlock;


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
	// TODO: Do I need this declared here or can I just implement it in the Blueprint?
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UCanvasPanel* CanvasPanel;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	ULazyHealthBar* LazyHealthBar;

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

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* CurrentAmmoText;


	/* Stores crosshairs slots so their position can be changed to reflect crosshair spread */
	UCanvasPanelSlot* SlotTop;
	UCanvasPanelSlot* SlotBottom;
	UCanvasPanelSlot* SlotLeft;
	UCanvasPanelSlot* SlotRight;

public:
	// TODO: Apparently I don't need a force inline?
	UTextBlock* GetCurrentAmmoText() { return CurrentAmmoText; }

	void SetHealthPercent(float Percent) { LazyHealthBar->SetPercent(Percent); }
};
