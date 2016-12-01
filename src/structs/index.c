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

//TEMPORARY TODO

HashTablesArray *createHashTablesArray()
{

	HashTablesArray *hashStruct;
	int i;

	hashStruct = malloc(sizeof(HashTablesArray));

	if(hashStruct == NULL)
	{
		printError(HASH_TABLE_ALLOCATION_FAIL);
		return NULL;
	}

	hashStruct->array = malloc(INITIAL_HASH_TABLE_SIZE*sizeof(table*));

	if(hashStruct == NULL)
	{
		printError(HASH_TABLE_ALLOCATION_FAIL);
		return NULL;
	}

	hashStruct->arraySize = INITIAL_HASH_TABLE_SIZE;

	for(i = 0; i < INITIAL_HASH_TABLE_SIZE; i++)
	{
		hashStruct->array[i] = NULL;
	}

	return hashStruct;

}

OK_SUCCESS insertEdgeToHash(HashTablesArray *hashStruct, ptr outNode, ptr inNode)
{

	int expansionFlag = 0;
	int previousArraySize;
	int i;
	
	previousArraySize = hashStruct->arraySize;
	while((outNode >= hashStruct->arraySize) || (inNode >= hashStruct->arraySize))
	{
		expansionFlag = 1;
		hashStruct->arraySize = hashStruct->arraySize*2;
	}
	
	if(expansionFlag == 1)
	{
	
		hashStruct->array = realloc(hashStruct->array, hashStruct->arraySize*sizeof(table*));
		
		if(hashStruct->array == NULL)
		{
			printError(HASH_TABLE_REALLOCATION_FAIL);
			return NO;
		}
		
		for(i = previousArraySize; i < hashStruct->arraySize; i++)
		{
			hashStruct->array[i] = NULL;
		}

	}

	if(hashStruct->array[outNode] == NULL)
	{
		hashStruct->array[outNode] = initializeHashTable(hashStruct->array[outNode], HASH_TABLE_BUCKET_ENTRIES, HASH_TABLE_LOAD_FACTOR, INITIAL_HASH_TABLE_SIZE);
	}

	addEntryToTable(hashStruct->array[outNode], NULL, inNode);

	return YES;

}

void destroyHashTables(HashTablesArray *hashStruct)
{
	int i;

	for(i = 0; i < hashStruct->arraySize; i++)
	{
		deleteHash(hashStruct->array[i]);
	}
	
	free(hashStruct->array);

	free(hashStruct);

}

