CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

querier:    querier.c    
						gcc $(CFLAGS) $^ $(LIBS) -o $@

clean:
						rm -f querier
