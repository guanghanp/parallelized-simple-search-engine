/* querier.c --- 
 * Author: Yu Chen
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
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

static hashtable_t* ht;
static hashtable_t* ht_temp;
static hashtable_t* final;
static hashtable_t* index;
static char pagedir[100];

typedef struct map_t{
	queue_t* q;
	hashtable_t* ht;
}map_t;

typedef struct querier_t{
	int rank;
	int id;
	char url[100];
}querier_t;

map_t* init_map(queue_t* qp){
	map_t* mp = (map_t*)malloc(sizeof(map_t));
	mp->q = qp;
	return mp;
}

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

void init_insert(void* docp){
	doc_t* dp = (doc_t*)docp;
	querier_t* qp = init_querier(dp->count,dp->document);\
	char key[100];
	sprintf(key,"%d",dp->document);
	hput(ht,qp,key,strlen(key));
}

void insert(void* dp){
	doc_t* docp = (doc_t*)dp;
	char key[100];
	sprintf(key,"%d",docp->document);
	querier_t* result = (querier_t*)hsearch(ht,searchquerier,key,strlen(key));
	if(result != NULL){
		int new_rank = (docp->count < result->rank)?(docp->count):(result->rank);
		querier_t* qrp = init_querier(new_rank,result->id);
		int keylen = strlen(key);
		hput(ht_temp,qrp,key,keylen);
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
	
	ht_temp = hopen(97);
	if(result !=NULL)
		qapply(result->docq,insert);
	
	happly(ht,free_querier);
	hclose(ht);
	ht = ht_temp;
}

void fill_hash(void* mapp){
	map_t* mp = (map_t*)mapp;
	char* first = (char*)qget(mp->q);
	ht = hopen(97);
	word_t* result = (word_t*)hsearch(index,searchhash,first,strlen(first));
	if(result !=NULL){
		qapply(result->docq,init_insert);
		qapply(mp->q,insert_docs);
	}
	mp->ht = ht;
}

void compare_final(void* qup){
	querier_t* qp = (querier_t*) qup;
	char key[100];
	sprintf(key,"%d",qp->id);
	querier_t* result = (querier_t*)hsearch(final,searchquerier,key,strlen(key));
	if(result==NULL){
		querier_t* new = init_querier(qp->rank,qp->id);
		hput(final,new,key,strlen(key));
	} else {
		if(result->rank<qp->rank)
			result->rank = qp->rank;
	}
}

void get_final(void* mapp){
	map_t* mp = (map_t*)mapp;
	happly(mp->ht,compare_final);
}

void free_all_of_them(void* mapp){
	map_t* mp = (map_t*) mapp;
	qclose(mp->q);
	happly(mp->ht,free_querier);
	hclose(mp->ht);
	free(mp);
}


int main(void){
	char input[100];
	const char s[3] = " \t";
	char *word;
	char *output;
	bool valid = true;
	strcpy(pagedir,"../pages/");
	index = indexload("../test/index_m6");

	struct stat buff;
	if (stat(pagedir,&buff) < 0 )
		exit(EXIT_FAILURE);
	
	if (!S_ISDIR(buff.st_mode))
		exit(EXIT_FAILURE);
		
	while(1) {
		printf("> ");
		queue_t *qp = qopen();
		
		valid = true;
		output = fgets(input,100,stdin);
		if (output == NULL) {
			qclose(qp);
			break;
		}
		input[strlen(input)-1] = '\0'; // to replace the newline with \0
		if(strcmp(input,"\0")==0){
			valid = false;
			printf("[Invalid input]\n");
		}
		
		int flag = 1;
		word = strtok(input, s);
		while (word != NULL && valid){
			for(int i=0; word[i]!= '\0'; i++){
				if(isalpha(word[i]) == 0){
					printf("[Invalid input]\n");
					valid = false;
					break;
				}	
				word[i] = tolower(word[i]);
			}
			
			if (!valid)
				break;
			
			if (strcmp(word,"or") == 0 || strcmp(word,"and") == 0){
				if(flag==1){
					printf("[Invalid input]\n");
					valid = false;
					break;
				} else
					flag = 1; 
			} else
				flag = 0;
			
			if ((strcmp(word,"or") == 0 || strlen(word) >= 3) && strcmp(word,"and") != 0)
				qput(qp,word);
			
			word = strtok(NULL, s);
		}
		
		if(flag==1 && valid){
			printf("[Invalid input]\n");
			valid = false;
		}

		if(valid){
			queue_t* q_or = qopen();
			queue_t* q_and = qopen();

			char* ptr = (char*)qget(qp);
			if (ptr == NULL)
				valid = false;
			else {
				
				do{
					
					if (strcmp(ptr,"or") == 0){
						map_t* map = init_map(q_and);
						qput(q_or,map);				
						q_and = qopen();
					} else 
						qput(q_and,ptr);
					
					ptr = (char*)qget(qp);
				} while(ptr!=NULL);
				
				map_t* map = init_map(q_and);
				qput(q_or,map);				
								
			}
			
			if(valid){
				qapply(q_or,fill_hash);
			}
			
			map_t* first_mp = qget(q_or);
			hashtable_t* first_ht = first_mp->ht;
			
			final = first_ht;
			qapply(q_or,get_final);
			happly(final,print_querier);
			
			happly(first_ht,free_querier);
			qclose(first_mp->q);
			hclose(first_ht);
			free(first_mp);
			
			qapply(q_or,free_all_of_them);
			qclose(q_or);
			qclose(qp);
		
		}
		
	}

	happly(index,freeWords);
	hclose(index);
	
	return 0;
}

