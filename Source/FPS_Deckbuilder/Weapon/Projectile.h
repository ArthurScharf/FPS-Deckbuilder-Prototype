#pragma once

#include "CoreMinimal.h"

#include "FPS_Deckbuilder/Character/GameCharacter.h"

#include "GameFramework/Actor.h"
#include "Projectile.generated.h"



class AActor;
class UProjectileMovementComponent;
class USphereComponent;
struct FDamageStruct;

/*  */
DECLARE_EVENT_TwoParams(AProjectile, FOnBeginOverlapNotifyEvent, AGameCharacter*, FVector) // GameCharacter so projectiles spawned from things other than guns can hurt the player
//DECLARE_EVENT_OneParam(AProjectile, FOnBeginOverlapNotifyEvent)


UCLASS(Abstract, Blueprintable, BlueprintType)
class FPS_DECKBUILDER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

private:
	UFUNCTION()
	void OnSphereComponentBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
			

public:
	/* Using an event (instead of a delegate) to allow projectile spawning classes have an easier time binding 
	 * their callback methods to the event without the need for extra code to do so */
	FOnBeginOverlapNotifyEvent OnBeginOverlapNotifyEvent;


protected:
	UPROPERTY(EditDefaultsOnly)
	USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly)
	UProjectileMovementComponent* ProjectileMovementComponent;

};
