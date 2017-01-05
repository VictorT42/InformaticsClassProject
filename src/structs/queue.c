//File queue.c

#include <stdio.h>
#include <string.h>
#include "../../headers/bfs.h"
#include "../../headers/print.h"

OK_SUCCESS initializeQueue(queue *q, int size)
//Initializes the queue with given initial size.
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
//Pushes into queue.
{	
	q->array[q->end] = nodeID;
	
	if((q->end == q->start-1) || (q->end == q->size -1 && q->start == 0))  //If the queue is full and needs more space
	{
		int newSize = q->size*2;

		q->array = realloc(q->array, newSize*sizeof(int));

		if(q->end < q->start)
		{
			memcpy(q->array + q->size, q->array, q->end*sizeof(int));
			q->end = q->start + q->end +1;
		}
		q->size = q->newSize;
	}
	
	q->end++;
	
	if(q->end == q->size)  //The queue has reached the end of the array and continues from its start
	{
		q->end=0;
	}
/*	else if(q->end > (q->size))
		printError(QUEUE_OUT_OF_BOUNDS_FAIL);
*/	
}

int pop(queue *q)
//Pops from queue.
{
	
	int nodeID;

	if(q->start == q->end)
	{
		printError(QUEUE_POP_FAIL);
		return -2;
	}
	else
	{
		nodeID = q->array[q->start];
		q->start++;
		
		if(q->start==q->size)
		{
			q->start=0;
		}
/*		else if(q->start>q->size)
			printError(QUEUE_OUT_OF_BOUNDS_FAIL);
*/		
	}
	
	return nodeID;	
	
}

void cleanQueue(queue *q)
//Reinitializes the queue without releasing the allocated memory.
{
	q->start=0;
	q->end=0;
	return;
}

OK_SUCCESS isEmptyQueue(queue *q)
//Checks if the queue only contains special item DEPTH.
{

	if(q->end == 0 && q->start == q->size-1 && q->array[q->start] == DEPTH)
		return YES;
	if(q->start!= q->size-1 && q->start + 1 == q->end && q->array[q->start] == DEPTH)
		return YES;
	return NO;
	
}

void deleteQueue(queue *q)
//Releases the array the queue uses.
{
	free(q->array);
	return;
}

void printQueue(queue *q)
//Prints what is in the queue. (For debugging purposes)
{
	int i;
		
	for(i=q->start;i<q->end;i++)
		printf("%d->",q->array[i]);
	printf("\b\b  \n");
	return;
}
