#pragma once
/* 
 * indexio.h --- saving and loading index 
 * 
 * Description: pagesave saves an existing webpage to a file with a
 * numbered name (e.g. 1,2,3 etc); pageload creates a new page by
 * loading a numbered file. For pagesave, the directory must exist and
 * be writable; for loadpage it must be readable.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <hash.h>
#include <queue.h>

typedef struct word_t{
	char* word;
	queue_t* docq;
} word_t;

typedef struct doc_t{
	int document;
	int count;
} doc_t;

void init_word(word_t *wordp, char *word, queue_t* wordq);

void init_doc(doc_t *dp, int id, int count);

bool searchWord(void *wordp, const void *wordc);

bool searchDoc(void *docp, const void *id);


void freeDocs(void *docp);

void freeWords(void *wordp);

/*
 * pagesave -- save the page in filename id in directory dirnm
 *
 * returns: 0 for success; nonzero otherwise
 *
 * The suggested format for the file is:
 *   <url>
 *   <depth>
 *   <html-length>
 *   <html>
 */
int32_t indexsave(hashtable_t *indexp, char *indexnm, char *dirnm);

/* 
 * pageload -- loads the numbered filename <id> in direcory <dirnm>
 * into a new webpage
 *
 * returns: non-NULL for success; NULL otherwise
 */
hashtable_t *indexload(char *indexnm, char *dirnm);
