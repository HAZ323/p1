#include <cassert>
#include <iostream>
#include <string>

#include "../common/crypto.h"
#include "../common/net.h"

#include "responses.h"

using namespace std;

/// Respond to an ALL command by generating a list of all the usernames in the
/// Auth table and returning them, one per line.
///
/// @param sd      The socket onto which the result should be written
/// @param storage The Storage object, which contains the auth table
/// @param ctx     The AES encryption context
/// @param req     The unencrypted contents of the request
///
/// @return false, to indicate that the server shouldn't stop
bool handle_all(int sd, Storage *storage, EVP_CIPHER_CTX *ctx,
                const vector<uint8_t> &req) {
  cout << "responses.cc::handle_all() is not implemented\n";
  // NB: These asserts are to prevent compiler warnings
  assert(sd);
  assert(storage);
  assert(ctx);
  assert(req.size() > 0);
  return false;
}

/// Respond to a SET command by putting the provided data into the Auth table
///
/// @param sd      The socket onto which the result should be written
/// @param storage The Storage object, which contains the auth table
/// @param ctx     The AES encryption context
/// @param req     The unencrypted contents of the request
///
/// @return false, to indicate that the server shouldn't stop
bool handle_set(int sd, Storage *storage, EVP_CIPHER_CTX *ctx,
                const vector<uint8_t> &req) {
  cout << "responses.cc::handle_set() is not implemented\n";
  // NB: These asserts are to prevent compiler warnings
  assert(sd);
  assert(storage);
  assert(ctx);
  assert(req.size() > 0);
  return false;
}

/// Respond to a GET command by getting the data for a user
///
/// @param sd      The socket onto which the result should be written
/// @param storage The Storage object, which contains the auth table
/// @param ctx     The AES encryption context
/// @param req     The unencrypted contents of the request
///
/// @return false, to indicate that the server shouldn't stop
bool handle_get(int sd, Storage *storage, EVP_CIPHER_CTX *ctx,
                const vector<uint8_t> &req) {
  cout << "responses.cc::handle_get() is not implemented\n";
  // NB: These asserts are to prevent compiler warnings
  assert(sd);
  assert(storage);
  assert(ctx);
  assert(req.size() > 0);
  return false;
}

/// Respond to a REG command by trying to add a new user
///
/// @param sd      The socket onto which the result should be written
/// @param storage The Storage object, which contains the auth table
/// @param ctx     The AES encryption context
/// @param req     The unencrypted contents of the request
///
/// @return false, to indicate that the server shouldn't stop
bool handle_reg(int sd, Storage *storage, EVP_CIPHER_CTX *ctx,
                const vector<uint8_t> &req) {
  cout << "responses.cc::handle_reg() is not implemented\n";
  // NB: These asserts are to prevent compiler warnings
  assert(sd);
  assert(storage);
  assert(ctx);
  assert(req.size() > 0);
  return false;
}

/// In response to a request for a key, do a reliable send of the contents of
/// the pubfile
///
/// @param sd The socket on which to write the pubfile
/// @param pubfile A vector consisting of pubfile contents
///
/// @return false, to indicate that the server shouldn't stop
bool handle_key(int sd, const vector<uint8_t> &pubfile) {
  cout << "responses.cc::handle_key() is not implemented\n";
  // NB: These asserts are to prevent compiler warnings
  assert(sd);
  assert(pubfile.size() > 0);
  return false;
}

/// Respond to a BYE command by returning false, but only if the user
/// authenticates
///
/// @param sd      The socket onto which the result should be written
/// @param storage The Storage object, which contains the auth table
/// @param ctx     The AES encryption context
/// @param req     The unencrypted contents of the request
///
/// @return true, to indicate that the server should stop, or false on an error
bool handle_bye(int sd, Storage *storage, EVP_CIPHER_CTX *ctx,
                const vector<uint8_t> &req) {
  cout << "responses.cc::handle_bye() is not implemented\n";
  // NB: These asserts are to prevent compiler warnings
  assert(sd);
  assert(storage);
  assert(ctx);
  assert(req.size() > 0);
  return false;
}

/// Respond to a SAV command by persisting the file, but only if the user
/// authenticates
///
/// @param sd      The socket onto which the result should be written
/// @param storage The Storage object, which contains the auth table
/// @param ctx     The AES encryption context
/// @param req     The unencrypted contents of the request
///
/// @return false, to indicate that the server shouldn't stop
bool handle_sav(int sd, Storage *storage, EVP_CIPHER_CTX *ctx,
                const vector<uint8_t> &req) {
  cout << "responses.cc::handle_sav() is not implemented\n";
  // NB: These asserts are to prevent compiler warnings
  assert(sd);
  assert(storage);
  assert(ctx);
  assert(req.size() > 0);
  return false;
}
