//File scc.h

#ifndef SCC_HDR
#define SCC_HDR

#include "index.h"
#include "buffer.h"

#define EMPTY -1
#define UNDEFINED -1
#define CONNECTED -2

typedef int GRAIL_ANSWER;
#define MAYBE 2

#define INITIAL_CC_ARRAY_SIZE 16
#define INITIAL_HYPERGRAPH_ARRAY_SIZE 32
#define INITIAL_SCC_SIZE 32

typedef struct Component
{
	uint32_t component_id; //current component id
	uint32_t included_nodes_count; //number of nodes in component
	uint32_t *included_node_ids;  //ids of included nodes
	int arraySize;
} Component;

typedef struct SCC
{
	Component **components; // Components index - a vector which stores the components information
	uint32_t components_count;
	uint32_t *id_belongs_to_component; //inverted index
	int arraySize;
} SCC;

typedef struct GrailIndex
{
	ptr *minRank;
	ptr *rank;
} GrailIndex;

typedef struct HypergraphEdges
{
	int *outNode;
	int *inNode;
	int arraySize;
	int edgeCount;
} HypergraphEdges;

typedef struct Hypergraph
{
	Buffer *buffer;
	NodeIndex *index;
} Hypergraph;


typedef struct ComponentCursor
{
	Component* component_ptr;
	int counter;
}ComponentCursor;

int estimateShortestPathStronglyConnectedComponents(NodeIndex *, Buffer *, NodeIndex *, Buffer *, int, int, BFSVisitedData *, queue *, queue *, SCC *);
OK_SUCCESS next_StronglyConnectedComponentID(SCC* components, ComponentCursor* cursor);
OK_SUCCESS iterateStronglyConnectedComponentID(SCC* components, ComponentCursor* cursor);
void createComponent(SCC *components, int *index, stack *tarjanStack, ptr root);
void deleteSCC(SCC *components);
OK_SUCCESS createSCC(SCC *components, int size);
SCC *estimateStronglyConnectedComponents(Buffer *outBuffer, NodeIndex* outIndex, HypergraphEdges *);
void addEdgeToHypergraph(HypergraphEdges *hypergraphEdges, ptr outNode, ptr inNode);
void createHypergraphEdges(HypergraphEdges *hypergraphEdges);
Hypergraph *buildHypergraph(HypergraphEdges *hypergraphEdges);
GrailIndex *buildGrailIndex(Hypergraph *hypergraph, SCC* components);
GRAIL_ANSWER isReachableGrailIndex(GrailIndex* grail, uint32_t source_node, uint32_t target_node);
OK_SUCCESS destroyGrailIndex(GrailIndex* index);

#endif