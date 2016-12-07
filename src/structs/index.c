//File index.c

#include "../../headers/structs.h"
#include "../../headers/index.h"
#include "../../headers/buffer.h"
#include "../../headers/print.h"
#include "../../headers/hash.h"

NodeIndex *createNodeIndex()
{
	
	NodeIndex *index;
	int i;

	index = malloc(sizeof(NodeIndex));
	if(index == NULL)
	{
		printError(INDEX_ALLOCATION_FAIL);
		return NULL;
	}
	
	index->listHead = malloc(INITIAL_INDEX_SIZE*sizeof(ptr));
	index->listTail = malloc(INITIAL_INDEX_SIZE*sizeof(ptr));
	if(index->listHead == NULL || index->listTail == NULL)
	{
		printError(INDEX_ALLOCATION_FAIL);
		return NULL;
	}
	
	index->arraySize = INITIAL_INDEX_SIZE;
	
	for(i = 0; i < INITIAL_INDEX_SIZE; i++)
	{
		index->listHead[i] = -1;
		index->listTail[i] = -1;
	}

	return index;

}

OK_SUCCESS insertNode(NodeIndex *index, ptr nodeId, Buffer *buffer)
{
	
	int expansionFlag = 0;
	int previousArraySize;
	int i;
	
	previousArraySize = index->arraySize;
	while(nodeId>=index->arraySize)
	{
		expansionFlag = 1;
		index->arraySize = index->arraySize*2;
	}
	
	if(expansionFlag == 1)
	{
	
		index->listHead = realloc(index->listHead, index->arraySize*sizeof(ptr));
		index->listTail = realloc(index->listTail, index->arraySize*sizeof(ptr));
		
		if(index->listHead == NULL || index->listTail == NULL)
		{
			printError(INDEX_REALLOCATION_FAIL);
			return NO;
		}
		
		for(i = previousArraySize; i < index->arraySize; i++)
		{
			index->listHead[i] = -1;
			index->listTail[i] = -1;
		}
		
	}
	
	index->listHead[nodeId] = allocNewNode(buffer);
	index->listTail[nodeId] = index->listHead[nodeId];

	return YES;

}

ptr getListHead(NodeIndex *index, ptr nodeId)
{

	ptr curNode;

	curNode = (index->listHead)[(int) nodeId];
	if(curNode == -1)
	{
		printError(NODE_ID_FAIL);
		return -2;
	}
	else
	{
		return curNode;
	}

}

ptr getListTail(NodeIndex *index, ptr nodeId)
{
	
	ptr curNode;

	curNode = (index->listTail)[(int) nodeId];
	if(curNode == -1)
	{
		printError(NODE_ID_FAIL);
		return -2;
	}
	else
	{
		return curNode;
	}

}

OK_SUCCESS destroyNodeIndex(NodeIndex *index)
{

	free(index->listHead);
	free(index->listTail);
	
	free(index);
	
	return YES;

}
