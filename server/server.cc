#include <iostream>
#include <libgen.h>
#include <openssl/rsa.h>
#include <string>
#include <unistd.h>

#include "../common/contextmanager.h"
#include "../common/crypto.h"
#include "../common/err.h"
#include "../common/file.h"
#include "../common/net.h"
#include "../common/pool.h"

#include "parsing.h"
#include "storage.h"

using namespace std;

/// arg_t represents the command-line arguments to the server
struct arg_t {
  int port;                    // The port on which to listen
  string datafile;             // The file for storing all data
  string keyfile;              // The file holding the AES key
  int threads = 1;             // Number of threads for the server to use
  size_t num_buckets = 1024;   // Number of buckets for the server's hash tables
  size_t quota_interval = 60;  // Seconds over which a quota is enforced
  size_t quota_up = 1048576;   // K/V upload quota (bytes/interval)
  size_t quota_down = 1048576; // K/V download quota (bytes/interval)
  size_t quota_req = 16;       // K/V request quota (requests/interval)
  size_t top_size = 4;         // Number of keys to track for TOP queries
  string admin_name = "";      // Name of the administrator

  /// Construct an arg_t from the command-line arguments to the program
  ///
  /// @param argc The number of command-line arguments passed to the program
  /// @param argv The list of command-line arguments
  ///
  /// @throw An intmd5eger exception (1) if an invalid argument is given, or if
  ///        `-h` is passed in
  arg_t(int argc, char **argv) {
    long opt;
    while ((opt = getopt(argc, argv, "p:f:k:ht:b:i:u:d:r:o:a:")) != -1) {
      switch (opt) {
      case 'p':
        port = atoi(optarg);
        break;
      case 'f':
        datafile = string(optarg);
        break;
      case 'k':
        keyfile = string(optarg);
        break;
      case 't':
        threads = atoi(optarg);
        break;
      case 'b':
        num_buckets = atoi(optarg);
        break;
      case 'i':
        quota_interval = atoi(optarg);
        break;
      case 'u':
        quota_up = atoi(optarg);
        break;
      case 'd':
        quota_down = atoi(optarg);
        break;
      case 'r':
        quota_req = atoi(optarg);
        break;
      case 'o':
        top_size = atoi(optarg);
        break;
      case 'a':
        admin_name = string(optarg);
        break;
      default: // on any error, print a help message.  This case subsumes `-h`
        throw 1;
        return;
      }
    }
  }

  /// Display a help message to explain how the command-line parameters for this
  /// program work
  ///
  /// @progname The name of the program
  static void usage(char *progname) {
    cout << basename(progname) << ": company user directory server\n"
         << "  -p [int]    Port on which to listen for incoming connections\n"
         << "  -f [string] File for storing all data\n"
         << "  -k [string] Basename of file for storing the server's RSA keys\n"
         << "  -t [int]    # of threads that server should use\n"
         << "  -b [int]    # of buckets for the server's hash tables\n"
         << "  -i [int]    Quota interval (seconds)\n"
         << "  -u [int]    Upload quota (MB/interval)\n"
         << "  -d [int]    Download quota (MB/interval)\n"
         << "  -r [int]    Request quota (requests/interval)\n"
         << "  -o [int]    Size of the TOP key cache\n"
         << "  -a [string] Specify name of admin user\n"
         << "  -h          Print help (this message)\n";
  }
};

int main(int argc, char **argv) {
  // Parse the command-line arguments
  //
  // NB: It would be better not to put the arg_t on the heap, but then we'd need
  //     an extra level of nesting for the body of the rest of this function.
  arg_t *args;
  try {
    args = new arg_t(argc, argv);
  } catch (int i) {
    arg_t::usage(argv[0]);
    return 1;
  }

  // print the configuration
  cout << "Listening on port " << args->port << " using (key/data) = ("
       << args->keyfile << ", " << args->datafile << ")\n";

  // If the key files don't exist, create them and then load the private key.
  RSA *pri = init_RSA(args->keyfile);
  if (pri == nullptr)
    return -1;
  ContextManager r([&]() { RSA_free(pri); });

  // load the public key file contents
  auto pub = load_entire_file(args->keyfile + ".pub");
  if (pub.size() == 0)
    return 1;

  // If the data file exists, load the data into a Storage object.  Otherwise,
  // create an empty Storage object.
  Storage *storage = storage_factory(
      args->datafile, args->num_buckets, args->quota_up, args->quota_down,
      args->quota_req, args->quota_interval, args->top_size, args->admin_name);
  auto res = storage->load_file();
  if (!res.succeeded)
    return err(1, res.msg.c_str());
  cout << res.msg << endl;

  // Start listening for connections.
  int sd = create_server_socket(args->port);
  ContextManager csd([&]() { close(sd); });
  // Create a thread pool that will invoke parse_request (from a pool thread)
  // each time a new socket is given to it.
  thread_pool *pool = pool_factory(args->threads, [&](int sd) {
    return parse_request(sd, pri, pub, storage);
  });

  // Start accepting connections and passing them to the pool.
  accept_client(sd, *pool);

  // The program can't exit until all threads in the pool are done.
  pool->await_shutdown();
  storage->shutdown();
  delete pool;
  delete args;
  delete storage;
  cout << "Server terminated\n";
}
