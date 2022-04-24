/*
 * indexer.cpp --- create and save an indexer from a directory of crawled webpages
 * 
 */


extern "C"
{
	#include <webpage.h>
	#include <pageio.h>
}

#include <iostream>
#include <fstream>
#include <stdio.h>

#include <vector>
#include <unordered_map>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;

typedef struct doc_t{
	int document;
	int count;
} doc_t;


void init_doc(doc_t *dp, int id, int count){
	dp->document = id;
	dp->count = count;
}

/* normalize the word by filtering out words which
 countain numbers, with length less than 3, and then
 convert to lower case.
*/ 
char *normalizeWord(char *word){
	if(strlen(word) < 3)
		return NULL;
	for(int i=0; word[i]!= '\0'; i++){
		if(!isalpha(word[i]))
			return NULL;
		word[i] = tolower(word[i]);
	}
	return word;
}

int32_t indexsave(unordered_map<string, vector<doc_t*>> indexp, char *indexnm){

	// open the file and write the contents

	ofstream myfile;
	myfile.open(indexnm);

	for (auto const& cur_index : indexp){

		myfile << cur_index.first;
		vector<doc_t*> doc_vec = cur_index.second;
		for (auto const& cur_doc : doc_vec){
			myfile <<  " " << cur_doc -> document <<  " " << cur_doc -> count;
		}
		myfile << endl;

	}

	myfile.close();
	return 0;
}

// main method
int main(int argc, char *argv[]){

	// check number of arguments
	if(argc != 3){
		printf("usage: indexer <pagedir> <outputfile>\n");
		exit(EXIT_FAILURE);
	}

	// check the validity of pagedir
	char* pagedir = argv[1];
	struct stat buff;
	if (stat(pagedir,&buff) < 0 || !S_ISDIR(buff.st_mode))
		exit(EXIT_FAILURE);

	int file_count = 0;
	struct dirent *de;
  
	DIR *dr = opendir(pagedir);
  
	if (dr == NULL){
		printf("Could not open page directory" );
		exit(EXIT_FAILURE);
	}

    // count number of files in pagedir
	while ((de = readdir(dr)) != NULL)
		file_count++;
  
	closedir(dr);

	// create the hash map for indexer
    unordered_map<string, vector<doc_t*>> index_hash;
	
	// load crawled pages and fill out the indexer
    webpage_t *current;
	int id = 0;

	for (; id < file_count-2; id++){
		current = pageload(id, pagedir);
		char* result;
		
		int pos = 0;
		while ( (pos = webpage_getNextWord(current, pos, &result)) > 0) {

			// go through each word and normalize
			if(normalizeWord(result) != NULL){

                unordered_map<string, vector<doc_t*>>::const_iterator got = index_hash.find(string(result));
				if(got == index_hash.end()){ // if the word is not in indexer, create a new word

					doc_t* docp = (doc_t*)malloc(sizeof(doc_t));
					init_doc(docp,id,1);
                    index_hash[string(result)].push_back(docp);

				} else { // if the word is already in the indexer
					
                    vector<doc_t*> cur_vec = index_hash[string(result)];
                    bool found = false;
                    for (doc_t *cur_doc : cur_vec){
                        if(cur_doc -> document == id){
                            cur_doc -> count += 1;
                            found = true;
                            break;
                        }
                    }

                    if(!found){
                        doc_t* docp = (doc_t*)malloc(sizeof(doc_t));
                        init_doc(docp,id,1);
                        index_hash[string(result)].push_back(docp);
                    }
				}
            }
		}

		webpage_delete((void*)current);
	}

	// save the indexer to specified position
	indexsave(index_hash, argv[2]);

	exit(EXIT_SUCCESS);
}