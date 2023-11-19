#include "EnemyCharacter.h"
#include "AIController.h"
#include "Animation/AnimMontage.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/WidgetComponent.h"
#include "FPS_Deckbuilder/Character/EnemyAnimInstance.h"
#include "FPS_Deckbuilder/Weapon/Projectile.h"

#include "DrawDebugHelpers.h"

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

	// Setting for smooth rotation
	bUseControllerRotationYaw = false; 
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}



void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!WidgetComponent) { UE_LOG(LogTemp, Error, TEXT("AEnemyCharacter::BeginPlay -- !WidgetComponent")); return;  }
	if (!BehaviorTree) { UE_LOG(LogTemp, Error, TEXT("AEnemyCharacter::BeginPlay -- !BehaviorTree")); return; }
	
	SetLazyHealthBar(Cast<ULazyHealthBar>(WidgetComponent->GetWidget()));
	
	EnemyAnimInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	if (!EnemyAnimInstance) { UE_LOG(LogTemp, Error, TEXT("AEnemyCharacter::BeginPlay -- failed to cast to EnemyAnimInstance")); }

	
	EnemyAIController = Cast<AEnemyAIController>(GetController());
	if (EnemyAIController && BehaviorTree) { EnemyAIController->RunBehaviorTree(BehaviorTree); }
	else { UE_LOG(LogTemp, Error, TEXT("AEnemyCharacter::BeginPlay -- !EnemyAIController OR !BehaviorTree")); }
}


void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyCharacter::ReceiveDamage(FDamageStruct& DamageStruct)
{
	// UE_LOG(LogTemp, Warning, TEXT("AEnemyCharacter::ReceiveDamage"));

	if (DamageStruct.DamageCauser && DamageStruct.DamageCauser->IsA<APlayerCharacter>())
	{	// Entering searching behavior
		UE_LOG(LogTemp, Warning, TEXT("AEnemyCharacter::ReceiveDamage -- Entering Search Mode"));
		//EnemyAIController->SetSearchTimer();
		EnemyAIController->SetBlackboardTargetLocation(DamageStruct.DamageCauser->GetActorLocation()); // needs this for seeking mode
		EnemyAIController->SetBlackboardTargetPlayerCharacter(Cast<APlayerCharacter>(DamageStruct.DamageCauser)); // Downcasting !!!
		//EnemyAIController->SetFocus(DamageStruct.DamageCauser);
	}
	
	// I need an new damage struct here as a workaround. 
	// Look at the comment for AEnemyCharacter::HandleSpecialDamageConditions for context
	FDamageStruct _DamageStruct = HandleSpecialDamageConditions(DamageStruct);
	HitBoneName = "None";
	AGameCharacter::ReceiveDamage(_DamageStruct);

	if (EnemyAnimInstance) EnemyAnimInstance->PlayHitReactMontage();
}


void AEnemyCharacter::NotifyOfDamageDealt(FDamageStruct& DamageStruct)
{
	UE_LOG(LogTemp, Warning, TEXT("AEnemyCharacter::NotifyOfDamageDealt"));
}


FDamageStruct AEnemyCharacter::HandleSpecialDamageConditions_Implementation(FDamageStruct DamageStruct)
{
	UE_LOG(LogTemp, Error, TEXT("AEnemyCharacter::HandleSpecialDamageConditions_Implementation -- This is a stub. Do you need to implement a BP override?"));
	return DamageStruct;
}



FRotator AEnemyCharacter::GetViewRotation() const
{
	if (EnemyAIController->GetFocusActor())
	{   // Locked on to player character
		return Controller->GetControlRotation();
	}

	// Facing the same direction as the pawn
	return GetActorForwardVector().Rotation();
}




void AEnemyCharacter::Attack(UAnimMontage* AttackMontage)
{
	EnemyAnimInstance->Montage_Play(AttackMontage);
}







void AEnemyCharacter::Die()
{
	// TODO: Ragdoll and/or spawned debris
	EnemyAIController->ClearSearchTimer();

	UBehaviorTreeComponent* BT = Cast<UBehaviorTreeComponent>(EnemyAIController->GetBrainComponent());
	if (IsValid(BT))
	{
		BT->StopTree(EBTStopMode::Safe);
	}
	
	// TODO: Propery Die animation and etc
	this->SetActorHiddenInGame(true);
	this->SetActorEnableCollision(false);

	Super::Die();
}