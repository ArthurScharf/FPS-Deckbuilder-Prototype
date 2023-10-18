// Fill out your copyright notice in the Description page of Project Settings.


#include "TestGraphGrammar.h"

#include "GeomNode.h"




void UTestGraphGrammar::Init()
{
	// -- Initializing Graph -- //
	FGeomNode Temp;
	Head = &Temp;
	Head->Label = "start";
	Nodes.Add(Head->Label, Head);

	LeastBounds = GreatestBounds = FVector(0);

	// -- Initializing Rules -- // 
	Rules.Add(
		"start",
		[&](FGeomNode* Node)
		{
			// -- Modifying Transformed Node -- //
			PutNode("start|hallway", Node); // Don't want to call start multiple times
			Node->Location = FVector(0);
			Node->Width = 4; // 3 units either side of node


			// -- Constructing New Node & Setting Bounds -- // 
			FGeomNode NewNode;
			NewNode.Label = "hallway";
			NewNode.Location = Node->Location + FVector(10, 0, 0);
			Node->Width = 4;


			// -- Updating Bounds -- //
			GreatestBounds[0] = 10; // TEMP


			// -- Linking Nodes -- //
			Node->Adjacent.Add(&NewNode);
			// NewNode.Adjacent.Add(*Node);		// NOTE: for now, we might not be doing bidirectional graphs
		}
	);
}