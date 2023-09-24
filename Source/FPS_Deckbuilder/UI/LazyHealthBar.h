#pragma once

#include "CoreMinimal.h"

#include "Components/ProgressBar.h"

#include "Blueprint/UserWidget.h"
#include "LazyHealthBar.generated.h"




/**
 * 
 */
UCLASS()
class FPS_DECKBUILDER_API ULazyHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& Geometry, float InDeltaTime) override;

	void SetPercent(float Percent);


private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* SlowHealthBar;

	UPROPERTY(EditDefaultsOnly)
	float DecayDelaySeconds; // A temporary value for testing different decay delays

	FTimerHandle TimerHandle; // Used to manage the delay on damage being taken and the lazy health bar beginning to decay

	float TargetPercent;

	/* Linear rate at which the slow par percent decays per a second */
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float DecayPerSecond;
};
