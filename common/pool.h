#pragma once

#include <functional>
#include <memory>

/// thread_pool encapsulates a pool of threads that are all waiting for data to
/// appear in a queue.  Whenever data arrives in the queue, a thread will pull
/// the data off and process it, using the handler function provided at
/// construction time.
///
/// NB: As in storage.h, we declare a class that only has pure virtual
///     functions, and then there are "my_pool.h/my_pool.cc" which subclass
///     thread_pool and then implement everything.  While pure virtual methods
///     have overhead in C++, this style of programming allows us to share
///     solutions without sharing code.
class thread_pool {
public:
  /// destruct a thread pool
  virtual ~thread_pool(){};

  /// Allow a user of the pool to provide some code to run when the pool decides
  /// it needs to shut down.
  ///
  /// @param func The code that should be run when the pool shuts down
  virtual void set_shutdown_handler(std::function<void()> func) = 0;

  /// Allow a user of the pool to see if the pool has been shut down
  virtual bool check_active() = 0;

  /// Shutting down the pool can take some time.  await_shutdown() lets a user
  /// of the pool wait until the threads are all done servicing clients.
  virtual void await_shutdown() = 0;

  /// When a new connection arrives at the server, it calls this to pass the
  /// connection to the pool for processing.
  ///
  /// @param sd The socket descriptor for the new connection
  virtual void service_connection(int sd) = 0;
};

/// pool_factory creates a pool object that can serve as a thread pool
///
/// @param size    The number of threads in the pool
/// @param handler The code to run whenever something arrives in the pool
///
/// @return A thread pool (technically a subclass of thread_pool that is not
///         abstract)
thread_pool *pool_factory(int size, std::function<bool(int)> handler);
