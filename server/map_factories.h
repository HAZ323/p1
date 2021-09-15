#pragma once

#include <string>
#include <vector>

#include "authtableentry.h"
#include "map.h"

/// Create an instance of HashTable that can be used as an authentication table
///
/// @param _buckets The number of buckets in the table
Map<std::string, AuthTableEntry> *authtable_factory(size_t _buckets);
