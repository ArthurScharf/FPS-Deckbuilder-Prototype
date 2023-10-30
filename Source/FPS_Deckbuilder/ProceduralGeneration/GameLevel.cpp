

#include "GameLevel.h"

#include "Components/SceneComponent.h"
#include "GeomNode.h"
#include "ProceduralMeshComponent.h"
#include "Shape.h"
#include "Grammar.h"
#include "GraphGrammar.h"

#include "NavigationSystem.h"


#include "DrawDebugHelpers.h"



/* -- PROBLEM --
*  How do I effectively subdivide rooms based on some sort of grammar?
* 1. Tile approach lets me have loops.
*/


AGameLevel::AGameLevel()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComponent);

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Mesh->bUseAsyncCooking = true;
}

void AGameLevel::BeginPlay()
{
	Super::BeginPlay();

	// == Shape Mutation Approach == // 
	if (!GrammarClass)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameLevel::BeginPlay -- !Grammar"));
		return;
	}
	Scale = 100.f;

	Grammar = NewObject<UGrammar>(this, GrammarClass);
	Grammar->GameLevel = this;
	Grammar->Init(Shapes);

	for (int i = 0; i < NumMutations; i++)
	{
		Grammar->Mutate(Shapes);
	}





	// -- Getting the ShapeTextureMultiMap for use in constructing

	//TArray<UShape*> ShapeValues;
	//Shapes.GenerateValueArray(ShapeValues);
	//for (UShape* _Shape : ShapeValues)
	//{
	//	// -- TEMP: Debugging -- // 
	//	for (FFace* Face : _Shape->Faces)
	//	{
	//		Face->DrawLabel(this, true);
	//	}
	//}

	MakeMesh();
	AddActorWorldOffset(FVector(0, 0, -1), true); // This is a hack solution for forcing the nav mesh to be generated on the surface of the procedural mesh at runtime














	return;
	// ==  Graph Mutation Approach ==  //
	// -- Initializing Graph Grammar -- // 
	if (!GraphGrammarClass)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameLevel::BeginPlay -- !GraphGrammarClass"));
		return;
	}
	GraphGrammar = NewObject<UGraphGrammar>(this, GraphGrammarClass);
	GraphGrammar->Init();

	// -- Mutating Graph -- //
	for (int i = 0; i < NumMutations; i++)
	{
		GraphGrammar->Mutate();
	}

	// -- Constructing Geometry -- //
	UGeomNode* Head = GraphGrammar->GetGraph();
	MakeMesh(Head);

	//UE_LOG(LogTemp, Warning, TEXT("BeginPlay -- Node[ID: %d |  Neighbours: %d]"), Head->GetID(), Head->Adjacent.Num());
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay -- Node%s"), *Head->ToString());


	// -- Debugging: Drawing Graph -- //
	DrawGraph(Head);

}






void AGameLevel::MakeMesh(UGeomNode* Node)
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> Tangents;
	TArray<FLinearColor> VertexColors;

	// -- Constructing 3-Dimensional Grid -- //
	/* [0]: Rows
	*  [1]: Columns
	*  [2]: Aisles
	*  Finding the dimensions for the bounding box of the graph nodes. NOT the geometry
	*/
	Dimensions = GraphGrammar->GetGreatestBounds() - GraphGrammar->GetLeastBounds();
	LargestNodeWidth = GraphGrammar->GetLargestNodeWidth();
	Dimensions.X = FMath::Floor(FMath::Abs(Dimensions.X));
	Dimensions.Y = FMath::Floor(FMath::Abs(Dimensions.Y));
	Dimensions.Z = FMath::Floor(FMath::Abs(Dimensions.Z));

	// 2*LargestNodeWidth because there must be room for largest width in any direction
	Dimensions.X = 2 * LargestNodeWidth + (Dimensions.X == 0) ? 1 : Dimensions.X;
	Dimensions.Y = 2 * LargestNodeWidth + (Dimensions.Y == 0) ? 1 : Dimensions.Y;
	Dimensions.Z = (Dimensions.Z == 0) ? 1 : Dimensions.Z; // Not used in the Z because node width only deals with X,Y coordinates

	UE_LOG(LogTemp, Warning, TEXT("AGameLevel::MakeMesh -- Dimensions: %s"), *Dimensions.ToCompactString());

	// Initialize FloorFaces to null as a way to detect which positions haven't been created yet 
	FloorFaces.AddZeroed((int)(Dimensions.X * Dimensions.Y * Dimensions.Z));

	UE_LOG(LogTemp, Warning, TEXT("AGameLevel::MakeMesh -- FloorFaces.Num(): %d"), FloorFaces.Num());


	// -- Setting the values in FloorFaces -- //
	TSet<UGeomNode*> Visited; // Nodes already explored
	TFunction<void(UGeomNode*)> Explore = [&](UGeomNode* Node)
	{
		Visited.Add(Node);
		for (UGeomNode* Adjacent : Node->Adjacent)
		{
			if (!Visited.Contains(Adjacent))
			{
				// Raster(Node, Adjacent);
				Explore(Adjacent);
			}
		}
	};





	// -- Building the Geometry -- //
	// TODO
}


void AGameLevel::MakeMesh()
{
	int TriangleVertexIndex = 0;
	int MeshSectionIndex = 0;

	TMap<FString, UShape*> MaterialShapeMap = Grammar->GetMaterialShapeMap();

	TArray<FString> Labels; 
	MaterialShapeMap.GetKeys(Labels);
	for (FString Label : Labels) { MakeMeshSection(Label); } // Each label corresponds to one and only one material to be applied
	
	// Apparently I need this for collision to work, though collision works without it
	Mesh->ContainsPhysicsTriMeshData(true);

}


void AGameLevel::MakeMeshSection(FString Label)
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> Tangents;
	TArray<FLinearColor> VertexColors;

	int TriangleVertexIndex = 0;

	TMap<FString, UShape*> MaterialShapeMap = Grammar->GetMaterialShapeMap();
	UShape* Shape = MaterialShapeMap[Label];

	for (FFace* Face : Shape->Faces)
	{
		Vertices.Add(Scale * Face->Vertices[0]->Location);
		Vertices.Add(Scale * Face->Vertices[1]->Location);
		Vertices.Add(Scale * Face->Vertices[2]->Location);
		Vertices.Add(Scale * Face->Vertices[3]->Location);

		// NOTE: These are VERY expensive for the framerate
		DrawDebugSphere(GetWorld(), GetActorLocation() + Face->Vertices[0]->Location * Scale, 500, 12, FColor::Orange, true);
		DrawDebugSphere(GetWorld(), GetActorLocation() + Face->Vertices[1]->Location * Scale, 500, 12, FColor::Orange, true);
		DrawDebugSphere(GetWorld(), GetActorLocation() + Face->Vertices[2]->Location * Scale, 500, 12, FColor::Orange, true);
		DrawDebugSphere(GetWorld(), GetActorLocation() + Face->Vertices[3]->Location * Scale, 500, 12, FColor::Orange, true);

		Triangles.Add(0 + TriangleVertexIndex * 4); // Upper Left
		Triangles.Add(1 + TriangleVertexIndex * 4); // Bottom Left
		Triangles.Add(3 + TriangleVertexIndex * 4); // Upper Right

		Triangles.Add(3 + TriangleVertexIndex * 4); // Upper Right
		Triangles.Add(1 + TriangleVertexIndex * 4); // Bottom Left
		Triangles.Add(2 + TriangleVertexIndex * 4); // Bottom Right

		Normals.Add(Face->Normal);
		Normals.Add(Face->Normal);
		Normals.Add(Face->Normal);
		Normals.Add(Face->Normal);

		// -- Calculating UV's -- //
		// TODO: Calculate UV values during Grammar::Mutate() instead of here
		UV0.Add(FVector2D(0, 0));
		UV0.Add(FVector2D(10, 0));
		UV0.Add(FVector2D(10, 10));
		UV0.Add(FVector2D(0, 10));

		// TODO
		Tangents.Add(FProcMeshTangent());
		Tangents.Add(FProcMeshTangent());
		Tangents.Add(FProcMeshTangent());
		Tangents.Add(FProcMeshTangent());

		// TODO: Remove?
		VertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
		VertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
		VertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
		VertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));

		TriangleVertexIndex++;
	}

	Mesh->CreateMeshSection_LinearColor(Mesh->GetNumSections(), Vertices, Triangles, Normals, UV0, VertexColors, Tangents, true);

	// -- Applying material to mesh section -- //
	if (SectionMaterialMap.Contains(Label)) { Mesh->SetMaterial(Mesh->GetNumSections()-1, SectionMaterialMap[Label]); }
}





/* Debugging function */
void AGameLevel::PrintShapes()
{
	TArray<FString> Keys;
	Shapes.GenerateKeyArray(Keys);
	
	UE_LOG(LogTemp, Warning, TEXT("AGameLevel::PrintShapes -- BEGIN"));

	for (FString Key : Keys)
	{
		UE_LOG(LogTemp, Warning, TEXT("AGameLevel::PrintShapes -- %s"), *Key);
	}
}




void AGameLevel::DrawGraph(UGeomNode* Head)
{
	

	// int Count = 0;

	UE_LOG(LogTemp, Warning, TEXT("AGameLevel::DrawGraph -- Node[ID: %d |  Neighbours: %d]"), Head->GetID(), Head->Adjacent.Num());

	TSet<UGeomNode*> Visited; // Nodes already drawn
	// Helper function for drawing graph
	TFunction<void(UGeomNode*)> Explore = [&](UGeomNode* Node)
	{
		UE_LOG(LogTemp, Warning, TEXT("AGameLevel::DrawGraph::Explore -- Node[ID: %d |  Neighbours: %d]"), Node->GetID(), Node->Adjacent.Num());
		Visited.Add(Node);
		
		DrawDebugSphere(GetWorld(), (Node->Location * Scale) + GetActorLocation() , 20, 12, FColor::Orange, true);
		DrawDebugString(GetWorld(),
			(Node->Location * Scale) + GetActorLocation(),
			FString::Printf(TEXT("%s [ID: %d]"), *Node->Label, Node->GetID()),
			(AActor*)0,
			FColor::White,
			1000
		);

		// Count++;
		// if (Count > 20) { UE_LOG(LogTemp, Error, TEXT("AGameLevel::DrawGraph::Explore -- Exceeded Limit")); return; }
		
		for (UGeomNode* Adjacent : Node->Adjacent)
		{
			UE_LOG(LogTemp, Warning, TEXT("AGameLevel::DrawGraph::Explore -- Adjacent_ID: %d"), Adjacent->GetID());

			if (!Visited.Contains(Adjacent))
			{
				DrawDebugLine(
					GetWorld(), 
					(Node->Location * Scale) + GetActorLocation() ,
					(Adjacent->Location * Scale) + GetActorLocation(),
					FColor::Black, 
					true
				);
				Explore(Adjacent);
			}
		}
	};


	Explore(Head);
}