#pragma once

#include "CoreMinimal.h"

#include "FPS_Deckbuilder/Character/GameCharacter.h"

#include "GameFramework/Actor.h"
#include "Projectile.generated.h"



class AActor;
class UProjectileMovementComponent;
class USphereComponent;
struct FDamageStruct;


// DECLARE_DELEGATE_OneParam(FOnBeginOverlapNotifyDelegate, FDamageStruct)
DECLARE_EVENT_OneParam(AProjectile, FOnBeginOverlapNotifyEvent, AGameCharacter*)

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
	

	/*
	* UPrimitiveComponent* OtherComp, 
		AActor* OtherActor,  
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResul
	*/
	// UPrimitiveComponent OnComponentBeginOverlap, 
		

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
