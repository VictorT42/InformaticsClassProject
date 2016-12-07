//File index.h

#ifndef INDEX
#define INDEX

#include <stdlib.h>
#include <stdint.h>
#include "structs.h"
#include "buffer.h"

#define INITIAL_INDEX_SIZE 65536

NodeIndex *createNodeIndex();

OK_SUCCESS insertNode(NodeIndex *, ptr, Buffer *);

ptr getListHead(NodeIndex *, ptr);

ptr getListTail(NodeIndex *, ptr);

OK_SUCCESS destroyNodeIndex(NodeIndex *);

#endif
