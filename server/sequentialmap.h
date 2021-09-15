#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <string>
#include <vector>

#include "map.h"

/// SequentialMap is a sequential implementation of the Map interface (a
/// Key/Value store).  This map has O(n) complexity.  It's just for p1.
///
/// The SequentialMap is templated on the Key and Value types.
///
/// @param K The type of the keys in this map
/// @param V The type of the values in this map
template <typename K, typename V> class SequentialMap : public Map<K, V> {
  /// The vector of key/value pairs in this bucket
  std::list<std::pair<K, V>> entries;

public:
  /// Construct by specifying the number of buckets it should have
  ///
  /// @param _buckets (unused) The number of buckets
  SequentialMap(size_t) {}

  /// Destruct the SequentialMap
  virtual ~SequentialMap() {}

  /// Clear the map.  This operation needs to use 2pl
  virtual void clear() {
    std::cout << "sequentialmap.h::clear() is not implemented\n";
  }

  /// Insert the provided key/value pair only if there is no mapping for the key
  /// yet.
  ///
  /// @param key        The key to insert
  /// @param val        The value to insert
  /// @param on_success Code to run if the insertion succeeds
  ///
  /// @return true if the key/value was inserted, false if the key already
  ///         existed in the table
  virtual bool insert(K key, V val, std::function<void()> on_success) {
    std::cout << "sequentialmap.h::insert() is not implemented\n";
    return false;
  }

  /// Insert the provided key/value pair if there is no mapping for the key yet.
  /// If there is a key, then update the mapping by replacing the old value with
  /// the provided value
  ///
  /// @param key    The key to upsert
  /// @param val    The value to upsert
  /// @param on_ins Code to run if the upsert succeeds as an insert
  /// @param on_upd Code to run if the upsert succeeds as an update
  ///
  /// @return true if the key/value was inserted, false if the key already
  ///         existed in the table and was thus updated instead
  virtual bool upsert(K key, V val, std::function<void()> on_ins,
                      std::function<void()> on_upd) {
    std::cout << "sequentialmap.h::upsert() is not implemented\n";
    return false;
  }

  /// Apply a function to the value associated with a given key.  The function
  /// is allowed to modify the value.
  ///
  /// @param key The key whose value will be modified
  /// @param f   The function to apply to the key's value
  ///
  /// @return true if the key existed and the function was applied, false
  ///         otherwise
  virtual bool do_with(K key, std::function<void(V &)> f) {
    std::cout << "sequentialmap.h::do_with() is not implemented\n";
    return false;
  }

  /// Apply a function to the value associated with a given key.  The function
  /// is not allowed to modify the value.
  ///
  /// @param key The key whose value will be modified
  /// @param f   The function to apply to the key's value
  ///
  /// @return true if the key existed and the function was applied, false
  ///         otherwise
  virtual bool do_with_readonly(K key, std::function<void(const V &)> f) {
    std::cout << "sequentialmap.h::do_with_readonly() is not implemented\n";
    return false;
  }

  /// Remove the mapping from a key to its value
  ///
  /// @param key        The key whose mapping should be removed
  /// @param on_success Code to run if the remove succeeds
  ///
  /// @return true if the key was found and the value unmapped, false otherwise
  virtual bool remove(K key, std::function<void()> on_success) {
    std::cout << "sequentialmap.h::remove() is not implemented\n";
    return false;
  }

  /// Apply a function to every key/value pair in the map.  Note that the
  /// function is not allowed to modify keys or values.
  ///
  /// @param f    The function to apply to each key/value pair
  /// @param then A function to run when this is done, but before unlocking...
  ///             useful for 2pl
  virtual void do_all_readonly(std::function<void(const K, const V &)> f,
                               std::function<void()> then) {
    std::cout << "sequentialmap.h::do_all_readonly() is not implemented\n";
  }
};
