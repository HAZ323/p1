#include <arpa/inet.h>
#include <atomic>
#include <functional>
#include <iostream>
#include <netdb.h>
#include <unistd.h>

#include "contextmanager.h"
#include "err.h"
#include "net.h"

using namespace std;

/// Internal method to send a buffer of data over a socket.
///
/// @param sd    The socket on which to send
/// @param bytes A pointer to the first byte of the data to send
/// @param len   The number of bytes to send
///
/// @return True if the whole buffer was sent, false otherwise
bool reliable_send(int sd, const unsigned char *bytes, int len) {
  // When we send, we need to be ready for the possibility that not all the
  // data will transmit at once
  const unsigned char *next_byte = bytes;
  int remain = len;
  while (remain) {
    int sent = send(sd, next_byte, remain, 0);
    // NB: Sending 0 bytes means the server closed the socket, and we should
    //     fail, so it's only EINTR that is recoverable.
    if (sent <= 0) {
      if (errno != EINTR)
        return err(false, "Error in send(): ", msg_from_errno(errno).c_str());
    } else {
      next_byte += sent;
      remain -= sent;
    }
  }
  return true;
}

/// Send a vector of data over a socket.
///
/// @param sd  The socket on which to send
/// @param msg The message to send
///
/// @return True if the whole vector was sent, false otherwise
bool send_reliably(int sd, const vector<uint8_t> &msg) {
  return reliable_send(sd, msg.data(), msg.size());
}

/// Send a string over a socket.
///
/// @param sd  The socket on which to send
/// @param msg The message to send
///
/// @return True if the whole string was sent, false otherwise
bool send_reliably(int sd, const string &msg) {
  return reliable_send(sd, (const unsigned char *)msg.c_str(), msg.length());
}

/// Perform a reliable read when we have a guess about how many bytes we might
/// get, but it's OK if the socket EOFs before we get that many bytes.
///
/// @param sd   The socket from which to read
/// @param pos  The start of the vector where datashould go.  It is assumed to
///             be pre-allocated to amnt or more.
/// @param amnt The maximum number of bytes to get
///
/// @return The actual number of bytes read, or -1 on a non-eof error
int reliable_get_to_eof_or_n(int sd, vector<uint8_t>::iterator pos, int amnt) {
  int remain = amnt;
  unsigned char *next_byte = &*pos;
  int total = 0;
  while (remain) {
    int rcd = recv(sd, next_byte, remain, 0);
    // NB: 0 bytes received means server closed socket, and -1 means an error.
    //     EINTR means try again, otherwise we will just fail
    if (rcd <= 0) {
      if (errno != EINTR)
        return (rcd == 0)
                   ? total
                   : err(-1,
                         "Error in recv(): ", msg_from_errno(errno).c_str());
    } else {
      next_byte += rcd;
      remain -= rcd;
      total += rcd;
    }
  }
  return total;
}

/// Perform a reliable read when we are not sure how many bytes we are going to
/// receive.
///
/// @param sd The socket from which to read
///
/// @return A vector with the data that was read, or an empty vector on error
vector<uint8_t> reliable_get_to_eof(int sd) {
  // set up the initial buffer
  vector<uint8_t> res(16);
  int recd = 0;
  // start reading.  Double the buffer any time we fill up
  while (true) {
    int remain = res.size() - recd;
    int justgot = recv(sd, (res.data() + recd), remain, 0);
    // EOF means we're done reading
    if (justgot == 0) {
      res.resize(recd);
      return res;
    }
    // On error, fail for non-EINTR, no-op on EINTR
    else if (justgot < 0) {
      if (errno != EINTR)
        return err<vector<uint8_t>>(
            {}, "Error in recv(): ", msg_from_errno(errno).c_str());
    }
    // bytes received.  advance pointer, maybe double the buffer
    else {
      recd += justgot;
      if (recd == (int)res.size())
        res.resize(2 * res.size());
    }
  }
}

/// Connect to a server so that we can have bidirectional communication on the
/// socket (represented by a file descriptor) that this function returns
///
/// @param hostname The name of the server (ip or DNS) to connect to
/// @param port     The server's port that we should use
///
/// @return The socket descriptor for further communication, or -1 on error
int connect_to_server(const string &hostname, int port) {
  // figure out the IP address that we need to use and put it in a sockaddr_in
  struct hostent *host = gethostbyname(hostname.c_str());
  if (host == nullptr)
    return err(-1, "connect_to_server():DNS error:", hstrerror(h_errno));
  sockaddr_in addr = {0, 0, 0, 0};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr =
      inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
  addr.sin_port = htons(port);
  // create the socket and try to connect to it
  int sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0)
    return err(-1,
               "Error making client socket: ", msg_from_errno(errno).c_str());
  if (connect(sd, (sockaddr *)&addr, sizeof(addr)) < 0) {
    close(sd);
    return err(-1, "Error connecting socket to address: ",
               msg_from_errno(errno).c_str());
  }
  return sd;
}

/// Create a server socket that we can use to listen for new incoming requests
///
/// @param port The port on which the program should listen for new connections
///
/// @return The new listening socket, or -1 on error
int create_server_socket(size_t port) {
  // A socket is just a kind of file descriptor.  We want our connections to use
  // IPV4 and TCP:
  int sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0)
    return err(-1,
               "Error making server socket: ", msg_from_errno(errno).c_str());
  ContextManager cs([&]() { close(sd); });

  // The default is that when the server crashes, the socket can't be used for a
  // few minutes.  This lets us re-use the socket immediately:
  int tmp = 1;
  if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(int)) < 0)
    return err(
        -1, "setsockopt(SO_REUSEADDR) failed: ", msg_from_errno(errno).c_str());

  // bind the socket to the server's address and the provided port, and then
  // start listening for connections
  sockaddr_in addr = {0, 0, 0, 0};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    return err(-1, "Error binding socket to local address: ",
               msg_from_errno(errno).c_str());

  if (listen(sd, 1024) < 0)
    return err(-1,
               "Error listening on socket: ", msg_from_errno(errno).c_str());
  // Don't close the socket when there aren't errors!
  cs.disable();
  return sd;
}

/// Given a listening socket, start calling accept() on it to get new
/// connections.  Each time a connection comes in, pass it to the thread pool so
/// that it can be processed.
///
/// @param sd   The socket file descriptor on which to call accept
/// @param pool The thread pool that handles new requests
///
/// @return true on a graceful shutdown, false on an error
bool accept_client(int sd, thread_pool &pool) {
  atomic<bool> safe_shutdown(false);
  pool.set_shutdown_handler([&]() {
    safe_shutdown = true;
    shutdown(sd, SHUT_RDWR);
  });
  // Use accept() to wait for a client to connect.  When it connects, hand it to
  // a thread pool for servicing
  while (pool.check_active()) {
    cout << "Waiting for a client to connect...\n";
    sockaddr_in clientAddr = {0, 0, 0, 0};
    socklen_t clientAddrSize = sizeof(clientAddr);
    int connSd = accept(sd, (sockaddr *)&clientAddr, &clientAddrSize);
    if (connSd < 0) {
      // If safe_shutdown() was called, and it's EINVAL, then the pool has been
      // halted, and the listening socket closed, so don't print an error.
      if (errno != EINVAL || !safe_shutdown)
        return err(false, "Error accepting request from client: ",
                   msg_from_errno(errno).c_str());
    }
    char cliName[1024];
    cout << "Connected to "
         << inet_ntop(AF_INET, &clientAddr.sin_addr, cliName, sizeof(cliName))
         << endl;
    pool.service_connection(connSd);
  }
  return true;
}
