
#pragma once

#include "CoreMinimal.h"
#include "FPS_Deckbuilder/Character/GameCharacter.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class FPS_DECKBUILDER_API AEnemyCharacter : public AGameCharacter
{
	GENERATED_BODY()

public:

	AEnemyCharacter();

protected:

	virtual void BeginPlay() override;
};
