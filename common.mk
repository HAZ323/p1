# Configure g++:
# - Default to 64 bits, but allow overriding on the command line
# - Use CXXEXTRA and LDEXTRA to allow additional flags
BITS     ?= 64
CXX       = g++
LD        = g++
CXXFLAGS  = -MMD -O3 -m$(BITS) -ggdb -std=c++17 -Wall -Wextra -fPIC $(CXXEXTRA)
LDFLAGS   = -m$(BITS) -lpthread -lcrypto -ldl $(LDEXTRA)

# Hard-coded name of the solutions folder
SDIR = solutions

# Give a name to the output folder, and ensure it is created before any
# compilation happens
ODIR      := ./obj$(BITS)
OUTFOLDER := $(shell mkdir -p $(ODIR))

# Names of all the .o files needed to create the client executable
CLIENT_O  = $(patsubst %, $(ODIR)/%.o, $(CLIENT_CXX))
CLIENT_O += $(patsubst %, $(ODIR)/%.o, $(CLIENT_COMMON))
CLIENT_O += $(patsubst %, $(SDIR)/%.o, $(CLIENT_PROVIDED))

# Names of all the .o files needed to create the server executable
SERVER_O  = $(patsubst %, $(ODIR)/%.o, $(SERVER_CXX))
SERVER_O += $(patsubst %, $(ODIR)/%.o, $(SERVER_COMMON))
SERVER_O += $(patsubst %, $(SDIR)/%.o, $(SERVER_PROVIDED))

# Names of all the .o and .exe files to build
OFILES   = $(CLIENT_O) $(SERVER_O)
EXEFILES = $(patsubst %, $(ODIR)/%.$(EXESUFFIX), $(CLIENT_MAIN) $(SERVER_MAIN))

# Names of all .d files, so we can get dependencies right
DFILES     = $(patsubst %.o, %.d, $(OFILES))

# Build 'all' by default, and don't clobber .o files after each build
.DEFAULT_GOAL = all
.PRECIOUS: $(OFILES)
.PHONY: all clean

# Typing 'make' should build all the .exe files
all: $(EXEFILES)

# Typing 'make clean' should clean up by removing $(OUTFOLDER)
clean:
	@echo Cleaning up...
	@rm -rf $(ODIR)

# Rules for building .o files
$(ODIR)/%.o: client/%.cc
	@echo "[CXX] $< --> $@"
	@$(CXX) $< -o $@ -c $(CXXFLAGS)
$(ODIR)/%.o: server/%.cc
	@echo "[CXX] $< --> $@"
	@$(CXX) $< -o $@ -c $(CXXFLAGS)
$(ODIR)/%.o: common/%.cc
	@echo "[CXX] $< --> $@"
	@$(CXX) $< -o $@ -c $(CXXFLAGS)

# Rules for building executables
$(ODIR)/$(CLIENT_MAIN).$(EXESUFFIX): $(CLIENT_O)
	@echo "[LD] $^ --> $@"
	@$(CXX) $^ -o $@ $(LDFLAGS)
$(ODIR)/$(SERVER_MAIN).$(EXESUFFIX): $(SERVER_O)
	@echo "[LD] $^ --> $@"
	@$(CXX) $^ -o $@ $(LDFLAGS)

# Include any dependencies we generated previously
-include $(DFILES)
