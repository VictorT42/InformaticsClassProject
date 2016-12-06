//File list.h

#ifndef STRUCTS
#define STRUCTS

#include <stdint.h>
#include "hash.h"

#define YES 1
#define NO 0

#define N 32

typedef uint32_t ptr;

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

typedef struct queueNode
{
	int nodeID;
	struct queueNode *next;
	struct queueNode *previous;
} queueNode;

/* typedef struct queue
{
	queueNode *start;
	queueNode *end;
} queue; */

typedef struct queue
{
	int *array;
	int size;
	ptr start;	//first node
	ptr end;	//first empty space in array
} queue;

typedef struct pathNode
{
	int nodeID;
	struct pathNode *previous;
	struct pathNode *next;
} pathNode;

typedef struct BFSVisitedData
{
	int *fVisited;
	int *bVisited;
	int arraySize;
	int roundCounter;
} BFSVisitedData;

typedef struct HashTablesArray
{
	table **array;
	int arraySize;
} HashTablesArray;

#endif
