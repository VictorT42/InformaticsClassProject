//File read.c

#include <stdio.h>
#include "../../headers/structs.h"
#include "../../headers/index.h"
#include "../../headers/buffer.h"
#include "../../headers/bfs.h"
#include "../../headers/print.h"
#include "../../headers/read.h"
#include "../../headers/hash.h"
#include "../../headers/wcc.h"

void readInput(Buffer *outBuffer, Buffer *inBuffer, NodeIndex *outIndex, NodeIndex *inIndex, HashTablesArray *hashStruct)
{
	int inNode, outNode;
	char line[16];
	char operation;
	int pathLength;
	queue forwardQueue, backwardQueue;
	BFSVisitedData visited;
	CC *components;

	while(1)
	{	
		if(scanf("%s", line) < 0)
			break;

		if(line[0] == 'S')
			break;
		else
		{
			sscanf(line, "%d", &outNode);
			scanf("%d", &inNode);
		}

		add(outBuffer, inBuffer, outIndex, inIndex, hashStruct, outNode, inNode);

	}
	
	//TEMP
	fprintf(stderr,"Done with graph\n");
	//////
	
	components = estimateConnectedComponents(outBuffer, outIndex, inBuffer, inIndex);
	
	initializeQueue(&forwardQueue, INITIAL_QUEUE_SIZE);
	initializeQueue(&backwardQueue, INITIAL_QUEUE_SIZE);
	initializeVisited(&visited, outIndex->arraySize);

	while(1)
	{
		if(scanf("%s", line) < 0)
			break;

		operation = line[0];

		if(operation == 'F')
			continue;
		else
		{
			scanf("%d %d", &outNode, &inNode);
			if(operation == 'Q')
			{
				//Check the CC
				if(components->updateIndex[components->ccIndex[outNode]] != components->updateIndex[components->ccIndex[inNode]])
					pathLength = -1;
				else
					pathLength = bBFS(outIndex, outBuffer, inIndex, inBuffer, outNode, inNode, &visited, &forwardQueue, &backwardQueue);

				printf("%d\n", pathLength);
			}
			else if(operation == 'A')
			{
				add(outBuffer, inBuffer, outIndex, inIndex, hashStruct, outNode, inNode);
				insertNewEdge(components, outNode, inNode);
			}
			else
			{
				printError(INVALID_COMMAND_FAIL);
			}
		}
	}

	deleteVisited(&visited);
	deleteQueue(&forwardQueue);
	deleteQueue(&backwardQueue);
	
}


void add(Buffer *outBuffer, Buffer *inBuffer, NodeIndex *outIndex, NodeIndex *inIndex, HashTablesArray *hashStruct, int outNode, int inNode)
{

	entry *added;
	
	//Make sure the outcoming node has a corresponding hash table
	insertNodeToHash(hashStruct, outNode);

	//Check if this edge has already been added
	added = findEntry(inNode, hashStruct->array[outNode]);

	if(added != NULL)
	{
		printError(EDGE_ADDITION_FAIL);
		return;
	}

	//Check if out-node is in out-index
	if((outIndex->arraySize <= outNode) || (outIndex->listHead[outNode] == -1))
	{
		insertNode(outIndex, outNode, outBuffer);
	}
	//Check if out-node is in in-index
	if((inIndex->arraySize <= outNode) || (inIndex->listHead[outNode] == -1))
	{
		insertNode(inIndex, outNode, inBuffer);
	}
	//Check if in-node is in out-index
	if((outIndex->arraySize <= inNode) || (outIndex->listHead[inNode] == -1))
	{
		insertNode(outIndex, inNode, outBuffer);
	}
	//Check if in-node is in in-index
	if((inIndex->arraySize <= inNode) || (inIndex->listHead[inNode] == -1))
	{
		insertNode(inIndex, inNode, inBuffer);
	}
	
	//Enter the nodes into the buffers
	insertNeighbor(outIndex, outBuffer, outNode, inNode);
	insertNeighbor(inIndex, inBuffer, inNode, outNode);
	
	//Enter the edge into the corresponding hash table
	addEntryToTable(hashStruct->array[outNode], NULL, inNode);
	
	return;

}
