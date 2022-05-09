CFLAGS=-Wall -pedantic -I../utils -L../lib -g
LIBS=-lutils -lcurl

all: indexer openmp_indexer

indexer:    indexer.cpp    
						g++ $(CFLAGS) $^ $(LIBS) -o $@


openmp_indexer:    openmp_indexer.cpp    
						g++ $(CFLAGS) $^ $(LIBS) -o $@ -fopenmp

# distribute_indexer:    distribute_indexer.cpp    
# 						g++ $(CFLAGS) $^ $(LIBS) -o $@


clean:
						rm -f indexer openmp_indexer
