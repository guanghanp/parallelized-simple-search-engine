CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl -lpthread

all:					test_page test_index lqtest1 lhtest1

test_page:    test_page.c 
							gcc $(CFLAGS) $^ $(LIBS) -o $@

test_index:   test_index.c
							gcc $(CFLAGS) $^ $(LIBS) -o $@

lqtest1:			lqtest1.c
							gcc $(CFLAGS) $^ $(LIBS) -o $@

lhtest1:			lhtest1.c
							gcc $(CFLAGS) $^ $(LIBS) -o $@

clean:
						rm -f test_page test_index lqtest1 lhtest1
