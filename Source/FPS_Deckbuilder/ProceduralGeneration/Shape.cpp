#include "Shape.h"





// --  Static Functions -- //
void UShape::InitCube(UShape* Shape)
{
	// Shifted by (-0.5, -0.5, -0.5) to center it
	FVertex* v1 = new FVertex(FVector(-0.5, -0.5, -0.5));
	FVertex* v2 = new FVertex(FVector(-0.5,  0.5, -0.5));
	FVertex* v3 = new FVertex(FVector( 0.5,  0.5, -0.5));
	FVertex* v4 = new FVertex(FVector( 0.5, -0.5, -0.5));
	FVertex* v5 = new FVertex(FVector(-0.5, -0.5,  0.5));
	FVertex* v6 = new FVertex(FVector(-0.5,  0.5,  0.5));
	FVertex* v7 = new FVertex(FVector( 0.5,  0.5,  0.5));
	FVertex* v8 = new FVertex(FVector( 0.5, -0.5,  0.5));

	FFace* f1 = new FFace();  // bottom (-Z)
	f1->Normal = FVector(0, 0, 1);
	f1->Label = FString("floor");
	f1->Vertices.Append({ v4,v3,v2,v1 });
	f1->SetAdjacency();

	FFace* f2 = new FFace();  // top (+Z)
	f2->Normal = FVector(0, 0, 1);
	f2->Label = FString("ceiling");
	f2->Vertices.Append({ v5,v6,v7,v8 });
	f2->SetAdjacency();
	
	FFace* f3 = new FFace();  // Back (-Y)
	f3->Normal = FVector(0, -1, 0);
	f3->Label = FString("backward");
	f3->Vertices.Append({ v5,v8,v4,v1 });
	f3->SetAdjacency();

	FFace* f4 = new FFace();  // Back (-Y)
	f4->Normal = FVector(0, 1, 0);
	f4->Label = FString("forward");
	f4->Vertices.Append({ v2,v3,v7,v6 });
	f4->SetAdjacency();

	FFace* f5 = new FFace();  // Right (+X)
	f5->Normal = FVector(-1, 0, 0);
	f5->Label = FString("right");
	f5->Vertices.Append({ v1,v2,v6,v5 });
	f5->SetAdjacency();

	FFace* f6 = new FFace();  // Right (+X)
	f6->Normal = FVector(1, 0, 0);
	f6->Label = FString("left");
	f6->Vertices.Append({ v8,v7,v3,v4 });
	f6->SetAdjacency();

	Shape->Faces = { f1, f2, f3, f4, f5, f6 };
}



// -- Member Functions  -- // 
void UShape::SetScale(float Scale)
{
	TSet<FVertex*> ScaledVerts;
	for (FFace* Face : Faces) 
	{
		for (FVertex* Vertex : Face->Vertices)
		{
			bool bAlreadyInSet;
			ScaledVerts.Add(Vertex, &bAlreadyInSet);
			if (!bAlreadyInSet) { Vertex->Location *= Scale; }
		}
	}
}

void UShape::SetScale(FVector Scale) 
{
	// Can't use FFace->SetScale because doing so would scale vertices shared between faces multiple times
	TSet<FVertex*> ScaledVerts;
	for (FFace* Face : Faces)
	{
		for (FVertex* Vertex : Face->Vertices)
		{
			bool bAlreadyInSet;
			ScaledVerts.Add(Vertex, &bAlreadyInSet);
			if (!bAlreadyInSet) 
			{ 
				Vertex->Location.X *= Scale.X;
				Vertex->Location.Y *= Scale.Y;
				Vertex->Location.Z *= Scale.Z;
			}
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

		OutFace1->Label = "forward";
		OutFace2->Label = "forward";

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
	OutFace->Label = "forward";
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
	f1->Label = "floor";
	f1->Vertices.Add(OutFace->Vertices[1]);
	f1->Vertices.Add(OutFace->Vertices[0]);
	f1->Vertices.Add(Face.Vertices[0]);
	f1->Vertices.Add(Face.Vertices[1]);
	
	FFace* f2 = new FFace();
	f2->Label = "left";
	f2->Vertices.Add(OutFace->Vertices[1]);
	f2->Vertices.Add(Face.Vertices[1]);
	f2->Vertices.Add(Face.Vertices[2]);
	f2->Vertices.Add(OutFace->Vertices[2]);
	

	FFace* f3 = new FFace();
	f3->Label = "roof";
	f3->Vertices.Add(Face.Vertices[2]);
	f3->Vertices.Add(Face.Vertices[3]);
	f3->Vertices.Add(OutFace->Vertices[3]);
	f3->Vertices.Add(OutFace->Vertices[2]);
	

	FFace* f4 = new FFace();
	f4->Label = "right";
	
	f4->Vertices.Add(Face.Vertices[0]);
	f4->Vertices.Add(OutFace->Vertices[0]);
	f4->Vertices.Add(OutFace->Vertices[3]);
	f4->Vertices.Add(Face.Vertices[3]);

	Faces.Append({ f1, f2, f3, f4 });
	

	// -- Using face verts to construct vectors which are then used to find the normal -- //
	FVector t1;
	FVector t2;
	
	t1 = f1->Vertices[1]->Location - f1->Vertices[0]->Location;
	t2 = f1->Vertices[3]->Location - f1->Vertices[0]->Location;
	f1->Normal = FVector::CrossProduct(t1, t2);
	f1->Normal.Normalize();

	t1 = f2->Vertices[3]->Location - f2->Vertices[0]->Location;
	t2 = f2->Vertices[1]->Location - f2->Vertices[0]->Location;
	f2->Normal = FVector::CrossProduct(t1, t2);
	f2->Normal.Normalize();

	t1 = f3->Vertices[3]->Location - f3->Vertices[0]->Location;
	t2 = f3->Vertices[1]->Location - f3->Vertices[0]->Location;
	f3->Normal = FVector::CrossProduct(t1, t2);
	f3->Normal.Normalize();

	t1 = f4->Vertices[3]->Location - f4->Vertices[0]->Location;
	t2 = f4->Vertices[1]->Location - f4->Vertices[0]->Location;
	f4->Normal = FVector::CrossProduct(t1, t2);
	f4->Normal.Normalize();

	Faces.Remove(&Face);
}

void UShape::InsetFace(FFace& Face, float Percent, FFace* OutFace)
{
	// -- Constructing inset-face vertices -- //
	for (int i = 0; i < Face.Vertices.Num(); i++)
	{
		OutFace->Vertices.Add(new FVertex(*Face.Vertices[i])); // Copy Constructing. Remember that adjencies aren't copied when copy constructing a vertex
		// -- Connecting inner face verts to their outer face counterparts -- // 
		OutFace->Vertices[i]->Adjacent.Add(Face.Vertices[i]);
		Face.Vertices[i]->Adjacent.Add(OutFace->Vertices[i]);
	}

	// -- Constructing new faces -- // NOTE: No SetAdjacency called required. Previous step set all adj that were needing setting
	FFace* f1 = new FFace(); // Sub-Right
	f1->Normal = OutFace->Normal;
	f1->Vertices.Add(Face.Vertices[0]);
	f1->Vertices.Add(OutFace->Vertices[0]);
	f1->Vertices.Add(OutFace->Vertices[3]);
	f1->Vertices.Add(Face.Vertices[3]);

	FFace* f2 = new FFace();
	f2->Normal = OutFace->Normal;
	f2->Vertices.Add(Face.Vertices[1]);
	f2->Vertices.Add(OutFace->Vertices[1]);
	f2->Vertices.Add(OutFace->Vertices[0]);
	f2->Vertices.Add(Face.Vertices[0]);

	FFace* f3 = new FFace();
	f3->Normal = OutFace->Normal;
	f3->Vertices.Add(Face.Vertices[2]);
	f3->Vertices.Add(OutFace->Vertices[2]);
	f3->Vertices.Add(OutFace->Vertices[1]);
	f3->Vertices.Add(Face.Vertices[1]);

	FFace* f4 = new FFace();
	f4->Normal = OutFace->Normal;
	f4->Vertices.Add(Face.Vertices[3]);
	f4->Vertices.Add(OutFace->Vertices[3]);
	f4->Vertices.Add(OutFace->Vertices[2]);
	f4->Vertices.Add(Face.Vertices[2]);

	Faces.Append({ f1, f2, f3, f4 });

	OutFace->SetAdjacency(); // resetting adj of verts that are a part of the face
	OutFace->Normal = Face.Normal;
	OutFace->Label = "forward";
	OutFace->SetScale(Percent);
	Faces.Add(OutFace);
	Faces.Remove(&Face);
}

FFace* UShape::FindFaceByLabel(FString _Label)
{
	UE_LOG(LogTemp, Warning, TEXT("UShape::FindFaceByLabel -- Searching for _Label: %s"), *_Label);

	for (FFace* Face : Faces)
	{
		 UE_LOG(LogTemp, Warning, TEXT("UShape::FindFaceByLabel ---- Label: %s"), *Face->Label);
		if (Face->Label == _Label)
		{	 
			return Face;
		}
	}
	return nullptr;
}