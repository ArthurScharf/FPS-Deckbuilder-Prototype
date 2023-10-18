#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameLevel.generated.h"


class UProceduralMeshComponent;
class USceneComponent;
class UShape;
class UGrammar;
class UGraphGrammar;
struct FGeomNode;



/* High level control object. Uses a passed grammar to construct a series of shapes that make up the level geometry 
*/
UCLASS()
class FPS_DECKBUILDER_API AGameLevel : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameLevel();

	/* Declaring a function ptr belonging to this class */
	// Left here for learning purposes
	// typedef void (AGameLevel::* GenerateFunction) (FNode Node);

protected:
	virtual void BeginPlay() override;

	// TODO: Deprecated 
	void MakeMesh();
	
	void MakeMesh(FGeomNode* Node);


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

	// -- Debugging Methods -- //
	void PrintShapes();
	void DrawGraph(FGeomNode* Head);


protected:
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere) 
	UProceduralMeshComponent* Mesh;

	/* The class of generative grammar that will be used to generate the level geometry */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGrammar> GrammarClass;

	UGrammar* Grammar;

	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGraphGrammar> GraphGrammarClass;

	UGraphGrammar* GraphGrammar;



	UPROPERTY(EditDefaultsOnly)
	float Scale;

	/* Level Dimensions */ // DEPRECATED 
	UPROPERTY(EditDefaultsOnly)
	int X;
	UPROPERTY(EditDefaultsOnly)
	int Y;
	UPROPERTY(EditDefaultsOnly)
	int Z;

	/* Map of shapes currently comprising the level. 
	* Key: Label for the shapes stored there
	* 
	*/
	TMultiMap<FString, UShape*> Shapes;

	UPROPERTY(EditAnywhere)
	int NumMutations;

public:
	FORCEINLINE float GetScale() { return Scale; }
};

