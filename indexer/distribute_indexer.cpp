/*
 * distribute_indexer.cpp --- distributed version of indexer
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
#include <string>

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
		printf("usage: distribute_indexer <pagedir> <outputfile>\n");
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

	using dobj_reduce_arr_t = upcxx::dist_object<vector<tuple<string, int>>> ;

	upcxx::global_ptr<int> cur_doc = upcxx::new_<int>(0);
	upcxx::atomic_domain<int> ad({upcxx::atomic_op::fetch_add});
	upcxx::global_ptr<int> bcast_doc = upcxx::broadcast(cur_doc, 0).wait();

	dobj_reduce_arr_t reduce_arr({});

	auto start = std::chrono::steady_clock::now();

	// Map 
	upcxx::future<> fut_all = upcxx::make_future();
	while(true){
		int doc_id = ad.fetch_add(bcast_doc, 1, memory_order_relaxed).wait();
		// cout << "rank: " << upcxx::rank_me() << " doc_id: " << doc_id << endl;
		if (doc_id >= file_count-2)
			break;
	
		webpage_t *current = pageload(doc_id, pagedir);
		char* result;
		int pos = 0;
		// upcxx::future<> fut_all = upcxx::make_future();
		while ( (pos = webpage_getNextWord(current, pos, &result)) > 0) {

			// go through each word and normalize
			if(normalizeWord(result) != NULL){
				string result_string = string(result);
				int result_hash = hash<string>{}(result_string) % upcxx::rank_n();
				upcxx::future<> fut = upcxx::rpc(result_hash, 
				[](dobj_reduce_arr_t &reduce_arr, string result_string, int doc_id) {
					reduce_arr -> push_back(make_tuple(result_string, doc_id));
				}, reduce_arr, result_string, doc_id);
				fut_all = upcxx::when_all(fut_all, fut);
				
			}
			if (pos % 5 == 0) upcxx::progress();
			free(result);
		}
		// fut_all.wait();
		webpage_delete((void*)current);
	}
	fut_all.wait();
	auto end_map = std::chrono::steady_clock::now();

	upcxx::barrier();

	auto end_all_map = std::chrono::steady_clock::now();
	double map_time = std::chrono::duration<double>(end_all_map - start).count();
    if (upcxx::rank_me() == 0) {
        cout << "Finished Map in = " << map_time << " seconds." << endl;
    }

	auto start_reduce = std::chrono::steady_clock::now();

	// Reduce
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

	upcxx::barrier();
    auto end = std::chrono::steady_clock::now();

	std::chrono::duration<double> diff = end - start;
	std::chrono::duration<double> map_diff = end_map - start;
	std::chrono::duration<double> reduce_diff = end - start_reduce;

    double seconds = diff.count();
	double map_seconds = map_diff.count();
	double reduce_seconds = reduce_diff.count();
	// cout << "Rank: " << upcxx::rank_me() << ", Total Time = " << seconds << " seconds. " << " Map Time = " << map_seconds << " seconds. " << "Reduce Time = " << reduce_seconds << " seconds" " for " << " indexing.\n";

    if (upcxx::rank_me() == 0) {
        cout << "Finished Reduce in = " << reduce_seconds << " seconds." << endl;
		cout << "Total time = " << seconds << " seconds." << endl;
    }
    upcxx::barrier();

	upcxx::delete_(cur_doc);
	upcxx::finalize();

	exit(EXIT_SUCCESS);
}