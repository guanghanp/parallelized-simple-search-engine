/* 
 * hash.c -- implements a generic hash table as an indexed set of queues.
 * Author: Samira Afreen, Yu Chen, Guanghan Pan
 */
#include <stdint.h>
#include <stdbool.h>
#include <lhash.h>
#include <pthread.h>

typedef struct lhash_t{
	hashtable_t* ht;
	pthread_mutex_t m;
}lhash_t;

lhashtable_t *lhopen(uint32_t hsize){
	lhash_t* lht;
	if(!(lht = (lhash_t*)malloc(sizeof(lhash_t)))){
		printf("Malloc Failure\n");
	}
	lht->ht = hopen(hsize);
	pthread_mutex_init(&(lht->m),NULL);
	return (lhashtable_t*)lht;
}

void lhclose(lhashtable_t *lhtp){
	lhash_t* lht = (lhash_t*)lhtp;
	hclose(lht->ht);
	pthread_mutex_destroy(&(lht->m));
	free(lht);
}

int32_t lhput(lhashtable_t *lhtp, void *ep, const char *key, int keylen){
	lhash_t* lht = (lhash_t*)lhtp;
	int32_t output;
	pthread_mutex_lock(&(lht->m));
	output = hput(lht->ht,ep,key,keylen);
	pthread_mutex_unlock(&(lht->m));
	return output;	
}

void lhapply(lhashtable_t *lhtp, void (*fn)(void* ep)){
	lhash_t* lht = (lhash_t*)lhtp;
	pthread_mutex_lock(&(lht->m));
	happly(lht->ht,fn);
	pthread_mutex_unlock(&(lht->m));
}

void *lhsearch(lhashtable_t *lhtp, 
							bool (*searchfn)(void* elementp, const void* searchkeyp), 
							const char *key, 
							int32_t keylen){
	lhash_t* lht = (lhash_t*)lhtp;
	void* output;
	pthread_mutex_lock(&(lht->m));
	output = hsearch(lht->ht,searchfn,key,keylen);
	pthread_mutex_unlock(&(lht->m));
	return output;
}

void *lhremove(lhashtable_t *lhtp, 
							bool (*searchfn)(void* elementp, const void* searchkeyp), 
							const char *key, 
							int32_t keylen){
	lhash_t* lht = (lhash_t*)lhtp;
	void* output;
	pthread_mutex_lock(&(lht->m));
	output = hremove(lht->ht,searchfn,key,keylen);
	pthread_mutex_unlock(&(lht->m));
	return output;
}



