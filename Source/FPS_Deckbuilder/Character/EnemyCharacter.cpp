#include "EnemyCharacter.h"
#include "AIController.h"
#include "Animation/AnimMontage.h"
#include "Components/WidgetComponent.h"

#include "FPS_Deckbuilder/Character/EnemyAnimInstance.h"


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
	EnemyAIController->RunBehaviorTree(BehaviorTree);

}


void AEnemyCharacter::ReceiveDamage(FDamageStruct& DamageStruct)
{
	UE_LOG(LogTemp, Warning, TEXT("AEnemyCharacter::ReceiveDamage"));

	// Being attacked
	if (DamageStruct.DamageCauser && DamageStruct.DamageCauser->IsA<APlayerCharacter>())
	{	// Entering searching behavior
		UE_LOG(LogTemp, Warning, TEXT("AEnemyCharacter::ReceiveDamage -- Entering Search Mode"));
		EnemyAIController->SetSearchTimer();
		EnemyAIController->SetBlackboardTargetLocation(DamageStruct.DamageCauser->GetActorLocation()); // needs this for seeking mode
		EnemyAIController->SetBlackboardTargetPlayerCharacter(Cast<APlayerCharacter>(DamageStruct.DamageCauser)); // Downcasting !!!
	}
	/*
	* I need an new damage struct here as a workaround. 
	* Look at the comment for AEnemyCharacter::HandleSpecialDamageConditions for context
	*/
	FDamageStruct _DamageStruct = HandleSpecialDamageConditions(DamageStruct);
	HitBoneName = "None";
	AGameCharacter::ReceiveDamage(_DamageStruct); 
}


void AEnemyCharacter::NotifyOfDamageDealt(FDamageStruct& DamageStruct)
{
	UE_LOG(LogTemp, Warning, TEXT("AEnemyCharacter::NotifyOfDamageDealt"));
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


	Destroy();
}