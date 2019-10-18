CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

test_page:    test_page.c 
							gcc $(CFLAGS) $^ $(LIBS) -o $@

clean:
						rm -f test_page
