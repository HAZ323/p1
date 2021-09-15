#include <iostream>
#include <libgen.h>
#include <openssl/rsa.h>
#include <string>
#include <unistd.h>
#include <vector>

#include "../common/contextmanager.h"
#include "../common/crypto.h"
#include "../common/file.h"
#include "../common/net.h"
#include "../common/protocol.h"

#include "requests.h"

using namespace std;

/// These are all of the commands that the client supports, with descriptions
/// that are formatted nicely for printing in `usage()`.
static vector<pair<string, const char *>> commands = {
    {REQ_BYE, "             Force the server to stop"},
    {REQ_SAV, "             Instruct the server to save its data"},
    {REQ_REG, "             Register a new user"},
    {REQ_SET, " -1 [file]   Set user's data to the contents of the file"},
    {REQ_GET, " -1 [string] Get data for the provided user"},
    {REQ_ALL, " -1 [file]   Get all users' names, save to a file"}};

/// arg_t represents the command-line arguments to the client
struct arg_t {
  string server = "";   // The IP or hostname of the server
  int port = 0;         // The port on which to connect to the server
  string keyfile = "";  // The file for storing the server's public key
  string username = ""; // The user's name
  string userpass = ""; // The user's password
  string command = "";  // The command to execute
  string arg1 = "";     // The first argument to the command (if any)
  string arg2 = "";     // The second argument to the command (if any)

  /// Construct an arg_t from the command-line arguments to the program
  ///
  /// @param argc The number of command-line arguments passed to the program
  /// @param argv The list of command-line arguments
  ///
  /// @throw An integer exception (1) if an invalid argument is given, or if
  ///        `-h` is passed in
  arg_t(int argc, char **argv) {
    // First, use getopt to parse the command-line arguments
    long opt;
    while ((opt = getopt(argc, argv, "k:u:w:s:p:C:1:2:h")) != -1) {
      switch (opt) {
      case 'p': // port of server
        port = atoi(optarg);
        break;
      case 's': // hostname of server
        server = string(optarg);
        break;
      case 'k': // name of keyfile
        keyfile = string(optarg);
        break;
      case 'u': // username
        username = string(optarg);
        break;
      case 'w': // password
        userpass = string(optarg);
        break;
      case 'C': // command
        command = string(optarg);
        break;
      case '1': // first argument
        arg1 = string(optarg);
        break;
      case '2': // second argument
        arg2 = string(optarg);
        break;
      default: // on any error, print a help message.  This case subsumes `-h`
        throw 1;
        return;
      }
    }

    // Validate that the argument to `-C` is valid, and accompanied by `-1` and
    // `-2` as appropriate
    string args0[] = {REQ_BYE, REQ_SAV, REQ_REG};
    string args1[] = {
        REQ_SET,
        REQ_GET,
        REQ_ALL,
    };
    string args2[] = {};
    for (auto a : args0) {
      if (command == a) {
        if (arg1 != "" || arg2 != "")
          throw 1;
        return;
      }
    }
    for (auto a : args1) {
      if (command == a) {
        if (arg1 == "" || arg2 != "")
          throw 1;
        return;
      }
    }
    for (auto a : args2) {
      if (command == a) {
        if (arg1 == "" || arg2 == "")
          throw 1;
        return;
      }
    }
    throw 1;
  }

  /// Display a help message to explain how the command-line parameters for this
  /// program work
  ///
  /// @progname The name of the program
  static void usage(char *progname) {
    cout << basename(progname) << ": company user directory client\n\n";

    cout << " Required Configuration Parameters:\n";
    cout << "  -k [file]   The filename for storing the server's public key\n"
         << "  -u [string] The username to use for authentication\n"
         << "  -w [string] The password to use for authentication\n"
         << "  -s [string] IP address or hostname of server\n"
         << "  -p [int]    Port to use to connect to server\n"
         << "  -C [string] The command to execute (choose one from below)\n\n";

    cout << " Admin Commands (pass via -C):\n";
    for (int i = 0; i < 2; ++i)
      cout << "  " << commands[i].first << commands[i].second << endl;
    cout << endl;

    cout << " Auth Table Commands (pass via -C, with argument as -1)\n";
    for (int i = 2; i < 6; ++i)
      cout << "  " << commands[i].first << commands[i].second << endl;
    cout << endl;

    cout << " K/V Table Commands (pass via -C, with arguments as -1 and -2)\n";
    for (int i = 6; i < 11; ++i)
      cout << "  " << commands[i].first << commands[i].second << endl;
    cout << endl;

    cout << " K/V MRU Commands (pass via -C, with argument as -1)\n";
    for (int i = 11; i < 12; ++i)
      cout << "  " << commands[i].first << commands[i].second << endl;
    cout << endl;

    cout << " Map/Reduce Commands (pass via -C, with arguments as -1 and -2)\n";
    for (int i = 12; i < 14; ++i)
      cout << "  " << commands[i].first << commands[i].second << endl;
    cout << endl;

    cout << " Other Options:\n";
    cout << "  -h          Print help (this message)\n";
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

  // If we don't have the keyfile on disk, get the file from the server.  Once
  // we have the file, load the server's key.
  if (!file_exists(args->keyfile)) {
    int sd = connect_to_server(args->server, args->port);
    req_key(sd, args->keyfile);
    close(sd);
  }
  RSA *pubkey = load_pub(args->keyfile.c_str());
  ContextManager pkr([&]() { RSA_free(pubkey); });

  // Connect to the server and perform the appropriate operation
  int sd = connect_to_server(args->server, args->port);
  ContextManager sdc([&]() { close(sd); });

  // Figure out which command was requested, and run it
  vector<string> cmd = {REQ_REG, REQ_BYE, REQ_SET, REQ_GET, REQ_ALL, REQ_SAV};
  decltype(req_reg) *func[] = {req_reg, req_bye, req_set,
                               req_get, req_all, req_sav};
  for (size_t i = 0; i < cmd.size(); ++i)
    if (args->command == cmd[i])
      func[i](sd, pubkey, args->username, args->userpass, args->arg1,
              args->arg2);
  delete args;
  return 0;
}
