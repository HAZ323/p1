# Build a client and server from the reference solution, but use the student's
# server/storage.cc and server/sequentialhashtable_impl.h.

# The executables will have the suffix p1.file.exe.
EXESUFFIX = p1.file.exe

# Names for building the client:
CLIENT_MAIN     = client
CLIENT_CXX      = # no client/*.cc files needed for this build
CLIENT_COMMON   = # no common/*.cc files needed for this build
CLIENT_PROVIDED = client crypto err file net requests my_crypto

# Names for building the server:
SERVER_MAIN     = server
SERVER_CXX      = my_storage sequentialmap_factories
SERVER_COMMON   = # no common/*.cc files needed for this build
SERVER_PROVIDED = server responses parsing  crypto my_crypto err file \
                  net my_pool

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