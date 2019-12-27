/* indexer.c --- crate and save an indexer from a directory of crawled webpages
 * 
 * Author: Guanghan Pan, Yu Chen
 * Created: Thu Oct 17 21:28:03 2019 (-0400)
 * 
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
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

// global variable used to debug
static int total = 0;

/* normalize the word by filtering out words which
 countain numbers, with length less than 3, and then
 convert to lower case.
*/ 
char *normalizeWord(char *word){
	if(strlen(word)<3)
		return NULL;
	for(int i=0; word[i]!= '\0'; i++){
		if(!isalpha(word[1]))
			return NULL;
		word[i] = tolower(word[i]);
	}
	return word;
}

// helper functions used to debug
void sumOne(void *docp){
	doc_t* dp = (doc_t*)docp;
	total += dp->count;
}

void sumWords(void *wordp){
	word_t* wp = (word_t*)wordp;
	qapply(wp->docq,sumOne);
}

// main method
int main(int argc, char *argv[]){

	// check the validity of pagedir
	char* pagedir = argv[1];
	struct stat buff;
	if (stat(pagedir,&buff) < 0 )
		exit(EXIT_FAILURE);
	
	if (!S_ISDIR(buff.st_mode))
		exit(EXIT_FAILURE);
	
	webpage_t *current;

	int file_count = 0;
	struct dirent *de;
  
	DIR *dr = opendir(pagedir);
  
	if (dr == NULL){
		printf("Could not open page directory" );
		exit(EXIT_FAILURE);
	}

	while ((de = readdir(dr)) != NULL)
		file_count++;
  
	closedir(dr);

	// create the hash table for indexer
	hashtable_t *word_ht;
	word_ht=hopen(97);
	
	// load crawled pages and fill out the indexer
	int id = 1;
	for (; id<=file_count-2; id++){
		current = pageload(id,pagedir);
		char* result;
		
		int pos=0;
		while ( (pos = webpage_getNextWord(current, pos, &result)) > 0) {

			// go through each word and normalize
			if(normalizeWord(result)!=NULL){
				// printf("normalized result:%s\n",result);
				word_t *search_res;
				search_res=(word_t*)hsearch(word_ht,searchWord,result,strlen(result));

				if(search_res == NULL){ // if the word is not in indexer, create a new word
					word_t *new_word = (word_t*)malloc(sizeof(word_t));
					doc_t* docp = (doc_t*)malloc(sizeof(doc_t));
					init_word(new_word,result,qopen());
					init_doc(docp,id,1);
					qput(new_word->docq,docp);
					hput(word_ht,new_word,result,strlen(result));
				} else { // if the word is already in the indexer
					doc_t *search_q;
					search_q = (doc_t*)qsearch(search_res->docq,searchDoc,&id);
					if(search_q == NULL){ // if the doc is not in the queue
						doc_t* docp = (doc_t*)malloc(sizeof(doc_t));
						init_doc(docp,id,1);
						qput(search_res->docq,docp);
					} else // if the doc is in the queue
						search_q->count++;
					
					free(result);
				}
			} else 
				free(result);
			
		}
		
		webpage_delete((void*)current);

	}

	// save the indexer to specified position
	indexsave(word_ht,argv[2]);

	// free all the data structures
	happly(word_ht,freeWords);
	hclose(word_ht);

	exit(EXIT_SUCCESS);
}
