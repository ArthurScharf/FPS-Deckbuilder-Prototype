
#include "StatusEffect.h"
#include "Kismet/KismetSystemLibrary.h"
#include "FPS_Deckbuilder/Character/GameCharacter.h"

#include "DrawDebugHelpers.h"

void UStatusEffect::Init_Implementation(AGameCharacter* _GameCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("UStatusEffect::Init / %s"), *_GameCharacter->GetName());

	GameCharacter = _GameCharacter;
	World = GameCharacter->GetWorld();
	InitialNumTriggers = NumTriggers;

	// TODO: Add icon to game character widget
	
	SetLifetimeTimer();
}



void UStatusEffect::SetLifetimeTimer()
{

	// Listening mode
	if (NumTriggers == 0)
	{
		// Setting lifetime timer
		GameCharacter->GetWorldTimerManager().SetTimer(
			EffectHandle,
			[&]() { Cleanup(); },
			Seconds,
			false
		);
		return;
	}

	// Self-triggering mode
	GameCharacter->GetWorldTimerManager().SetTimer(
		EffectHandle,
		[&]() { Trigger(GameCharacter); },
		Seconds,
		true,
		FirstDelaySeconds
	);
}



// WARNING: Only call this within the BP implementation of `Cleanup`
void UStatusEffect::Cleanup_Native()
{
	UE_LOG(LogTemp, Warning, TEXT("UStatusEffect::Cleanup_Native"));

	// TODO: Remove icon from game character widget


	GameCharacter->GetWorldTimerManager().ClearTimer(EffectHandle); // I'm not sure if flagging for garbage collection will gaurentee the avoidance of a race condition. 
	GameCharacter->RemoveStatusEffect(this);

	/* An `Outer` must be passed in BP's when an instance of this class is created.
	*  I'm completely confident that this won't prevent garbage collection, I'd like to explicitly flag for collection here
	*/
	GEngine->ForceGarbageCollection();
	// GameCharacter->GetWorld()->ForceGarbageCollection(true);
	GameCharacter = nullptr;
}



void UStatusEffect::DecrementNumTriggers()
{
	--NumTriggers;
	if (NumTriggers == 0) { Cleanup(); }
}



void UStatusEffect::IncrementNumInstances()
{
	++NumInstances;
	UE_LOG(LogTemp, Warning, TEXT("UStatusEffect::IncrementNumInstances -- %d"), NumInstances);
	if (bResetLifetimeOnStack) 
	{ 
		NumTriggers = InitialNumTriggers;
		SetLifetimeTimer(); 
	}
}


void UStatusEffect::Trigger_OnDamageDealt_Implementation(FDamageStruct& DamageStruct)
{

}



bool UStatusEffect::MultiSphereTraceForObjects(FVector Start, FVector End, float Radius, TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes, bool bIgnoreSelf, TArray<FHitResult>& OutHits)
{
	TArray<AActor*> ActorsToIgnore = { GameCharacter };
	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		World,
		Start,
		End,
		Radius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::Persistent,
		OutHits,
		true
	);


	for (FHitResult Hit : OutHits)
	{
		DrawDebugPoint(
			World,
			Hit.ImpactPoint,
			5.f,
			FColor::Blue,
			true
		);
	}

	return bHit;
}


bool UStatusEffect::SphereOverlapActors(const FVector Location, float Radius, const TArray< TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, UClass* ActorClassFilter, const TArray<AActor*>& ActorsToIgnore, bool bIgnoreSelf, TArray<AActor*>& OutActors)
{
	//if (bIgnoreSelf)
	//{
	//	ActorsToIgnore.Add(GameCharacter);
	//}

	return UKismetSystemLibrary::SphereOverlapActors(
		World,
		Location,
		Radius,
		ObjectTypes,
		ActorClassFilter,
		ActorsToIgnore,
		OutActors
	);
}