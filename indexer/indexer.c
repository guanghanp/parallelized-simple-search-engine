/* indexer.c --- 
1;95;0c * 
 * 
 * Author: Guanghan Pan
 * Created: Thu Oct 17 21:28:03 2019 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <webpage.h>
#include <hash.h>
#include <queue.h>
#include <pageio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>

static int total = 0;

typedef struct word_t{
	char* word;
	queue_t* docq;
} word_t;

void init_word(word_t *wordp, char *word){
	wordp->word = word;
	wordp->docq = qopen();
}

typedef struct doc_t{
	int document;
	int count;
} doc_t;


void init_doc(doc_t *dp, int id){
	dp->document = id;
	dp->count = 1;
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

char *NormalizeWord(char *word){
	if(strlen(word)<3)
		return NULL;
	for(int i=0; word[i]!= '\0'; i++){
		if(!isalpha(word[1]))
			return NULL;
		word[i] = tolower(word[i]);
	}
	return word;
}

void sumOne(void *docp){
	doc_t* dp = (doc_t*)docp;
	total += dp->count;
}

void sumWords(void *wordp){
	word_t* wp = (word_t*)wordp;
	qapply(wp->docq,sumOne);
}

int main(int argc, char *argv[]){
	hashtable_t *word_ht;
	word_ht=hopen(97);

	webpage_t *one;
	int max_id = atoi(argv[1]);
	int id = 1;
	for (; id<=max_id; id++){
		one = pageload(id,"../pages/");

		char* result;
		int pos=0;
		while ( (pos = webpage_getNextWord(one, pos, &result)) > 0) {
			if(NormalizeWord(result)!=NULL){
				// printf("normalized result:%s\n",result);
				word_t *search_res;
				search_res=(word_t*)hsearch(word_ht,searchWord,result,strlen(result));
				if(search_res == NULL){
					word_t *new_word = (word_t*)malloc(sizeof(word_t));
					doc_t* docp = (doc_t*)malloc(sizeof(doc_t)); 
					init_word(new_word,result);
					init_doc(docp,id);
					qput(new_word->docq,docp);
					hput(word_ht,new_word,result,strlen(result));
				} else {
					doc_t *search_q;
					search_q = (doc_t*)qsearch(search_res->docq,searchDoc,&id);
					if(search_q == NULL){
						doc_t* docp = (doc_t*)malloc(sizeof(doc_t));
						init_doc(docp,id);
						qput(search_res->docq,docp);
					}
					else{
						search_q->count++;
					}
					free(result);
				}
			} else {
				free(result);
			}
			// free(result); // ?????
		}
	}
	happly(word_ht,sumWords);
	printf("total:%d\n",total);
	
	happly(word_ht,freeWords);
	hclose(word_ht);
}
