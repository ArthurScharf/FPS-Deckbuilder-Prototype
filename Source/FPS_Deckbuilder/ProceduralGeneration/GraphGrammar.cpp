// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphGrammar.h"
#include "GeomNode.h"



void UGraphGrammar::Mutate()
{
	UE_LOG(LogTemp, Warning, TEXT("UGraphGrammar::Mutate"));

	TArray<FString> Labels;
	Rules.GetKeys(Labels);
	UGeomNode* Node;
	TFunction<void(UGeomNode* Node)> Rule;
	for (FString Label : Labels)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGraphGrammar::Mutate -- Searching for Label: %s"), *Label);
		if (!Nodes.Contains(Label)) { continue; }

		UE_LOG(LogTemp, Warning, TEXT("UGraphGrammar::Mutate -- Label Found"));
		Node = *Nodes.Find(Label);
		Rule = *Rules.Find(Label);
		Rule(Node);
		break;
	}
	return;
}


void UGraphGrammar::PutNode(FString NewLabel, UGeomNode* Node)
{
	Nodes.Remove(Node->Label, Node);
	Node->Label = NewLabel;
	Nodes.Add(NewLabel, Node);
}


void UGraphGrammar::UpdateBounds(UGeomNode* Node)
{
	UE_LOG(LogTemp, Warning, TEXT("UGraphGrammar::UpdateBounds"));

	if (Node->Width > LargestNodeWidth) { LargestNodeWidth = Node->Width; }

	if (Node->Location.X < LeastBounds.X) { LeastBounds.X = Node->Location.X; }
	else if (GreatestBounds.X < Node->Location.X) { GreatestBounds.X = Node->Location.X; }

	if (Node->Location.Y < LeastBounds.Y) { LeastBounds.Y = Node->Location.Y; }
	else if (GreatestBounds.Y < Node->Location.Y) { GreatestBounds.Y = Node->Location.Y; }

	if (Node->Location.Z < LeastBounds.Z) { LeastBounds.Z = Node->Location.Z; }
	else if (GreatestBounds.Z < Node->Location.Z) { GreatestBounds.Z = Node->Location.Z; }
}