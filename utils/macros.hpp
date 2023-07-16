
#pragma once

#include <cassert>
#include <stdexcept>

// (TODO) Is namespace needed
namespace chess {

#define NO_ASSERT

#ifndef NO_ASSERT
#define CHESS_ASSERT(expr, msg) assert((expr) && (msg))

#define CHESS_ENSURE(expr, msg)                                                \
  if (!(expr)) {                                                               \
    throw std::logic_error(msg);                                               \
  }
#else 

#define CHESS_ASSERT(expr, msg) ((void)0)
#define CHESS_ENSURE(expr, msg) ((void)0)    
                                      \
#endif 

} // namespace chess
