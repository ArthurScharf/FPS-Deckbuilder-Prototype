// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "TrayStackWidget.h"


#include "FPS_Deckbuilder/Card/Card.h"



void UTrayStackWidget::Update(UCard* Card, bool bRotated)
{
	// UE_LOG(LogTemp, Warning, TEXT("UTrayStackWidget::Update"));

	if (!Image)
	{
		UE_LOG(LogTemp, Error, TEXT("UTrayStackWidget::Update -- !Image"));
		return;
	}
	if (!DefaultImageTexture)
	{
		UE_LOG(LogTemp, Error, TEXT("UTrayStackWidget::Update -- !DefaultImageTexture"));
		return;
	}

	if (Card) Image->SetBrushFromTexture(Card->GetTexture());
	else Image->SetBrushFromTexture(DefaultImageTexture);

	// TODO: Play Animations & Affects
}