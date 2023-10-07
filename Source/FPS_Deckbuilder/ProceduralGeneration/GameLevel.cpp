

#include "GameLevel.h"



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

	// CreateShape();
	
	// CreateShapeWithDimension();

	/*
	// aisle : z * X * Y
	// X*Y == an entire plane of points
	X = 5; // column
	Y = 5; // Row
	Z = 3; // aisle
	TArray<int> Matrix = {
		// Row
		0, 1, 1, 1, 0,		0, 0, 0, 0, 0,		0, 0, 0, 0, 0,		0, 0, 0, 0, 0,		0, 0, 0, 0, 0, // An aisle
		0, 0, 0, 0, 0,		0, 1, 1, 1, 0,		0, 1, 1, 1, 0,		0, 1, 1, 1, 0,		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,		0, 0, 0, 0, 0,		0, 0, 0, 0, 0,		0, 0, 0, 0, 0,		0, 1, 1, 1, 0
	};
	CreateShapeFromMatrix(Matrix);
	*/

	// Partition();

	/*
	FNode* n1 = new FNode();
	n1->Location = FVector(0, 0, 0); // Where the actor is
	n1->Width = 2.f;
	n1->TypeName = "hallway";
	n1->ID = FString("n1");

	GenerateTree(n1); // Extending the test tree
	GenerateGeometry(n1); // Constructing geometry for the tree

	// Deleting tree
	FNode* Current = n1;
	FNode* Next;
	while (Current->Adjacent.Num())
	{
		Next = Current->Adjacent[0];
		FNode* Temp = Current;
		Current = Next;
		delete Temp;
	} 
	*/


	// -- Making Test Shape -- //
	UShape* Shape = NewObject<UShape>(this);

	FVertex* v1 = new FVertex();
	v1->Location = FVector(0, 0, 0);
	FVertex* v2 = new FVertex();
	v2->Location = FVector(0, 1, 0) * Scale;
	FVertex* v3 = new FVertex();
	v3->Location = FVector(1, 1, 0) * Scale;
	FVertex* v4 = new FVertex();
	v4->Location = FVector(1, 0, 0) * Scale;
	
	FVertex* v5 = new FVertex();
	v5->Location = FVector(0, 0, 1) * Scale;
	FVertex* v6 = new FVertex();
	v6->Location = FVector(0, 1, 1) * Scale;
	FVertex* v7 = new FVertex();
	v7->Location = FVector(1, 1, 1) * Scale;
	FVertex* v8 = new FVertex();
	v8->Location = FVector(1, 0, 1) * Scale;

	v1->Adjacent = { v2, v4, v5 }; 
	v2->Adjacent = { v1, v3, v6 };
	v3->Adjacent = { v2, v4, v7 };
	v4->Adjacent = { v3, v1, v8 };
	
	v5->Adjacent = { v6, v8, v1 };
	v6->Adjacent = { v5, v7, v2 };
	v7->Adjacent = { v6, v8, v3 };
	v8->Adjacent = { v5, v7, v4 };

	// TODO: consider having the adjacency set when faces are constructed
	FFace* f1 = new FFace();  // Front
	f1->Vertices.Add(v1);
	f1->Vertices.Add(v2);
	f1->Vertices.Add(v3);
	f1->Vertices.Add(v4);

	FFace* f2 = new FFace(); // Back
	f2->Vertices.Add(v5);
	f2->Vertices.Add(v6);
	f2->Vertices.Add(v7);
	f2->Vertices.Add(v8);

	FFace* f3 = new FFace(); // Top
	f3->Vertices.Add(v1);
	f3->Vertices.Add(v4);
	f3->Vertices.Add(v8);
	f3->Vertices.Add(v5);

	FFace* f4 = new FFace(); // Bottom
	f4->Vertices.Add(v2);
	f4->Vertices.Add(v3);
	f4->Vertices.Add(v7);
	f4->Vertices.Add(v6);

	FFace* f5 = new FFace(); // Left
	f5->Vertices.Add(v1);
	f5->Vertices.Add(v2);
	f5->Vertices.Add(v6);
	f5->Vertices.Add(v5);

	FFace* f6 = new FFace(); // Right
	f6->Vertices.Add(v4);
	f6->Vertices.Add(v3);
	f6->Vertices.Add(v7);
	f6->Vertices.Add(v8);

	Shape->Faces = { f1, f2, f3, f4, f5, f6 };


	// -- Modifying Shape -- // 
	FFace* OutFace1 = new FFace();
	FFace* OutFace2 = new FFace();

	EFaceAxis Axis = EFaceAxis::X;

	Shape->SubdivideFace(Shape->Faces[2], EFaceAxis::X, 0.5, OutFace1, OutFace2);

	Shape->MoveFace(OutFace1, FVector(0, -1, 0) * Scale * 0.38);


	MakeMeshFromShape(Shape);
}

void AGameLevel::GenerateTree(FNode* Tail)
{
	TMap<FName, TFunctionRef<FNode* (FNode* Node)>> GeneratorMap;
	GeneratorMap.Add(
		TTuple<FName, TFunctionRef<FNode* (FNode* Node)>>(
			FString("hallway"), [&](FNode* Node) -> FNode*
			{
				FNode* Next = new FNode();
				Next->TypeName = "hallway";
				Next->Width = 2.f;

				// Angle of next hallway
				float Theta = ((FMath::Rand() % 4) + 1) * (PI / 2.f);
				float Size = (FMath::Rand() % 7) + 7; // 10 +- 3
				Next->Location = FVector(
					FMath::Cos(Theta) * Size,
					FMath::Sin(Theta) * Size,
					FMath::RandBool() ? -2 : 2
				) + Node->Location;

				Node->Adjacent.Add(Next);
				return Next;
			}
			)
	);
	TFunctionRef<FNode* (FNode* Node)>* Generator;

	int Counter = 0; // Max depth limit
	while (true && Counter < 10)
	{
		Generator = GeneratorMap.Find(Tail->TypeName);
		if (Generator) { Tail = (*Generator)(Tail); }
		else { UE_LOG(LogTemp, Error, TEXT("AGameLevel::GenerateTree -- Tail->TypeName !exist")); return; }

		if (FMath::Rand() % 5 > 3) break; // escape condition;
		++Counter;
	}
}


// Uses node data to generate the level geometry
void AGameLevel::GenerateGeometry(FNode* Node)
{
	// NOTE: Number of vertices should depend on the edge generation function and the subdivion setting (Neither are implemented yet)
	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector> normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> tangents;
	TArray<FLinearColor> vertexColors;

	FNode* Current = Node;
	FNode* Next;
	int Counter = 0; // counts which section we're on
	DrawDebugSphere(GetWorld(), GetActorLocation() + (Current->Location * Scale), 5, 12, FColor::Cyan, true);
	do 
	{
		Next = Current->Adjacent[0];

		// -- Calculating locations of vertices -- //
		FVector DirVector = (Next->Location - Current->Location);

		DrawDebugLine(GetWorld(), GetActorLocation() + Current->Location * Scale, GetActorLocation() + (Current->Location + DirVector)*Scale, FColor::White, true);

		// Orthogonal vector to  DirVector and (0,0,1). Position of upper left vertex
		FVector OrthVector = FVector::CrossProduct(FVector(0, 0, 1), DirVector).GetSafeNormal() * Current->Width;
		
		DrawDebugLine(GetWorld(), GetActorLocation() + Current->Location * Scale, GetActorLocation() + (Current->Location + OrthVector) * Scale, FColor::Orange, true);


		vertices.Add((Current->Location + -OrthVector) * Scale);
		vertices.Add((Current->Location + OrthVector) * Scale); // NOTE: I think this is wrong
		vertices.Add((Next->Location + -OrthVector) * Scale);
		vertices.Add((Next->Location + OrthVector) * Scale);

		DrawDebugSphere(GetWorld(), GetActorLocation() + (Current->Location + -OrthVector) * Scale, 5, 12, FColor::Orange, true);
		DrawDebugSphere(GetWorld(), GetActorLocation() + (Current->Location + OrthVector) * Scale, 5, 12, FColor::Orange, true);
		DrawDebugSphere(GetWorld(), GetActorLocation() + (Next->Location + -OrthVector) * Scale, 5, 12, FColor::Orange, true);
		DrawDebugSphere(GetWorld(), GetActorLocation() + (Next->Location + OrthVector) * Scale, 5, 12, FColor::Orange, true);


		triangles.Add(0 + Counter * 4);
		triangles.Add(1 + Counter * 4);
		triangles.Add(2 + Counter * 4);

		triangles.Add(2 + Counter * 4);
		triangles.Add(1 + Counter * 4);
		triangles.Add(3 + Counter * 4);

		normals.Add(FVector(0, 0, 1));
		normals.Add(FVector(0, 0, 1));
		normals.Add(FVector(0, 0, 1));
		normals.Add(FVector(0, 0, 1));

		// TODO: UV's aren't quite right
		float Ratio = DirVector.Size() / Current->Width;
		UV0.Add(FVector2D(0, 0));
		UV0.Add(FVector2D(Current->Width, 0));
		UV0.Add(FVector2D(0, DirVector.Size() * Ratio));
		UV0.Add(FVector2D(Current->Width, DirVector.Size() * Ratio));

		tangents.Add(FProcMeshTangent(0, 1, 0));
		tangents.Add(FProcMeshTangent(0, 1, 0));
		tangents.Add(FProcMeshTangent(0, 1, 0));
		tangents.Add(FProcMeshTangent(0, 1, 0));

		vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
		vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
		vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
		vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));

		Current = Next;
		DrawDebugSphere(GetWorld(), GetActorLocation() + (Current->Location * Scale), 5, 12, FColor::Cyan, true);
		++Counter;

	} while (Current->Adjacent.Num());

	// NOTE: I'm assuming that by having all parts as one mesh we ave fewer draw calls
	Mesh->CreateMeshSection_LinearColor(0, vertices, triangles, normals, UV0, vertexColors, tangents, true);

	// Enable collision data
	Mesh->ContainsPhysicsTriMeshData(true);
}




void AGameLevel::MakeMeshFromShape(UShape* Shape)
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> Tangents;
	TArray<FLinearColor> VertexColors;

	int Counter = 0;

	// TODO: Use a set to reuse verts that are already in the procedural mesh
	for (FFace* Face : Shape->Faces)
	{
		Vertices.Add(Face->Vertices[0]->Location);
		Vertices.Add(Face->Vertices[1]->Location);
		Vertices.Add(Face->Vertices[2]->Location);
		Vertices.Add(Face->Vertices[3]->Location);


		DrawDebugSphere(GetWorld(), GetActorLocation() + Face->Vertices[0]->Location, 5, 12, FColor::Orange, true);
		DrawDebugSphere(GetWorld(), GetActorLocation() + Face->Vertices[1]->Location, 5, 12, FColor::Orange, true);
		DrawDebugSphere(GetWorld(), GetActorLocation() + Face->Vertices[2]->Location, 5, 12, FColor::Orange, true);
		DrawDebugSphere(GetWorld(), GetActorLocation() + Face->Vertices[3]->Location, 5, 12, FColor::Orange, true);


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

		UV0.Add(FVector2D(0,0));
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

	Mesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, true);

	// Enable collision data
	Mesh->ContainsPhysicsTriMeshData(true);
}



void AGameLevel::CreateShape()
{
	TArray<FVector> vertices;
	vertices.Add(FVector(0, 0, 0)); // Upper left (red)
	vertices.Add(FVector(0, 1 * Scale, 0)); // bottom left (green)
	vertices.Add(FVector(1 * Scale, 0, 0)); // upper right (blue)
 
	vertices.Add(FVector(1 * Scale, 1 * Scale, 0)); // bottom right (yellow)

	DrawDebugSphere(GetWorld(), FVector(0, 0, 0) + GetActorLocation(), 10, 12, FColor::Red, true);
	DrawDebugSphere(GetWorld(), FVector(0, 100, 0) + GetActorLocation(), 10, 12, FColor::Green, true);
	DrawDebugSphere(GetWorld(), FVector(100, 0, 0) + GetActorLocation(), 10, 12, FColor::Blue, true);
	DrawDebugSphere(GetWorld(), FVector(100, 100, 0) + GetActorLocation(), 10, 12, FColor::Yellow, true);


	TArray<int32> Triangles;
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);

	Triangles.Add(2);
	Triangles.Add(1);
	Triangles.Add(3);

	// Normals: Per vertex
	// Direction effects rendering of light on object, not the face which is visible
	TArray<FVector> normals;
	normals.Add(FVector(0, 0, 1));
	normals.Add(FVector(0, 0, 1));
	normals.Add(FVector(0, 0, 1));
	normals.Add(FVector(0, 0, 1));

	// Per vertex calculated per triangle
	TArray<FVector2D> UV0;
	UV0.Add(FVector2D(0, 0));
	UV0.Add(FVector2D(10, 0));
	UV0.Add(FVector2D(0, 10));
	UV0.Add(FVector2D(10, 10));

	// per vertex
	TArray<FProcMeshTangent> tangents;
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));

	// per vertex
	TArray<FLinearColor> vertexColors;
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));


	Mesh->CreateMeshSection_LinearColor(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);

	// Enable collision data
	Mesh->ContainsPhysicsTriMeshData(true);
}

/* This method is pretending to have an array already passed to it with the data on which locations should be created*/
void AGameLevel::CreateShapeWithDimension()
{
	TArray<FVector> vertices;
	vertices.AddZeroed(X * Y);
	
	TArray<int32> triangles;
	TArray<FVector> normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> tangents;
	TArray<FLinearColor> vertexColors;


	// Creating vertices & setting vertex properties
	for (int c = 0; c < X; c++)	// Init the first row of vertices
	{
		vertices[c] = FVector(c * Scale, 0, 0);
		normals.Add(FVector(0, 0, 1));
		UV0.Add(FVector2D(c / (float)X, 0));
		tangents.Add(FProcMeshTangent(0, 1, 0));
		vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	}
	for (int r = 1; r < Y; r++)
	{
		for (int c = 0; c < X; c++)
		{
			vertices[IndexMap(c, r, 0)] = FVector(c * Scale, r * Scale, 0);	
			normals.Add(FVector(0, 0, 1));
			UV0.Add(FVector2D(c/(float)X, r/(float)X)); // use same dimension as denominator to prevent stretching
			tangents.Add(FProcMeshTangent(0, 1, 0));
			vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
		}
	}

	// Creating triangles
	for (int r = 0; r < Y - 1; r++)
	{
		for (int c = 0; c < X - 1; c++)
		{
			triangles.Add(IndexMap(c, r, 0)); // Upper left (0)
			triangles.Add(IndexMap(c, r + 1, 0)); // bottom left (1)
			triangles.Add(IndexMap(c + 1, r, 0)); // top right (2)

			triangles.Add(IndexMap(c + 1, r, 0)); // top right (2)
			triangles.Add(IndexMap(c, r + 1, 0)); // bottom left (1)
			triangles.Add(IndexMap(c + 1, r + 1, 0)); // bottom right (3)
		}
	}

	Mesh->CreateMeshSection_LinearColor(0, vertices, triangles, normals, UV0, vertexColors, tangents, true);
	// Enable collision data
	Mesh->ContainsPhysicsTriMeshData(true);
}

void AGameLevel::CreateShapeFromMatrix(const TArray<int>& Matrix)
{


	TArray<FVector> vertices;
	vertices.AddZeroed(X * Y);

	TArray<int32> triangles;
	TArray<FVector> normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> tangents;
	TArray<FLinearColor> vertexColors;


	for (int a = 0; a < Z; a++)
	{
		for (int r = 0; r < Y; r++)
		{
			for (int c = 0; c < X; c++)
			{
				return;
				if (Matrix[IndexMap(c, r, a)] == 0) { continue; }
			}
		}
	}
}

void AGameLevel::Partition()
{
	TArray<int> Level;
	Level.AddZeroed(X * Y * Z);

	TArray<FColor> PartitionColors = { FColor::Red, FColor::Green, FColor::Blue };
	
	// Used for efficiently checking neighbours in the grid
	TArray<FVector> Directions =
	{
		FVector(1, 0, 0),
		FVector(0, 1, 0),
		FVector(0, 0, 1),
		FVector(-1, 0, 0),
		FVector(0, -1, 0),
		FVector(0, 0, -1),
	};

	// Location of the CA in the grid
	FVector Coordinates = { 0, 0, 0 };

	FVector Next; 
	int Counter = 0;


	while (true)
	{

		// Setting coordinate value
		int RandColorIndex = FMath::RandRange(1, 3);
		Level[IndexMap(Coordinates[0], Coordinates[1], Coordinates[2])] = RandColorIndex;
		DrawDebugBox(GetWorld(), (Coordinates * Scale) + GetActorLocation(), FVector(Scale/2.f), PartitionColors[RandColorIndex - 1], true);


		bool bFoundNeighbor = false;
		
		// Shuffling order of direction
		for (int32 i = Directions.Num() - 1; i > 0; i--) {
			int32 j = FMath::FloorToInt(FMath::SRand() * (i + 1)) % Directions.Num();
			FVector temp = Directions[i];
			Directions[i] = Directions[j];
			Directions[j] = temp;
		}

		// Finding valid neighbor
		for (FVector dir : Directions)
		{
			Next = Coordinates + dir;
			// GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Orange, FString::Printf(TEXT("Next: (%f, %f, %f)"), (int)Next[0], (int)Next[1], (int)Next[2]));

			// Shouldn't leave bounds
			if (Next[0] < 0 || Next[0] >= X) { Counter++; continue; }
			if (Next[1] < 0 || Next[1] >= Y) { Counter++; continue; }
			if (Next[2] < 0 || Next[2] >= Z) { Counter++; continue; }

			// GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Orange, FString::Printf(TEXT("Index: %i"), IndexMap(Next)));
			// UE_LOG(LogTemp, Warning, TEXT("Index: %i"), IndexMap(Next));

			if (Level[IndexMap(Next)] == 0) // If unset neighbor
			{
				// GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Orange, FString::Printf(TEXT("Found: %i"), IndexMap(Next)));
				Coordinates = Next;
				bFoundNeighbor = true;
				break;
			}
		}


		if (bFoundNeighbor == false)
		{
			// failed to find any other unset neighbors.
			// GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Orange, "Failed");
			return;
		}
	}
}