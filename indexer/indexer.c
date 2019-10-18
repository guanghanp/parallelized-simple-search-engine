/* indexer.c --- 
 * 
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
	int count;
} word_t;

void init_word(word_t *wordp,char *wordc){
	wordp->word = wordc;
	wordp->count = 1;
}

bool searchWord(void *wordp, const void *wordc){
	word_t *wp = (word_t*) wordp;
  char *wc = (char*) wordc;
	return strcmp(wp->word,wc)==0;
}

void freeWords(void *wordp){
	word_t *wp = (word_t*) wordp;
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

void sumWords(void *wordp){
	word_t* wp = (word_t*)wordp;
	total += wp->count;
}

int main(void){
	hashtable_t *word_ht;
	word_ht=hopen(100);

	webpage_t *one;
	one = pageload(1,"../pages/");

	
	char* result;
	int pos=0;
	while ( (pos = webpage_getNextWord(one, pos, &result)) > 0) {
		if(NormalizeWord(result)!=NULL){
			printf("normalized result:%s\n",result);
			word_t *search_res;
			search_res=(word_t*)hsearch(word_ht,searchWord,result,strlen(result));
			if(search_res == NULL){
				search_res=(word_t*)malloc(sizeof(word_t));
				init_word(search_res,result);
				hput(word_ht,search_res,result,strlen(result));
			} else {
				search_res->count++;
			}
		}
		free(result);
	}
	happly(word_ht,sumWords);
	printf("total:%d\n",total);
		happly(word_ht,freeWords);
	hclose(word_ht);
}
