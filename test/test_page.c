/* test_page.c --- 
 * 
 * 
 * Author: Yu Chen
 * Created: Thu Oct 17 21:27:52 2019 (-0400)
 * Version: 
 * 
 * Description: test pageio module
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <webpage.h>
#include <pageio.h>

int main(void){
	int id = 2;
	char* dirnm = "../pages/";

	webpage_t* page = pageload(id,dirnm);
	pagesave(page,500,dirnm);
	
	webpage_t* page_new = pageload(500,dirnm);

	//	printf("%d\n",webpage_getDepth(page_new));
	//printf("%s\n",webpage_getURL(page_new));
	printf("%s\n",webpage_getHTML(page));
	// printf("%s\n",webpage_getHTML(page_new));
	bool dep = (webpage_getDepth(page) == webpage_getDepth(page_new));
	int html = (strcmp(webpage_getHTML(page),webpage_getHTML(page_new)));
	int url = (strcmp(webpage_getURL(page),webpage_getURL(page_new)));
	bool htmllen = (webpage_getHTMLlen(page) == webpage_getHTMLlen(page_new));
	webpage_delete((void*)page);
	webpage_delete((void*)page_new);
	if (dep == true && html == 0 && url == 0 && htmllen == true)
		exit(EXIT_SUCCESS);
 	exit(EXIT_FAILURE);
}
