CFLAGS=-Wall -pedantic -std=c11 -I. -g
OFILES=webpage.o pageio.o

all:					$(OFILES)
							ar cr ../lib/libutils.a $(OFILES)

%.o:					%.c %.h
							gcc ${CFLAGS} -c $<
