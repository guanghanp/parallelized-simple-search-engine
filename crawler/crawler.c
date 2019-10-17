/* crawler.c --- web crawler
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
#include <sys/stat.h>

void free_content(void* element){
	char* cp = (char*)element;
	free(cp);
}

bool searchurl(void* urlp, const void* urlkey){
	char* up = (char*)urlp;
	char* key = (char*)urlkey;
	return strcmp(up,key)==0;
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
		printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
		exit(EXIT_FAILURE);
	}
	// char* seedurl = "https://thayer.github.io/engs50/";
	// char* pagedir = "../pages/";
	char* seedurl = argv[1];
	
	char* pagedir = argv[2];
	int maxdepth = atoi(argv[3]);

	if(pagedir[strlen(pagedir)-1]!='/')
		pagedir = strcat(pagedir,"/");
	
	struct stat statbuf;
	if (stat(pagedir, &statbuf)!= 0||!S_ISDIR(statbuf.st_mode)||maxdepth<0){
		printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
		exit(EXIT_FAILURE);
	}

	queue_t* qp = qopen();
	hashtable_t* visited_ht = hopen(100);
	webpage_t* webby = webpage_new(seedurl, 0, NULL);
	//	queue_t* qurl = qopen();
	int id = 1;
	
	if (webpage_fetch(webby)){
		hput(visited_ht,(void*)seedurl,seedurl,strlen(seedurl));
		qput(qp,(void*)webby);
		pagesave(webby,id++,pagedir);
	} else {
		exit(EXIT_FAILURE);
	}

	webpage_t *next;
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
					// printf("Not in the ht.\n");
					webpage_t* inter_web = webpage_new(result, webpage_getDepth(next)+1, NULL);
					//			qput(qurl,(void*)result);			
					hput(visited_ht,(void*)result,result,strlen(result));
					qput(qp,(void*)inter_web);
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
	}

	/*	char *n;
	while ( ( n = (char*)qget(qurl) )!=NULL )
		free(n);
	*/
	hremove(visited_ht,searchurl,seedurl,strlen(seedurl));
	happly(visited_ht,free_content);
	hclose(visited_ht);
	//	qclose(qurl);
	qclose(qp);
	exit(EXIT_SUCCESS);
	
}
