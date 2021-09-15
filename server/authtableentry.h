#pragma once

#include <string>
#include <vector>

/// AuthTableEntry represents one user stored in the authentication table
struct AuthTableEntry {
  std::string username;           // The name of the user
  std::vector<uint8_t> salt;      // The salt to use with the password
  std::vector<uint8_t> pass_hash; // The hashed password
  std::vector<uint8_t> content;   // The user's content
};
