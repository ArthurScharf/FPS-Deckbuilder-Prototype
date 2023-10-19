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


