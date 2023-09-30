
#include "StatusEffect.h"

#include "FPS_Deckbuilder/Character/GameCharacter.h"











void UStatusEffect::Init_Implementation(AGameCharacter* _GameCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("UStatusEffect::Init"));

	GameCharacter = _GameCharacter;

	// TODO: Add icon to game character widget

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




void UStatusEffect::Cleanup_Native()
{
	UE_LOG(LogTemp, Warning, TEXT("UStatusEffect::Cleanup_Native"));

	// TODO: Remove icon from game character widget

	/* An `Outer` must be passed in BP's when an instance of this class is created.
	*  I'm completely confident that this won't prevent garbage collection, I'm explicitly flagging for collection here
	*/
	GameCharacter->GetWorld()->ForceGarbageCollection(true);
	GameCharacter = nullptr;
}