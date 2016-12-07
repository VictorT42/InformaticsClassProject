//File print.c

#include <stdio.h>
#include "../../headers/structs.h"
#include "../../headers/index.h"
#include "../../headers/buffer.h"
#include "../../headers/bfs.h"
#include "../../headers/print.h"

void printGraph(NodeIndex *index, Buffer *buffer)
{
	node *curNode;
	int i,j;
	for(i=0;i<index->arraySize;i++)
	{
		if(index->listHead[i]==-1)
			continue;
		printf("%d: ", i);
		curNode=getListNode(buffer, getListHead(index,(ptr)i));
		for(j=0;j<curNode->neighborCounter;j++)
			printf("%d ", curNode->neighbor[j]);
		while(curNode->nextListNode != -1)
		{
			curNode=getListNode(buffer, curNode->nextListNode);
			for(j=0;j<curNode->neighborCounter;j++)
				printf("%d ", curNode->neighbor[j]);
		}
		putchar('\n');
	}
	return;
}



void printPath(pathNode *start)
{
	while(start != NULL)
	{
		if(start->next != NULL)
		{
			printf("%d -> ", start->nodeID);
			start = start->next;
			free(start->previous);
		}
		else
		{
			printf("%d\n", start->nodeID);
			free(start);
			return;
		}
		fflush(stdout);
	}
}



void printError(int errorCode)
{
	switch(errorCode)
	{
		case INDEX_ALLOCATION_FAIL:
			fprintf(stderr, "Fatal error in allocating index memory. Aborting.\n");
			exit(1);
		case INDEX_REALLOCATION_FAIL:
			fprintf(stderr, "Fatal error in reallocating index memory. Aborting.\n");
			exit(1);
		case BUFFER_ALLOCATION_FAIL:
			fprintf(stderr, "Fatal error in allocating buffer memory. Aborting.\n");
			exit(1);
		case BUFFER_REALLOCATION_FAIL:
			fprintf(stderr, "Fatal error in reallocating buffer memory. Aborting.\n");
			exit(1);
		case LIST_ALLOCATION_FAIL:
			fprintf(stderr, "Fatal error in allocating list memory. Aborting.\n");
			exit(1);
		case QUEUE_ALLOCATION_FAIL:
			fprintf(stderr, "Fatal error in allocating queue memory. Aborting.\n");
			exit(1);
		case QUEUE_POP_FAIL:
			fprintf(stderr, "Fatal error in popping from empty queue. Aborting.\n");
			exit(1);
		case BFS_ARRAY_ALLOCATION_FAIL:
			fprintf(stderr, "Fatal error in allocating BFS parent array memory. Aborting.\n");
			exit(1);
		case NODE_ID_FAIL:
			fprintf(stderr, "Invalid node requested. Ignoring.\n");
			return;
		case INVALID_COMMAND_FAIL:
			fprintf(stdout, "Invalid command given. Ignoring.\n");
			return;
		case EDGE_ADDITION_FAIL:
			fprintf(stderr, "Request to add edge that is already in the graph. Ignoring.\n");
			return;
		case HASH_TABLE_ALLOCATION_FAIL:
			fprintf(stderr, "Fatal error in allocating hash table memory. Aborting.\n");
			exit(1);
		case HASH_TABLE_REALLOCATION_FAIL:
			fprintf(stderr, "Fatal error in reallocating hash table memory. Aborting.\n");
			exit(1);
		case BFS_VISITED_DATA_FAIL:
			fprintf(stderr, "Fatal error in allocating data structure memory for BFS. Aborting.\n");
			exit(1);
		case QUEUE_OUT_OF_BOUNDS_FAIL:
			fprintf(stderr, "Fatal error caused failure of queue. Aborting.\n");
			exit(1);
	}

	return;

}
