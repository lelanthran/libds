
# ######################################################################
# First, some housekeeping to determine if we are running on a POSIX
# platform or on Windows

MAKEPROGRAM_EXE=$(findstring exe,$(MAKE))
MAKEPROGRAM_MINGW=$(findstring mingw,$(MAKE))
GITSHELL=$(findstring Git,$(SHELL))
GITSHELL+=$(findstring git,$(SHELL))

# TODO: Remember that freebsd might use not gmake/gnu-make; must add in
# some diagnostics so that user gets a message to install gnu make.

ifneq ($(MAKEPROGRAM_EXE),)
ifeq ($(strip $(GITSHELL)),)
$(error On windows this must be executed from the Git bash shell)
endif
	PLATFORM=Windows
	EXE_EXT=.exe
	LIB_EXT=.dll
	PLATFORM_LDFLAGS=-lmingw32 -lws2_32 -lmsvcrt -lgcc
endif

ifneq ($(MAKEPROGRAM_MINGW),)
ifeq ($(strip $(GITSHELL)),)
$(error On windows this must be executed from the Git bash shell)
endif
	PLATFORM=Windows
	EXE_EXT=.exe
	LIB_EXT=.dll
	PLATFORM_LDFLAGS=-lmingw32 -lws2_32 -lmsvcrt -lgcc
endif

# If neither of the above are true then we assume a working POSIX
# platform
ifeq ($(PLATFORM),)
	PLATFORM=POSIX
	EXE_EXT=.elf
	LIB_EXT=.so
endif



# ######################################################################
# Set the output directories, output filenames

OUTDIR=debug

ifneq (,$(findstring debug,$(MAKECMDGOALS)))
OUTDIR=debug
endif

ifneq (,$(findstring release,$(MAKECMDGOALS)))
OUTDIR=release
endif

VERSION=1.0.2
TARGET=$(shell $(GCC) -dumpmachine)
OUTLIB=$(OUTDIR)/lib/$(TARGET)
OUTBIN=$(OUTDIR)/bin/$(TARGET)
OUTOBS=$(OUTDIR)/obs/$(TARGET)
OUTDIRS=$(OUTLIB) $(OUTBIN) $(OUTOBS)


# ######################################################################
# Declare the final outputs
BINPROGS=\
	$(OUTBIN)/ds_array_test$(EXE_EXT)\
	$(OUTBIN)/ds_str_test$(EXE_EXT)\
	$(OUTBIN)/ds_ll_test$(EXE_EXT)\
	$(OUTBIN)/ds_hmap_test$(EXE_EXT)

DYNLIB=$(OUTLIB)/libds-$(VERSION)$(LIB_EXT)
STCLIB=$(OUTLIB)/libds-$(VERSION).a
DYNLNK_TARGET=libds-$(VERSION)$(LIB_EXT)
STCLNK_TARGET=libds-$(VERSION).a
DYNLNK_NAME=$(OUTLIB)/libds$(LIB_EXT)
STCLNK_NAME=$(OUTLIB)/libds.a


# ######################################################################
# Declare the intermediate outputs
BINOBS=\
	$(OUTOBS)/ds_array_test.o\
	$(OUTOBS)/ds_str_test.o\
	$(OUTOBS)/ds_ll_test.o\
	$(OUTOBS)/ds_hmap_test.o


OBS=\
	$(OUTOBS)/ds_array.o\
	$(OUTOBS)/ds_str.o\
	$(OUTOBS)/ds_ll.o\
	$(OUTOBS)/ds_hmap.o


HEADERS=\
	src/ds_array.h\
	src/ds_str.h\
	src/ds_ll.h\
	src/ds_hmap.h


# ######################################################################
# Declare the build programs
ifndef GCC
	GCC=gcc
endif
ifndef GXX
	GXX=g++
endif

CC=$(GCC)
CXX=$(GXX)

COMMONFLAGS=\
	-W -Wall -c -fPIC \
	-DPLATFORM=$(PLATFORM) -DPLATFORM_$(PLATFORM)

CFLAGS=$(COMMONFLAGS) -std=c99
CXXFLAGS=$(COMMONFLAGS) -std=c++x11
LD=$(GCC)
LDFLAGS= -lm $(PLATFORM_LDFLAGS)
AR=ar
ARFLAGS= rcs


.PHONY:	help real-help show real-show debug release clean-all

# ######################################################################
# All the conditional targets

help: real-help

debug:	CFLAGS+= -ggdb
debug:	CXXFLAGS+= -ggdb
debug:	all

release:	CFLAGS+= -O3
release:	CXXFLAGS+= -O3
release:	all

# ######################################################################
# Finally, build the system

real-help:
	@echo "Possible targets:"
	@echo "help:                This message."
	@echo "show:                Display all the variable values that will be"
	@echo "                     used during execution. Also 'show debug' or"
	@echo "                     'show release' works."
	@echo "debug:               Build debug binaries."
	@echo "release:             Build release binaries."
	@echo "clean-debug:         Clean a debug build (debug is ignored)."
	@echo "clean-release:       Clean a release build (release is ignored)."
	@echo "clean-all:           Clean everything."

real-all:	real-show  $(DYNLIB) $(STCLIB) $(BINPROGS)

all:	real-all
	mkdir -p include
	cp -Rv $(HEADERS) include
	ln -f -s $(STCLNK_TARGET) $(STCLNK_NAME)
	ln -f -s $(DYNLNK_TARGET) $(DYNLNK_NAME)
	cp $(OUTLIB)/* $(OUTDIR)/lib

real-show:	$(OUTDIRS)
	@echo "SHELL:        $(GITSHELL)"
	@echo "EXE_EXT:      $(EXE_EXT)"
	@echo "LIB_EXT:      $(LIB_EXT)"
	@echo "DYNLIB:       $(DYNLIB)"
	@echo "STCLIB:       $(STCLIB)"
	@echo "CC:           $(CC)"
	@echo "CXX:          $(CXX)"
	@echo "CFLAGS:       $(CFLAGS)"
	@echo "CXXFLAGS:     $(CXXFLAGS)"
	@echo "LD:           $(LD)"
	@echo "LDFLAGS:      $(LDFLAGS)"
	@echo "AR:           $(AR)"
	@echo "ARFLAGS:      $(ARFLAGS)"
	@echo ""
	@echo "PLATFORM:     $(PLATFORM)"
	@echo "TARGET:       $(TARGET)"
	@echo "OUTBIN:       $(OUTBIN)"
	@echo "OUTLIB:       $(OUTLIB)"
	@echo "OUTOBS:       $(OUTOBS)"
	@echo "OUTDIRS:      "
	@for X in $(OUTDIRS); do echo "              $$X"; done
	@echo "HEADERS:      "
	@for X in $(HEADERS); do echo "              $$X"; done
	@echo "OBS:          "
	@for X in $(OBS); do echo "              $$X"; done
	@echo "BINOBS:       "
	@for X in $(BINOBS); do echo "              $$X"; done
	@echo "BINPROGS:     "
	@for X in $(BINPROGS); do echo "              $$X"; done
	@echo "PWD:          $(PWD)"

show:	real-show
	@echo "Only target 'show' selected, ending now."
	@false

$(BINOBS) $(OBS):	$(OUTOBS)/%.o:	src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<


$(OUTBIN)/%.exe:	$(OUTOBS)/%.o $(OBS) $(OUTDIRS)
	$(LD) $< $(OBS) -o $@ $(LDFLAGS)

$(OUTBIN)/%.elf:	$(OUTOBS)/%.o $(OBS) $(OUTDIRS)
	$(LD) $< $(OBS) -o $@ $(LDFLAGS)

$(DYNLIB):	$(OBS)
	$(LD) -shared $^ -o $@ $(LDFLAGS)

$(STCLIB):	$(OBS)
	$(AR) $(ARFLAGS) $@ $^

$(OUTDIRS):
	mkdir -p $@

clean-release:
	rm -rfv release

clean-debug:
	rm -rfv debug

clean-all:	clean-release clean-debug


clean:
	echo Choose either clean-release or clean-debug

