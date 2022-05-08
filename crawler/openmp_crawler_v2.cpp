/* crawler.cpp --- web crawler
 *
 * Author: Guanghan Pan
 * 
 */


#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <omp.h>
extern "C"
{
	#include <webpage.h>
	#include <pageio.h>
}


#include <string>
#include <cstdlib>
#include <iostream>
#include <list>
#include <set>
#include <cstring>
#include <chrono>

using namespace std;

// helper function used to free hashtable content
// void free_content(void* element){
// 	char* cp = (char*)element;
// 	free(cp);
// }

// search whether url is in the hashtable
// bool searchurl(void* urlp, const void* urlkey){
// 	char* up = (char*)urlp;
// 	char* key = (char*)urlkey;
// 	return strcmp(up,key)==0;
// }

// main method
int main(int argc,char *argv[]) {

	// check number of arguments
	if(argc != 4){
		// printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
		exit(EXIT_FAILURE);
	}
	
	char* seedurl_char = argv[1];
	string seedurl = string(seedurl_char);
	char* pagedir_char = argv[2];
	string pagedir = string(pagedir_char);

	int maxdepth = atoi(argv[3]);

	if (pagedir[pagedir.length()-1]!='/')
		pagedir = pagedir + "/";
	
	// check whether arguments are valid
	struct stat statbuf;
	if (stat(pagedir_char, &statbuf)!= 0||!S_ISDIR(statbuf.st_mode)||maxdepth<0){
		cout << "usage: crawler <seedurl> <pagedir> <maxdepth>" << endl;
		exit(EXIT_FAILURE);
	}

	auto start_time = std::chrono::steady_clock::now();
	// queue_t* qp = qopen();
	list<webpage_t*> qp;
	// hashtable_t* visited_ht = hopen(97);
	set<string> visited;
	webpage_t* webby = webpage_new(seedurl_char, 0, NULL);
	//	queue_t* qurl = qopen();
	int id = 0;

	// fetch seedurl, put it in queue and hashtable
	if (webpage_fetch(webby)){
		visited.insert(string(seedurl));
		// hput(visited_ht,(void*)seedurl,seedurl,strlen(seedurl));
		qp.push_back(webby);
		// qput(qp,(void*)webby);
		pagesave(webby,id++,pagedir_char);
	} else {
		webpage_delete((void*)webby);
		exit(EXIT_FAILURE);
	}

	// webpage_t *next;
    int init = 1, running = 0;
    #pragma omp parallel
    {
        while (running > 0||init == 1) {
            // if (!qp.empty()) {
                // int size = qp.size();
                // # pragma omp parallel for
                // for (int i = 0; i < size; i++) {
            webpage_t *next;
            bool flag = false;
            # pragma omp critical
            {   
                if (!qp.empty()) {
                    next = qp.front();
                    qp.pop_front();
                } else {
                    flag = true;
                }
            }

            if (flag) continue;
            
            #pragma omp atomic
            running++;
            init = 0;

            // save the page to pagedir
            if(strcmp(webpage_getURL(next),seedurl_char)!=0){
                if(!webpage_fetch(next)){
                    # pragma omp atomic
                    running--;
                    webpage_delete((void*)next);
                    continue;
                } else {
                    # pragma omp atomic
                    id++;
                    pagesave(next,id - 1,pagedir_char);
                }
            }

            // put all the urls which are not visited in the queue and hashtable
            int pos = 0;
            char* result;
            while (webpage_getDepth(next)<maxdepth && (pos = webpage_getNextURL(next, pos, &result)) > 0) {
                bool flag = false;
                // printf("Found url: %s  \n", result);
                if (IsInternalURL(result)){
                    // printf("Internal URL.\n");
                    # pragma omp critical 
                    {
                        if(visited.count(string(result)) == 0) {
                            visited.insert(string(result));
                        } else {
                            free(result);
                            flag = true;
                        }
                    }
                    if (flag) continue;
                    webpage_t* inter_web = webpage_new(result, webpage_getDepth(next)+1, NULL);
                    # pragma omp critical
                    {
                        qp.push_back(inter_web);
                    }
                } else {
                    // printf("External URL.\n");
                    free(result);
                }
            }
            webpage_delete((void*)next);
            # pragma omp atomic
            running--;
            // }
        }
    }


	auto end_time = std::chrono::steady_clock::now();
	std::chrono::duration<double> diff = end_time - start_time;
    double seconds = diff.count();

	std::cout << "Simulation Time = " << seconds << " seconds for depth of " << maxdepth << " crawling.\n";
	// free all the data structure
	// hremove(visited_ht,searchurl,seedurl,strlen(seedurl));
	// happly(visited_ht,free_content);
	// hclose(visited_ht);
	// qclose(qp);
	exit(EXIT_SUCCESS);
	
}
