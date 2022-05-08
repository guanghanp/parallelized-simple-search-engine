CFLAGS=-Wall -pedantic -std=c11 -I. -g
OFILES=webpage.o pageio.o

all:					$(OFILES)
							ar cr ../lib/libutils.a $(OFILES)

pageio.o:    pageio.cpp pageio.hpp    
						g++ -Wall -pedantic -I. -g -c $<
webpage.o:   webpage.c webpage.h
						gcc ${CFLAGS} -c $<

clean:
			rm -f webpage.o pageio.o
# %.o:					%.c %.h
# 							gcc ${CFLAGS} -c $<
