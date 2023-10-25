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
* 
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

	/*
	* Constructs the mesh from UShape data. 
	* 
	* NOTE: A shape will not be turned into a mesh if it hasn't been mapped to a material
	*/
	void MakeMesh();
	
	void MakeMesh(UGeomNode* Node);

	

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

	/* Map of shapes currently comprising the level. 
	* Key: Label for the shapes stored there
	* 
	*/
	TMultiMap<FString, UShape*> Shapes;

	UPROPERTY(EditAnywhere)
	int NumMutations;

	// Amount in each dimension we offset FGeomNodes to allow them to begin within the matrix generated during `MakeMesh`
	FVector Dimensions;


	/*
	* NOTE: It's weird that the faces UV's are calculated during mutation but their materials, which
	* are strongly coupled with the UV calculation process aren't set in the same place.
	* Makes it confusing to work on in the future
	* 
	* Shapes are generated with labels and mapped into a bucket.
	* All shapes in the same bucket must have same material.
	* This map is how we map labeled shapes to that material.
	* 
	* Different shape labels must have their UV's set differently.
	* All UV's are set as squares and on a ratio of Engine units. 
	* 
	* SideLength / TextureEdgeLength = UV coordinate
	* 
	* This TMap is for mapping labeled shapes to their appropriate Material and UV coordinate rate
	* 
	* TTuple<float, _> where the float is side length of the texture in units in the world
	* 
	* 
	* Step 2: Modify MakeMesh method to construct mesh sections properly and assign materials based the below map
	*/
	UPROPERTY(EditInstanceOnly) 
	TMap<FString, UMaterial*> SectionMaterialMap;




	// -- DEPRECATED -- //
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UGraphGrammar> GraphGrammarClass;
	UGraphGrammar* GraphGrammar;

	UPROPERTY(EditAnywhere)
		float Scale;

	/* Level Dimensions */ // DEPRECATED 
	UPROPERTY(EditDefaultsOnly)
		int X;
	UPROPERTY(EditDefaultsOnly)
		int Y;
	UPROPERTY(EditDefaultsOnly)
		int Z;

	TArray<FFace*> FloorFaces;

	uint32 LargestNodeWidth;
	// ------------------ //


public:
	FORCEINLINE float GetScale() { return Scale; }
};

