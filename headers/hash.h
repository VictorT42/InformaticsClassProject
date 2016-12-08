//File hash.h

#ifndef HASH
#define HASH

#include <stdio.h>

//Added for this project...
///////////////////////////
#include "structs.h"
#include "print.h"

#define HASH_TABLE_BUCKET_ENTRIES 16
#define HASH_TABLE_LOAD_FACTOR 0.75
#define INITIAL_HASH_TABLE_SIZE 2
#define INITIAL_HASH_TABLE_ARRAY_SIZE 65536

HashTablesArray *createHashTablesArray();

OK_SUCCESS insertNodeToHash(HashTablesArray *hashStruct, ptr outNode);

void destroyHashTables(HashTablesArray *hashStruct);
///////////////
//...up to here

typedef struct HashTableEntry
{
	long key;
	void *data;
} entry;

typedef struct HashTableBucket
{
	entry **entries;
	struct HashTableBucket *overflow;
	
} bucket;

typedef struct HashTable
{
	int bucketentries;
	float loadfactor;
	int round, p, firstsize, size, numofentries;
	bucket **buckets;
	
} table;

int hash(long, int, int);

table *initializeHashTable(table *, int, float, int);
bucket *addBucketToTable(bucket *, int);
entry *addEntryToTable(table *, void *, long);
entry *addEntryToBucket(bucket *, entry *, int);

void split(table *);
void splitOverflow(bucket *, bucket *, table *);

entry *findEntry(long, table *);

void deleteHash(table *);
void deleteBucket(bucket *, int);

void query(long, table *);

void printHash(table *);

#endif


