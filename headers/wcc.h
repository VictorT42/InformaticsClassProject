//File wcc.h

#ifndef WCC
#define WCC

#include "structs.h"

#define INITIAL_STACK_SIZE 1000000
#define INITIAL_CONNECTION_ARRAY_SIZE 32
#define UNDISCOVERED -1
#define CC_METRIC 0.001

void initializeStack(stack *s);
void stackPush(stack *s, int nodeID);
int stackPop(stack *s);
void deleteStack(stack *s);

CC *createCC(int size);
CC *estimateConnectedComponents(Buffer *outBuffer, NodeIndex *outIndex, Buffer *inBuffer, NodeIndex *inIndex);

OK_SUCCESS insertNewEdge(CC* components, ptr outNode, ptr inNode);
OK_SUCCESS rebuildIndexes(CC* components);

OK_SUCCESS destroyConnectedComponents(CC *components);

#endif
