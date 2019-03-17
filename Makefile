
#
# Small test programs
#

BINPROGS=\
	ds_array_test

BINOBS=\
	ds_array_test.o\
	ds_str_test.o

OBS=\
	ds_array.o\
	ds_str.o

HEADERS=\
	ds_array.h\
	ds_str.h


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

