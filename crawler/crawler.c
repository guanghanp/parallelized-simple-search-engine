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

int main(void) {

	char* url = "https://thayer.github.io/eng50";
	char* html;
	webpage_t* webby = webpage_new(url, 0, html);

	html = webpage_getHTML(webby);

	if (html==NULL){
		exit(EXIT_FAILURE);
	}

	
	// log one word (1-9 chars) about a given url
	inline static void logr(const char *word, const int depth, const char *url)
	{
  printf("%2d %*s%9s: %s\n", depth, depth, "", word, url);
	}
	

	webpage_delete((void*) webby);
	exit(EXIT_SUCCESS);
}
