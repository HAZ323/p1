#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

/// Storage is an interface that describes the main object that our server will
/// use.  In general, it is not good to use polymorphism (virtual methods) in
/// C++.  Furthermore, our program really doesn't need polymorphism.  The reason
/// we have it is as a convenience for the professor and students.  The students
/// benefit when the professor can share compiled `.o` files for the various
/// phases of the assignment, so that they can get full credit even if they did
/// not complete previous assignments.  The instructor benefits if no source
/// code solutions are provided, since that reduces inter-semester cheating. The
/// cost is performance... every call to a method of Storage will have vtable
/// indirection overhead.
///
/// Storage provides access to three concurrent maps.  The first is an
/// authentication table.  The authentication table holds user names, salts, and
/// hashed passwords, as well as a single vector of content per user.  The
/// second is a key/value store that can be accessed by all threads.  The third
/// is used for managing quotas.
///
/// The public interface of Storage provides functions that correspond 1:1 with
/// the data requests that a client can make.  In that manner, the server
/// command handlers need only parse a request, send its parts to the Storage
/// object, and then format and return the result.
///
/// Note that the functions that correspond to client requests all return a
/// tuple, consisting of a bool, a string, and a vector.  When the bool is
/// *false*, it means that the operation did not succeed, and the string is an
/// error message that can be sent to the client.  When the bool is *true*, it
/// means that the operation succeeded.  In this case the string is mostlikely
/// RES_OK, and the vector, if not empty, is additional data to send to the
/// client.
class Storage {
public:
  /// Result_t is the tuple that is sent to the caller after any operation
  /// requested by a client.
  struct result_t {
    bool succeeded;  // True if the operation succeeded, false otherwise
    std::string msg; // The message to send to the client
    std::vector<uint8_t>
        data; // Optional additional content to return to the client
  };

  /// Destructor for the storage object.
  virtual ~Storage() {}

  /// Populate the Storage object by loading this.filename.  Note that
  /// load_file() begins by clearing the maps, so that when the call is
  /// complete, exactly and only the contents of the file are in the Storage
  /// object.
  ///
  /// @return A result tuple, as described above.  Note that a non-existent file
  ///         is not an error.
  virtual result_t load_file() = 0;

  /// Create a new entry in the Auth table.  If the user already exists, return
  /// an error.  Otherwise, create a salt, hash the password, and then save an
  /// entry with the username, salt, hashed password, and a zero-byte content.
  ///
  /// @param user The user name to register
  /// @param pass The password to associate with that user name
  ///
  /// @return A result tuple, as described above
  virtual result_t add_user(const std::string &user,
                            const std::string &pass) = 0;

  /// Set the data bytes for a user, but do so if and only if the password
  /// matches
  ///
  /// @param user    The name of the user whose content is being set
  /// @param pass    The password for the user, used to authenticate
  /// @param content The data to set for this user
  ///
  /// @return A result tuple, as described above
  virtual result_t set_user_data(const std::string &user,
                                 const std::string &pass,
                                 const std::vector<uint8_t> &content) = 0;

  /// Return a copy of the user data for a user, but do so only if the password
  /// matches
  ///
  /// @param user The name of the user who made the request
  /// @param pass The password for the user, used to authenticate
  /// @param who  The name of the user whose content is being fetched
  ///
  /// @return A result tuple, as described above.  Note that "no data" is an
  ///         error
  virtual result_t get_user_data(const std::string &user,
                                 const std::string &pass,
                                 const std::string &who) = 0;

  /// Return a newline-delimited string containing all of the usernames in the
  /// auth table
  ///
  /// @param user The name of the user who made the request
  /// @param pass The password for the user, used to authenticate
  ///
  /// @return A result tuple, as described above
  virtual result_t get_all_users(const std::string &user,
                                 const std::string &pass) = 0;

  /// Authenticate a user
  ///
  /// @param user The name of the user who made the request
  /// @param pass The password for the user, used to authenticate
  ///
  /// @return A result tuple, as described above
  virtual result_t auth(const std::string &user, const std::string &pass) = 0;

  /// Write the entire Storage object to the file specified by this.filename.
  /// To ensure durability, Storage must be persisted in two steps.  First, it
  /// must be written to a temporary file (this.filename.tmp).  Then the
  /// temporary file can be renamed to replace the older version of the Storage
  /// object.
  ///
  /// @return A result tuple, as described above
  virtual result_t save_file() = 0;

  /// Shut down the storage when the server stops.  This method needs to close
  /// any open files related to incremental persistence.  It also needs to clean
  /// up any state related to .so files.  This is only called when all threads
  /// have stopped accessing the Storage object.
  virtual void shutdown() = 0;
};

/// Create an empty Storage object and specify the file from which it should be
/// loaded.  To avoid exceptions and errors in the constructor, the act of
/// loading data is separate from construction.
///
/// @param fname   The name of the file to use for persistence
/// @param buckets The number of buckets in the hash table
/// @param upq     The upload quota
/// @param dnq     The download quota
/// @param rqq     The request quota
/// @param qd      The quota duration
/// @param top     The size of the "top keys" cache
/// @param admin   The administrator's username
Storage *storage_factory(const std::string &fname, size_t buckets, size_t upq,
                         size_t dnq, size_t rqq, double qd, size_t top,
                         const std::string &admin);
