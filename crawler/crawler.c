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
#include <stdlib.h>

void printy(void* element){
	webpage_t* wp = (webpage_t*)element;
	printf("%s\n",webpage_getURL(wp));
	
	webpage_delete((void*)wp);
}

int main(void) {

	char* url = "https://thayer.github.io/engs50/";
	char* html = NULL;
	char* result;
	int pos = 0;
	queue_t* qp = qopen(); 
	
	webpage_t* webby = webpage_new(url, 0, NULL);

	if (webpage_fetch(webby)){
		html = webpage_getHTML(webby);	
	}
	else if (html == NULL)
		exit(EXIT_FAILURE);

	while ((pos = webpage_getNextURL(webby, pos, &result)) > 0) {
		printf("Found url: %s  ", result);
		if (IsInternalURL(result)){
			webpage_t* inter_web = webpage_new(result, 1, NULL);
			qput(qp,(void*)inter_web);
			printf("Internal URL.\n");
		}
		else
			printf("External URL.\n");
		free(result);
	}

	qapply(qp,printy);
	qclose(qp);

	webpage_delete((void*) webby);
	exit(EXIT_SUCCESS);
}
