CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

ALL:				crawler crawler_c

crawler:    crawler.c    
						gcc $(CFLAGS) $^ $(LIBS) -o $@ 

crawler_c:  crawler_c.c
						gcc $(CFLAGS) $^ $(LIBS) -o $@ -lpthread

clean:
						rm -f crawler crawler_c
