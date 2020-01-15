/* crawler.c --- web crawler
 *
 * Author: Guanghan Pan
 * 
 */


#include <stdio.h>
#include <webpage.h>
#include <queue.h>
#include <hash.h>
#include <pageio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>

// helper function used to free hashtable content
void free_content(void* element){
	char* cp = (char*)element;
	free(cp);
}

// search whether url is in the hashtable
bool searchurl(void* urlp, const void* urlkey){
	char* up = (char*)urlp;
	char* key = (char*)urlkey;
	return strcmp(up,key)==0;
}

// main method
int main(int argc,char *argv[]) {

	// check number of arguments
	if(argc != 4){
		printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
		exit(EXIT_FAILURE);
	}
	
	char* seedurl = argv[1];
	char* pagedir = argv[2];
	int maxdepth = atoi(argv[3]);

	if(pagedir[strlen(pagedir)-1]!='/')
		pagedir = strcat(pagedir,"/");
	
	// check whether arguments are valid
	struct stat statbuf;
	if (stat(pagedir, &statbuf)!= 0||!S_ISDIR(statbuf.st_mode)||maxdepth<0){
		printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
		exit(EXIT_FAILURE);
	}

	queue_t* qp = qopen();
	hashtable_t* visited_ht = hopen(97);
	webpage_t* webby = webpage_new(seedurl, 0, NULL);
	//	queue_t* qurl = qopen();
	int id = 1;

	// fetch seedurl, put it in queue and hashtable
	if (webpage_fetch(webby)){
		hput(visited_ht,(void*)seedurl,seedurl,strlen(seedurl));
		qput(qp,(void*)webby);
		pagesave(webby,id++,pagedir);
	} else {
		webpage_delete((void*)webby);
		qclose(qp);
		hclose(visited_ht);
		exit(EXIT_FAILURE);
	}

	webpage_t *next;
	while ( ( next = (webpage_t*)qget(qp) )!=NULL ){

		// save the page to pagedir
		if(strcmp(webpage_getURL(next),seedurl)!=0){
			if(!webpage_fetch(next)){
				webpage_delete((void*)next);
				continue;
			} else
				pagesave(next,id++,pagedir);
		}

		// put all the urls which are not visited in the queue and hashtable
		int pos = 0;
		char* result;
		while (webpage_getDepth(next)<maxdepth && (pos = webpage_getNextURL(next, pos, &result)) > 0) {

			printf("Found url: %s  \n", result);
			if (IsInternalURL(result)){
				
				printf("Internal URL.\n");
				
				if(hsearch(visited_ht,searchurl,result,strlen(result))==NULL){
					webpage_t* inter_web = webpage_new(result, webpage_getDepth(next)+1, NULL);
					hput(visited_ht,(void*)result,result,strlen(result));
					qput(qp,(void*)inter_web);
				} else
					free(result);
			} else {
				printf("External URL.\n");
				free(result);
			}
		}
		webpage_delete((void*)next);
	}

	// free all the data structure
	hremove(visited_ht,searchurl,seedurl,strlen(seedurl));
	happly(visited_ht,free_content);
	hclose(visited_ht);
	qclose(qp);
	exit(EXIT_SUCCESS);
	
}
