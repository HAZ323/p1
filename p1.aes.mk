# Build a client and server from the reference solution, but use the student's
# crypto.cc.

# The executables will have the suffix p1.aestest.exe
EXESUFFIX = p1.aes.exe

# Names for building the client:
CLIENT_MAIN     = client
CLIENT_CXX      = # no client/*.cc files needed for this build
CLIENT_COMMON   = crypto my_crypto
CLIENT_PROVIDED = client requests err file net

# Names for building the server:
SERVER_MAIN     = server
SERVER_CXX      = # no server/*.cc files needed for this build
SERVER_COMMON   = crypto my_crypto
SERVER_PROVIDED = server responses parsing my_storage  sequentialmap_factories \
                  err file net my_pool

# Names for building the benchmark executable
BENCH_MAIN = # No benchmarks are built by this build
BENCH_CXX  = # No benchmarks are built by this build

# Names for building shared objects (one .so per SO_CXX)
SO_CXX    = # No shared objects are built by this build
SO_COMMON = # No shared objects are built by this build

# All warnings should be treated as errors
CXXEXTRA = -Werror

# Pull in the common build rules
include common.mk