#pragma once

#include <iostream>
#include <string>
#include <vector>

/// Create a string representation of the error message associated with the
/// given errno
///
/// @param err The error number to use when producing an error message
///
/// @return A string representing the error message
std::string msg_from_errno(int err);

/// A helper function for printing an error and returning the appropriate value
///
/// @param val  The value to return
/// @param msg1 The message to print
/// @param msg2 More of the message to print
/// @param msg3 More of the message to print
///
/// @return The value given by `val`
///
/// NB: A warning will be generated if the caller of this function ignores the
///     return value.
template <typename T>
__attribute__((warn_unused_result)) T
err(T val, const char *msg1, const char *msg2 = "", const char *msg3 = "") {
  std::cout << msg1 << msg2 << msg3 << std::endl;
  return val;
}