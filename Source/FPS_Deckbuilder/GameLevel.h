#pragma once

#include "CoreMinimal.h"

#include "ProceduralMeshComponent.h"
#include "Components/SceneComponent.h"

#include "GameFramework/Actor.h"
#include "GameLevel.generated.h"


// class UProceduralMeshComponent;


UCLASS()
class FPS_DECKBUILDER_API AGameLevel : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameLevel();

protected:
	virtual void BeginPlay() override;

	void CreateShape();

	void CreateShapeWithDimension();

	void CreateShapeFromMatrix(const TArray<int>& Matrix);

	void Partition();



private:
	// converts the grid coordinates of a vertex to it's index in the vertex array
	FORCEINLINE int IndexMap(int column, int row, int aisle)
	{
		return column + (row * X) + (aisle * X * Y);
	}

	FORCEINLINE int IndexMap(FVector Coordinates)
	{
		return IndexMap((int)Coordinates[0], (int)Coordinates[1], (int)Coordinates[2]);
	}


protected:
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere)
	UProceduralMeshComponent* Mesh;



	UPROPERTY(EditDefaultsOnly)
	float Scale;

	UPROPERTY(EditDefaultsOnly)
	int X;
	UPROPERTY(EditDefaultsOnly)
	int Y;
	UPROPERTY(EditDefaultsOnly)
	int Z;


	// Size of a single level 1 cell in units
	UPROPERTY(EditDefaultsOnly)
	float LevelScale;


	// Integers. Dimensions of the level measured in level 1 cells
	UPROPERTY(EditDefaultsOnly)
	FVector LevelDimensions;
};

