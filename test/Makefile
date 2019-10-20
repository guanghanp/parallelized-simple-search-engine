CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all:					test_page test_index

test_page:    test_page.c 
							gcc $(CFLAGS) $^ $(LIBS) -o $@

test_index:   test_index.c
							gcc $(CFLAGS) $^ $(LIBS) -o $@

clean:
						rm -f test_page test_index
