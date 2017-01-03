//File stack.c

#include <stdlib.h>
#include "../../headers/wcc.h"
#include "../../headers/print.h"

void initializeStack(stack *s)
{
	s->size = INITIAL_STACK_SIZE;
	s->end = 0;
	s->array = malloc(s->size*sizeof(int));
	
	if(s->array == NULL)
	{
		printError(STACK_ALLOCATION_FAIL);
		return;
	}
}

void stackPush(stack *s, int nodeID)
{
	if(s->end == s->size-1)
	{
		s->size*=2;
		s->array = realloc(s->array, s->size*sizeof(int));
		if(s->array == NULL)
		{
			printError(STACK_REALLOCATION_FAIL);
			return;
		}
	}

	s->array[s->end] = nodeID;
	s->end++;
}

int stackPop(stack *s)
{
	if(s->end == 0)
	{
		printError(STACK_POP_FAIL);
		return -1;
	}

	s->end--;
	return s->array[s->end];
}

int stackVirtualPop(stack *s)
{
	if(s->end == 0)
	{
		printError(STACK_POP_FAIL);
		return -1;
	}
	
	return s->array[s->end-1];
}

int stackIsEmpty(stack *s)
{
	if(s->end==0)
		return YES;
	else
		return NO;
}

void deleteStack(stack *s)
{
	free(s->array);
}