#include "HUDWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/NamedSlot.h"


#include "Components/CanvasPanelSlot.h"

void UHUDWidget::NativeConstruct()
{
	// TODO: Is this method needed?
	SlotTop = Cast<UCanvasPanelSlot>(CrosshairTop->Slot);
	SlotBottom = Cast<UCanvasPanelSlot>(CrosshairBottom->Slot);
	SlotLeft = Cast<UCanvasPanelSlot>(CrosshairLeft->Slot);
	SlotRight= Cast<UCanvasPanelSlot>(CrosshairRight->Slot);

}


void UHUDWidget::UpdateCrosshairsSpread(float Spread)
{
	FVector2D ViewportCenter;
	GEngine->GameViewport->GetViewportSize(ViewportCenter);
	ViewportCenter /= 2.f;


	//for (UPanelSlot* S : CanvasPanel->GetSlots())
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, S->GetFName().ToString());
	//}

	SlotTop->SetPosition(FVector2D(     0.f,                           -(BaseCrosshairOffset + Spread)));
	SlotBottom->SetPosition(FVector2D(  0.f,                             BaseCrosshairOffset + Spread ));
	SlotLeft->SetPosition(FVector2D(  -(BaseCrosshairOffset + Spread),   0.f                          ));
	SlotRight->SetPosition(FVector2D(   BaseCrosshairOffset + Spread,    0.f                          ));
}