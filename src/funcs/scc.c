//File scc.c

#include "../../headers/structs.h"
#include "../../headers/index.h"
#include "../../headers/buffer.h"
#include "../../headers/scc.h"
#include "../../headers/wcc.h"
#include "../../headers/bfs.h"

#include <stdio.h>
void printStack(stack *s)
{
	int i;
	for(i=0;i<s->end;i++)
		printf("%d->",s->array[i]);
	printf("\b\b  \n");
	return;
}
int stackVirtualPop(stack*);

void createHypergraphEdges(HypergraphEdges *hypergraphEdges)
{
	hypergraphEdges->edgeCount = 0;
	hypergraphEdges->arraySize = INITIAL_HYPERGRAPH_ARRAY_SIZE;
	hypergraphEdges->outNode = malloc(INITIAL_HYPERGRAPH_ARRAY_SIZE*sizeof(ptr));
	hypergraphEdges->inNode = malloc(INITIAL_HYPERGRAPH_ARRAY_SIZE*sizeof(ptr));
}

void addEdgeToHypergraph(HypergraphEdges *hypergraphEdges, ptr outNode, ptr inNode)
{
	if(hypergraphEdges->edgeCount == hypergraphEdges->arraySize)
	{
		hypergraphEdges->arraySize *= 2;
		hypergraphEdges->outNode = realloc(hypergraphEdges->outNode, hypergraphEdges->arraySize*sizeof(ptr));
		hypergraphEdges->inNode = realloc(hypergraphEdges->inNode, hypergraphEdges->arraySize*sizeof(ptr));
	}
	hypergraphEdges->outNode[hypergraphEdges->edgeCount] = outNode;
	hypergraphEdges->inNode[hypergraphEdges->edgeCount] = inNode;
	hypergraphEdges->edgeCount++;
}

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
		index[i] = UNDEFINED;
		lastVisitedListNode[i] = 0;
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
					stackPop(&callStack);
					if(index[curNode] == lowLink[curNode])
					{fprintf(stderr,"Done with node %d\n",curNode);
						createComponent(components, index, &tarjanStack, curNode);
						if(!stackIsEmpty(&tarjanStack))  //Unless this is a root graph node, there is an edge connecting its parent's component to its own
						{
							parent = stackVirtualPop(&callStack);
							addEdgeToHypergraph(hypergraphEdges, parent, curNode);
//							stackPush(&callStack, parent);
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
			
//			stackPush(&callStack, curNode);  //Unless the graph node has added its last child already, it will need to be revisited
			
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
	//error check
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
	//error check
	if(components->components_count == components->arraySize)
	{
		components->arraySize *= 2;
		components->components = realloc(components->components, components->arraySize);
	}
	components->components_count++;
	components->components[components->components_count] = newCC;
	
	newCC->component_id = components->components_count;
	newCC->included_nodes_count = 0;
	newCC->arraySize = INITIAL_CC_ARRAY_SIZE;
	newCC->included_node_ids = malloc(newCC->arraySize*sizeof(uint32_t));
	
	do
	{
		curNode = stackPop(tarjanStack);
		components->id_belongs_to_component[curNode] = newCC->component_id;
		if(newCC->included_nodes_count == newCC->arraySize)
		{
			newCC->arraySize *= 2;
			newCC->included_node_ids = realloc(newCC->included_node_ids, newCC->arraySize);
		}
		newCC->included_nodes_count++;
		newCC->included_node_ids[newCC->included_nodes_count] = curNode;
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

Hypergraph *buildHypergraph(HypergraphEdges *hypergraphEdges)
{
	int i;
	ptr outNode, inNode;
	Hypergraph *hypergraph = malloc(sizeof(Hypergraph));
	
	hypergraph->buffer = createBuffer();
	hypergraph->index = createNodeIndex();
	
	for(i = 0; i < hypergraphEdges->edgeCount; i++)
	{
		outNode = hypergraphEdges->outNode[i];
		inNode = hypergraphEdges->inNode[i];
		
		if((hypergraph->index->arraySize <= outNode) || (hypergraph->index->listHead[outNode] == -1))
			insertNode(hypergraph->index, outNode, hypergraph->buffer);
		if((hypergraph->index->arraySize <= inNode) || (hypergraph->index->listHead[inNode] == -1))
			insertNode(hypergraph->index, inNode, hypergraph->buffer);
		
		insertNeighbor(hypergraph->index, hypergraph->buffer, outNode, inNode);
		
	}
	
	return hypergraph;
	
}

GrailIndex *buildGrailIndex(Hypergraph *hypergraph, SCC* components)
{
	int i;
	GrailIndex *grail;
	int curComponent, neighbor;
	stack callStack;
	int rank = 0;
	node *listNode;
	int *lastVisitedListNode, *lastVisitedNeighbor;
	
	initializeStack(&callStack);
	
	grail = malloc(sizeof(GrailIndex));
	grail->rank = malloc(components->components_count * sizeof(int));
	grail->minRank = malloc(components->components_count * sizeof(int));
	
	lastVisitedListNode = malloc(components->components_count * sizeof(int));
	lastVisitedNeighbor = malloc(components->components_count * sizeof(int));
	
	for(i = 0; i < components->components_count; i++)
	{
		grail->rank[i] = UNDEFINED;
		grail->minRank[i] = (components->components_count) + 1;
		lastVisitedListNode[i] = 0;
		lastVisitedNeighbor[i] = -1;
	}
	
	for(i = 0; i < components->components_count; i++)
	{
		if(grail->rank[i] != UNDEFINED)
			continue;
		
		if(hypergraph->index->listHead[i] == -1)
		{
			grail->rank[i] = rank;
			grail->minRank[i] = rank;
			rank++;
			continue;
		}
		
		curComponent = components->components[i]->component_id;
		
		stackPush(&callStack, curComponent);
		
		while(!stackIsEmpty(&callStack))
		{
			curComponent = stackPop(&callStack);
			listNode = getListNode(hypergraph->buffer, lastVisitedListNode[curComponent]);
			
			//GRAIL algorithm
			if(grail->rank[curComponent] != UNDEFINED) //If this is not the first time visiting this component
			{
				neighbor = listNode->neighbor[lastVisitedNeighbor[curComponent]];
				if(grail->minRank[neighbor] < grail->minRank[curComponent])
					grail->minRank[curComponent] = grail->minRank[neighbor];
			}
			
			lastVisitedNeighbor[curComponent]++;
			if(lastVisitedNeighbor[curComponent] == listNode->neighborCounter)
			{
				if(listNode->nextListNode == -1)
				{
					grail->rank[curComponent] = rank;
					grail->minRank[curComponent] = rank;
					rank++;
				}
				else
				{
					lastVisitedListNode[curComponent] = listNode->nextListNode;
					lastVisitedNeighbor[curComponent] = 0;
				}
			}
			
			neighbor = listNode->neighbor[lastVisitedNeighbor[curComponent]];
			
			if(grail->rank[neighbor] == UNDEFINED)
			{
				stackPush(&callStack, neighbor);
			}
			else
			{
				if(grail->minRank[neighbor] < grail->minRank[curComponent])
					grail->minRank[curComponent] = grail->minRank[neighbor];
			}
			
			stackPush(&callStack, curComponent);
			
		}
	}
	
	deleteStack(&callStack);
	
	return grail;
	
}

GRAIL_ANSWER isReachableGrailIndex(GrailIndex* grail, uint32_t source_node, uint32_t target_node)
{
	if((grail->rank[target_node] < grail->rank[source_node]) && (grail->minRank[target_node] > grail->minRank[source_node]))
		return NO;
	else
		return MAYBE;
}

OK_SUCCESS destroyGrailIndex(GrailIndex* index)
{
	free(index->rank);
	free(index->minRank);
	free(index);
	return YES;
}
