#pragma once

#include "CoreMinimal.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"
#include "ImpactPackage.generated.h"


USTRUCT(BlueprintType, Blueprintable)
struct FImpactPackage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* ImpactSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* ImpactCue;

public:
	bool IsValid() { return ImpactSystem && ImpactCue; }
};
