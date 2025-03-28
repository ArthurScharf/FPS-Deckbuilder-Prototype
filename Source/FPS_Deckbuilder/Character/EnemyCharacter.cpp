#include "EnemyCharacter.h"
#include "AIController.h"
#include "Animation/AnimMontage.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "FPS_Deckbuilder/Character/EnemyAnimInstance.h"
#include "FPS_Deckbuilder/UI/EnemyWidget.h"
#include "FPS_Deckbuilder/Weapon/Projectile.h"
#include "Materials/Material.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


#include "DrawDebugHelpers.h"


class UBehaviorTree;


AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	ShellMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Shell Mesh"));
	ShellMeshComponent->SetMasterPoseComponent(GetMesh()); // Uses the character meshes pose
	ShellMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShellMeshComponent->SetupAttachment(RootComponent);

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
	
	if (WidgetComponent->GetWidgetClass())
	{
		UEnemyWidget* EnemyWidget = Cast<UEnemyWidget>(WidgetComponent->GetWidget());
		SetLazyHealthBar(EnemyWidget->LazyHealthBar);
		SetStatusEffectHorizontalBox(EnemyWidget->StatusEffectsHorizontalBox);
	}
	
	EnemyAnimInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	if (!EnemyAnimInstance) { UE_LOG(LogTemp, Error, TEXT("AEnemyCharacter::BeginPlay -- failed to cast to EnemyAnimInstance")); }

	EnemyAIController = Cast<AEnemyAIController>(GetController());
	if (EnemyAIController && BehaviorTree) { EnemyAIController->RunBehaviorTree(BehaviorTree); }
	else { UE_LOG(LogTemp, Error, TEXT("AEnemyCharacter::BeginPlay -- !EnemyAIController OR !BehaviorTree")); }

	// -- Creating & Storing Dynamic Material Instance for Shell Material & PostureBreakMat-- //
	ShellMatInstance = UMaterialInstanceDynamic::Create(ShellMat, this);
	ShellMeshComponent->SetMaterial(0, ShellMatInstance);
	ShellOpacity = 0.f;
	PostureBreakMatInst = UMaterialInstanceDynamic::Create(PostureBreakMat, this); //  Stored for later
}


void AEnemyCharacter::Tick(float DeltaTime)
{
	if (bPausedTick) return;

	Super::Tick(DeltaTime);

#if WITH_EDITOR
	if (EnemyAIController && bDrawAim)
	{
		AActor* TargetPlayerCharacter = Cast<AActor>(EnemyAIController->GetBlackboardComponent()->GetValueAsObject("TargetPlayerCharacter"));
		if (TargetPlayerCharacter)
		{
			FRotator Rotation = CalculateAimAtRotation(TargetPlayerCharacter->GetActorLocation());

			DrawDebugLine(
				GetWorld(),
				GetActorLocation(),
				(Rotation.RotateVector(FVector(1.f, 0, 0)) * 500.f) + GetActorLocation(),
				FColor::White,
				false,
				0.1f,
				(uint8)0U,
				1.f
			);
		}
	}
#endif 
}


void AEnemyCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	WidgetComponent->DestroyComponent(); // Avoids the "Destroyed while GCing" issue
}


void AEnemyCharacter::ReceiveDamage(FDamageStruct& DamageStruct, bool bTriggersStatusEffects)
{
	/* TODO
	* This function is becoming spaghetti code. I need to rework it with the posture system in mind 
	*/

	// -- Player detection -- //
	if (DamageStruct.DamageCauser && DamageStruct.DamageCauser->IsA<APlayerCharacter>())
	{	// Entering searching behavior
		// UE_LOG(LogTemp, Warning, TEXT("AEnemyCharacter::ReceiveDamage -- Entering Search Mode"));
		//EnemyAIController->SetSearchTimer();
		EnemyAIController->SetFocus(DamageStruct.DamageCauser);
		EnemyAIController->SetBlackboardTargetLocation(DamageStruct.DamageCauser->GetActorLocation()); // needs this for seeking mode
		EnemyAIController->SetBlackboardTargetPlayerCharacter(Cast<APlayerCharacter>(DamageStruct.DamageCauser)); // Downcasting !!!
		//EnemyAIController->SetFocus(DamageStruct.DamageCauser);
	}
	
	// -- Special DamageHandling -- //
	// NOTE: Since these methods handle impact effects, we need this to run before the next block that handles impulse on already dead character.
	// This is a code stink. See first comment in this function
	HandleSpecialDamageConditions(DamageStruct);
	
	//- Shooting an Enemy After it's died -//
	if (bIsDead)
	{
		GetMesh()->AddImpulseAtLocation(-DamageStruct.HitResult.ImpactNormal * 5000.f, DamageStruct.HitResult.ImpactPoint, HitBoneName);
		return;
	}


	bool bEnemyShouldDie = false;
	// -- Posture Damage & Shell Material Update -- //
	if (Posture < MaxPosture) // Posture Currently Unbroken
	{
		//- Posture Damage being dealt
		DamageStruct.bWasPostureDamage = true;
		Posture += DamageStruct.Damage;

		if (Posture >= MaxPosture) 
		{
			//- Posture is broken by this instance of damage
			if (GetHealth() <= 0.f)
			{
				bEnemyShouldDie = true; // Instant death for enemies with no health
				ShellMeshComponent->SetVisibility(false);
			}
			else
			{
				ShellMeshComponent->SetMaterial(0, PostureBreakMatInst);
				Stun(PostureBreakSeconds);
			}
			

			// -- Setting Recovery Timer -- //
			GetWorldTimerManager().SetTimer(
				PostureBreakRecoveryHandle,
				[&]() {
					if (!IsValid(this)) return;
					ShellMeshComponent->SetMaterial(0, ShellMatInstance);
					Posture = 0.f;
					ShellMatInstance->SetScalarParameterValue(FName("MatAlpha"), 0.2);
					ShellOpacity = 1.f;
					ShellMatInstance->SetScalarParameterValue(FName("Opacity"), ShellOpacity);
					
					GetWorldTimerManager().SetTimer(
						ShellOpacityHandle,
						[&]() {
							if (!IsValid(this)) return;
							UE_LOG(LogTemp, Warning, TEXT("Fading Shell"));
							ShellOpacity -= ShellOpacityDecayAmount;
							ShellMatInstance->SetScalarParameterValue(FName("Opacity"), ShellOpacity);
							if (ShellOpacity <= 0.f) 
							{
								ShellMatInstance->SetScalarParameterValue(FName("MatAlpha"), 0.f);
								GetWorldTimerManager().ClearTimer(ShellOpacityHandle); 
							}
						},
						ShellOpacityDecayRate,
						true,
						0.f
					);
				},
				PostureBreakSeconds,
				false
			);
			if (NiagaraPostureBreakSystem)
			{
				UNiagaraComponent* PostureBreakComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
					NiagaraPostureBreakSystem,
					GetMesh(),
					"",
					FVector(0), // GetCapsuleComponent()->GetScaledCapsuleHalfHeight()
					FRotator(0),
					EAttachLocation::SnapToTarget,
					true
				);
			}
		}
		else
		{//- Posture NOT broken by this instance of damage. Modifying shell mat and setting opacity timer
			float PosturePercent = Posture / MaxPosture;
			ShellMatInstance->SetScalarParameterValue(FName("MatAlpha"), PosturePercent);
			ShellOpacity = 1.f;
			ShellMatInstance->SetScalarParameterValue(FName("Opacity"), ShellOpacity);

			if (PosturePercent < 0.8f)
			{
				GetWorldTimerManager().SetTimer(
					ShellOpacityHandle,
					[&]() {
						ShellOpacity -= ShellOpacityDecayAmount;
						ShellMatInstance->SetScalarParameterValue(FName("Opacity"), ShellOpacity);
						if (ShellOpacity <= 0.f) { GetWorldTimerManager().ClearTimer(ShellOpacityHandle); }
					},
					ShellOpacityDecayRate,
					true,
					FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1.f), FVector2D(0.f, 3.f), PosturePercent)
				);
			}
		}		
	}

	AGameCharacter::ReceiveDamage(DamageStruct, bTriggersStatusEffects);
	
	if (bEnemyShouldDie) Die();
	//- Did damage kill it? -//
	if (bIsDead)
	{
		ShellMeshComponent->SetVisibility(false);
		GetMesh()->AddImpulseAtLocation(-DamageStruct.HitResult.ImpactNormal * 20000.f, DamageStruct.HitResult.ImpactPoint, HitBoneName);
	}
	HitBoneName = "None";



	// -- Hitstun -- //
	// EnemyAIController->BrainComponent->PauseLogic("test"); // Alternate method that allows for more control over hit react animations
	CustomTimeDilation = 0.2f;
	FTimerHandle UnpauseTickHandle;
	GetWorldTimerManager().SetTimer
	(
		UnpauseTickHandle,
		[&]() {
			CustomTimeDilation = 1.f;
			// EnemyAIController->BrainComponent->ResumeLogic("test");
		},
		0.03,
		false
	);

	// -- Animation -- //
	if (EnemyAnimInstance && DamageStruct.Damage > 0.f) EnemyAnimInstance->PlayHitReactMontage();
}


void AEnemyCharacter::Die()
{
	// TODO: Ragdoll and/or spawned debris
	// -- Clearing Timers -- //
	EnemyAIController->ClearSearchTimer();
	GetWorldTimerManager().ClearTimer(ShellOpacityHandle);
	ShellOpacityHandle.Invalidate();
	GetWorldTimerManager().ClearTimer(PostureBreakRecoveryHandle);
	PostureBreakRecoveryHandle.Invalidate();
	// WidgetComponent->GetWidget()->RemoveFromViewport(); // Attempting to avoid a double free bug

	// -- Stopping Logic -- //
	UBehaviorTreeComponent* BT = Cast<UBehaviorTreeComponent>(EnemyAIController->GetBrainComponent());
	if (IsValid(BT))
	{
		BT->StopTree(EBTStopMode::Safe);
	}

	// -- Modifying Collision to support ragdolled state -- //
	GetCharacterMovement()->DisableMovement();
	USkeletalMeshComponent* LocalMesh = GetMesh();
	LocalMesh->SetSimulatePhysics(true);
	LocalMesh->SetCollisionProfileName(FName("PhysicsActor"));
	LocalMesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	for (AActor* Actor : AttachedActors)
	{
		Actor->Destroy();
	}

	Super::Die();
}




void AEnemyCharacter::Stun(float StunSeconds)
{
	if (!EnemyAIController)
	{
		UE_LOG(LogTemp, Error, TEXT("AEnemyCharacter::Stun / %s -- !EnemyAIController"), *GetName());
		return;
	}
	if (!EnemyAnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("AEnemyCharacter::Stun / %s -- !EnemyAnimInstance"), *GetName());
		return;
	}

	// -- Setting stun timer -- //
	UCharacterMovementComponent* CharMovement = GetCharacterMovement();
	if (!bIsStunned)
	{
		bIsStunned = true;
		EnemyAIController->BrainComponent->PauseLogic("Stunned");
		EnemyAnimInstance->SetIsStunned(true);
		StoredRotationRate = CharMovement->RotationRate;
		CharMovement->RotationRate = FRotator(0.f);
		CharMovement->StopMovementImmediately();
	}

	GetWorldTimerManager().SetTimer(
		StunnedHandle,
		[&]()
		{
			if (!IsValid(this) && !IsValid(EnemyAIController)) return;
			bIsStunned = false;
			EnemyAIController->BrainComponent->ResumeLogic("Stun Completed");
			EnemyAnimInstance->SetIsStunned(false);
			GetCharacterMovement()->RotationRate = StoredRotationRate;
		},
		StunSeconds,
		false
	);
}

//void AEnemyCharacter::NotifyOfDamageDealt(FDamageStruct& DamageStruct)
//{
//	UE_LOG(LogTemp, Warning, TEXT("AEnemyCharacter::NotifyOfDamageDealt"));
//}


//FDamageStruct AEnemyCharacter::HandleSpecialDamageConditions_Implementation(FDamageStruct& DamageStruct)
//{
//	UE_LOG(LogTemp, Error, TEXT("AEnemyCharacter::HandleSpecialDamageConditions_Implementation -- This is a stub. Do you need to implement a BP override?"));
//	return DamageStruct;
//}


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





FRotator AEnemyCharacter::CalculateAimAtRotation(const FVector& TargetLocation)
{
	return	FRotator(
		(TargetLocation - (GetActorLocation() + FVector(0, 0, AimOffset))).Rotation().Pitch,
		GetActorRotation().Yaw,
		0.f
	);
}