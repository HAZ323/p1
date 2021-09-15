#include <string>

#include "authtableentry.h"
#include "sequentialmap.h"

using namespace std;

/// Create an instance of SequentialMap that can be used as an authentication
/// table
///
/// @param _buckets The number of buckets in the table
Map<string, AuthTableEntry> *authtable_factory(size_t _buckets) {
  return new SequentialMap<string, AuthTableEntry>(_buckets);
}
