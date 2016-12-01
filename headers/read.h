//File read.h

#ifndef READ
#define READ

#include "structs.h"
#include "buffer.h"
#include "index.h"

void readInput(Buffer*, Buffer*, NodeIndex*, NodeIndex*, HashTablesArray*);
void add(Buffer*, Buffer*, NodeIndex*, NodeIndex*, HashTablesArray*, int, int);

#endif
