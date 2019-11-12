#pragma once
/*
 * lhash.h -- A generic lock hash table implementation, allowing arbitrary
 * key structures.
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <queue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hash.h>

typedef void lhashtable_t;	/* representation of a hashtable hidden */

/* hopen -- opens a hash table with initial size hsize */
lhashtable_t *lhopen(uint32_t hsize);

/* hclose -- closes a hash table */
void lhclose(lhashtable_t *lhtp);

/* hput -- puts an entry into a hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t lhput(hashtable_t *lhtp, void *ep, const char *key, int keylen);

/* happly -- applies a function to every entry in hash table */
void lhapply(hashtable_t *lhtp, void (*fn)(void* ep));

/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or NULL if
 * not found
 */
void *lhsearch(lhashtable_t *lhtp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen);

/* hremove -- removes and returns an entry under a designated key
 * using a designated search fn -- returns a pointer to the entry or
 * NULL if not found
 */
void *lhremove(lhashtable_t *lhtp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen);

