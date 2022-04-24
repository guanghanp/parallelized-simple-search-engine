CFLAGS=-Wall -pedantic -I../utils -L../lib -g
LIBS=-lutils -lcurl

ALL:				crawler crawler_c

crawler:    crawler.cpp    
						g++ $(CFLAGS) $^ $(LIBS) -o $@ 

crawler_c:  crawler_c.c
						gcc $(CFLAGS) $^ $(LIBS) -o $@ -lpthread

clean:
						rm -f crawler crawler_c
