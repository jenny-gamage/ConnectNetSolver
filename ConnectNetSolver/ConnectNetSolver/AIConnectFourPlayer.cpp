#include "pch.h"
#include "AIConnectFourPlayer.h"
#include <vector>
#include <iostream>
#include "ConnectNeuralNet.h"

AIConnectFourPlayer::AIConnectFourPlayer()
{
	primeNet = ConnectNeuralNet();
}


AIConnectFourPlayer::~AIConnectFourPlayer()
{
}

void AIConnectFourPlayer::MultiplyNet()
{
	plusNet = ConnectNeuralNet();
	minusNet = ConnectNeuralNet();
	plusNet.BecomeClone(primeNet);
	minusNet.BecomeClone(primeNet);
	weightsToModify = selectWeightsToModify();
	for (AIConnectFourPlayer::PerceptronWeightSelector weightToModify : weightsToModify)
	{
		switch (weightToModify.layer)
		{
		case 0: //base layer
		{
			plusNet.baseLayer[weightToModify.index].SetWeight(plusNet.baseLayer[weightToModify.index].GetWeight(weightToModify.weight)+learningRate,weightToModify.weight);
			minusNet.baseLayer[weightToModify.index].SetWeight(minusNet.baseLayer[weightToModify.index].GetWeight(weightToModify.weight) - learningRate, weightToModify.weight);
			break;
		}		
		case 1: //hidden layer
		{
			plusNet.hiddenLayer[weightToModify.index].SetWeight(plusNet.hiddenLayer[weightToModify.index].GetWeight(weightToModify.weight) + learningRate, weightToModify.weight);
			minusNet.hiddenLayer[weightToModify.index].SetWeight(minusNet.hiddenLayer[weightToModify.index].GetWeight(weightToModify.weight) - learningRate, weightToModify.weight);
			break;
		}		
		case 2: //output layer
		{
			plusNet.outputLayer[weightToModify.index].SetWeight(plusNet.outputLayer[weightToModify.index].GetWeight(weightToModify.weight) + learningRate, weightToModify.weight);
			minusNet.outputLayer[weightToModify.index].SetWeight(minusNet.outputLayer[weightToModify.index].GetWeight(weightToModify.weight) - learningRate, weightToModify.weight);
			break;
		}
		}
	}
}

void AIConnectFourPlayer::RefineNet(int plusScore, int primeScore, int minusScore)
{
	DisplayWeightsBeingUpdated();
	if (plusScore > primeScore&&plusScore > minusScore)
		primeNet = plusNet;
	else if (minusScore > primeScore &&minusScore > plusScore)
		primeNet = minusNet;
	DisplayWeightsBeingUpdated();
	MultiplyNet();
}

void AIConnectFourPlayer::MakeMovePlus(ConnectFourGame * currentGame, int numTries)
{
}

void AIConnectFourPlayer::MakeMoveMinus(ConnectFourGame * currentGame, int numTries)
{
}

void AIConnectFourPlayer::DisplayWeightsBeingUpdated()
{
	for (AIConnectFourPlayer::PerceptronWeightSelector weightToModify : weightsToModify)
	{
		std::cout << weightToModify.layer << " / " << weightToModify.index << " / " << weightToModify.weight << " : ";
		switch (weightToModify.layer)
		{
		case 0: //base layer
		{
			std::cout<<primeNet.baseLayer[weightToModify.index].GetWeight(weightToModify.weight)<<" ";
			break;
		}
		case 1: //hidden layer
		{

			std::cout << primeNet.hiddenLayer[weightToModify.index].GetWeight(weightToModify.weight) << " ";
			break;
		}
		case 2: //output layer
		{
			std::cout << primeNet.outputLayer[weightToModify.index].GetWeight(weightToModify.weight) << " ";
			break;
		}
		}
	}
	std::cout << "\n";
}

int AIConnectFourPlayer::SelectMove(ConnectFourGame * currentGame, int nthChoice)
{
	std::vector<float> preferences = primeNet.GenerateMoves(currentGame);
	if (nthChoice >= 7) {
		std::cout << "all options rejected! Game should be over.\n";
		return 1;
	}
		//get the best reccomendation from the net
	std::vector<int> pickOrder = GetPickOrder(preferences);
	return pickOrder.at(nthChoice);
}int AIConnectFourPlayer::SelectMovePlus(ConnectFourGame * currentGame, int nthChoice)
{
	std::vector<float> preferences = plusNet.GenerateMoves(currentGame);
	if (nthChoice >= 7) {
		std::cout << "all options rejected! Game should be over.\n";
		return 1;
	}
	//get the best reccomendation from the net
	std::vector<int> pickOrder = GetPickOrder(preferences);
	return pickOrder.at(nthChoice);
}int AIConnectFourPlayer::SelectMoveMinus(ConnectFourGame * currentGame, int nthChoice)
{
	std::vector<float> preferences = minusNet.GenerateMoves(currentGame);
	if (nthChoice >= 7) {
		std::cout << "all options rejected! Game should be over.\n";
		return 1;
	}
	//get the best reccomendation from the net
	std::vector<int> pickOrder = GetPickOrder(preferences);
	return pickOrder.at(nthChoice);
}

std::vector<int> AIConnectFourPlayer::GetPickOrder(std::vector<float> preferences)
{
	std::vector<int> pickOrder = std::vector<int>(7,0);
	for (int i = 0; i < 7; i++)
	{
		int minAddress = 0;
		int maxAddress = 0;
		for (int j = 1; j < 7; j++)
		{
			if (preferences[j] > preferences[maxAddress])
				maxAddress = j;			
			if (preferences[j] < preferences[minAddress])
				minAddress = j;
		}
		pickOrder[i] = maxAddress;
		preferences[maxAddress] = preferences[minAddress] - 0.000001f;
	}
	return pickOrder;
}

std::vector<AIConnectFourPlayer::PerceptronWeightSelector> AIConnectFourPlayer::selectWeightsToModify()
{
	int numNodesToSelect = batchSize;
	std::vector<AIConnectFourPlayer::PerceptronWeightSelector> allNodes = GetAllNodes();
	if (numNodesToSelect > allNodes.size())
		numNodesToSelect = allNodes.size();
	std::vector<AIConnectFourPlayer::PerceptronWeightSelector>  selectedNodes = std::vector<AIConnectFourPlayer::PerceptronWeightSelector>(numNodesToSelect);
	int numRemainingNodes= allNodes.size();
	for (int i = 0; i < numNodesToSelect; i++)
	{
		int nodeAddrToSelect = rand() % numRemainingNodes;
		selectedNodes[i] = allNodes[nodeAddrToSelect];
		allNodes[nodeAddrToSelect] = allNodes[numRemainingNodes - 1];
		numRemainingNodes--;
	}
	return selectedNodes;
}

std::vector<AIConnectFourPlayer::PerceptronWeightSelector> AIConnectFourPlayer::GetAllNodes()
{
	std::vector<AIConnectFourPlayer::PerceptronWeightSelector> allNodes = std::vector<AIConnectFourPlayer::PerceptronWeightSelector>(147);
	for (int i = 0; i < 147; i++)
	{
		allNodes[i].index = (i/7) % 7;
		allNodes[i].layer = i / 49;
		allNodes[i].weight = i % 7;
	}
	return allNodes;
}

