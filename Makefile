# ######################################################################
# We include the user-specified variables first
include build.config
ifndef PROJNAME
$(error $$PROJNAME not defined. Is the 'build.config' file missing?)
endif

VERSION?=0.0.0

# ######################################################################
# Set some colours for $(ECHO) to use
NONE:=\e[0m
INV:=\e[7m
RED:=\e[31m
GREEN:=\e[32m
BLUE:=\e[34m
CYAN:=\e[36m
YELLOW:=\e[33m


# ######################################################################
# We record the start time, for determining how long the build took
START_TIME:=$(shell date +"%s")


# ######################################################################
# Some housekeeping to determine if we are running on a POSIX
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
	HOME:=$(HOMEDRIVE)$(HOMEPATH)
	PLATFORM:=Windows
	EXE_EXT:=.exe
	LIB_EXT:=.dll
	PLATFORM_LDFLAGS:=--L$(HOME)/lib lmingw32 -lws2_32 -lmsvcrt -lgcc
	PLATFORM_CFLAGS:= -D__USE_MINGW_ANSI_STDIO
	ECHO:=echo -e
endif

ifneq ($(MAKEPROGRAM_MINGW),)
ifeq ($(strip $(GITSHELL)),)
$(error On windows this must be executed from the Git bash shell)
endif
	HOME:=$(HOMEDRIVE)$(HOMEPATH)
	PLATFORM:=Windows
	EXE_EXT:=.exe
	LIB_EXT:=.dll
	PLATFORM_LDFLAGS:=-L$(HOME)/lib -lmingw32 -lws2_32 -lmsvcrt -lgcc
	PLATFORM_CFLAGS:= -D__USE_MINGW_ANSI_STDIO
	ECHO:=echo -e
endif

# If neither of the above are true then we assume a working POSIX
# platform
ifeq ($(PLATFORM),)
	PLATFORM:=POSIX
	EXE_EXT:=.elf
	LIB_EXT:=.so
	PLATFORM_LDFLAGS:= -lpthread -ldl
	ECHO:=echo
	REAL_SHOW:=real-show
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

TARGET:=$(shell $(GCC) -dumpmachine)
OUTLIB:=$(OUTDIR)/lib/$(TARGET)
OUTBIN:=$(OUTDIR)/bin/$(TARGET)
OUTOBS:=$(OUTDIR)/obs/$(TARGET)
OUTDIRS:=$(OUTLIB) $(OUTBIN) $(OUTOBS) include


# ######################################################################
# Declare the final outputs
BINPROGS:=\
	$(foreach fname,$(MAIN_PROGRAM_CSOURCEFILES),$(OUTBIN)/$(fname)$(EXE_EXT))\
	$(foreach fname,$(MAIN_PROGRAM_CPPSOURCEFILES),$(OUTBIN)/$(fname)$(EXE_EXT))

DYNLIB:=$(OUTLIB)/lib$(PROJNAME)-$(VERSION)$(LIB_EXT)
STCLIB:=$(OUTLIB)/lib$(PROJNAME)-$(VERSION).a
DYNLNK_TARGET:=lib$(PROJNAME)-$(VERSION)$(LIB_EXT)
STCLNK_TARGET:=lib$(PROJNAME)-$(VERSION).a
DYNLNK_NAME:=$(OUTLIB)/lib$(PROJNAME)$(LIB_EXT)
STCLNK_NAME:=$(OUTLIB)/lib$(PROJNAME).a


# ######################################################################
# Declare the intermediate outputs
BIN_COBS:=\
	$(foreach fname,$(MAIN_PROGRAM_CSOURCEFILES),$(OUTOBS)/$(fname).o)

BIN_CPPOBS:=\
	$(foreach fname,$(MAIN_PROGRAM_CPPSOURCEFILES),$(OUTOBS)/$(fname).o)

BINOBS:=$(BIN_COBS) $(BIN_CPPOBS)

COBS:=\
	$(foreach fname,$(LIBRARY_OBJECT_CSOURCEFILES),$(OUTOBS)/$(fname).o)

CPPOBS:=\
	$(foreach fname,$(LIBRARY_OBJECT_CPPSOURCEFILES),$(OUTOBS)/$(fname).o)

OBS:=$(COBS) $(CPPOBS)
ALL_OBS:=$(OBS) $(BINOBS)
DEPS:=\
	$(subst $(OUTOBS),src,$(subst .o,.d,$(ALL_OBS)))

# ######################################################################
# Find all the source files so that we can do dependencies properly
SOURCES:=\
	$(shell find . | grep -E "\.(c|cpp)\$$")

# ######################################################################
# Declare the build programs
ifndef GCC
	GCC=gcc
endif
ifndef GXX
	GXX=g++
endif
ifndef LD_PROG
	LD_PROG=gcc
endif
ifndef LD_LIB
	LD_LIB=gcc
endif

# ######################################################################
# Declare all the flags we need to compile and link
CC:=$(GCC)
CXX:=$(GXX)
PROG_LD=$(GCC_LD_PROG)
LIB_LD=$(GCC_LD_LIB)

INCLUDE_DIRS:=\
	$(foreach ipath,$(INCLUDE_PATHS),-I$(ipath))

LIBDIRS:=\
	$(foreach lpath,$(LIBRARY_PATHS),-L$(lpath))

LIBFILES:=\
	$(foreach lfile,$(LIBRARY_FILES),-l$(lfile))

COMMONFLAGS:=\
	$(EXTRA_COMPILER_FLAGS)\
	-W -Wall -c -fPIC \
	-DPLATFORM=$(PLATFORM) -DPLATFORM_$(PLATFORM) \
	-D$(PROJNAME)_version='"$(VERSION)"'\
	$(PLATFORM_CFLAGS)\
	$(INCLUDE_DIRS)

CFLAGS:=$(COMMONFLAGS) $(EXTRA_CFLAGS)
CXXFLAGS:=$(COMMONFLAGS) $(EXTRA_CXXFLAGS)
LD:=$(GCC)
LDFLAGS:= $(LIBDIRS) $(LIBFILES) -lm $(PLATFORM_LDFLAGS)
AR:=ar
ARFLAGS:= rcs


.PHONY:	help real-help show real-show debug release clean-all deps

# ######################################################################
# All the conditional targets

help: real-help

debug:	CFLAGS+= -ggdb -DDEBUG
debug:	CXXFLAGS+= -ggdb -DDEBUG
debug:	all

release:	CFLAGS+= -O3
release:	CXXFLAGS+= -O3
release:	all

# ######################################################################
# Finally, build the system

real-help:
	@$(ECHO) "Possible targets:"
	@$(ECHO) "help:                This message."
	@$(ECHO) "show:                Display all the variable values that will be"
	@$(ECHO) "                     used during execution. Also 'show debug' or"
	@$(ECHO) "                     'show release' works."
	@$(ECHO) "deps:                Make the dependencies only."
	@$(ECHO) "debug:               Build debug binaries."
	@$(ECHO) "release:             Build release binaries."
	@$(ECHO) "clean-debug:         Clean a debug build (release is ignored)."
	@$(ECHO) "clean-release:       Clean a release build (debug is ignored)."
	@$(ECHO) "clean-all:           Clean everything."


real-all:	$(OUTDIRS) $(DYNLIB) $(STCLIB) $(BINPROGS)

all:	real-all
	@$(ECHO) "[$(CYAN)Soft linking$(NONE)]    [$(STCLNK_TARGET)]"
	@ln -f -s $(STCLNK_TARGET) $(STCLNK_NAME)
	@$(ECHO) "[$(CYAN)Soft linking$(NONE)]    [$(DYNLNK_TARGET)]"
	@ln -f -s $(DYNLNK_TARGET) $(DYNLNK_NAME)
	@$(ECHO) "[$(CYAN)Copying$(NONE)     ]    [ -> $(OUTDIR)/lib]"
	@cp $(OUTLIB)/* $(OUTDIR)/lib
	@$(ECHO) "[$(CYAN)Copying$(NONE)     ]    [ -> ./include/]"
	@cp -R $(HEADERS) include
	@$(ECHO) "$(INV)$(YELLOW)Build completed: `date`$(NONE)"
	@$(ECHO) "$(YELLOW)Total build time:  $$((`date +"%s"` - $(START_TIME)))s"\
		"$(NONE)"


real-show:
	@$(ECHO) "$(GREEN)HOME$(NONE)         $(HOME)"
	@$(ECHO) "$(GREEN)SHELL$(NONE)        $(SHELL)"
	@$(ECHO) "$(GREEN)EXE_EXT$(NONE)      $(EXE_EXT)"
	@$(ECHO) "$(GREEN)LIB_EXT$(NONE)      $(LIB_EXT)"
	@$(ECHO) "$(GREEN)DYNLIB$(NONE)       $(DYNLIB)"
	@$(ECHO) "$(GREEN)STCLIB$(NONE)       $(STCLIB)"
	@$(ECHO) "$(GREEN)CC$(NONE)           $(CC)"
	@$(ECHO) "$(GREEN)CXX$(NONE)          $(CXX)"
	@$(ECHO) "$(GREEN)CFLAGS$(NONE)       $(CFLAGS)"
	@$(ECHO) "$(GREEN)CXXFLAGS$(NONE)     $(CXXFLAGS)"
	@$(ECHO) "$(GREEN)LD_LIB$(NONE)       $(LD_LIB)"
	@$(ECHO) "$(GREEN)LD_PROG$(NONE)      $(LD_PROG)"
	@$(ECHO) "$(GREEN)LDFLAGS$(NONE)      $(LDFLAGS)"
	@$(ECHO) "$(GREEN)AR$(NONE)           $(AR)"
	@$(ECHO) "$(GREEN)ARFLAGS$(NONE)      $(ARFLAGS)"
	@$(ECHO) "$(GREEN)"
	@$(ECHO) "$(GREEN)PLATFORM$(NONE)     $(PLATFORM)"
	@$(ECHO) "$(GREEN)TARGET$(NONE)       $(TARGET)"
	@$(ECHO) "$(GREEN)OUTBIN$(NONE)       $(OUTBIN)"
	@$(ECHO) "$(GREEN)OUTLIB$(NONE)       $(OUTLIB)"
	@$(ECHO) "$(GREEN)OUTOBS$(NONE)       $(OUTOBS)"
	@$(ECHO) "$(GREEN)OUTDIRS$(NONE)      "
	@for X in $(OUTDIRS); do $(ECHO) "              $$X"; done
	@$(ECHO) "$(GREEN)DEPS$(NONE)      "
	@for X in $(DEPS); do $(ECHO) "              $$X"; done
	@$(ECHO) "$(GREEN)HEADERS$(NONE)      "
	@for X in $(HEADERS); do $(ECHO) "              $$X"; done
	@$(ECHO) "$(GREEN)COBS$(NONE)          "
	@for X in $(COBS); do $(ECHO) "              $$X"; done
	@$(ECHO) "$(GREEN)CPPOBS$(NONE)          "
	@for X in $(CPPOBS); do $(ECHO) "              $$X"; done
	@$(ECHO) "$(GREEN)OBS$(NONE)          "
	@for X in $(OBS); do $(ECHO) "              $$X"; done
	@$(ECHO) "$(GREEN)BIN_COBS$(NONE)       "
	@for X in $(BIN_COBS); do $(ECHO) "              $$X"; done
	@$(ECHO) "$(GREEN)BIN_CPPOBS$(NONE)       "
	@for X in $(BIN_CPPOBS); do $(ECHO) "              $$X"; done
	@$(ECHO) "$(GREEN)BINOBS$(NONE)       "
	@for X in $(BINOBS); do $(ECHO) "              $$X"; done
	@$(ECHO) "$(GREEN)BINPROGS$(NONE)     "
	@for X in $(BINPROGS); do $(ECHO) "              $$X"; done
	@$(ECHO) "$(GREEN)SOURCES$(NONE)     "
	@for X in $(SOURCES); do $(ECHO) "              $$X"; done
	@$(ECHO) "$(GREEN)PWD$(NONE)          $(PWD)"

show:	real-show
	@$(ECHO) "Only target 'show' selected, ending now."
	@false

$(DEPS): $(HEADERS)

deps: $(DEPS)

src/%.d: src/%.c
	@$(ECHO) "[$(RED)Dependency$(NONE)  ]    [$@]"
	@$(CC) $(CFLAGS) -MM -MF $@ -MT $(OUTOBS)/$*.o $< ||\
		($(ECHO) "$(INV)$(RED)[Depend failure ]   [$@]$(NONE)" ; exit 127)

src/%.d: src/%.cpp
	@$(ECHO) "[$(RED)Dependency$(NONE)  ]    [$@]"
	@$(CXX) $(CXXFLAGS) -MM -MF $@ $< ||\
		($(ECHO) "$(INV)$(RED)[Depend failure ]   [$@]$(NONE)" ; exit 127)

$(BIN_COBS) $(COBS):	$(OUTOBS)/%.o:	src/%.c src/%.d
	@$(ECHO) "[$(BLUE)Building$(NONE)    ]    [$@]"
	@$(CC) $(CFLAGS) -o $@ $< ||\
		($(ECHO) "$(INV)$(RED)[Compile failure]   [$@]$(NONE)" ; exit 127)

$(BIN_CPPOBS) $(CPPOBS):	$(OUTOBS)/%.o:	src/%.cpp src/%.d
	@$(ECHO) "[$(BLUE)Building$(NONE)    ]    [$@]"
	@$(CXX) $(CXXFLAGS) -o $@ $< ||\
		($(ECHO) "$(INV)$(RED)[Compile failure]   [$@]$(NONE)" ; exit 127)

$(OUTBIN)/%.exe:	$(OUTOBS)/%.o $(OBS)
	@$(ECHO) "[$(GREEN)Linking$(NONE)     ]    [$@]"
	@$(LD_PROG) $< $(OBS) -o $@ $(LDFLAGS) $(EXTRA_PROG_LDFLAGS) ||\
		($(ECHO) "$(INV)$(RED)[Link failure]   [$@]$(NONE)" ; exit 127)

$(OUTBIN)/%.elf:	$(OUTOBS)/%.o $(OBS)
	@$(ECHO) "[$(GREEN)Linking$(NONE)     ]    [$@]"
	@$(LD_PROG) $< $(OBS) -o $@ $(LDFLAGS) $(EXTRA_PROG_LDFLAGS) ||\
		($(ECHO) "$(INV)$(RED)[Link failure]   [$@]$(NONE)" ; exit 127)

$(DYNLIB):	$(OBS)
	@$(ECHO) "[$(GREEN)Linking$(NONE)     ]    [$@]"
	@$(LD_LIB) -shared $^ -o $@ $(LDFLAGS) $(EXTRA_LIB_LDFLAGS) ||\
		($(ECHO) "$(INV)$(RED)[Link failure]   [$@]$(NONE)" ; exit 127)

$(STCLIB):	$(OBS)
	@$(ECHO) "[$(GREEN)Linking$(NONE)     ]    [$@]"
	@$(AR) $(ARFLAGS) $@ $^ ||\
		($(ECHO) "$(INV)$(RED)[Link failure]   [$@]$(NONE)" ; exit 127)

$(OUTDIRS):
	@$(ECHO) "[$(CYAN)Creating dir$(NONE)]    [$@]"
	@mkdir -p $@ ||\
		($(ECHO) "$(INV)$(RED)[mkdir failure]   [$@]$(NONE)" ; exit 127)

clean-release:
	@rm -rfv release

clean-debug:
	@rm -rfv debug

clean-all:	clean-release clean-debug
	@rm -rfv include
	@rm -rfv `find . | grep "\.d$$"`

clean:
	@$(ECHO) Choose either clean-release or clean-debug

include $(DEPS)
