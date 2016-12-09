//File scc.c

typedef struct Component
{
	uint32_t component_id; //current component id
	uint32_t included_nodes_count; //number of nodes in component
	uint32_t* included_node_ids;
	//ids of included nodes
} Component;

typedef struct SCC
{
	Component* components; // Components index - a vector which stores
	the components information
	uint32_t components_count;
	uint32_t id_belongs_to_component[N]; //inverted index
} SCC;

struct GRAIL
{
	ptr *min_rank;
	ptr *rank;
} GRAIL;

SCC *estimateStronglyConnectedComponents(Buffer *outBuffer, NodeIndex* outIndex)
{
	
}

