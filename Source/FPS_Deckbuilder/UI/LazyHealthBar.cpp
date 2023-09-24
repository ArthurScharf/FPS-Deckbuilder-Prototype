

#include "LazyHealthBar.h"



void ULazyHealthBar::NativeConstruct()
{
	Super::NativeConstruct();

	HealthBar->SetPercent(1.f);
	SlowHealthBar->SetPercent(1.f);
	TargetPercent = 1.f;
}

void ULazyHealthBar::NativeTick(const FGeometry& Geometry, float InDeltaTime)
{
	Super::NativeTick(Geometry, InDeltaTime);

	if (TargetPercent < SlowHealthBar->Percent) { SlowHealthBar->SetPercent(SlowHealthBar->Percent - (DecayPerSecond * InDeltaTime)); }
}


void ULazyHealthBar::SetPercent(float Percent)
{
	float ClampedPercent = FMath::Clamp(Percent, 0.f, 1.f);

	if (ClampedPercent < HealthBar->Percent)
	{ // Damage dealt
		HealthBar->SetPercent(Percent);
		GetWorld()->GetTimerManager().SetTimer( TimerHandle, [&](){ TargetPercent = HealthBar->Percent; }, DecayDelaySeconds, false ); // Starting delayed timer before beginning decay
	}
	else
	{ //  Healing
		HealthBar->SetPercent(ClampedPercent);
		if (ClampedPercent > TargetPercent) { TargetPercent = ClampedPercent; }
		if (ClampedPercent > SlowHealthBar->Percent) { SlowHealthBar->SetPercent(ClampedPercent); }
	}
}