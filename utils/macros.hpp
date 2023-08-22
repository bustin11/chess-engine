
#pragma once

#include <cassert>
#include <stdexcept>

// (TODO) Is namespace needed
namespace chess {

using u8 = __uint8_t;
using u16 = __uint16_t;
using u32 = __uint32_t;
using u64 = __uint64_t;

inline bool in_bounds(int square) { return 0 <= square && square < 64; }

#define linear_index(x, y) y * 8 + x
#define xy_index(x, y, square)                                                 \
  int x = square % 8;                                                          \
  int y = square / 8;

#define sign(num) (num > 0 ? 1 : -1)

#ifndef NO_ASSERT
#define CHESS_ASSERT(expr, msg) assert((expr) && (msg))

#define CHESS_ENSURE(expr, msg)                                                \
  if (!(expr)) {                                                               \
    throw std::logic_error(msg);                                               \
  }
#else

#define CHESS_ASSERT(expr, msg) ((void)0)
#define CHESS_ENSURE(expr, msg) ((void)0)

#endif

} // namespace chess
