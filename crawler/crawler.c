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
#include <stdint.h>
#include <unistd.h>

void printy(void* element){
	webpage_t* wp = (webpage_t*)element;
	printf("%s\n",webpage_getURL(wp));
	webpage_delete((void*)wp);
}

bool searchurl(void* webp, const void* urlkey){
	webpage_t* wp = (webpage_t*)webp;
	char* key = (char*)urlkey;
	4
Step 
4
.
Hash Table of URL’s.
Extend  your  solution  to  Step  3
to  c
reate  a  hash  table  of 
visited 
URL’s
: 
No new webpage is placed in the queue if its associated URL is already in the hash table. 
If a URL is not in the hash table, 
the associated webpage
is added to the queue and 
the URL is 
also added to the hash table
. 
Verify
that
only one
instance of Coding
Style.html is
printed
.
Run 
valgrind
to validate your prograreturn strcmp(webpage_getURL(wp),key)==0;
}

int32_t pagesave(webpage_t *pagep, int id, char *dirname){
	FILE *fp;
	char filename[20];
	// create file name using dirname
	sprintf(filename,"%s%d",dirname,id);

	// open the file and write the contents
	fp = fopen(filename,"w");
	fprintf(fp,"%s\n%d\n%d\n%s",webpage_getURL(pagep),webpage_getDepth(pagep)
					,webpage_getHTMLlen(pagep), webpage_getHTML(pagep));
	fclose(fp);
	return 0;
}

int main(int argc,char *argv[]) {

	if(argc != 4){
		printf("usage: crawler <seedurl> <pagedir> <maxdepth>");
		exit(EXIT_FAILURE);
	}
	
	char* seedurl = "https://thayer.github.io/engs50/";
	char* pagedir = "../pages/";
	// char* seedurl = argv[1];
	// char* pagedir = argv[2];

	int maxdepth = atoi(argv[3]);
	queue_t* qp = qopen();
	hashtable_t* visited_ht = hopen(100);
	webpage_t* webby = webpage_new(seedurl, 0, NULL);
	
	if (webpage_fetch(webby)){
		hput(visited_ht,(void*)webby,seedurl,strlen(seedurl));
		qput(qp,(void*)webby);		
	} else {
		exit(EXIT_FAILURE);
	}


	webpage_t *next;
	int id = 1;
	while ( ( next = (webpage_t*)qget(qp) )!=NULL ){

		if(strcmp(webpage_getURL(next),seedurl)!=0){
			if(!webpage_fetch(next)){
				webpage_delete((void*)next);
				continue;
			} else
				pagesave(next,id++,pagedir);
		}
		
		int pos = 0;
		char* result;
		while (webpage_getDepth(next)<maxdepth && (pos = webpage_getNextURL(next, pos, &result)) > 0) {
			
			printf("Found url: %s  \n", result);
			if (IsInternalURL(result)){
				
				printf("Internal URL.\n");
				
				if(hsearch(visited_ht,searchurl,result,strlen(result))==NULL){
					printf("Not in the ht.\n");
					webpage_t* inter_web = webpage_new(result, webpage_getDepth(next)+1, NULL);
					hput(visited_ht,(void*)inter_web,result,strlen(result));
					qput(qp,(void*)inter_web);
				} else
					printf("in the ht\n");
				
			}
			else
				printf("External URL.\n");
			free(result);
		}
		
		// webpage_delete((void*)next);
		
	}

	hclose(visited_ht);
	qclose(qp);
	exit(EXIT_SUCCESS);
	
}
