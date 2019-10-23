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
#include <sys/types.h>
#include <sys/stat.h>
#include <indexio.h>

static FILE *fp;

void init_word(word_t *wordp, char *word, queue_t* wordq){
	wordp->word = word;
	wordp->docq = wordq;
}

void init_doc(doc_t *dp, int id, int count){
	dp->document = id;
	dp->count = count;
}

void saveDoc(void* docp){
	doc_t *dp=(doc_t*) docp;
	fprintf(fp, " %d", dp->document);

	fprintf(fp," %d",dp->count);
}

void saveWord(void* wordp){
	word_t *wp = (word_t*) wordp;
	fprintf(fp,"%s" ,wp->word);
	qapply(wp->docq,saveDoc);
	//strcat(content,"\n");
	fprintf(fp,"\n");
}

void freeDocs(void *docp){
	doc_t *dp = (doc_t*)docp;
	free(dp);
}

void freeWords(void *wordp){
	word_t *wp = (word_t*) wordp;
	qapply(wp->docq,freeDocs);
	qclose(wp->docq);
	free(wp->word);
	free(wp);
}

bool searchWord(void *wordp, const void *wordc){
	word_t *wp = (word_t*) wordp;
  char *wc = (char*) wordc;
	return strcmp(wp->word,wc)==0;
}

bool searchDoc(void *docp, const void *id){
	doc_t *dp = (doc_t*) docp;
	int *doc_id = (int*) id;
	return dp->document == *doc_id;
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
int32_t indexsave(hashtable_t *indexp, char *indexnm){

	// open the file and write the contents

	fp = fopen(indexnm,"w");
	happly(indexp,saveWord);

	fclose(fp);
	return 0;
}

/* 
 * pageload -- loads the numbered filename <id> in direcory <dirnm>
 * into a new webpage
 *
 * returns: non-NULL for success; NULL otherwise
 */
hashtable_t *indexload(char *indexnm){
	FILE *fp = NULL;
	hashtable_t *index=NULL;
	
	if((fp = fopen(indexnm,"r"))==NULL)
		return NULL;
	
	struct stat buf;
	stat(indexnm, &buf);
	if (buf.st_mode & S_IRUSR){
		index = hopen(97);
		char buffer[1000];
			
		while (fgets(buffer,sizeof(buffer),fp) != NULL){
			queue_t *wordq = qopen();
			word_t *wordp = (word_t*)malloc(sizeof(word_t));
			// extract first word
			char tmp[30];
			sscanf(buffer,"%s",tmp);
			char *word = (char*)malloc(strlen(tmp)+1);
			strcpy(word,tmp);

			int res = 0;
			int id, count;
		
			int i = strlen(word);
		
			while ((res = sscanf(buffer+i,"%d %d",&id,&count)) == 2 ){
				char temp[10];
				sprintf(temp," %d %d",id,count);
				i += strlen(temp);

				doc_t* docp = (doc_t*)malloc(sizeof(doc_t));
			
				init_doc(docp,id,count);
				qput(wordq,(void*)docp);
			}
		
			init_word(wordp,word,wordq);
			hput(index,wordp,word,strlen(word));
		}
	}
	
	fclose(fp);
	return index;

}
