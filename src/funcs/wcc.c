//File wcc.c

#include "../../headers/structs.h"
#include "../../headers/index.h"
#include "../../headers/buffer.h"
#include "../../headers/wcc.h"
#include "../../headers/hash.h"

CC *createCC(int size)
{
	int i;
	CC *components;

	components = malloc(sizeof(CC));
	
	if(components == NULL)
	{
		printError(COMPONENTS_STRUCTURE_ALLOCATION_FAIL);
		return NULL;
	}

	components->ccIndexSize = size;
	components->ccIndex = malloc(size * sizeof(ptr));
	if(components->ccIndex == NULL)
	{
		printError(COMPONENTS_STRUCTURE_ALLOCATION_FAIL);
		return NULL;
	}

	for(i = 0; i < size; i++)
	{
		components->ccIndex[i] = UNDISCOVERED;
	}
	
	components->queries = 0;
	components->updateQueries = 0;
	
	components->updateIndex = malloc(sizeof(UpdateIndex));
	if(components->updateIndex == NULL)
	{
		printError(COMPONENTS_STRUCTURE_ALLOCATION_FAIL);
		return NULL;
	}
	
	components->updateIndex->connectionArraySize = INITIAL_CONNECTION_ARRAY_SIZE;
	components->updateIndex->connectA = malloc(INITIAL_CONNECTION_ARRAY_SIZE * sizeof(int*));
	components->updateIndex->connectB = malloc(INITIAL_CONNECTION_ARRAY_SIZE * sizeof(int*));
	if(components->updateIndex->connectA == NULL || components->updateIndex->connectA == NULL)
	{
		printError(COMPONENTS_STRUCTURE_ALLOCATION_FAIL);
		return NULL;
	}
	
	for(i = 0; i < INITIAL_CONNECTION_ARRAY_SIZE; i++)
	{
		components->updateIndex->connectA[i] = -1;
		components->updateIndex->connectB[i] = -1;
	}
	
	components->updateIndex->connectedComponents = 0;

	return components;
}

int findNodeConnectedComponentID(CC *components, int nodeID)
{
	return components->ccIndex[nodeID];
}

CC *estimateConnectedComponents(Buffer *outBuffer, NodeIndex *outIndex, Buffer *inBuffer, NodeIndex *inIndex)
{
	int i, j;
	CC *components;
	stack dfsStack;
	node *listNode;
	int curNodeID;
	int componentID = 0;

	components = createCC(outIndex->arraySize);

	initializeStack(&dfsStack);

	for(i=0; i < outIndex->arraySize; i++)
	{
		if(outIndex->listHead[i] == -1)
			continue;

		if(findNodeConnectedComponentID(components, i) == UNDISCOVERED)
		{
			(&dfsStack)->end = 0;
			//DFS
			stackPush(&dfsStack, i);

			while((&dfsStack)->end != 0)
			{
				curNodeID = stackPop(&dfsStack);

				//Out
				listNode = getListNode(outBuffer, getListHead(outIndex, curNodeID));
				while((listNode->neighborCounter == N) && (listNode->nextListNode != -1))
				{
					for(j = 0; j < listNode->neighborCounter; j++)
					{
						if(components->ccIndex[listNode->neighbor[j]] != UNDISCOVERED)
							continue;

						components->ccIndex[listNode->neighbor[j]] = componentID;
						stackPush(&dfsStack, listNode->neighbor[j]);
					}

					listNode = getListNode(outBuffer, listNode->nextListNode);

				}

				for(j = 0; j < listNode->neighborCounter; j++)
				{
					if(components->ccIndex[listNode->neighbor[j]] != UNDISCOVERED)
						continue;

					components->ccIndex[listNode->neighbor[j]] = componentID;
					stackPush(&dfsStack, listNode->neighbor[j]);
				}

				//In
				listNode = getListNode(inBuffer, getListHead(inIndex, curNodeID));
				while((listNode->neighborCounter == N) && (listNode->nextListNode != -1))
				{
					for(j = 0; j < listNode->neighborCounter; j++)
					{
						if(components->ccIndex[listNode->neighbor[j]] != UNDISCOVERED)
							continue;

						components->ccIndex[listNode->neighbor[j]] = componentID;
						stackPush(&dfsStack, listNode->neighbor[j]);
					}

					listNode = getListNode(inBuffer, listNode->nextListNode);

				}

				for(j = 0; j < listNode->neighborCounter; j++)
				{
					if(components->ccIndex[listNode->neighbor[j]] != UNDISCOVERED)
						continue;

					components->ccIndex[listNode->neighbor[j]] = componentID;
					stackPush(&dfsStack, listNode->neighbor[j]);
				}

			}

			componentID++;

		}
		
	}

	//Make the hash tables for the update index
	components->componentsNumber = componentID;
	components->updateIndex->hashTableArray = malloc(components->componentsNumber * sizeof(table*));
	if(components->updateIndex->hashTableArray == NULL)
	{
		printError(COMPONENTS_STRUCTURE_ALLOCATION_FAIL);
		return NULL;
	}
	for(i = 0; i < components->componentsNumber; i++)
	{
		components->updateIndex->hashTableArray[i] = initializeHashTable(components->updateIndex->hashTableArray[i], HASH_TABLE_BUCKET_ENTRIES, HASH_TABLE_LOAD_FACTOR, INITIAL_HASH_TABLE_SIZE);
		if(components->updateIndex->hashTableArray[i] == NULL)
		{
			printError(COMPONENTS_STRUCTURE_ALLOCATION_FAIL);
			return NULL;
		}
	}
	
	return components;

}

OK_SUCCESS insertNewEdge(CC* components, ptr outNode, ptr inNode)
{
	int previousArraySize, expansionFlag;
	int componentA, componentB;
	int i;
	
	previousArraySize = components->ccIndexSize;
	while(outNode >= components->ccIndexSize || inNode >= components->ccIndexSize)
	{
		expansionFlag = 1;
		components->ccIndexSize = components->ccIndexSize*2;
	}
	
	if(expansionFlag == 1)
	{
		components->ccIndex = realloc(components->ccIndex, components->ccIndexSize * sizeof(int));
		if(components->ccIndex == NULL)
		{
			printError(COMPONENTS_STRUCTURE_ALLOCATION_FAIL);
			return NO;
		}
		
		for(i = previousArraySize; i < components->ccIndexSize; i++)
		{
			components->ccIndex[i] = UNDISCOVERED;
		}
		
		if(components->ccIndex[outNode] == UNDISCOVERED)
		{
			components->ccIndex[outNode] = components->componentsNumber;
			components->componentsNumber++;
			
			if(components->ccIndex[inNode] == UNDISCOVERED)
			{
				components->ccIndex[inNode] = components->ccIndex[outNode];
				return YES;
			}
		}
		if(components->ccIndex[inNode] == UNDISCOVERED)
		{
			components->ccIndex[inNode] = components->componentsNumber;
			components->componentsNumber++;
			
			if(components->ccIndex[outNode] == UNDISCOVERED)
			{
				components->ccIndex[outNode] = components->ccIndex[inNode];
				return YES;
			}
		}
		
	}
	
	componentA = components->ccIndex[outNode];
	componentB = components->ccIndex[inNode];
	
	if(componentA < componentB)
	{
		if(findEntry(componentB, components->updateIndex->hashTableArray[componentA]) == NULL)
			return connectComponents(components->updateIndex, componentA, componentB);
	}
	else if(componentA > componentB)
	{
		if(findEntry(componentA, components->updateIndex->hashTableArray[componentB]) == NULL)
			return connectComponents(components->updateIndex, componentB, componentA);
	}
	
	return YES;
	
}

OK_SUCCESS connectComponents(UpdateIndex *updateIndex, int componentA, int componentB)
{
	addEntryToTable(updateIndex->hashTableArray[componentA], NULL, componentB);
	
	if(updateIndex->connectedComponents == updateIndex->connectionArraySize - 1)
	{
		updateIndex->connectionArraySize *= 2;
		updateIndex->connectA = realloc(updateIndex->connectA, updateIndex->connectionArraySize * sizeof(int));
		updateIndex->connectB = realloc(updateIndex->connectB, updateIndex->connectionArraySize * sizeof(int));
		if(updateIndex->connectA == NULL || updateIndex->connectB == NULL)
		{
			printError(COMPONENTS_STRUCTURE_ALLOCATION_FAIL);
			return NO;
		}
	}
	
	updateIndex->connectA[updateIndex->connectedComponents] = componentA;
	updateIndex->connectB[updateIndex->connectedComponents] = componentB;
	
	updateIndex->connectedComponents++;
	
	return YES;
	
}
