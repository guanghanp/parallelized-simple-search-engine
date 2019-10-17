/* crawler.c --- just prints hello
 * 
 * 
 * Author: Guanghan Pan
 * Created: Thu Oct 15 17:48:22 2019 (-0400)
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <webpage.h>
#include <queue.h>
#include <hash.h>
#include <stdlib.h>
#include <string.h>

void printy(void* element){
	webpage_t* wp = (webpage_t*)element;
	printf("%s\n",webpage_getURL(wp));
	webpage_delete((void*)wp);
}

bool searchurl(void* webp, const void* urlkey){
	webpage_t* wp = (webpage_t*)webp;
	char* key = (char*)urlkey;
	return strcmp(webpage_getURL(wp),key)==0;
}

int main(void) {

	char* url = "https://thayer.github.io/engs50/";
	queue_t* qp = qopen();
	hashtable_t* visited_ht = hopen(100);
	webpage_t* webby = webpage_new(url, 0, NULL);

	if (!webpage_fetch(webby))
		exit(EXIT_FAILURE);

	int pos = 0;
	char* result;
	while ((pos = webpage_getNextURL(webby, pos, &result)) > 0) {
		
		printf("Found url: %s  \n", result);
		if (IsInternalURL(result)){
			
			printf("Internal URL.\n");
			
			if(hsearch(visited_ht,searchurl,result,strlen(result))==NULL){
				printf("Not in the ht.\n");
				webpage_t* inter_web = webpage_new(result, 1, NULL);
				hput(visited_ht,(void*)inter_web,result,strlen(result));
				qput(qp,(void*)inter_web);
			} else
				printf("in the ht\n");
			
		}
		else
			printf("External URL.\n");
		free(result);
	}	
	


	qapply(qp,printy);
	hclose(visited_ht);
	qclose(qp);
	
	webpage_delete((void*) webby);
	exit(EXIT_SUCCESS);
}
