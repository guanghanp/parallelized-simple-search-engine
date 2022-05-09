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
#include <upcxx/upcxx.hpp>

#include <vector>
#include <unordered_map>
#include <string>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <chrono>

using namespace std;

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

	upcxx::init();

	auto start_time = std::chrono::steady_clock::now();

	// upcxx::global_ptr<list<int>> gptr = upcxx::new_<int>(0);
	using dobj_list_t = upcxx::dist_object<list<int>>;
	using dobj_reduce_arr_t = upcxx::dist_object<vector<tuple<string, int>>> ;

	dobj_list_t job_queue({});
	dobj_reduce_arr_t reduce_arr({});
	
	if(upcxx::rank_me() == 0){
		for(int i = 0; i < file_count-2; i++){
			job_queue -> push_back(i);
		}
		
	} 
	
	if(upcxx::rank_me() > 0) {
		// Map 
		while(true){
			int doc_id = upcxx::rpc(0, 
                      [](dobj_list_t &job_queue) -> int {
						if(!job_queue -> empty()){
							int next = job_queue -> front();
                    		job_queue -> pop_front();
							return next;
						}
						return -1;
					  }, job_queue).wait();
			if (doc_id == -1)
				break;
		
			webpage_t *current = pageload(doc_id, pagedir);
			char* result;
			int pos = 0;
			while ( (pos = webpage_getNextWord(current, pos, &result)) > 0) {

				// go through each word and normalize
				if(normalizeWord(result) != NULL){
					string result_string = string(result);
					int result_hash = hash<string>{}(result_string) % (upcxx::rank_n()-1);
					int idc = upcxx::rpc(result_hash+1, 
                      [](dobj_reduce_arr_t &reduce_arr, string result_string, int doc_id) -> int {
						reduce_arr -> push_back(make_tuple(result_string, doc_id));
						return 0;
					  }, reduce_arr, result_string, doc_id).wait();
				}
			}

			webpage_delete((void*)current);
			
		}
	}

	upcxx::barrier();

	// Reduce
	if(upcxx::rank_me() > 0) {

		sort(reduce_arr -> begin(), reduce_arr -> end());
		ofstream myfile;
		myfile.open(string(argv[2]) + "-" + to_string(upcxx::rank_me()));
		string prev_word = "";
		for (int i = 0; i < reduce_arr -> size();){
			string cur_word = get<0>((reduce_arr -> at(i)));
			int cur_doc = get<1>((reduce_arr -> at(i)));
			if(cur_word.compare(prev_word) != 0){
				if(prev_word.compare("") != 0){
					myfile << endl;
				}
				prev_word = cur_word;
				myfile << cur_word;
			}
			int j = i + 1;
			while( j < reduce_arr -> size() && get<0>((reduce_arr -> at(j))).compare(cur_word) == 0 && get<1>((reduce_arr -> at(j))) == cur_doc)
				j++;
			myfile << " " << cur_doc << " " << j-i;
			i = j;
		}
		myfile << endl;
		myfile.close();
		
	}

	upcxx::finalize();
	exit(EXIT_SUCCESS);
}