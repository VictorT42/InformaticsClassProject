//File index.h

#ifndef INDEX
#define INDEX

#include <stdlib.h>
#include <stdint.h>
#include "structs.h"
#include "buffer.h"
#include "hash.h"

#define INITIAL_INDEX_SIZE 3

NodeIndex *createNodeIndex();

OK_SUCCESS insertNode(NodeIndex *, ptr, Buffer *);

ptr getListHead(NodeIndex *, ptr);

ptr getListTail(NodeIndex *, ptr);

OK_SUCCESS destroyNodeIndex(NodeIndex *);

//TEMPORARY! TODO
HashTablesArray *createHashTablesArray();

OK_SUCCESS insertEdgeToHash(HashTablesArray *hashStruct, ptr outNode, ptr inNode);

void destroyHashTables(HashTablesArray *hashStruct);

#endif
