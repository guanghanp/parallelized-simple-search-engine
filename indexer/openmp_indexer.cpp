/*
 * indexer.cpp --- create and save an indexer from a directory of crawled webpages
 * 
 */


extern "C"
{
	#include <webpage.h>
}

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <omp.h>

#include <vector>
#include <unordered_map>
#include <string>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <chrono>

#include <pageio.hpp>

using namespace std;

class doc_t {
	public:
		int document;
		int count;
		doc_t(int doc) : document(doc), count(1) {}
};

class word_t {
	public:
		omp_lock_t lock;
		vector<doc_t*> doc_vec;
		word_t(){
			omp_init_lock(&lock);
		}

		~word_t(){
			omp_destroy_lock(&lock);
		}
};

/* normalize the word by filtering out words which
 countain numbers, with length less than 3, and then
 convert to lower case.
*/ 
char *normalizeWord(char *word) {
	if(strlen(word) < 3)
		return NULL;
	for(int i=0; word[i]!= '\0'; i++){
		if(!isalpha(word[i]))
			return NULL;
		word[i] = tolower(word[i]);
	}
	return word;
}

int32_t indexsave(unordered_map<string, word_t*> indexp, char *indexnm) {

	// open the file and write the contents

	ofstream myfile;
	myfile.open(indexnm);

	for (auto const& cur_index : indexp){

		myfile << cur_index.first;
		vector<doc_t*> doc_vec = cur_index.second -> doc_vec;
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

	auto start_time = std::chrono::steady_clock::now();

	// create the hash map for indexer
    unordered_map<string, word_t*> index_hash;
	
	// load crawled pages and fill out the indexer
	# pragma omp parallel for
	for (int id = 0; id < file_count-2; id++){
		webpage_t *current = pageload(id, pagedir);
		char* result;
		
		int pos = 0;
		while ( (pos = webpage_getNextWord(current, pos, &result)) > 0) {

			// go through each word and normalize
			if(normalizeWord(result) != NULL){

				bool flag = false;
				unordered_map<string, word_t*>::const_iterator got;

				# pragma omp critical
				{
					got = index_hash.find(string(result));
					if(got == index_hash.end()){ // if the word is not in indexer, create a new word
						doc_t *docp = new doc_t(id);
						word_t *cur_word = new word_t();
						cur_word -> doc_vec.push_back(docp);
						index_hash[string(result)] = cur_word;
						flag = true;
					}
				}
				
				if (flag) {
					free(result);
					continue;
				}

				omp_set_lock(&got -> second -> lock);
				vector<doc_t*> cur_vec = got -> second -> doc_vec;
				bool found = false;
				for (doc_t *cur_doc : cur_vec){
					if(cur_doc -> document == id){
						cur_doc -> count += 1;
						found = true;
						break;
					}
				}

				if(!found){
					doc_t *docp = new doc_t(id);
					got -> second -> doc_vec.push_back(docp);
				}
				omp_unset_lock(&got -> second -> lock);
            } 
			free(result);
		}

		webpage_delete((void*)current);
	}

	auto index_time = std::chrono::steady_clock::now();

	// save the indexer to specified position
	indexsave(index_hash, argv[2]);

	auto end_time = std::chrono::steady_clock::now();
	std::chrono::duration<double> diff = end_time - start_time;
	std::chrono::duration<double> index_diff = index_time - start_time;
	std::chrono::duration<double> save_diff = end_time - index_time;
    double seconds = diff.count();
	double index_seconds = index_diff.count();
	double save_seconds = save_diff.count();

	std::cout << "Total Time = " << seconds << " seconds. " << " Index Time = " << index_seconds << " seconds. " << " Save Time = " << save_seconds << " seconds" " for " << "indexing.\n";

	for (auto const& cur_index : index_hash){

		vector<doc_t*> doc_vec = cur_index.second -> doc_vec;
		for (auto const& cur_doc : doc_vec){
			delete cur_doc;
		}
		delete cur_index.second;
	}

	exit(EXIT_SUCCESS);
}