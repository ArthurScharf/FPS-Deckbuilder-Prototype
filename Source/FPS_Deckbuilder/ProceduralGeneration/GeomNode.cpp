#include "GeomNode.h"


UGeomNode::UGeomNode()
{
	ID = NextNodeID++;
	Width = 0.f;
	Label = "";
}



FString UGeomNode::ToString()
{
	return FString::Printf(TEXT("[ID: %d , Label: %s , Width: %f , Location: %s]"), ID, *Label, Width, *Location.ToCompactString());
}