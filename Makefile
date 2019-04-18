
#
# Small test programs
#

BINPROGS=\
	ds_array_test\
	ds_str_test\
	ds_ll_test

BINOBS=\
	ds_array_test.o\
	ds_str_test.o\
	ds_ll_test.o

OBS=\
	ds_array.o\
	ds_str.o\
	ds_ll.o

HEADERS=\
	ds_array.h\
	ds_str.h\
	ds_ll.h


CC=gcc
CFLAGS=-W -Wall -c -std=c99 -pedantic
LD=gcc
LDFLAGS=
RM=rm -rfv

debug:	CFLAGS+= -ggdb
debug:	all

release:	CFLAGS+= -O3
release:	all

all:	$(BINPROGS)

$(BINPROGS):	$(BINOBS) $(OBS)
	$(LD) $(LDFLAGS) -o $@ $@.o $(OBS)

%.o:	%.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(BINOBS) $(BINPROGS)

