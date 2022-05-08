#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
extern "C"
{
    #include <webpage.h>
}

using namespace::std;

int32_t pagesave(webpage_t *pagep, int id, char *dirnm) {
    string filename = string(dirnm) + to_string(id);
    ofstream myfile;
	myfile.open(filename);

    myfile << string(webpage_getURL(pagep)) << endl << to_string(webpage_getDepth(pagep)) << endl << to_string(webpage_getHTMLlen(pagep)) << endl << string(webpage_getHTML(pagep));

    myfile.close();
    return 0;
}


webpage_t *pageload(int id, char *dirnm) {
    string filename = string(dirnm) + to_string(id);
    webpage_t *web = NULL;

    ifstream myfile;
    myfile.open(filename);

	struct stat buffer;
	stat(filename.c_str(), &buffer);

    if (buffer.st_mode & S_IRUSR) {
        int depth = 0, length = 0;
        string url = "", html = "";
        myfile >> url;
        myfile >> depth;
        myfile >> length;
        string html_init = "";
        while (getline(myfile, html)) {
            html_init = html_init + html + "\n";
        }

        char *urlArr = (char*)calloc(url.length() + 1, sizeof(char));
        strcpy(urlArr, url.c_str());

        char *htmlArr = (char*)calloc(html_init.length() + 1, sizeof(char));
        strcpy(htmlArr, html_init.c_str());

        web = webpage_new(urlArr, depth, htmlArr);
    }

    myfile.close();
    return web;    
}


