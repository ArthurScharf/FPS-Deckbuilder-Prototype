#include "HUDWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/NamedSlot.h"

#include "FPS_Deckbuilder/Card/TrayStack.h"



void UHUDWidget::NativeConstruct()
{
	// TODO: Is this method needed?
	SlotTop = Cast<UCanvasPanelSlot>(CrosshairTop->Slot);
	SlotBottom = Cast<UCanvasPanelSlot>(CrosshairBottom->Slot);
	SlotLeft = Cast<UCanvasPanelSlot>(CrosshairLeft->Slot);
	SlotRight= Cast<UCanvasPanelSlot>(CrosshairRight->Slot);

	BaseCrosshairOffset = 10.f;
}


void UHUDWidget::UpdateCrosshairsSpread(float Spread)
{
	FVector2D ViewportCenter;
	GEngine->GameViewport->GetViewportSize(ViewportCenter);
	ViewportCenter /= 2.f;

	SlotTop->SetPosition(   FVector2D(   0.f,                                 -(BaseCrosshairOffset + Spread * 20.f) ));
	SlotBottom->SetPosition(FVector2D(   0.f,                                   BaseCrosshairOffset + Spread * 20.f));
	SlotLeft->SetPosition(  FVector2D( -(BaseCrosshairOffset + Spread * 20.f) , 0.f                                  ));
	SlotRight->SetPosition( FVector2D(   BaseCrosshairOffset + Spread * 20.f  , 0.f                                  ));
}


void UHUDWidget::CreateAndLinkTrayStackWidget(UTrayStack* TrayStack)
{
	UE_LOG(LogTemp, Warning, TEXT("UHUDWidget::CreateAndLinkTrayStackWidget"));

	if (!TrayStackWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UHUDWidget::CreateAndLinkTrayStackWidget -- !TrayStackWidgetClass"));
		return;
	}
	UTrayStackWidget* Widget = CreateWidget<UTrayStackWidget>(this, TrayStackWidgetClass);
	TrayStack->SetWidget(Widget);
	// Widget->Padding = FMargin(5.f);
	TrayHorizontalBox->AddChildToHorizontalBox(Widget);
}