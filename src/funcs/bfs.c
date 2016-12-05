//File bfs.c

#include <stdio.h>
#include <string.h>
#include "../../headers/structs.h"
#include "../../headers/index.h"
#include "../../headers/buffer.h"
#include "../../headers/bfs.h"
#include "../../headers/print.h"

OK_SUCCESS initializeQueue(queue *q, int size)
{
	q->array=malloc(size*sizeof(int));
	if(q->array == NULL)
	{
		printError(QUEUE_ALLOCATION_FAIL);
		return NO;
	}
	q->size=size;
	q->start=0;
	q->end=0;

	return YES;
}

void push(queue *q, int nodeID)
{
	
	q->array[q->end]=nodeID;
	
	if((q->end == q->start-1) || (q->end==q->size && q->start==0)){
		//Grow
		printf("NEEDS TO GROW\n");
		fflush(stdout);
	}
	
	q->end++;
	if(q->end == q->size){
		q->end=0;
	}
	else if(q->end > (q->size)) printf("SOMETHING WENT HORRIBLY WRONG!\n");
}

int pop(queue *q)
{
	
	int nodeID;

	if(q->start == q->end)
	{
		printError(QUEUE_POP_FAIL);
		return -1;
	}
	else
	{
		nodeID = q->array[q->start];
		q->start++;
		if(q->start==q->size){
			q->start=0;
		}
		else if(q->start>q->size) printf("SOMETHING WENT HORRIBLY WRONG!\n");
	}
	
	return nodeID;	
	
}

OK_SUCCESS initializeVisited(BFSVisitedData *visited, int size)
{

	visited->arraySize = size;
	visited->fVisited = malloc(size*sizeof(int));
	visited->bVisited = malloc(size*sizeof(int));
	visited->roundCounter = 0;

	//TODO error checking

	memset(visited->fVisited,0,size);
	memset(visited->bVisited,0,size);

	return YES;
}

void deleteVisited(BFSVisitedData *visited)
{
	free(visited->fVisited);
	free(visited->bVisited);
}

int bBFS(NodeIndex *outIndex, Buffer *outBuffer, NodeIndex *inIndex, Buffer *inBuffer, int start, int goal, BFSVisitedData *visited, queue *forwardQueue, queue *backwardQueue)
{
	node *fListNode, *bListNode;
	int curForwardID, curBackwardID;
	int fPathLength = -1, bPathLength = -1;

	if(outIndex->arraySize > visited->arraySize)
	{
		deleteVisited(visited);
		initializeVisited(visited, outIndex->arraySize);
	}

	push(forwardQueue, start);
	push(forwardQueue, DEPTH);
	push(backwardQueue, goal);
	push(backwardQueue, DEPTH);
	
	while(1)
	{
		//Forward BFS
		curForwardID = pop(forwardQueue);
		if(curForwardID == DEPTH)
		{
			fPathLength++;
			push(forwardQueue, DEPTH);
			continue;
		}
		fListNode = getListNode(outBuffer, getListHead(outIndex, curForwardID));

		while((fListNode->neighborCounter == N) && (fListNode->nextListNode != -1))
		{
			if(checkNeighbors(visited, forwardQueue, 'f', fListNode) == YES)
			{
				return fPathLength + bPathLength + 1;
			}
			fListNode = getListNode(outBuffer,fListNode->nextListNode);
		}

		if(checkNeighbors(visited, forwardQueue, 'f', fListNode) == YES)
		{
			return fPathLength + bPathLength + 1;
		}

		if(forwardQueue->start == forwardQueue->end)
		{
			return -1;
		}
		
		//Backward BFS
		curBackwardID = pop(backwardQueue);
		if(curBackwardID == DEPTH)
		{
			bPathLength++;
			push(backwardQueue, DEPTH);
			continue;
		}
		bListNode = getListNode(outBuffer, getListHead(outIndex, curBackwardID));

		while((bListNode->neighborCounter == N) && (bListNode->nextListNode != -1))
		{
			if(checkNeighbors(visited, backwardQueue, 'b', bListNode) == YES)
			{
				return fPathLength + bPathLength + 1;
			}
			bListNode = getListNode(outBuffer,bListNode->nextListNode);
		}

		if(checkNeighbors(visited, backwardQueue, 'b', bListNode) == YES)
		{
			return fPathLength + bPathLength + 1;
		}

		if(backwardQueue->start == backwardQueue->end)
		{
			return -1;
		}
	}
	
}

OK_SUCCESS checkNeighbors(BFSVisitedData *visited, queue *q, char direction, node *listNode)
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


/*
pathNode *bBFS(NodeIndex *outIndex, Buffer *outBuffer, NodeIndex *inIndex, Buffer *inBuffer, int start, int goal)
{
	
	node *fListNode, *bListNode;
	queue forwardQueue, backwardQueue;
	int *forwardParentArray, *backwardParentArray;
	int curForwardID, curBackwardID;
	int i;

	 forwardQueue.start = NULL;
	forwardQueue.end = NULL;
	backwardQueue.start = NULL;
	backwardQueue.end = NULL; 
	
	initializeQueue(&forwardQueue, INITIAL_QUEUE_SIZE);
	initializeQueue(&backwardQueue, INITIAL_QUEUE_SIZE);

	//Create arrays to keep the parents of visited nodes
	forwardParentArray = malloc(sizeof(int)*outIndex->arraySize);
	backwardParentArray = malloc(sizeof(int)*inIndex->arraySize);
	if((forwardParentArray == NULL) || (backwardParentArray == NULL))
	{
		printError(BFS_ARRAY_ALLOCATION_FAIL);
		return NULL;
	}

	//Initialize the arrays
	for(i = 0; i < outIndex->arraySize; i++)
	{
		forwardParentArray[i] = UNVISITED;
		backwardParentArray[i] = UNVISITED;
	}

	forwardParentArray[start] = END_NODE;
	backwardParentArray[goal] = END_NODE;

	push(&forwardQueue, start);
	push(&backwardQueue, goal);
	
	while(1)
	{

		curForwardID = pop(&forwardQueue);
		curBackwardID = pop(&backwardQueue);
		
		fListNode = getListNode(outBuffer, getListHead(outIndex, curForwardID));
		bListNode = getListNode(inBuffer, getListHead(inIndex, curBackwardID));

		//Forward BFS (start to goal)
		while((fListNode->neighborCounter == N) && (fListNode->nextListNode != -1))
		{
			for(i = 0; i < fListNode->neighborCounter; i++)
			{
				if(forwardParentArray[fListNode->neighbor[i]] != UNVISITED)
				{
					continue;
				}

				push(&forwardQueue, fListNode->neighbor[i]);
				forwardParentArray[fListNode->neighbor[i]] = curForwardID;

				if(backwardParentArray[fListNode->neighbor[i]] != UNVISITED)
				{
					pathNode* path = pathfinder(forwardParentArray, backwardParentArray, fListNode->neighbor[i]);
					
					//Release allocared memory before returning
					free(forwardParentArray);
					free(backwardParentArray);
					while(forwardQueue.start != NULL)
						pop(&forwardQueue);
					while(backwardQueue.start != NULL)
						pop(&backwardQueue);
					
					return path;
				}
			}

			fListNode = getListNode(outBuffer,fListNode->nextListNode);

		}

		for(i = 0; i < fListNode->neighborCounter; i++)
		{
			if(forwardParentArray[fListNode->neighbor[i]] != UNVISITED)
			{
				continue;
			}

			push(&forwardQueue, fListNode->neighbor[i]);
			forwardParentArray[fListNode->neighbor[i]] = curForwardID;

			if(backwardParentArray[fListNode->neighbor[i]] != UNVISITED)
			{
				pathNode* path = pathfinder(forwardParentArray, backwardParentArray, fListNode->neighbor[i]);
				
				//Release allocared memory before returning
				free(forwardParentArray);
				free(backwardParentArray);
				while(forwardQueue.start != NULL)
					pop(&forwardQueue);
				while(backwardQueue.start != NULL)
					pop(&backwardQueue);
				
				return path;
			}
		}

		//Backward BFS (goal to start)
		while((bListNode->neighborCounter == N) && (bListNode->nextListNode != -1))
		{
			for(i = 0; i < bListNode->neighborCounter; i++)
			{
				if(backwardParentArray[bListNode->neighbor[i]] != UNVISITED)
				{
					continue;
				}

				push(&backwardQueue, bListNode->neighbor[i]);
				backwardParentArray[bListNode->neighbor[i]] = curBackwardID;

				if(forwardParentArray[bListNode->neighbor[i]] != UNVISITED)
				{
					pathNode* path = pathfinder(forwardParentArray, backwardParentArray, bListNode->neighbor[i]);
					
					//Release allocared memory before returning
					free(forwardParentArray);
					free(backwardParentArray);
					while(&forwardQueue.start != NULL)
						pop(&forwardQueue);
					while(backwardQueue.start != NULL)
						pop(&backwardQueue);
					
					return path;
				}
			}

			bListNode = getListNode(inBuffer, bListNode->nextListNode);

		}

		for(i = 0; i < bListNode->neighborCounter; i++)
		{
			if(backwardParentArray[bListNode->neighbor[i]] != UNVISITED)
			{
				continue;
			}

			push(&backwardQueue, bListNode->neighbor[i]);
			backwardParentArray[bListNode->neighbor[i]] = curBackwardID;

			if(forwardParentArray[bListNode->neighbor[i]] != UNVISITED)
			{
				pathNode* path = pathfinder(forwardParentArray, backwardParentArray, bListNode->neighbor[i]);
				
				//Release allocared memory before returning
				free(forwardParentArray);
				free(backwardParentArray);
				while(forwardQueue.start != NULL)
					pop(&forwardQueue);
				while(backwardQueue.start != NULL)
					pop(&backwardQueue);
				
				return path;			
			}
		}

		if(forwardQueue.start == NULL || backwardQueue.start == NULL)
		{
			//Release allocared memory before returning
			free(forwardParentArray);
			free(backwardParentArray);
			while(forwardQueue.start != NULL)
				pop(&forwardQueue);
			while(backwardQueue.start != NULL)
				pop(&backwardQueue);
			
			return NULL;
		}

	}

}


*/
