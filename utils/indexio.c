/* 
 * indexio.c --- saving and loading index 
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


static char content[2000000];

typedef struct word_t{
	char* word;
	queue_t* docq;
} word_t;

typedef struct doc_t{
	int document;
	int count;
} doc_t;

void saveDoc(void* docp){
	doc_t *dp=(doc_t*) docp;
	strcat(content," ");
	char doc[10];
	sprintf(doc,"%d",dp->document);
	strcat(content,doc);
	strcat(content," ");
	char count[10];
	sprintf(count,"%d",dp->count);
	strcat(content,count);
}

void saveWord(void* wordp){
	word_t *wp = (word_t*) wordp;
	strcat(content,wp->word);
	qapply(wp->docq,saveDoc);
	strcat(content,"\n");
}

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
int32_t indexsave(hashtable_t *indexp, char *indexnm, char *dirnm){
	FILE *fp;
	char filename[20];
	content[0]='\0';
	happly(indexp,saveWord);
	// create file name using dirname
	sprintf(filename,"%s%s",dirnm,indexnm);
	// open the file and write the contents
	fp = fopen(filename,"w");
	fprintf(fp,"%s",content);
	fclose(fp);
	return 0;
}

/* 
 * pageload -- loads the numbered filename <id> in direcory <dirnm>
 * into a new webpage
 *
 * returns: non-NULL for success; NULL otherwise
 */
// hashtable_t *indexload(char *indexnm, char *dirnm);
