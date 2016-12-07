//File print.h

#ifndef PRINT
#define PRINT

#include "bfs.h"

#define INDEX_ALLOCATION_FAIL 0
#define INDEX_REALLOCATION_FAIL 4
#define BUFFER_ALLOCATION_FAIL 2
#define BUFFER_REALLOCATION_FAIL 6
#define LIST_ALLOCATION_FAIL 3
#define QUEUE_ALLOCATION_FAIL 7
#define QUEUE_POP_FAIL 8
#define BFS_ARRAY_ALLOCATION_FAIL 9
#define NODE_ID_FAIL 1
#define INVALID_COMMAND_FAIL 5
#define EDGE_ADDITION_FAIL 10
#define HASH_TABLE_ALLOCATION_FAIL 11
#define HASH_TABLE_REALLOCATION_FAIL 12
#define BFS_VISITED_DATA_FAIL 13
#define QUEUE_OUT_OF_BOUNDS_FAIL 14

void printGraph(NodeIndex *index, Buffer *buffer);
void printPath(pathNode *start);
void printError(int);

#endif
