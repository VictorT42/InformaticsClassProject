//File buffer.c

#include "../../headers/structs.h"
#include "../../headers/index.h"
#include "../../headers/buffer.h"
#include "../../headers/print.h"

Buffer *createBuffer()
{
	
	Buffer *buffer = NULL;
	buffer = malloc(sizeof(Buffer));
	if(buffer == NULL)
	{
		printError(BUFFER_ALLOCATION_FAIL);
		return NULL;
	}
	
	else
	{
		buffer->list = malloc(INITIAL_BUFFER_SIZE * sizeof(node));
		if(buffer->list == NULL)
		{
			printError(LIST_ALLOCATION_FAIL);
			return NULL;
		}
		else
		{
			buffer->maxNodes = INITIAL_BUFFER_SIZE;
			buffer->nodeCounter = 0;
			return buffer;
		}
	}
	
}

ptr allocNewNode(Buffer* buffer)
{
	
	ptr newNode;
	
	if(buffer->nodeCounter >= buffer->maxNodes)
	{
		buffer->maxNodes = 2*(buffer->maxNodes);
		buffer->list = realloc(buffer->list, buffer->maxNodes*sizeof(node));
		if(buffer->list == NULL)
		{
			printError(BUFFER_REALLOCATION_FAIL);
			return -1;
		}
	}
	
	newNode = buffer->nodeCounter;

	(((buffer->list)[buffer->nodeCounter]).neighborCounter) = 0;
	
	(((buffer->list)[buffer->nodeCounter]).nextListNode) = -1;

	buffer->nodeCounter++;

	return newNode;

}

OK_SUCCESS insertNeighbor(NodeIndex *index, Buffer *buffer, int outNode, int inNode)
{

	ptr offset;
	node *curNode;
	ptr newNode;
	
	offset = getListTail(index, outNode);
	
	curNode = getListNode(buffer, offset);
	
	if(curNode->neighborCounter == N)  //If the last node is full
	{


		newNode = allocNewNode(buffer);
		index->listTail[outNode] = newNode;
		curNode = getListNode(buffer, offset);  //In case the node has been moved during memory reallocation
		curNode->nextListNode = newNode;
		curNode = getListNode(buffer, curNode->nextListNode);
	}
	
	curNode->neighbor[curNode->neighborCounter] = inNode;
	curNode->neighborCounter++;
	
	return YES;
	
}

struct list_node *getListNode(Buffer *buffer, ptr offset)
{

	node *curNode;
	curNode = &((buffer->list)[offset]);
	return curNode;

}

OK_SUCCESS destroyBuffer(Buffer *buffer)
{

	free(buffer->list);
	free(buffer);
	
	return YES;

}


