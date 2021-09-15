# Build a client and server using the student's parsing.cc

# The executables will have the suffix .exe
EXESUFFIX = exe

# Names for building the client:
CLIENT_MAIN     = client
CLIENT_CXX      = 
CLIENT_COMMON   = 
CLIENT_PROVIDED = client requests crypto err file net my_crypto

# Names for building the server
SERVER_MAIN     = server
SERVER_CXX      = parsing
SERVER_COMMON   = 
SERVER_PROVIDED = server responses my_storage sequentialmap_factories crypto \
                  err file net my_pool my_crypto

# NB: This Makefile does not add extra CXXFLAGS

# Pull in the common build rules
include common.mk