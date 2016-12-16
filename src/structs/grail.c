//File grail.c

#include "../../headers/structs.h"
#include "../../headers/index.h"
#include "../../headers/buffer.h"
#include "../../headers/scc.h"
#include "../../headers/wcc.h"
#include "../../headers/bfs.h"

Hypergraph *buildHypergraph(HypergraphEdges *hypergraphEdges, SCC* components)
{
	int i;
	ptr outNode, inNode;
	Hypergraph *hypergraph = malloc(sizeof(Hypergraph));
	
	hypergraph->buffer = createBuffer();
	hypergraph->index = createNodeIndex();
	
	for(i = 0; i < hypergraphEdges->edgeCount; i++)
	{
		outNode = components->id_belongs_to_component[hypergraphEdges->outNode[i]];
		inNode = components->id_belongs_to_component[hypergraphEdges->inNode[i]];
		
		if((hypergraph->index->arraySize <= outNode) || (hypergraph->index->listHead[outNode] == -1))
			insertNode(hypergraph->index, outNode, hypergraph->buffer);
		if((hypergraph->index->arraySize <= inNode) || (hypergraph->index->listHead[inNode] == -1))
			insertNode(hypergraph->index, inNode, hypergraph->buffer);
		
		insertNeighbor(hypergraph->index, hypergraph->buffer, outNode, inNode);
		
	}
	
	return hypergraph;
	
}

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
		else
		{
			lastVisitedListNode[i] = getListHead(hypergraph->index, i);
			lastVisitedNeighbor[i] = -1;
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
					continue;
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
