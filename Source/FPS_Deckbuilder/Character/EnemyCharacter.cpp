#include "EnemyCharacter.h"
#include "Components/WidgetComponent.h"
#include "AIController.h"


class UBehaviorTree;


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

	if (!WidgetComponent) { UE_LOG(LogTemp, Error, TEXT("AEnemyCharacter::BeginPlay -- !WidgetComponent")); return;  }
	if (!BehaviorTree) { UE_LOG(LogTemp, Error, TEXT("AEnemyCharacter::BeginPlay -- !BehaviorTree")); return; }
	
	SetLazyHealthBar(Cast<ULazyHealthBar>(WidgetComponent->GetWidget()));
	Cast<AAIController>(GetController())->RunBehaviorTree(BehaviorTree);
}



void AEnemyCharacter::ReceiveDamage(FDamageStruct& DamageStruct)
{
	UE_LOG(LogTemp, Warning, TEXT("AEnemyCharacter::ReceiveDamage"));

	AGameCharacter::ReceiveDamage(DamageStruct);
}



void AEnemyCharacter::NotifyOfDamageDealt(FDamageStruct& DamageStruct)
{
	UE_LOG(LogTemp, Warning, TEXT("AEnemyCharacter::NotifyOfDamageDealt"));
}



void AEnemyCharacter::Die()
{
	// TODO: Ragdoll and/or spawned debris
	
	Destroy();
}