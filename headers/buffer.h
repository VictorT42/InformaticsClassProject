//File buffer.h

#ifndef BUFFER
#define BUFFER

#include <stdlib.h>
#include <stdint.h>
#include "structs.h"
#include "index.h"

#define INITIAL_BUFFER_SIZE 3

typedef int OK_SUCCESS;

Buffer *createBuffer();

ptr allocNewNode(Buffer*);

OK_SUCCESS insertNeighbor(NodeIndex*, Buffer*, int, int);

struct list_node *getListNode(Buffer*, ptr);

OK_SUCCESS destroyBuffer(Buffer*);

#endif
