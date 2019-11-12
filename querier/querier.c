/* querier.c --- 
1;95;0c * Author: Yu Chen
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
static FILE* foutput = NULL;

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

void fprint_querier(void* querierp){
	querier_t* qp = (querier_t*) querierp;
	fprintf(foutput,"rank:%d:doc:%d:%s\n",qp->rank,qp->id,qp->url);
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
		result->rank += qp->rank;
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


int main(int argc,char* argv[]){
	char input[100];                                                            
  const char s[3] = " \t";                                                    
  char *word;                                                                 
  char *output;
	bool valid = true;
	char indexnm[100];
  char command[100];
	int quiet = 0;
	FILE* fp = NULL;
	
  if (argc != 3 && argc != 6){
    printf("usage: query <pageDirectory> <indexFile>[-q]\n");
		exit(EXIT_FAILURE);
	}
	
	if(argc == 6){
		if(strcmp(argv[3],"-q")==0){
			quiet = 1;
			if (access(argv[4],R_OK) != 0)
				exit(EXIT_FAILURE);
			fp = fopen(argv[4],"r");
			foutput = fopen(argv[5],"w");
		} else{
			printf("usage: query <pageDirectory> <indexFile>[-q]\n");
			exit(EXIT_FAILURE);
		}
	} else
		foutput = stdout;

	
  strcpy(pagedir,argv[1]);
  strcpy(indexnm,argv[2]);
	
  struct stat buffer;
	if (stat(pagedir,&buffer) != 0 || !S_ISDIR(buffer.st_mode)){
		printf("usage: query <pageDirectory> <indexFile>[-q]\n");
    exit(EXIT_FAILURE);
	}
  strcpy(command,"../indexer/indexer ");
  strcat(command, pagedir);
	
  strcat(command, " ");
	strcat(command, indexnm);
  system(command);
  index = indexload(indexnm);
	
	struct stat buff;
	if (stat(pagedir,&buff) < 0 )
		exit(EXIT_FAILURE);
	
	if (!S_ISDIR(buff.st_mode))
		exit(EXIT_FAILURE);
 
	while(1) {
		
		valid = true;
		if (quiet == 1)
			output = fgets(input,100,fp);
		else{
			printf("> ");
			output = fgets(input,100,stdin);
		}
		
		if (output == NULL) 
			break;

		queue_t *qp = qopen();

		if(quiet == 1)
			fprintf(foutput,"query:%s",input);
		
		input[strlen(input)-1] = '\0'; // to replace the newline with \0
		if(strcmp(input,"\0")==0){
			valid = false;
			fprintf(foutput,"[Invalid input]\n");
		}
		
		int flag = 1;
		word = strtok(input, s);
		while (word != NULL && valid){
			for(int i=0; word[i]!= '\0'; i++){
				if(isalpha(word[i]) == 0){
					fprintf(foutput,"[Invalid input]\n");
					valid = false;
					break;
				}	
				word[i] = tolower(word[i]);
			}
			
			if (!valid)
				break;
			
			if (strcmp(word,"or") == 0 || strcmp(word,"and") == 0){
				if(flag==1){
					fprintf(foutput,"[Invalid input]\n");
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
			fprintf(foutput,"[Invalid input]\n");
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
			
			qapply(q_or,fill_hash);
			
			map_t* first_mp = qget(q_or);
			hashtable_t* first_ht = first_mp->ht;
			
			final = first_ht;
			qapply(q_or,get_final);
			
			if (quiet == 1)
				happly(final,fprint_querier);
			else
				happly(final,print_querier);
			
			happly(first_ht,free_querier);
			qclose(first_mp->q);
			hclose(first_ht);
			free(first_mp);
			
			qapply(q_or,free_all_of_them);
			qclose(q_or);
				
		}

		qclose(qp);	
		
	}
	
	if(quiet == 1){
		fclose(fp);
		fclose(foutput);
	}

	happly(index,freeWords);
	hclose(index);
	
	exit(EXIT_SUCCESS);
}

