//File main.c

#include <stdio.h>
#include "../../headers/structs.h"
#include "../../headers/index.h"
#include "../../headers/buffer.h"
#include "../../headers/read.h"
#include "../../headers/hash.h"

int main(int argc, char *argv[])
{

	Buffer *outBuffer;
	Buffer *inBuffer;
	NodeIndex *inIndex;
	NodeIndex *outIndex;
	HashTablesArray *hashStruct;
	
	outBuffer=createBuffer();
	inBuffer=createBuffer();
	outIndex=createNodeIndex();
	inIndex=createNodeIndex();
	hashStruct = createHashTablesArray();
	
	readInput(outBuffer, inBuffer, outIndex, inIndex, hashStruct);
	
	destroyBuffer(outBuffer);
	destroyBuffer(inBuffer);
	destroyNodeIndex(outIndex);
	destroyNodeIndex(inIndex);
	destroyHashTables(hashStruct);
	
	return 0;
	
}





