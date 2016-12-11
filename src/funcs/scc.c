//File scc.c

#include "../../headers/structs.h"
#include "../../headers/index.h"
#include "../../headers/buffer.h"
#include "../../headers/scc.h"
#include "../../headers/wcc.h"
#include "../../headers/bfs.h"
#include "../../headers/print.h"

SCC *estimateStronglyConnectedComponents(Buffer *outBuffer, NodeIndex* outIndex, HypergraphEdges *hypergraphEdges)
{
	int i, indexCounter = 0;
	stack callStack, tarjanStack;
	int *index, *lowLink, *lastVisitedListNode, *lastVisitedNeighbor;
	int neighbor, parent;
	node *listNode;
	ptr curNode;
	SCC *components;
	
	index = malloc(outIndex->arraySize * sizeof(int));
	lowLink = malloc(outIndex->arraySize * sizeof(int));
	lastVisitedListNode = malloc(outIndex->arraySize * sizeof(int));
	lastVisitedNeighbor = malloc(outIndex->arraySize * sizeof(int));
	
	components = malloc(sizeof(SCC));
	createSCC(components, outIndex->arraySize);
	
	initializeStack(&callStack);
	initializeStack(&tarjanStack);
	
	for(i = 0; i < outIndex->arraySize; i++)
	{
		if(outIndex->listHead[i] == -1)
			break;
		index[i] = UNDEFINED;
		lastVisitedListNode[i] = getListHead(outIndex, i);
		lastVisitedNeighbor[i] = -1;
	}
	
	for(i = 0; i < outIndex->arraySize; i++)
	{
		
		if(outIndex->listHead[i] == -1)
			break;
		if(index[i] == CONNECTED)
			continue;
				
		curNode = i;
		stackPush(&callStack, curNode);
		
		while(!stackIsEmpty(&callStack))
		{
			curNode = stackVirtualPop(&callStack);
			
			listNode = getListNode(outBuffer, lastVisitedListNode[curNode]);
			
			//Tarjan's algorithm
			if(index[curNode] == UNDEFINED) //If this is the first time visiting this node
			{
				index[curNode] = indexCounter;
				lowLink[curNode] = indexCounter;
				indexCounter++;
			
				stackPush(&tarjanStack, curNode);
			}
			else  //If returning after a child node has been processed, check the child node's low link
			{
				neighbor = listNode->neighbor[lastVisitedNeighbor[curNode]];
				if(lowLink[neighbor] < lowLink[curNode])
					lowLink[curNode] = lowLink[neighbor];
			}
			
			lastVisitedNeighbor[curNode]++;
			if(lastVisitedNeighbor[curNode] == listNode->neighborCounter)  //If at the end of the current list node
			{
				if(listNode->nextListNode == -1)  //If there are no more children to be added
				{
					stackPop(&callStack);  //The current node does not need to be called again
					if(index[curNode] == lowLink[curNode])
					{
						createComponent(components, index, &tarjanStack, curNode);
						if(!stackIsEmpty(&tarjanStack))  //Unless this is a root graph node, there is an edge connecting its parent's component to its own
						{
							parent = stackVirtualPop(&callStack);
							addEdgeToHypergraph(hypergraphEdges, parent, curNode);
						}
					}
					continue;
				}
				else  //If there are more children to be added, go to the next list node
				{
					lastVisitedListNode[curNode] = listNode->nextListNode;
					lastVisitedNeighbor[curNode] = 0;
				}
			}
			
			neighbor = listNode->neighbor[lastVisitedNeighbor[curNode]];
			
			if(index[neighbor] == UNDEFINED)  //If the child has not been visited, add it to the stack
			{
				stackPush(&callStack, neighbor);
			}
			else if(index[neighbor] == CONNECTED)  //If the child has been put in a component, there is an edge connecting this node's component to the child's
			{
				addEdgeToHypergraph(hypergraphEdges, curNode, neighbor);
			}
			
		}
	}
	
	deleteStack(&callStack);
	deleteStack(&tarjanStack);
	
	free(index);
	free(lowLink);
	free(lastVisitedListNode);
	free(lastVisitedNeighbor);
	
	return components;
	
}

OK_SUCCESS createSCC(SCC *components, int size)
{
	components->components = malloc(INITIAL_SCC_SIZE * sizeof(Component*));
	components->components_count = 0;
	components->arraySize = INITIAL_SCC_SIZE;
	components->id_belongs_to_component = malloc(size * sizeof(int));
	if(components->id_belongs_to_component == NULL)
	{
		printError(COMPONENTS_STRUCTURE_ALLOCATION_FAIL);
		return NO;
	}
	return YES;
}

void deleteSCC(SCC *components)
{
	int i;
	
	free(components->id_belongs_to_component);
	for(i = 0; i < components->components_count; i++)
	{
		free((components->components[i])->included_node_ids);
		free(components->components[i]);
	}
	free(components);
	return;
}

void createComponent(SCC *components, int *index, stack *tarjanStack, ptr root)
{
	ptr curNode;
	Component *newCC = malloc(sizeof(Component));
	if(newCC == NULL)
	{
		printError(COMPONENTS_STRUCTURE_ALLOCATION_FAIL);
		return;
	}
	
	if(components->components_count == components->arraySize -1)
	{
		components->arraySize *= 2;
		components->components = realloc(components->components, components->arraySize);
		if(components->components == NULL)
		{
			printError(COMPONENTS_STRUCTURE_ALLOCATION_FAIL);
			return;
		}
	}
	components->components[components->components_count] = newCC;
	newCC->component_id = components->components_count;
	components->components_count++;
	
	newCC->included_nodes_count = 0;
	newCC->arraySize = INITIAL_CC_ARRAY_SIZE;
	newCC->included_node_ids = malloc(newCC->arraySize*sizeof(ptr));
	
	do
	{
		curNode = stackPop(tarjanStack);
		components->id_belongs_to_component[curNode] = newCC->component_id;
		if(newCC->included_nodes_count == newCC->arraySize)
		{
			newCC->arraySize *= 2;
			newCC->included_node_ids = realloc(newCC->included_node_ids, newCC->arraySize);
			if(newCC->included_node_ids == NULL)
			{
				printError(COMPONENTS_STRUCTURE_ALLOCATION_FAIL);
				return;
			}
		}
		(newCC->included_node_ids)[newCC->included_nodes_count] = curNode;
		newCC->included_nodes_count++;
		index[curNode] = CONNECTED;
		
	}
	while(curNode != root);
	
	return;
}

OK_SUCCESS iterateStronglyConnectedComponentID(SCC* components, ComponentCursor* cursor)
{
	if(components->components==NULL)
	{
		return NO;
	}
	cursor->counter=0;
	cursor->component_ptr = components->components[0];
	return YES;
}

OK_SUCCESS next_StronglyConnectedComponentID(SCC* components, ComponentCursor* cursor)
{
	if(cursor->counter == components->components_count-1)
	{
		return NO;
	}
	cursor->counter++;
	cursor->component_ptr++;
	return YES;
}

int estimateShortestPathStronglyConnectedComponents(NodeIndex *outIndex, Buffer *outBuffer, NodeIndex *inIndex, Buffer *inBuffer, int start, int goal, BFSVisitedData *visited, queue *forwardQueue, queue *backwardQueue, SCC *components)
{
	return bBFS(outIndex, outBuffer, inIndex, inBuffer, start, goal, visited, forwardQueue, backwardQueue, components);
}
