#include <cassert>
#include <cstring>
#include <iostream>
#include <openssl/rand.h>
#include <vector>

#include "../common/contextmanager.h"
#include "../common/crypto.h"
#include "../common/file.h"
#include "../common/net.h"
#include "../common/protocol.h"

#include "requests.h"

using namespace std;

/// req_key() writes a request for the server's key on a socket descriptor.
/// When it gets a key back, it writes it to a file.
///
/// @param sd      The open socket descriptor for communicating with the server
/// @param keyfile The name of the file to which the key should be written
void req_key(int sd, const string &keyfile) {
  cout << "requests.cc::req_key() is not implemented\n";
  // NB: These asserts are to prevent compiler warnings
  assert(sd);
  assert(keyfile.length() > 0);
}

/// req_reg() sends the REG command to register a new user
///
/// @param sd      The open socket descriptor for communicating with the server
/// @param pubkey  The public key of the server
/// @param user    The name of the user doing the request
/// @param pass    The password of the user doing the request
void req_reg(int sd, RSA *pubkey, const string &user, const string &pass,
             const string &, const string &) {
  cout << "requests.cc::req_reg() is not implemented\n";
  // NB: These asserts are to prevent compiler warnings
  assert(sd);
  assert(pubkey);
  assert(user.length() > 0);
  assert(pass.length() > 0);
}

/// req_bye() writes a request for the server to exit.
///
/// @param sd      The open socket descriptor for communicating with the server
/// @param pubkey  The public key of the server
/// @param user    The name of the user doing the request
/// @param pass    The password of the user doing the request
void req_bye(int sd, RSA *pubkey, const string &user, const string &pass,
             const string &, const string &) {
  cout << "requests.cc::req_bye() is not implemented\n";
  // NB: These asserts are to prevent compiler warnings
  assert(sd);
  assert(pubkey);
  assert(user.length() > 0);
  assert(pass.length() > 0);
}

/// req_sav() writes a request for the server to save its contents
///
/// @param sd      The open socket descriptor for communicating with the server
/// @param pubkey  The public key of the server
/// @param user    The name of the user doing the request
/// @param pass    The password of the user doing the request
void req_sav(int sd, RSA *pubkey, const string &user, const string &pass,
             const string &, const string &) {
  cout << "requests.cc::req_sav() is not implemented\n";
  // NB: These asserts are to prevent compiler warnings
  assert(sd);
  assert(pubkey);
  assert(user.length() > 0);
  assert(pass.length() > 0);
}

/// req_set() sends the SET command to set the content for a user
///
/// @param sd      The open socket descriptor for communicating with the server
/// @param pubkey  The public key of the server
/// @param user    The name of the user doing the request
/// @param pass    The password of the user doing the request
/// @param setfile The file whose contents should be sent
void req_set(int sd, RSA *pubkey, const string &user, const string &pass,
             const string &setfile, const string &) {
  cout << "requests.cc::req_set() is not implemented\n";
  // NB: These asserts are to prevent compiler warnings
  assert(sd);
  assert(pubkey);
  assert(user.length() > 0);
  assert(pass.length() > 0);
  assert(setfile.length() > 0);
}

/// req_get() requests the content associated with a user, and saves it to a
/// file called <user>.file.dat.
///
/// @param sd      The open socket descriptor for communicating with the server
/// @param pubkey  The public key of the server
/// @param user    The name of the user doing the request
/// @param pass    The password of the user doing the request
/// @param getname The name of the user whose content should be fetched
void req_get(int sd, RSA *pubkey, const string &user, const string &pass,
             const string &getname, const string &) {
  cout << "requests.cc::req_get() is not implemented\n";
  // NB: These asserts are to prevent compiler warnings
  assert(sd);
  assert(pubkey);
  assert(user.length() > 0);
  assert(pass.length() > 0);
  assert(getname.length() > 0);
}

/// req_all() sends the ALL command to get a listing of all users, formatted
/// as text with one entry per line.
///
/// @param sd      The open socket descriptor for communicating with the server
/// @param pubkey  The public key of the server
/// @param user    The name of the user doing the request
/// @param pass    The password of the user doing the request
/// @param allfile The file where the result should go
void req_all(int sd, RSA *pubkey, const string &user, const string &pass,
             const string &allfile, const string &) {
  cout << "requests.cc::req_all() is not implemented\n";
  // NB: These asserts are to prevent compiler warnings
  assert(sd);
  assert(pubkey);
  assert(user.length() > 0);
  assert(pass.length() > 0);
  assert(allfile.length() > 0);
}
