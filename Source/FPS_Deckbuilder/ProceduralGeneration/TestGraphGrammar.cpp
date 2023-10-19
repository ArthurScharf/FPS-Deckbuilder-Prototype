// Fill out your copyright notice in the Description page of Project Settings.


#include "TestGraphGrammar.h"

#include "GeomNode.h"




void UTestGraphGrammar::Init()
{
	// -- Initializing Graph -- //
	Head = NewObject<UGeomNode>();
	Head->Label = "start";
	Nodes.Add(Head->Label, Head);

	LeastBounds = GreatestBounds = FVector(0);

	// -- Initializing Rules -- // 
	Rules.Add(
		"start",
		[&](UGeomNode* Node)
		{
			UE_LOG(LogTemp, Warning, TEXT("UTestGraphGrammar::Init -- <RULE: start --> hallway>"));

			// -- Modifying Transformed Node -- //
			PutNode("start|hallway", Node); // Don't want to call start multiple times
			Node->Location = FVector(0);
			Node->Width = 4; // 3 units either side of node


			// -- Constructing New Node & Setting Bounds -- // 
			UGeomNode* NewNode = NewObject<UGeomNode>();
			PutNode("hallway", NewNode);
			NewNode->Location = Node->Location + FVector(10, 0, 0);
			NewNode->Width = 4;
			

			// -- Updating Bounds -- //
			GreatestBounds[0] = 10; // TEMP


			// -- Linking Nodes -- //
			Node->Adjacent.Add(NewNode);
			// NewNode.Adjacent.Add(*Node);		// NOTE: for now, we might not be doing bidirectional graphs
		}
	);

	Rules.Add(
		"hallway",
		[&](UGeomNode* Node)
		{
			UE_LOG(LogTemp, Warning, TEXT("UTestGraphGrammar::Init -- <RULE: hallway --> hallway>"));

			// -- Modifying Transformed Node -- //
			PutNode("", Node); // This hallway is no longer used
			Node->Width = 4; // 3 units either side of node


			// -- Constructing New Node & Setting Bounds -- // 
			UGeomNode* NewNode = NewObject<UGeomNode>();
			PutNode("hallway", NewNode);
			FRotator Rotation;
			float RotationYaw = FMath::FRand() > 0.7 ? 45.f : 90.f;
			Rotation.Yaw = FMath::RandBool() ? 0.f : ( FMath::Pow(-1.f, (int)FMath::RandBool()) * RotationYaw); // Do we turn right, left, or remain straight?
			NewNode->Location = Node->Location + Rotation.RotateVector(FVector(10, 0, 0));
			NewNode->Width = 4;
			


			// -- Updating Bounds -- //
			GreatestBounds[0] = 10; // TEMP


			// -- Linking Nodes -- //
			Node->Adjacent.Add(NewNode);
			// NewNode.Adjacent.Add(*Node);		// NOTE: for now, we might not be doing bidirectional graphs
		}
	);


}