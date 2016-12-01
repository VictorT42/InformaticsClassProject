//File bfs.c

#include <stdio.h>
#include "../../headers/structs.h"
#include "../../headers/index.h"
#include "../../headers/buffer.h"
#include "../../headers/bfs.h"
#include "../../headers/print.h"

void push(queue *q, int nodeID)
{
	
	queueNode *newNode;

	if(q->start == NULL)
	{
		newNode = malloc(sizeof(queueNode));
		if(newNode == NULL)
		{
			printError(QUEUE_ALLOCATION_FAIL);
			return;
		}

		q->start = newNode;
		newNode->previous = NULL;
	}
	else
	{
		newNode = malloc(sizeof(queueNode));
		if(newNode == NULL)
		{
			printError(QUEUE_ALLOCATION_FAIL);
			return;
		}

		newNode->previous = q->end;
		q->end->next = newNode;
	}
	
	q->end = newNode;
	newNode->nodeID = nodeID;
	newNode->next = NULL;
	
}

int pop(queue *q)
{
	
	int nodeID;

	if(q->start == NULL)
	{
//		printError(QUEUE_POP_FAIL);
		return -1;
	}
	else
	{
		nodeID = q->end->nodeID;
		q->end = q->end->previous;
		if(q->end == NULL)
		{
			free(q->start);
			q->start = NULL;
		}
		else
		{
			free(q->end->next);
			q->end->next = NULL;
		}
	}
	
	return nodeID;	
	
}

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
					
					deleteBFS(forwardParentArray, backwardParentArray, &forwardQueue, &backwardQueue);
					
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
				
				deleteBFS(forwardParentArray, backwardParentArray, &forwardQueue, &backwardQueue);
				
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
					
					deleteBFS(forwardParentArray, backwardParentArray, &forwardQueue, &backwardQueue);
					
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
				
				deleteBFS(forwardParentArray, backwardParentArray, &forwardQueue, &backwardQueue);
				
				return path;			
			}
		}

		if(forwardQueue.start == NULL || backwardQueue.start == NULL)
		{
			deleteBFS(forwardParentArray, backwardParentArray, &forwardQueue, &backwardQueue);
			
			return NULL;
		}

	}

}

void deleteBFS(int *forwardParentArray, int *backwardParentArray, queue *forwardQueue, queue *backwardQueue)
{

	free(forwardParentArray);
	free(backwardParentArray);

	while(forwardQueue->start != NULL)
	{
		pop(forwardQueue);
	}
	while(backwardQueue->start != NULL)
	{
		pop(backwardQueue);
	}

	return;

}

pathNode *pathfinder(int *forwardParentArray, int *backwardParentArray, int midpoint)
{
	int curNode;
	pathNode *start;
	pathNode *end;

	start = malloc(sizeof(pathNode));
	start->nodeID = midpoint;

	end = start;

	//From midpoint forward
	curNode = backwardParentArray[midpoint];

	while(curNode != END_NODE)
	{
		end->next = malloc(sizeof(pathNode));
		end->next->previous = end;
		end = end->next;
		end->nodeID = curNode;
		curNode = backwardParentArray[curNode];
	}

	end->next = NULL;

	//From midpoint backward
	curNode = forwardParentArray[midpoint];

	while(curNode != END_NODE)
	{
		start->previous = malloc(sizeof(pathNode));
		start->previous->next = start;
		start = start->previous;
		start->nodeID = curNode;
		curNode = forwardParentArray[curNode];
	}

	start->previous = NULL;

	return start;

}

int pathLength(pathNode *start)
{

	int length = -1;

	while(start != NULL)
	{
		length++;
		if(start->next != NULL)
		{
			start = start->next;
			free(start->previous);
		}
		else
		{
			free(start);
			break;
		}
	}

	return length;

}
