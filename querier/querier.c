/* querier.c --- 
1;5202;0c * Author: Yu Chen
 * Created: Thu Oct 24 17:21:39 2019 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <queue.h>
#include <indexio.h>
#include <hash.h>

static hashtable_t* ht;
static char pagedir[100];
static hashtable_t* index;
typedef struct querier_t{
	int rank;
	int id;
	char url[100];
}querier_t;

bool searchhash(void* elementp, const void* searchkeyp){
	char* key = (char*)searchkeyp;
	word_t* wp = (word_t*)elementp;
	if(strcmp(wp->word,key) == 0)
		return true;
	return false;
}

bool searchqueue(void* elementp, const void* keyp){
	int* page = (int*)keyp;
	doc_t* dp = (doc_t*) elementp;

	if(dp->document == *page)
		return true;	
	return false;
}

bool searchquerier(void* elementp, const void* keyp){
	querier_t* qp = (querier_t*) elementp;
	char* id = (char*) keyp;
	int id_int = atoi(id);

	if(qp->id == id_int)
		return true;
	return false;
}

querier_t* init_querier(int rank,int id){
	querier_t* qr = (querier_t*)malloc(sizeof(querier_t));
	qr->rank = rank;
	qr->id = id;
	FILE *fp;
	char url[100];
	char page_id[10];
	char path[20];
	sprintf(page_id,"%d",id);
	strcpy(path,pagedir);
	strcat(path,page_id);
	fp = fopen(path,"r");
	fscanf(fp,"%s\n",url);
	strcpy(qr->url,url);
	fclose(fp);
	return qr;
}

void insert(void* dp){
	doc_t* docp = (doc_t*)dp;
	char key[100];
	sprintf(key,"%d",docp->document);
	querier_t* result = (querier_t*)hsearch(ht,searchquerier,key,strlen(key));
	if(result == NULL){
		querier_t* qrp = init_querier(docp->count,docp->document);
		int keylen = strlen(key);
		hput(ht,qrp,key,keylen);
	} else {

		if(docp->count<result->rank)
			result->rank = docp->count;
	}
}


void print_querier(void* querierp){
	querier_t* qp = (querier_t*) querierp;
	printf("rank:%d:doc:%d:%s\n",qp->rank,qp->id,qp->url);
}

void free_querier(void* querierp){
	querier_t* qp = (querier_t*) querierp;
	free(qp);
}

void insert_docs(void* wordp){
	char* wp = (char*)wordp;
	word_t* result = (word_t*)hsearch(index,searchhash,wp,strlen(wp));
	if(result !=NULL){
		qapply(result->docq,insert);
	}
}

int main(void){
	char input[100];
	const char s[2] = " ";
	char *word;
	char *output;
	bool valid = true;
	strcpy(pagedir,"../pages/");
	index = indexload("../test/index_m6");
	
	while(1) {
		printf("> ");
		queue_t *qp = qopen();
		ht = hopen(97);
		
		valid = true;
		output = fgets(input,100,stdin);
		if (output == NULL) {
			qclose(qp);
			break;
		}
		input[strlen(input)-1] = '\0'; // to replace the newline with \0
		if(strcmp(input,"\0")==0)
			valid = false;
		word = strtok(input, s);

		while (word != NULL ){
			for(int i=0; word[i]!= '\0'; i++){
				if(isalpha(word[i]) == 0 && valid){
					printf("[Invalid input]\n");
			
					valid = false;
					break;
				}
				if (valid)
					word[i] = tolower(word[i]);
			} // to lower letters
			if (!valid)
				break;
			if (strlen(word) >= 3 && strcmp(word,"and")!= 0 )
				qput(qp,word);
			
			word = strtok(NULL, s);
		}

		if (valid){
			qapply(qp,insert_docs);
			happly(ht,print_querier);
		}
		qclose(qp);
		happly(ht,free_querier);
		hclose(ht);
	}
	
	return 0;
}

