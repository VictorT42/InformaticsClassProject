//File bfs.h

#ifndef BFS
#define BFS

#include "structs.h"
#include "index.h"
#include "buffer.h"

#define UNVISITED -1
#define END_NODE -2

void push(queue *q, int nodeID);
int pop(queue *q);

pathNode *bBFS(NodeIndex *outIndex, Buffer *outBuffer, NodeIndex *inIndex, Buffer *inBuffer, int start, int goal);
pathNode *pathfinder(int *forwardParentArray, int *backwardParentArray, int midpoint);
int pathLength(pathNode *start);
void deleteBFS(int *forwardParentArray, int *backwardParentArray, queue *forwardQueue, queue *backwardQueue);

#endif
