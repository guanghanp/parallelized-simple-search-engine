/* pageio.c --- 
1;95;0c1;95;0c1;95;0c * 
 * 
 * Author: Guanghan Pan
 * Created: Thu Oct 17 19:35:35 2019 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <webpage.h>
#include <pageio.h>
#include <sys/stat.h>
#include <sys/types.h>

int32_t pagesave(webpage_t *pagep, int id, char *dirnm){
	FILE *fp;
	char filename[20];
	// create file name using dirname
	sprintf(filename,"%s%d",dirnm,id);
	
	if (access(filename, W_OK) == 0){
		// open the file and write the contents
		fp = fopen(filename,"w");
		fprintf(fp,"%s\n%d\n%d\n%s",webpage_getURL(pagep),webpage_getDepth(pagep)
						,webpage_getHTMLlen(pagep), webpage_getHTML(pagep));
		fclose(fp);
		return 0;
	}
	return 1;
}

webpage_t *pageload(int id, char *dirnm){
	FILE *fp;
	char filename[20];
	webpage_t *web = NULL;
	
	sprintf(filename,"%s%d",dirnm,id);
	
	if((fp = fopen(filename,"r"))==NULL)
		return NULL;
	
	struct stat buffer;
	stat(filename, &buffer);
	if (buffer.st_mode & S_IRUSR){
		char url[50],depth[5],len[5],html[1000];
		fscanf(fp,"%s\n%s\n%s\n",url,depth,len);
		int length = atoi(len);
		char *html_init =(char*) malloc((length+1)*sizeof(char));
		html_init[0] = '\0';
		
		int depth_int = atoi(depth);

		while (fgets(html,1000,fp) != NULL)
			strcat(html_init,html);
		
		web = webpage_new(url,depth_int,html_init);
	}
	
	fclose(fp);
	return web;
}
