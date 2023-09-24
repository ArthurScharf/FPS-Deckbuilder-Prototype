// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "Components/WidgetComponent.h"


AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
	WidgetComponent->SetWorldScale3D(FVector(0.2));
	WidgetComponent->SetDrawSize(FVector2D(1000.f, 500.f));
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WidgetComponent->SetupAttachment(RootComponent);
}


void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (WidgetComponent)
	{
		SetLazyHealthBar(Cast<ULazyHealthBar>(WidgetComponent->GetWidget()));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AEnemyCharacter::BeginPlay -- !WidgetComponent"));
	}
	
}



void AEnemyCharacter::ReceiveDamage(FDamageStruct& DamageStruct)
{



	Super::ReceiveDamage(DamageStruct);
}



void AEnemyCharacter::Die()
{
	// TODO: Ragdoll and/or spawned debri
	
	Destroy();
}