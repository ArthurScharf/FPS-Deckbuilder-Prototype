#include "Shape.h"



#include "DrawDebugHelpers.h"



// --  Static Functions -- //
void UShape::InitCube(UShape* Shape)
{
	FVertex* v1 = new FVertex();
	v1->Location = FVector(0, 0, 0);
	FVertex* v2 = new FVertex();
	v2->Location = FVector(0, 1, 0);
	FVertex* v3 = new FVertex();
	v3->Location = FVector(1, 1, 0);
	FVertex* v4 = new FVertex();
	v4->Location = FVector(1, 0, 0);

	FVertex* v5 = new FVertex();
	v5->Location = FVector(0, 0, 1);
	FVertex* v6 = new FVertex();
	v6->Location = FVector(0, 1, 1);
	FVertex* v7 = new FVertex();
	v7->Location = FVector(1, 1, 1);
	FVertex* v8 = new FVertex();
	v8->Location = FVector(1, 0, 1);

	// NOTE: SetAdjacency was written after this. No sense making more work for myself
	v1->Adjacent = { v2, v4, v5 };
	v2->Adjacent = { v1, v3, v6 };
	v3->Adjacent = { v2, v4, v7 };
	v4->Adjacent = { v3, v1, v8 };

	v5->Adjacent = { v6, v8, v1 };
	v6->Adjacent = { v5, v7, v2 };
	v7->Adjacent = { v6, v8, v3 };
	v8->Adjacent = { v5, v7, v4 };

	// TODO: consider having the adjacency set when faces are constructed
	FFace* f1 = new FFace();  // bottom (-Z)
	f1->Normal = FVector(0, 0, -1);
	f1->Vertices.Add(v4);
	f1->Vertices.Add(v3);
	f1->Vertices.Add(v2);
	f1->Vertices.Add(v1);

	FFace* f2 = new FFace(); // top (+Z)
	f2->Normal = FVector(0, 0, 1);
	f2->Vertices.Add(v5);
	f2->Vertices.Add(v6);
	f2->Vertices.Add(v7);
	f2->Vertices.Add(v8);

	FFace* f3 = new FFace(); // Back (-Y)
	f3->Normal = FVector(0, -1, 0);
	f3->Vertices.Add(v5);
	f3->Vertices.Add(v8);
	f3->Vertices.Add(v4);
	f3->Vertices.Add(v1);

	FFace* f4 = new FFace(); // Front (+Y)
	f4->Normal = FVector(0, 1, 0);
	f4->Vertices.Add(v2);
	f4->Vertices.Add(v3);
	f4->Vertices.Add(v7);
	f4->Vertices.Add(v6);

	FFace* f5 = new FFace(); // Left (-X)
	f5->Normal = FVector(-1, 0, 0);
	f5->Vertices.Add(v1);
	f5->Vertices.Add(v2);
	f5->Vertices.Add(v6);
	f5->Vertices.Add(v5);

	FFace* f6 = new FFace(); // Right (+X)
	f6->Normal = FVector(1, 0, 0);
	f6->Vertices.Add(v8);
	f6->Vertices.Add(v7);
	f6->Vertices.Add(v3);
	f6->Vertices.Add(v4);

	Shape->Faces = { f1, f2, f3, f4, f5, f6 };
}



// -- Member Functions  -- // 
void UShape::SetScale(float Scale)
{
	TSet<FVertex> ScaledVerts;
	for (FFace* Face : Faces)
	{
		for (FVertex* Vertex : Face->Vertices)
		{
			bool bAlreadyInSet;
			ScaledVerts.Add(*Vertex, &bAlreadyInSet);
			if (!bAlreadyInSet) { Vertex->Location *= Scale; }
		}
	}

}

void UShape::MoveFace(FFace& Face, const FVector& DeltaLocation)
{
	for (FVertex* Vertex : Face.Vertices)
	{
		Vertex->Location += DeltaLocation;
	}
}


void UShape::SubdivideFace(FFace& Face, EFaceAxis Axis, float Percent, FFace* OutFace1, FFace* OutFace2)
{
	// TODO: Make the order of the vertices not matter for this function

	// edges{0,1} and edges{2,3} are subdivided
	if (Axis == EFaceAxis::X)
	{
		// Making new faces
		FVertex* v1 = new FVertex();
		FVertex* v2 = new FVertex();

		// Constructing 2 new vertices necessary for the subdivision
		v1->Location = Face.Vertices[0]->Location + (Face.Vertices[3]->Location - Face.Vertices[0]->Location) * Percent;
		v1->Adjacent.Add(Face.Vertices[0]);
		v1->Adjacent.Add(Face.Vertices[3]);
		v1->Adjacent.Add(v2);

		v2->Location = Face.Vertices[1]->Location + (Face.Vertices[2]->Location - Face.Vertices[1]->Location) * Percent;
		v2->Adjacent.Add(Face.Vertices[1]);
		v2->Adjacent.Add(Face.Vertices[2]);
		v2->Adjacent.Add(v1);

		// Setting values for faces 
		OutFace1->Normal = Face.Normal;
		OutFace2->Normal = Face.Normal;

		OutFace1->Vertices.Add(Face.Vertices[0]);
		OutFace1->Vertices.Add(Face.Vertices[1]);
		OutFace1->Vertices.Add(v2);
		OutFace1->Vertices.Add(v1);
		
		OutFace2->Vertices.Add(v1);
		OutFace2->Vertices.Add(v2);
		OutFace2->Vertices.Add(Face.Vertices[2]);
		OutFace2->Vertices.Add(Face.Vertices[3]);

		// Removing old face and adding new face
		int test = Faces.Remove(&Face);
		delete &Face;
		Faces.Add(OutFace1);
		Faces.Add(OutFace2);
	}
	else
	{
		// TODO
	}
}


void UShape::ExtrudeFace(FFace& Face, float Distance, FFace* OutFace)
{
	// -- Creating Extruded Face -- // 
	FVertex* NewVertex;
	for (FVertex* Vertex : Face.Vertices)
	{
		NewVertex = new FVertex();
		NewVertex->Location = Vertex->Location + Face.Normal * Distance;
		NewVertex->Adjacent.Add(Vertex);
		Vertex->Adjacent.Add(NewVertex);
		OutFace->Vertices.Add(NewVertex);
	}
	OutFace->SetAdjacency(); // Set's the adjacency of the faces within the face
	OutFace->Normal = Face.Normal;
	Faces.Add(OutFace);

	// -- Setting Adjacencies Manually since some of them are already set -- //
	OutFace->Vertices[0]->Adjacent.Add(Face.Vertices[0]);
	Face.Vertices[0]->Adjacent.Add(OutFace->Vertices[0]);

	OutFace->Vertices[1]->Adjacent.Add(Face.Vertices[1]);
	Face.Vertices[1]->Adjacent.Add(OutFace->Vertices[1]);

	OutFace->Vertices[2]->Adjacent.Add(Face.Vertices[2]);
	Face.Vertices[2]->Adjacent.Add(OutFace->Vertices[2]);

	OutFace->Vertices[3]->Adjacent.Add(Face.Vertices[3]);
	Face.Vertices[3]->Adjacent.Add(OutFace->Vertices[3]);

	
	// -- Creating side faces -- //
	FFace* f1 = new FFace();
	f1->Vertices.Add(Face.Vertices[0]);
	f1->Vertices.Add(Face.Vertices[1]);
	f1->Vertices.Add(OutFace->Vertices[1]);
	f1->Vertices.Add(OutFace->Vertices[0]);

	FFace* f2 = new FFace();
	f2->Vertices.Add(Face.Vertices[1]);
	f2->Vertices.Add(Face.Vertices[2]);
	f2->Vertices.Add(OutFace->Vertices[2]);
	f2->Vertices.Add(OutFace->Vertices[1]);
	
	FFace* f3 = new FFace();
	f3->Vertices.Add(Face.Vertices[2]);
	f3->Vertices.Add(Face.Vertices[3]);
	f3->Vertices.Add(OutFace->Vertices[3]);
	f3->Vertices.Add(OutFace->Vertices[2]);

	FFace* f4 = new FFace();
	f4->Vertices.Add(Face.Vertices[3]);
	f4->Vertices.Add(Face.Vertices[0]);
	f4->Vertices.Add(OutFace->Vertices[0]);
	f4->Vertices.Add(OutFace->Vertices[3]);

	Faces.Append({ f1, f2, f3, f4 });

	// -- Using face verts to construct vectors which are then used to find the normal -- //
	FVector t1;
	FVector t2;

	t1 = f1->Vertices[1]->Location - f1->Vertices[0]->Location;
	t2 = f1->Vertices[3]->Location - f1->Vertices[0]->Location;
	f1->Normal = FVector::CrossProduct(t1, t2);
	f1->Normal.Normalize();
	//DrawDebugLine(MyActor->GetWorld(), MyActor->GetActorLocation() + (t1 / 2.f) + (t2 / 2.f), MyActor->GetActorLocation() + (t1 / 2.f) + (t2 / 2.f) + f1->Normal * 100.f, FColor::Cyan, true);
	//DrawDebugSphere(MyActor->GetWorld(), MyActor->GetActorLocation() + (t1 / 2.f) + (t2 / 2.f), 12, 12, FColor::Cyan, true);

	t1 = f2->Vertices[0]->Location - f2->Vertices[1]->Location;
	t2 = f2->Vertices[0]->Location - f2->Vertices[3]->Location;
	f2->Normal = FVector::CrossProduct(t1, t2);
	f2->Normal.Normalize();

	t1 = f3->Vertices[0]->Location - f3->Vertices[1]->Location;
	t2 = f3->Vertices[0]->Location - f3->Vertices[3]->Location;
	f3->Normal = FVector::CrossProduct(t1, t2);
	f3->Normal.Normalize();

	t1 = f4->Vertices[0]->Location - f4->Vertices[1]->Location;
	t2 = f4->Vertices[0]->Location - f4->Vertices[3]->Location;
	f4->Normal = FVector::CrossProduct(t1, t2);
	f4->Normal.Normalize();

	Faces.Remove(&Face);
}