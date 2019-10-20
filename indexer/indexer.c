/* indexer.c --- 
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
#include <indexio.h>
#include <ctype.h>
#include <stdbool.h>

static int total = 0;

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

	webpage_t *current;
	int max_id = atoi(argv[1]);
	int id = 1;
	for (; id<=max_id; id++){
		current = pageload(id,"../pages/");

		char* result;
		int pos=0;
		while ( (pos = webpage_getNextWord(current, pos, &result)) > 0) {
			if(NormalizeWord(result)!=NULL){
				// printf("normalized result:%s\n",result);
				word_t *search_res;
				search_res=(word_t*)hsearch(word_ht,searchWord,result,strlen(result));
				if(search_res == NULL){
					word_t *new_word = (word_t*)malloc(sizeof(word_t));
					doc_t* docp = (doc_t*)malloc(sizeof(doc_t)); 
					init_word(new_word,result,qopen());
					init_doc(docp,id,1);
					qput(new_word->docq,docp);
					hput(word_ht,new_word,result,strlen(result));
				} else {
					doc_t *search_q;
					search_q = (doc_t*)qsearch(search_res->docq,searchDoc,&id);
					if(search_q == NULL){
						doc_t* docp = (doc_t*)malloc(sizeof(doc_t));
						init_doc(docp,id,1);
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
		webpage_delete((void*)current);
	}
	
	happly(word_ht,sumWords);
	printf("total:%d\n",total);
	indexsave(word_ht,"result1","../pages/");
	happly(word_ht,freeWords);
	hclose(word_ht);
}
