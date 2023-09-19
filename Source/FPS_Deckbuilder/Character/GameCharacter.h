#pragma once

#include "CoreMinimal.h"

#include "FPS_Deckbuilder/CommonHeaders/DamagePackage.h"

#include "Engine/EngineTypes.h"
#include "GameFramework/Character.h"
#include "GameCharacter.generated.h"



/* An abstract class used by all characters in the game */
UCLASS()
class FPS_DECKBUILDER_API AGameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGameCharacter();

	virtual void ReceiveDamage(FDamageStruct& DamageStruct);

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	
protected:
	UPROPERTY(VisibleAnywhere)
	float Health;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;
};
