CFLAGS=-Wall -pedantic -I../utils -L../lib -g
LIBS=-lutils -lcurl

ALL:				crawler openmp_crawler

crawler:    crawler.cpp    
						g++ $(CFLAGS) $^ $(LIBS) -o $@ 

openmp_crawler:  openmp_crawler.cpp
						g++ $(CFLAGS) $^ $(LIBS) -o $@ -fopenmp

clean:
						rm -f crawler openmp_crawler
