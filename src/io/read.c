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
#include "../../headers/scc.h"

#define TYPE 's'
void readInput(Buffer *outBuffer, Buffer *inBuffer, NodeIndex *outIndex, NodeIndex *inIndex, HashTablesArray *hashStruct)
{
	int inNode, outNode;
	char line[16];
	char operation;
	int pathLength;
	queue forwardQueue, backwardQueue;
	BFSVisitedData visited;
	CC *components;
	SCC *SCComponents;
	HypergraphEdges hypergraphEdges;
	Hypergraph *hypergraph;
	GrailIndex *grail;
	GRAIL_ANSWER answer;
	int outMinCC, inMinCC;
	int usedUpdateIndex;
	float metric;
	char graphType = TYPE;

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
	
	if(graphType == 's')
	{
		createHypergraphEdges(&hypergraphEdges);
		SCComponents = estimateStronglyConnectedComponents(outBuffer, outIndex, &hypergraphEdges);
		hypergraph = buildHypergraph(&hypergraphEdges);
		grail = buildGrailIndex(hypergraph, SCComponents);
	}
	else
	{
		components = estimateConnectedComponents(outBuffer, outIndex, inBuffer, inIndex);
	}
	
	initializeQueue(&forwardQueue, INITIAL_QUEUE_SIZE);
	initializeQueue(&backwardQueue, INITIAL_QUEUE_SIZE);
	initializeVisited(&visited, outIndex->arraySize);

	while(1)
	{
		if(scanf("%s", line) < 0)
			break;

		operation = line[0];

		if(operation == 'F')
		{
			if(graphType == 'd')
			{
				metric = ((float)components->updateQueries)/components->queries;
				if(metric > CC_METRIC)
				{
					rebuildIndexes(components, outBuffer, outIndex, inBuffer, inIndex);
				}
			}
		}
		else
		{
			scanf("%d %d", &outNode, &inNode);
			if(operation == 'Q')
			{
				//Check the CC
				if(graphType == 's')
				{
					if(SCComponents->id_belongs_to_component[outNode] == SCComponents->id_belongs_to_component[inNode])
						pathLength = estimateShortestPathStronglyConnectedComponents(outIndex, outBuffer, inIndex, inBuffer, outNode, inNode, &visited, &forwardQueue, &backwardQueue, SCComponents);
					else
					{
						answer = isReachableGrailIndex(grail, outNode, inNode);
						if(answer == NO)
							pathLength = -1;
						else
							pathLength = bBFS(outIndex, outBuffer, inIndex, inBuffer, outNode, inNode, &visited, &forwardQueue, &backwardQueue, NULL);
					}
				}
				else
				{
					usedUpdateIndex = 0;
					outMinCC = components->updateIndex[components->ccIndex[outNode]];
					inMinCC = components->updateIndex[components->ccIndex[inNode]];
					
					if(outMinCC == inMinCC && components->ccIndex[outNode] != components->ccIndex[inNode])
						usedUpdateIndex = 1;
					
					while(outMinCC != components->updateIndex[outMinCC])
					{
						outMinCC = components->updateIndex[outMinCC];
						usedUpdateIndex = 1;
					}
					while(inMinCC != components->updateIndex[inMinCC])
					{
						inMinCC = components->updateIndex[inMinCC];
						usedUpdateIndex = 1;
					}
					
					if(usedUpdateIndex)
						components->updateQueries++;
					components->queries++;

					if(outMinCC != inMinCC)
						pathLength = -1;
					else
						pathLength = bBFS(outIndex, outBuffer, inIndex, inBuffer, outNode, inNode, &visited, &forwardQueue, &backwardQueue, NULL);
				}

				printf("%d\n", pathLength);
			}
			else if(operation == 'A')
			{
				if(graphType == 's')
					continue;
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
	if(graphType == 'd')
		destroyConnectedComponents(components);
	else
		deleteSCC(SCComponents);
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
