#include "Shape.h"


void UShape::MoveFace(FFace* Face, const FVector& DeltaLocation)
{
	for (FVertex* Vertex : Face->Vertices)
	{
		Vertex->Location += DeltaLocation;
	}
}




void UShape::SubdivideFace(FFace* Face, EFaceAxis Axis, float Percent, FFace* OutFace1, FFace* OutFace2)
{
	// TODO: Make the order of the vertices not matter for this function

	// edges{0,1} and edges{2,3} are subdivided
	if (Axis == EFaceAxis::X)
	{
		// Making new faces
		FVertex* v1 = new FVertex();
		FVertex* v2 = new FVertex();

		v1->Location = Face->Vertices[0]->Location + (Face->Vertices[3]->Location - Face->Vertices[0]->Location) * Percent;
		v1->Adjacent.Add(Face->Vertices[0]);
		v1->Adjacent.Add(Face->Vertices[3]);
		v1->Adjacent.Add(v2);

		v2->Location = Face->Vertices[1]->Location + (Face->Vertices[2]->Location - Face->Vertices[1]->Location) * Percent;
		v2->Adjacent.Add(Face->Vertices[1]);
		v2->Adjacent.Add(Face->Vertices[2]);
		v2->Adjacent.Add(v1);

		// Making new faces
		FFace* f1 = new FFace();
		FFace* f2 = new FFace();

		f1->Vertices.Add(Face->Vertices[0]);
		f1->Vertices.Add(Face->Vertices[1]);
		f1->Vertices.Add(v2);
		f1->Vertices.Add(v1);
		
		f2->Vertices.Add(v1);
		f2->Vertices.Add(v2);
		f2->Vertices.Add(Face->Vertices[2]);
		f2->Vertices.Add(Face->Vertices[3]);

		// Removing old face and adding new face
		Faces.Remove(Face);
		Faces.Add(f1);
		Faces.Add(f2);

		OutFace1->Vertices = f1->Vertices;
		OutFace2->Vertices = f2->Vertices;
	}
	else
	{
		// TODO
	}
}