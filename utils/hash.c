/* 
 * hash.c -- implements a generic hash table as an indexed set of queues.
 * Author: Samira Afreen, Yu Chen, Guanghan Pan
 */
#include <stdint.h>
#include <stdbool.h>
#include <hash.h>
#include <queue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* 
 * SuperFastHash() -- produces a number between 0 and the tablesize-1.
 * 
 * The following (rather complicated) code, has been taken from Paul
 * Hsieh's website under the terms of the BSD license. It's a hash
 * function used all over the place nowadays, including Google Sparse
 * Hash.
 */
#define get16bits(d) (*((const uint16_t *) (d)))

static uint32_t SuperFastHash (const char *data,int len,uint32_t tablesize) {
  uint32_t hash = len, tmp;
  int rem;
  
  if (len <= 0 || data == NULL)
		return 0;
  rem = len & 3;
  len >>= 2;
  /* Main loop */
  for (;len > 0; len--) {
    hash  += get16bits (data);
    tmp    = (get16bits (data+2) << 11) ^ hash;
    hash   = (hash << 16) ^ tmp;
    data  += 2*sizeof (uint16_t);
    hash  += hash >> 11;
  }
  /* Handle end cases */
  switch (rem) {
  case 3: hash += get16bits (data);
    hash ^= hash << 16;
    hash ^= data[sizeof (uint16_t)] << 18;
    hash += hash >> 11;
    break;
  case 2: hash += get16bits (data);
    hash ^= hash << 11;
    hash += hash >> 17;
    break;
  case 1: hash += *data;
    hash ^= hash << 10;
    hash += hash >> 1;
  }
  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;
  return hash % tablesize;
}

typedef struct hash_t{
	int size;
	queue_t** q;
}hash_t;

hashtable_t *hopen(uint32_t hsize){
	hash_t* ht;
	if(!(ht = (hash_t*)malloc(sizeof(hash_t)))){
		printf("Malloc Failure\n");
	}
	
	if(!(ht->q = (queue_t**)malloc(hsize * sizeof(queue_t*)))){
		printf("Malloc Failure\n");
	}
	
	ht->size = hsize;
	for(int i=0;i<hsize;i++)
		ht->q[i] = qopen();
	return (hashtable_t*)ht;
}

void hclose(hashtable_t *htp){
	hash_t* ht = (hash_t*)htp;
	for(int i = 0;i < ht->size;i++)
		qclose(ht->q[i]);
	free(ht->q);
	free(ht);
}

int32_t hput(hashtable_t *htp, void *ep, const char *key, int keylen){
	hash_t* ht = (hash_t*)htp;
	uint32_t number = SuperFastHash(key,keylen,ht->size);
	
	if(qput(ht->q[number],ep)==0)
		return 0;
	else
		return 1;
}

void happly(hashtable_t *htp, void (*fn)(void* ep)){
	hash_t* ht = (hash_t*)htp;

	for(int i=0;i<ht->size ;i++)
		qapply(ht->q[i],fn);
}

void *hsearch(hashtable_t *htp, 
							bool (*searchfn)(void* elementp, const void* searchkeyp), 
							const char *key, 
							int32_t keylen){
	hash_t* ht = (hash_t*)htp;
	uint32_t number = SuperFastHash(key,keylen,ht->size);
	return qsearch(ht->q[number],searchfn,(void*)key); 
}

void *hremove(hashtable_t *htp, 
							bool (*searchfn)(void* elementp, const void* searchkeyp), 
							const char *key, 
							int32_t keylen){
	hash_t* ht = (hash_t*)htp;
	uint32_t number = SuperFastHash(key,keylen,ht->size);
	return qremove(ht->q[number],searchfn,(void*)key); /* NOT SURE ABOUT THIS */
}



