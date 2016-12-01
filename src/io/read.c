//File read.c

#include <stdio.h>
#include "../../headers/structs.h"
#include "../../headers/index.h"
#include "../../headers/buffer.h"
#include "../../headers/bfs.h"
#include "../../headers/print.h"
#include "../../headers/read.h"

void readInput(Buffer *outBuffer, Buffer *inBuffer, NodeIndex *outIndex, NodeIndex *inIndex, HashTablesArray *hashStruct)
{
	int inNode, outNode;
	char line[16];
	char operation;
	pathNode *path;

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
				path = bBFS(outIndex, outBuffer, inIndex, inBuffer, outNode, inNode);

				printf("%d\n", pathLength(path));
			}
			else if(operation == 'A')
			{
				add(outBuffer, inBuffer, outIndex, inIndex, hashStruct, outNode, inNode);
			}
			else
			{
				printError(INVALID_COMMAND_FAIL);
			}
		}
	}
	
}


void add(Buffer *outBuffer, Buffer *inBuffer, NodeIndex *outIndex, NodeIndex *inIndex, HashTablesArray *hashStruct, int outNode, int inNode)
{

	entry *added;

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

	insertEdgeToHash(hashStruct, outNode, inNode);
	
	insertNeighbor(outIndex, outBuffer, outNode, inNode);

	insertNeighbor(inIndex, inBuffer, inNode, outNode);
		
	return;

}
