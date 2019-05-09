
#
# Small test programs
#

BINPROGS=\
	ds_array_test\
	ds_str_test\
	ds_ll_test\
	ds_hmap_test

BINOBS=\
	ds_array_test.o\
	ds_str_test.o\
	ds_ll_test.o\
	ds_hmap_test.o

OBS=\
	ds_array.o\
	ds_str.o\
	ds_ll.o\
	ds_hmap.o

HEADERS=\
	ds_array.h\
	ds_str.h\
	ds_ll.h\
	ds_hmap.h

ifndef GCC
	GCC = gcc
endif

CC=$(GCC)
CFLAGS=-W -Wall -c -std=c99 -pedantic
LD=$(GCC)
LDFLAGS= -lm
RM=rm -rfv

debug:	CFLAGS+= -ggdb
debug:	all

release:	CFLAGS+= -O3
release:	all

all:	$(BINPROGS)

$(BINPROGS):	$(BINOBS) $(OBS)
	$(LD) -o $@ $@.o $(OBS) $(LDFLAGS)

%.o:	%.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(BINOBS) $(BINPROGS) $(OBS)

