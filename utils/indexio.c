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

static char content[2000000];

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
hashtable_t *indexload(char *indexnm, char *dirnm){
	FILE *fp;
	char filename[20];
	hashtable_t *index=NULL;

	
	sprintf(filename,"%s%s",dirnm,indexnm);
	if((fp = fopen(filename,"r"))==NULL)
		return NULL;
	
	struct stat buffer;
	stat(filename, &buffer);
	if (buffer.st_mode & S_IRUSR){
		index = hopen(97);
		char buffer[1000];
		while (fgets(buffer,sizeof(buffer),fp) != NULL){
			queue_t *wordq = qopen();
			word_t *wordp = (word_t*)malloc(sizeof(word_t));
			// extract first word
			char *pch = (char*)malloc(30);
			pch = strtok (buffer," ,.-");
			char *word = (char*)malloc(strlen(pch));
			strcpy(word,pch);
			
			pch = strtok (NULL, " ,.-");
			while (pch != NULL){
				doc_t* docp = (doc_t*)malloc(sizeof(doc_t));
				int id = atoi(pch);
				pch = strtok (NULL, " ,.-");
				int count = atoi(pch);
				pch = strtok (NULL, " ,.-");
				init_doc(docp,id,count);
				qput(wordq,(void*)docp);
			}
			free(pch);
			init_word(wordp,word,wordq);
			hput(index,wordp,word,strlen(word));
		}
	}
	
	fclose(fp);
	return index;

}
