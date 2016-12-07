//File hash.c

#include "../../headers/hash.h"
#include <stdio.h>
#include <stdlib.h>

//Added for this project...
///////////////////////////
HashTablesArray *createHashTablesArray()
{

	HashTablesArray *hashStruct;
	int i;

	hashStruct = malloc(sizeof(HashTablesArray));

	if(hashStruct == NULL)
	{
		printError(HASH_TABLE_ALLOCATION_FAIL);
		return NULL;
	}

	hashStruct->array = malloc(INITIAL_HASH_TABLE_ARRAY_SIZE*sizeof(table*));

	if(hashStruct == NULL)
	{
		printError(HASH_TABLE_ALLOCATION_FAIL);
		return NULL;
	}

	hashStruct->arraySize = INITIAL_HASH_TABLE_ARRAY_SIZE;

	for(i = 0; i < INITIAL_HASH_TABLE_ARRAY_SIZE; i++)
	{
		hashStruct->array[i] = NULL;
	}

	return hashStruct;

}

OK_SUCCESS insertNodeToHash(HashTablesArray *hashStruct, ptr node)
{

	int expansionFlag = 0;
	int previousArraySize;
	int i;
	
	previousArraySize = hashStruct->arraySize;
	while((node >= hashStruct->arraySize))
	{
		expansionFlag = 1;
		hashStruct->arraySize = hashStruct->arraySize*2;
	}
	
	if(expansionFlag == 1)
	{
	
		hashStruct->array = realloc(hashStruct->array, hashStruct->arraySize*sizeof(table*));
		
		if(hashStruct->array == NULL)
		{
			printError(HASH_TABLE_REALLOCATION_FAIL);
			return NO;
		}
		
		for(i = previousArraySize; i < hashStruct->arraySize; i++)
		{
			hashStruct->array[i] = NULL;
		}

	}

	if(hashStruct->array[node] == NULL)
	{
		hashStruct->array[node] = initializeHashTable(hashStruct->array[node], HASH_TABLE_BUCKET_ENTRIES, HASH_TABLE_LOAD_FACTOR, INITIAL_HASH_TABLE_SIZE);
	}

	return YES;

}

void destroyHashTables(HashTablesArray *hashStruct)
{
	int i;

	for(i = 0; i < hashStruct->arraySize; i++)
	{
		deleteHash(hashStruct->array[i]);
	}
	
	free(hashStruct->array);

	free(hashStruct);

}
///////////////
//...up to here

table *initializeHashTable(table *ht, int be, float lf, int s)
{
	int i = 0;
	
	ht = NULL;
	ht = malloc(sizeof(table));
	if(ht == NULL)
	{
		printf("Memory allocation failed; unable to make Hash Table. Operation cancelled.\n");
		return ht;
	}

	ht->bucketentries = be;
	ht->loadfactor = lf;
	ht->firstsize = s;
	ht->size = s;
	ht->round = 0;
	ht->p = 0;
	ht->numofentries = 0;
	
	//Create the dynamic index of buckets.
	ht->buckets = NULL;
	ht->buckets = malloc(s*sizeof(bucket *));
	if(ht->buckets == NULL)
	{
		printf("Memory allocation failed; unable to make Hash Table. Operation cancelled.\n");
		free(ht);
		return NULL;
	}
	
	//Create the buckets.
	for(i=0; i<s; i++)
	{
		(ht->buckets)[i] = NULL;
		(ht->buckets)[i] = addBucketToTable((ht->buckets)[i], be);
		if(ht->buckets[i] == NULL)   //If a bucket failed to be created, destroy the rest of the Table.
		{
			printf("Memory allocation failed; unable to make Hash Table. Operation cancelled.\n");
			for(; i>=0; i--)
				free(ht->buckets[i]);
			free(ht->buckets);
			free(ht);
			return NULL;
		}
	}

	return ht;
}

bucket *addBucketToTable(bucket *b, int be)
{
	int i = 0;
	
	b = malloc(sizeof(bucket));
	if (b == NULL)
		return NULL;
	
	b->entries = NULL;
	b->entries = malloc(be*sizeof(entry *));
	if(b->entries == NULL)
	{
		free(b);
		return NULL;
	}
	
	for(i=0; i<be; i++)
		(b->entries)[i] = NULL;   //The buckets start empty.
	
	b->overflow = NULL;
	
	return b;
}

entry *addEntryToBucket(bucket *b, entry *e, int be)
{

	int i = 0;
	
	for(i=0; i<be; i++)
		if(b->entries[i] == NULL)
		{
			b->entries[i] = e;
			return e;
		}
	
	if(b->overflow != NULL)
	{
		e = addEntryToBucket(b->overflow, e, be);
		return e;
	}
	
	b->overflow = addBucketToTable(b->overflow, be);
	e = addEntryToBucket(b->overflow, e, be);
	
	return e;
}

entry *addEntryToTable(table *ht, void *d, long key)
{
	int tablepos = 0;
	int i = 0;
	int twicetheprevious = ht->firstsize;
	entry *e;
	
	e = malloc(sizeof(entry));
	
	e->key = key;
	e->data = d;
	
	tablepos = hash(e->key, ht->round, ht->firstsize);
	if(tablepos < (ht->p))
		tablepos = hash(e->key, (ht->round)+1, ht->firstsize);
	
	e = addEntryToBucket(ht->buckets[tablepos], e, ht->bucketentries);
	
	(ht->numofentries)++;
	
	if((float) ht->numofentries / ((ht->size)*(ht->bucketentries)) > ht->loadfactor)
	{
		split(ht);
		
		for(i=0; i<=(ht->round); i++)
			twicetheprevious *= 2;
		
		if(ht->size == twicetheprevious)
		{
			(ht->round)++;
			ht->p = 0;
		}
	}
	
	return e;
}

void split(table *ht)
{
	int i = 0;
	bucket *pbucket = (ht->buckets)[ht->p];
	bucket *poverflow = pbucket->overflow;
	entry *current = NULL;
	
	(ht->size)++;
	ht->buckets = realloc(ht->buckets, (ht->size)*sizeof(bucket *));
	ht->buckets[(ht->size)-1] = addBucketToTable(ht->buckets[(ht->size)-1], ht->bucketentries);
	
	for(i=0; i<(ht->bucketentries); i++)
	{
		current = pbucket->entries[i];
		if(current == NULL)
			break;
		
		pbucket->entries[i] = NULL;   //Remove the entry from its current position in the Hash Table.
		if(hash(current->key, (ht->round)+1, ht->firstsize) == ht->p)   //Its new position will either be in the same bucket (if possibly in a different entry) or in the new bucket.
			addEntryToBucket(pbucket, current, ht->bucketentries);
		else
			addEntryToBucket(ht->buckets[(ht->size)-1], current, ht->bucketentries);
	}
	
	if(poverflow != NULL)
	{
		pbucket->overflow = NULL;
		splitOverflow(poverflow, pbucket, ht);   //Overflow buckets are split recursively, deleting themselves as they are emptied.
	}
	
	(ht->p)++;
	
	return;
}

void splitOverflow(bucket *poverflow, bucket *pbucket, table *ht)
{
	int i = 0;
	entry *current = NULL;
	
	for(i=0; i<(ht->bucketentries); i++)
	{
		current = poverflow->entries[i];
		if(current == NULL)
			break;
		
		if(hash(current->key, (ht->round)+1, ht->firstsize) == ht->p)
			addEntryToBucket(pbucket, current, ht->bucketentries);
		else
			addEntryToBucket(ht->buckets[(ht->size)-1], current, ht->bucketentries);
	}
	
	if(poverflow->overflow != NULL)
		splitOverflow(poverflow->overflow, pbucket, ht);
	
	free(poverflow->entries);
	free(poverflow);
	return;
}

int hash(long key, int round, int size)
{
	for(; round>0; round--)
		size *= 2;
	return key%size;
}

entry *findEntry(long key, table *ht)
{
	int i = 0;
	int tablepos = 0;
	bucket *curbucket = NULL;
	entry *e = NULL;
	
	if(ht == NULL)
		return NULL;
	
	tablepos = hash(key, ht->round, ht->firstsize);
	if(tablepos < (ht->p))
		tablepos = hash(key, (ht->round)+1, ht->firstsize);
	
	curbucket = ht->buckets[tablepos];
	e = curbucket->entries[i];
	
	while(e != NULL)
	{
		if(e->key == key) return e;
		
		i++;
		if(i == ht->bucketentries)   //If the entries have all been checked.
		{
			if((curbucket->overflow) != NULL)
			{
				i = 0;
				curbucket = curbucket->overflow;   //Start again with the overflow bucket, if it exists.
			}
			else
				break;
		}
		
		e = curbucket->entries[i];
	}
	
	return NULL;
}

void deleteHash(table *ht)
{
	if(ht == NULL)
		return;
	int i = 0;
	for(i=0; i<(ht->size); i++)
		deleteBucket(ht->buckets[i], ht->bucketentries);
	free(ht->buckets);
	free(ht);
}

void deleteBucket(bucket *b, int be)
{
	int i;
	
	if(b == NULL)
		return;
	if(b->overflow != NULL)
		deleteBucket(b->overflow, be);
	
	for(i=0; i<be; i++)
		free(b->entries[i]);
	
	free(b->entries);
	
	free(b);
	return;
}

void printHash(table *ht)   //Not required or used by the program but helped with the debugging.
{
	int i = 0, j;
	
	printf("Round %d, Size %d:\n", ht->round, ht->size);
	for(i=0; i<(ht->size); i++)
	{
		if(i == ht->p)
			printf("p -> %d |",i);
		else
			printf("     %d |",i);
		
		bucket *curbucket = (ht->buckets)[i];
		
		j = 0;
		while((curbucket->entries)[j] != NULL)
		{
			if(curbucket->entries[j]->key < 10)
				putchar(' ');
			printf(" %ld |", curbucket->entries[j]->key);
			if(j == (ht->bucketentries)-1 && curbucket->overflow)
			{
				printf(" ->");
				curbucket = curbucket->overflow;
				j = -1;
			}
			j++;
			if(j == ht->bucketentries)
				break;
		}
		putchar('\n');
	}
}


