/* crawler.c --- concurrent web crawler
 *
 * Author: Guanghan Pan
 * 
 */


#include <stdio.h>
#include <webpage.h>
#include <lqueue.h>
#include <lhash.h>
#include <pageio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>

// arguments for the program
static char* pagedir;
static int maxdepth;
static char* seedurl;

// conditions for threads to run
static int running = 0;
static int init = 1;

// variables used by threads
static lqueue_t* qp;
static lhashtable_t* visited_ht;
static int id = 1;
static pthread_mutex_t m_r;
static pthread_mutex_t m_id;

// helper function to free urls in hashtable
void free_content(void* element){
	char* cp = (char*)element;
	free(cp);
}

// helper function to search for urls in hashtable
bool searchurl(void* urlp, const void* urlkey){
	char* up = (char*)urlp;
	char* key = (char*)urlkey;
	return strcmp(up,key)==0;
}

// main crawler function used by each thread
void *crawlfunc(void *vargp){

	while(running>0||init==1){
		
		webpage_t *next;
		if ( ( next = (webpage_t*)lqget(qp) )!=NULL ){
			pthread_mutex_lock(&m_r);
			running++;
			pthread_mutex_unlock(&m_r);
			init = 0;			
			
			if(strcmp(webpage_getURL(next),seedurl)!=0){
				
				if(!webpage_fetch(next)){
					webpage_delete((void*)next);
					pthread_mutex_lock(&m_r);
					running--;
					pthread_mutex_unlock(&m_r);
					continue;
				} else{
					pthread_mutex_lock(&m_id);
					id++;
					pthread_mutex_unlock(&m_id);
					pagesave(next,id,pagedir);
				}
			}
			
			int pos = 0;
			char* result;
			while (webpage_getDepth(next)<maxdepth && (pos = webpage_getNextURL(next, pos, &result)) > 0) {

				printf("Found url: %s  \n", result);
				if (IsInternalURL(result)){
					
					printf("Internal URL.\n");
					
					if(lhadd(visited_ht,searchurl,result,strlen(result))==NULL){
						webpage_t* inter_web = webpage_new(result, webpage_getDepth(next)+1, NULL);
						lqput(qp,(void*)inter_web);
					} else {
						free(result);
						printf("in the ht\n");
					}
				} else {
					printf("External URL.\n");
					free(result);
				}
			}
			
			webpage_delete((void*)next);
			pthread_mutex_lock(&m_r);
			running--;
			pthread_mutex_unlock(&m_r);

		}
	}
	return NULL;
}

int main(int argc,char *argv[]) {

	// check number of arguments
	if(argc != 5){
		printf("usage: crawler <seedurl> <pagedir> <maxdepth> <numthread>\n");
		exit(EXIT_FAILURE);
	}
	
	seedurl = argv[1];
	pagedir = argv[2];
	maxdepth = atoi(argv[3]);
	int numthread = atoi(argv[4]);
	
	if(pagedir[strlen(pagedir)-1]!='/')
		pagedir = strcat(pagedir,"/");

	// check validity of arguments
	struct stat statbuf;
	if (stat(pagedir, &statbuf)!= 0||!S_ISDIR(statbuf.st_mode)||maxdepth<0){
		printf("usage: crawler <seedurl> <pagedir> <maxdepth> <numthread>\n");
		exit(EXIT_FAILURE);
	}

	// initialize variables
	qp = lqopen();
  visited_ht = lhopen(97);
	pthread_t threads[numthread];
	pthread_mutex_init(&m_r,NULL);
	pthread_mutex_init(&m_id,NULL);
	webpage_t* webby = webpage_new(seedurl, 0, NULL);

	// fetch seedurl
	if (webpage_fetch(webby)){
		lhput(visited_ht,(void*)seedurl,seedurl,strlen(seedurl));
		lqput(qp,(void*)webby);
		pagesave(webby,id,pagedir);
	} else
		exit(EXIT_FAILURE);

	
	// initialize the threads
	for(int i=0;i<numthread;i++){
		pthread_create(&threads[i],NULL,crawlfunc,NULL);
	}

	for(int i=0;i<numthread;i++){
		pthread_join(threads[i],NULL);
	}

	// free all the data structures
	pthread_mutex_destroy(&m_r);
	pthread_mutex_destroy(&m_id);	
	lhremove(visited_ht,searchurl,seedurl,strlen(seedurl));
	lhapply(visited_ht,free_content);
	lhclose(visited_ht);
	lqclose(qp);
	exit(EXIT_SUCCESS);
	
}
