#include "HUDWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/NamedSlot.h"




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


void UHUDWidget::AddTraySlot()
{
	UTraySlot* TraySlot = CreateWidget<UTraySlot>(this, TraySlotBlueprintClass);
	TraySlot->Padding = FMargin(5.f);
	TrayHorizontalBox->AddChildToHorizontalBox(TraySlot);
}


void UHUDWidget::RemoveTraySlot()
{
	TrayHorizontalBox->RemoveChildAt(TrayHorizontalBox->GetChildrenCount() - 1);
}

void UHUDWidget::SetCardForSlotAtIndex(int Index, UCard* Card)
{
	if (Index >= TrayHorizontalBox->GetChildrenCount()) { UE_LOG(LogTemp, Error, TEXT("UHUDWidget::SetCardForSlotAtIndex -- Index too big")); return; }
	if (!Card) { UE_LOG(LogTemp, Error, TEXT("UHUDWidget::SetCardForSlotAtIndex -- !Card")); return; }

	UTraySlot* TraySlot = Cast<UTraySlot>(TrayHorizontalBox->GetChildAt(Index));
	TraySlot->SetCard(Card);
	TraySlot->SetTexture(Card->GetTexture());
}


