//File list.h

#ifndef STRUCTS
#define STRUCTS

#include <stdint.h>

#define YES 1
#define NO 0

#define N 32

typedef uint32_t ptr;
typedef int OK_SUCCESS;

typedef struct list_node
{
	ptr neighbor[N]; //the ids of the neighbor nodes
	//ptr edgeProperty[N]; property for each edge
	ptr nextListNode;
	int neighborCounter;
} node;

typedef struct NodeIndex
{
	ptr *listHead;
	ptr *listTail;
	int arraySize;
} NodeIndex;

typedef struct Buffer
{
	unsigned int nodeCounter;
	unsigned int maxNodes;
	node *list;
} Buffer;

typedef struct queue
{
	int *array;
	int size;
	ptr start;	//first node
	ptr end;	//first empty space in array
} queue;

typedef struct BFSVisitedData
{
	int *fVisited;
	int *bVisited;
	int arraySize;
	int roundCounter;
} BFSVisitedData;

typedef struct HashTable table;

typedef struct HashTablesArray
{
	table **array;
	int arraySize;
} HashTablesArray;

typedef int UpdateIndex;

typedef struct CC
{
	ptr *ccIndex; //CCIndex
	int ccIndexSize;
	int componentsNumber;
	
	int queries;
	int updateQueries;
	
	UpdateIndex *updateIndex;
} CC;

typedef struct stack
{
	int *array;
	int size;
	int end;
} stack;

#endif
