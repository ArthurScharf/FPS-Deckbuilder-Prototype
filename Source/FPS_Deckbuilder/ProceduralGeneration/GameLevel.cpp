

#include "GameLevel.h"

#include "Components/SceneComponent.h"
#include "GeomNode.h"
#include "ProceduralMeshComponent.h"
#include "Shape.h"
#include "Grammar.h"
#include "GraphGrammar.h"

#include "DrawDebugHelpers.h"






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
	GraphGrammar->Mutate();

	// -- Constructing Geometry -- //
	FGeomNode* Head = GraphGrammar->GetGraph();
	MakeMesh(Head);

	// -- Debugging: Drawing Graph -- //
	DrawGraph(Head);


	return;
	// == Shape Mutation Approach == // 

	if (!GrammarClass)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameLevel::BeginPlay -- !Grammar"));
		return;
	}
	// -- Making Test Shape -- //
	UShape* Shape = NewObject<UShape>(this);
	// UShape::InitCylinder(Shape, 256, 2);
	Shape->Label = "start";
	Shapes.Add(Shape->Label, Shape);

	Grammar = NewObject<UGrammar>(this, GrammarClass);
	Grammar->Init(Shapes);
	Grammar->GameLevel = this;
	

	for (int i = 0; i < NumMutations; i++)
	{
		Grammar->Mutate(Shapes);
	}


	
	TArray<UShape*> ShapeValues;
	Shapes.GenerateValueArray(ShapeValues);
	for (UShape* _Shape : ShapeValues)
	{

		// -- TEMP: Debugging -- // 
		for (FFace* Face : _Shape->Faces)
		{
			Face->DrawLabel(this, false);
		}
	}

	MakeMesh();
}






void AGameLevel::MakeMesh(FGeomNode* Node)
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
	FVector Dimensions = GraphGrammar->GetGreatestBounds() - GraphGrammar->GetLeastBounds();
	Dimensions.X = FMath::Abs(Dimensions.X);
	Dimensions.Y = FMath::Abs(Dimensions.Y);
	Dimensions.Z = FMath::Abs(Dimensions.Z);
}


void AGameLevel::MakeMesh()
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> Tangents;
	TArray<FLinearColor> VertexColors;

	int Counter = 0;

	TArray<UShape*> ShapeValues;
	Shapes.GenerateValueArray(ShapeValues);
	
	for (UShape* Shape : ShapeValues)
	{
		for (FFace* Face : Shape->Faces)
		{
			Vertices.Add(Face->Vertices[0]->Location);
			Vertices.Add(Face->Vertices[1]->Location);
			Vertices.Add(Face->Vertices[2]->Location);
			Vertices.Add(Face->Vertices[3]->Location);

			// NOTE: These are VERY expensive for the framerate
			//DrawDebugSphere(GetWorld(), GetActorLocation() + Face->Vertices[0]->Location, 500, 12, FColor::Orange, true);
			//DrawDebugSphere(GetWorld(), GetActorLocation() + Face->Vertices[1]->Location, 500, 12, FColor::Orange, true);
			//DrawDebugSphere(GetWorld(), GetActorLocation() + Face->Vertices[2]->Location, 500, 12, FColor::Orange, true);
			//DrawDebugSphere(GetWorld(), GetActorLocation() + Face->Vertices[3]->Location, 500, 12, FColor::Orange, true);


			Triangles.Add(0 + Counter * 4); // Upper Left
			Triangles.Add(1 + Counter * 4); // Bottom Left
			Triangles.Add(3 + Counter * 4); // Upper Right

			Triangles.Add(3 + Counter * 4); // Upper Right
			Triangles.Add(1 + Counter * 4); // Bottom Left
			Triangles.Add(2 + Counter * 4); // Bottom Right

			// TODO: Face->Normal needs to be set
			Normals.Add(Face->Normal);
			Normals.Add(Face->Normal);
			Normals.Add(Face->Normal);
			Normals.Add(Face->Normal);

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

			++Counter;
		}
	}
	
	/* NOTE:  this is an issue that comes from having no meta object representing the mesh as whole */
	Mesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, true);
	// Enable collision data
	// Mesh->ContainsPhysicsTriMeshData(true);
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




void AGameLevel::DrawGraph(FGeomNode* Head)
{
	TSet<FGeomNode*> Visited; // Nodes already drawn

	int Count = 0;

	// Helper function for drawing graph
	TFunction<void(FGeomNode* Node)> Explore = [&](FGeomNode* Node)
	{
		UE_LOG(LogTemp, Warning, TEXT("AGameLevel::DrawGraph::Explore -- Node_ID: %d"), Node->GetID());
		Visited.Add(Node);
		
		DrawDebugSphere(GetWorld(), Node->Location + GetActorLocation(), 20, 12, FColor::Orange, true);

		Count++;
		if (Count > 10) { UE_LOG(LogTemp, Error, TEXT("AGameLevel::DrawGraph::Explore -- Exceeded Limit")); return; }
		
		for (FGeomNode* Adjacent : Node->Adjacent)
		{
			if (!Visited.Contains(Adjacent))
			{
				DrawDebugLine(GetWorld(), Node->Location + GetActorLocation(), Adjacent->Location + GetActorLocation(), FColor::Black, true);
				Explore(Adjacent);
			}
		}
	};


	Explore(Head);
}


