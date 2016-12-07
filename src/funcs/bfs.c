//File bfs.c

#include <stdio.h>
#include <string.h>
#include "../../headers/structs.h"
#include "../../headers/index.h"
#include "../../headers/buffer.h"
#include "../../headers/bfs.h"
#include "../../headers/print.h"

OK_SUCCESS initializeVisited(BFSVisitedData *visited, int size)
//Initialize the BFS support sructure holding visited node information.
{

	visited->arraySize = size;
	visited->fVisited = malloc(size*sizeof(int));
	visited->bVisited = malloc(size*sizeof(int));
	visited->roundCounter = 0;

	if(visited->fVisited == NULL || visited->bVisited == NULL)
	{
		printError(BFS_VISITED_DATA_FAIL);
		return NO;
	}

	memset(visited->fVisited,0,size);
	memset(visited->bVisited,0,size);

	return YES;
}

void deleteVisited(BFSVisitedData *visited)
//Release the allocated memory in the BFS support structure.
{
	free(visited->fVisited);
	free(visited->bVisited);
}

int bBFS(NodeIndex *outIndex, Buffer *outBuffer, NodeIndex *inIndex, Buffer *inBuffer, int start, int goal, BFSVisitedData *visited, queue *forwardQueue, queue *backwardQueue)
//Bidirectional Breadth-First Search.
{
	node *fListNode, *bListNode;
	int curForwardID, curBackwardID;
	int fPathLength = 0, bPathLength = 0;

	if(outIndex->arraySize > visited->arraySize)
	{
		deleteVisited(visited);
		initializeVisited(visited, outIndex->arraySize);
	}

	visited->roundCounter++;

	push(forwardQueue, start);
	push(forwardQueue, DEPTH);
	push(backwardQueue, goal);
	push(backwardQueue, DEPTH);

	visited->fVisited[start] = visited->roundCounter;
	visited->bVisited[goal] = visited->roundCounter;
	
	while(1)
	{
		//Forward BFS
		curForwardID = pop(forwardQueue);
		while(curForwardID != DEPTH)
		{
			fListNode = getListNode(outBuffer, getListHead(outIndex, curForwardID));

			while((fListNode->neighborCounter == N) && (fListNode->nextListNode != -1))
			{
				
				if(checkNeighbors(visited, forwardQueue, 'f', fListNode) == YES)
					return fPathLength + bPathLength + 1;
				
				fListNode = getListNode(outBuffer,fListNode->nextListNode);
			}

			if(checkNeighbors(visited, forwardQueue, 'f', fListNode) == YES)
				return fPathLength + bPathLength + 1;

			curForwardID = pop(forwardQueue);

		}

		fPathLength++;
		push(forwardQueue, DEPTH);
		
		if(isEmptyQueue(forwardQueue))
		{
		return -1;
		}
		
		
		//Backward BFS
		curBackwardID = pop(backwardQueue);
		while(curBackwardID != DEPTH)
		{
			bListNode = getListNode(inBuffer, getListHead(inIndex, curBackwardID));

			while((bListNode->neighborCounter == N) && (bListNode->nextListNode != -1))
			{
				if(checkNeighbors(visited, backwardQueue, 'b', bListNode) == YES)
					return fPathLength + bPathLength + 1;
				
				bListNode = getListNode(inBuffer,bListNode->nextListNode);
			}

			if(checkNeighbors(visited, backwardQueue, 'b', bListNode) == YES)
				return fPathLength + bPathLength + 1;

			curBackwardID = pop(backwardQueue);

		}

		bPathLength++;
		push(backwardQueue, DEPTH);

		if(isEmptyQueue(backwardQueue))
		{
			return -1;
		}
	}
	
}

OK_SUCCESS checkNeighbors(BFSVisitedData *visited, queue *q, char direction, node *listNode)
//Supporive function for BFS that handles all neighboring graph nodes in a single list node.
{

	int *forwardArray;
	int *backwardArray;
	int i;

	if(direction == 'f')
	{
		forwardArray = visited->fVisited;
		backwardArray = visited->bVisited;
	}
	else
	{
		backwardArray = visited->fVisited;
		forwardArray = visited->bVisited;
	}

	for(i = 0; i < listNode->neighborCounter; i++)
	{
		if(forwardArray[listNode->neighbor[i]] == visited->roundCounter)
		{
			continue;
		}

		push(q, listNode->neighbor[i]);
		forwardArray[listNode->neighbor[i]] = visited->roundCounter;

		if(backwardArray[listNode->neighbor[i]] == visited->roundCounter)
		{
			return YES;
		}
	}

	return NO;

}

