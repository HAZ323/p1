# Build a client and server using only the student's sequentialmap.h and
# sequentialmap_factories.cc

# The executables will have the suffix .exe
EXESUFFIX = exe

# Names for building the client:
CLIENT_MAIN     = client
CLIENT_CXX      = 
CLIENT_COMMON   = 
CLIENT_PROVIDED = client requests crypto err file net my_crypto

# Names for building the server
SERVER_MAIN     = server
SERVER_CXX      = sequentialmap_factories
SERVER_COMMON   = 
SERVER_PROVIDED = server responses parsing my_storage \
                  crypto err file net my_pool my_crypto

# Pull in the common build rules
include common.mk