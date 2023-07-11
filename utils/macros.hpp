
#pragma once

#include <cassert>
#include <stdexcept>

// (TODO) Is namespace needed
namespace chess {

#define CHESS_ASSERT(expr, msg) assert((expr) && (msg))

#define CHESS_ENSURE(expr, msg)                                                \
  if (!(expr)) {                                                               \
    throw std::logic_error(msg);                                               \
  }

} // namespace chess
