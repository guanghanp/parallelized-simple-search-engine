CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g -lpthread
LIBS=-lutils -lcurl

ALL:				crawler crawler_c

crawler:    crawler.c    
						gcc $(CFLAGS) $^ $(LIBS) -o $@

crawler_c:  crawler_c.c
						gcc $(CFLAGS) $^ $(LIBS) -o $@

clean:
						rm -f crawler
