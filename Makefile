
# ######################################################################
# First, some housekeeping to determine if we are running on a POSIX
# platform or on Windows

MAKEPROGRAM_EXE=$(findstring exe,$(MAKE))
MAKEPROGRAM_MINGW=$(findstring mingw,$(MAKE))

# TODO: Remember that freebsd might use not gmake/gnu-make; must add in
# some diagnostics so that user gets a message to install gnu make.

ifneq ($(MAKEPROGRAM_EXE),)
	# We are running on Windows for certain - not sure if cygwin or not
	# TODO: Issue a diagnostic and stop if git bash shell is not found
	PLATFORM=Windows
	EXE_EXT=.exe
	LIB_EXT=.dll
endif

ifneq ($(MAKEPROGRAM_MINGW),)
	# We are running on Windows/Mingw, no uncertainty about build system
	# TODO: Issue a diagnostic and stop if git bash shell is not found
	PLATFORM=Windows
	EXE_EXT=.exe
	LIB_EXT=.dll
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

OUTDIR=release

VERSION=0.0.1
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

RELEASE_LIB=$(OUTLIB)/libds-$(VERSION)$(LIB_EXT)

# TODO: Fix this so that the correct output is chosen. Also need to
# generate static libs as well.
LIB=$(OUTLIB)/libds-$(VERSION)$(LIB_EXT)


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
	-W -Wall -c -fPIC -Iinclude\
	-DPLATFORM=$(PLATFORM) -DPLATFORM_$(PLATFORM)

CFLAGS=$(COMMONFLAGS)
CXXFLAGS=$(COMMONFLAGS)
LD=$(GCC)
LDFLAGS= -lm
AR=ar


.PHONY:	show debug release

# ######################################################################
# All the conditional targets
debug:	LIB=$(DEBUG_LIB)
debug:	CFLAGS+= -ggdb
debug:	CXXFLAGS+= -ggdb
debug:	all

release:	LIB=$(RELEASE_LIB)
release:	CFLAGS+= -O3
release:	CXXFLAGS+= -O3
release:	all

# ######################################################################
# Finally, build the system
real-all:	show  $(LIB) $(BINPROGS)

all:	real-all
	mkdir -p include
	cp -Rv $(HEADERS) include

show:	$(OUTDIRS)
	@echo "EXE_EXT:      $(EXE_EXT)"
	@echo "LIB_EXT:      $(LIB_EXT)"
	@echo "BINPROGS:     $(BINPROGS)"
	@echo "LIB:          $(LIB)"
	@echo "CC:           $(CC)"
	@echo "CXX:          $(CXX)"
	@echo "CFLAGS:       $(CFLAGS)"
	@echo "CXXFLAGS:     $(CXXFLAGS)"
	@echo "LD:           $(LD)"
	@echo "LDFLAGS:      $(LDFLAGS)"
	@echo "AR:           $(AR)"
	@echo ""
	@echo "PLATFORM:     $(PLATFORM)"
	@echo "TARGET:       $(TARGET)"
	@echo "OUTBIN:       $(OUTBIN)"
	@echo "OUTLIB:       $(OUTLIB)"
	@echo "OUTOBS:       $(OUTOBS)"
	@echo "OUTDIRS:      $(OUTDIRS)"
	@echo "HEADERS:      $(HEADERS)"
	@echo "OBS:          $(OBS)"
	@echo "BINOBS:       $(BINOBS)"
	@echo "PWD:          $(PWD)"


$(BINOBS) $(OBS):	$(OUTOBS)/%.o:	src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<


$(BINPROGS):	$(OUTBIN)/%.elf:	$(OUTOBS)/%.o $(OBS) $(OUTDIRS)
	$(LD) $(OBS) $< -o $@ $(LDFLAGS)

$(LIB):	$(OBS)
	$(LD) -shared $^ -o $@ $(LDFLAGS)

$(OUTDIRS):
	mkdir -p $@

clean:
	rm -rfv $(BINOBS) $(OBS)

veryclean:
	rm -rfv $(OUTDIRS) lib bin obs include

