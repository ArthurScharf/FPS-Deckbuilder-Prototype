#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "FPS_Deckbuilder/Card/Card.h"
#include "FPS_Deckbuilder/Character/StatusEffect.h"
#include "FPS_Deckbuilder/UI/LazyHealthBar.h"
#include "Math/IntVector.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class UCanvasPanel;
class UCanvasPanelSlot;
class UNamedSlot;
class UTrayStackWidget;
class UTrayStack;

/**
 * The HUD widget class for the player character.
 * I've decided, as an exercise to write this class as agnostic to 
 * the class it's attached to, despite the fact that it won't be used by anthing else
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class FPS_DECKBUILDER_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeConstruct() override;
	
	/* Called by the owning player character each tick */
	void UpdateCrosshairsSpread(float Spread);

	// void UpdateTrayWidget();





	/* Adds a tray stack widget to the end of the tray 
	 * WARNING: This method doesn't know anything about the underlying data structure the TraySlot or TrayHorizontalBox is representing. 
	 */
	void CreateAndLinkTrayStackWidget(UTrayStack* TrayStack);






	// Creates and adds a status effect to the StatusEffectHorizontalBox. StatusEffect manages lifetime of widget
	UUserWidget* InstantiateStatusEffectWidget(UStatusEffect* StatusEffect);

	// Removes the widget from the widget bar. 
	void RemoveStatusEffectWidget(UUserWidget* Widget);





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
	float BaseCrosshairOffset = 0.f;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* CurrentAmmoText;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* Resource_X_Text;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* Resource_Y_Text;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* Resource_Z_Text;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* MoneyText;

	/* Stores TraySlot widgets representing the cards in the tray */
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UHorizontalBox* TrayHorizontalBox;

	UPROPERTY(EditDefaultsOnly, Category = "HUDWidget")
	TSubclassOf<UTrayStackWidget> TrayStackWidgetClass;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UHorizontalBox* StatusEffectHorizontalBox;

	/* Stores crosshairs slots so their position can be changed to reflect crosshair spread */
	UCanvasPanelSlot* SlotTop;
	UCanvasPanelSlot* SlotBottom;
	UCanvasPanelSlot* SlotLeft;
	UCanvasPanelSlot* SlotRight;

public:
	// TODO: Apparently I don't need a force inline?
	FORCEINLINE UTextBlock* GetCurrentAmmoText() { return CurrentAmmoText; }

	FORCEINLINE ULazyHealthBar* GetLazyHealthBar() { return LazyHealthBar; }

	FORCEINLINE UHorizontalBox* GetStatusEffectHorizontalBox() { return StatusEffectHorizontalBox; }

	FORCEINLINE void SetResourceText(FIntVector& Resources) 
	{
		// NOTE: This seems like too many casts
		Resource_X_Text->SetText(FText::FromString(FString::FromInt(Resources[0])));
		Resource_Y_Text->SetText(FText::FromString(FString::FromInt(Resources[1])));
		Resource_Z_Text->SetText(FText::FromString(FString::FromInt(Resources[2])));
	}

	FORCEINLINE void SetMoneyText(int Money) { MoneyText->SetText(FText::FromString(FString::FromInt(Money))); }
};
