
#pragma once

#include "CoreMinimal.h"
#include "FPS_Deckbuilder/Character/GameCharacter.h"
#include "EnemyCharacter.generated.h"


class UWidgetComponent;
class UBehaviorTree;


UCLASS()
class FPS_DECKBUILDER_API AEnemyCharacter : public AGameCharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	virtual void ReceiveDamage(FDamageStruct& DamageStruct) override;

	virtual void NotifyOfDamageDealt(FDamageStruct& DamageStruct) override;

protected:
	virtual void BeginPlay() override;

private:
	virtual void Die();


private:
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* BehaviorTree;
};
