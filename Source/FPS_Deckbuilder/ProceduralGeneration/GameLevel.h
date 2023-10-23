#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameLevel.generated.h"


class UProceduralMeshComponent;
class USceneComponent;
class UShape;
class UGrammar;
class UGraphGrammar;
class UGeomNode;

struct FFace;



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
	
	void MakeMesh(UGeomNode* Node);

	/* Set's the face values found in FloorFaces per the width of `Start` and the position of both `Start` and `End`*/
	void Raster(UGeomNode* Start, UGeomNode* End);

	

private:
	// DEPRECATED converts the grid coordinates of a vertex to it's index in the vertex array
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
	void DrawGraph(UGeomNode* Head);


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


	TArray<FFace*> FloorFaces;


	UPROPERTY(EditAnywhere)
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

	// Amount in each dimension we offset FGeomNodes to allow them to begin within the matrix generated during `MakeMesh`
	FVector Dimensions;
	uint32 LargestNodeWidth;

public:
	FORCEINLINE float GetScale() { return Scale; }
};

