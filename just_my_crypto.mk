# Build a client and server using only the student's my_crypto.cc

# The executables will have the suffix .exe
EXESUFFIX = exe

# Names for building the client:
CLIENT_MAIN     = client
CLIENT_CXX      = 
CLIENT_COMMON   = my_crypto
CLIENT_PROVIDED = crypto err file net client requests

# Names for building the server
SERVER_MAIN     = server
SERVER_CXX      = 
SERVER_COMMON   = my_crypto
SERVER_PROVIDED = crypto err file net my_pool server responses parsing \
				  my_storage sequentialmap_factories

# Pull in the common build rules
include common.mk