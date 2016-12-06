//File wcc.c

#include "../../headers/structs.h"
#include "../../headers/index.h"
#include "../../headers/buffer.h"
#include "../../headers/hash.h"

#define INITIAL_STACK_SIZE 1000000
#define UNDISCOVERED -1

typedef struct CC
{
uint32_t *ccIndex; //CCIndex
//UpdateIndex* updateIndex;
int queries;
int updateQueries;
} CC;

typedef struct stack
{
	int *array;
	int size;
	int end;
} stack;

typedef struct UpdateIndex
{
	table *array;
	int arraySize;
} UpdateIndex

void initializeStack(stack *s)
{
	s->size = INITIAL_STACK_SIZE;
	s->end = 0;
	s->array = malloc(s->size*sizeof(int));
	//TODO ERRORCHECKING
}

void stackPush(stack *s, int nodeID)
{
	if(s->end == s->size-1)
	{
		s->size*=2;
		s->array = realloc(s->array, s->size*sizeof(int));
	}

	s->array[s->end] = nodeID;
	s->end++;
}

int stackPop(stack *s)
{
	if(s->end == 0)
	{
//		printError(STACK_POP_FAIL);
		return -1;
	}

	s->end--;
	return s->array[s->end];
}

CC *createCC(int size)
{
	int i;
	CC *components;

	components = malloc(sizeof(CC));
	//TODO

	components->ccIndex = malloc(size*sizeof(uint32_t));
	//components->malloc(sizeof(UpdateIndex))
	components->queries = 0;
	components->updateQueries = 0;

	for(i = 0; i < size; i++)
	{
		components->ccIndex[i] = UNDISCOVERED;
	}

	return components;
}

int findNodeConnectedComponentID(CC *components, int nodeID)
{
	return components->ccIndex[nodeID];
}

CC *estimateConnectedComponents(Buffer *outBuffer, NodeIndex *outIndex, Buffer *inBuffer, NodeIndex *inIndex)
{
	int i, j;
	CC *components;
	stack dfsStack;
	node *listNode;
	int curNodeID;
	int componentID = 0;

	components = createCC(outIndex->arraySize);

	initializeStack(&dfsStack);

	for(i=0; i < outIndex->arraySize; i++)
	{
		if(outIndex->listHead[i] == -1)
			continue;

		if(findNodeConnectedComponentID(components, i) == UNDISCOVERED)
		{
			(&dfsStack)->end = 0;
			//DFS
			push(&dfsStack, i);

			while((&dfsStack)->end != 0)
			{
				curNodeID = stackPop(&dfsStack);

				//Out
				listNode = getListNode(outBuffer, getListHead(outIndex, curNodeID));
				while((listNode->neighborCounter == N) && (listNode->nextListNode != -1))
				{
					for(j = 0; j < listNode->neighborCounter; j++)
					{
						if(components->ccIndex[listNode->neighbor[j]] != UNDISCOVERED)
							continue;

						components->ccIndex[listNode->neighbor[j]] = componentID;
						stackPush(&dfsStack, listNode->neighbor[j]);
					}

					listNode = getListNode(outBuffer, listNode->nextListNode);

				}

				for(j = 0; j < listNode->neighborCounter; j++)
				{
					if(components->ccIndex[listNode->neighbor[j]] != UNDISCOVERED)
						continue;

					components->ccIndex[listNode->neighbor[j]] = componentID;
					stackPush(&dfsStack, listNode->neighbor[j]);
				}

				//In
				listNode = getListNode(inBuffer, getListHead(inIndex, curNodeID));
				while((listNode->neighborCounter == N) && (listNode->nextListNode != -1))
				{
					for(j = 0; j < listNode->neighborCounter; j++)
					{
						if(components->ccIndex[listNode->neighbor[j]] != UNDISCOVERED)
							continue;

						components->ccIndex[listNode->neighbor[j]] = componentID;
						stackPush(&dfsStack, listNode->neighbor[j]);
					}

					listNode = getListNode(inBuffer, listNode->nextListNode);

				}

				for(j = 0; j < listNode->neighborCounter; j++)
				{
					if(components->ccIndex[listNode->neighbor[j]] != UNDISCOVERED)
						continue;

					components->ccIndex[listNode->neighbor[j]] = componentID;
					stackPush(&dfsStack, listNode->neighbor[j]);
				}

			}

			componentID++;

		}
	}

	return components;

}





