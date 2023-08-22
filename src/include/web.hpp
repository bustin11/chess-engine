
/**
 * https://stackoverflow.com/questions/16758234/which-is-the-difference-between-declaring-a-constructor-private-and-delete#:~:text=The%20constructor%20still%20exists%2C%20but,delete%20is%20what%20you%20want.
 *
 * TODO: support the ability to switch between game modes?
 * 1) use setter function
 * 2)
 */

#pragma once

#include <cstring>

#include "game.hpp"
#include "httplib/httplib.h"
#include "logger.hpp"
#include "macros.hpp"

namespace chess {

const int DEFAULT_PORT = 8080;

class Web {
public:
  explicit Web(int port = DEFAULT_PORT);
  ~Web();
  void Run();

  Web(const Web &) = delete;
  auto operator=(const Web &) -> Web & = delete;
  Web(Web &&) = delete;
  auto operator=(Web &&) -> Web & = delete;

private:
  Game *game_;
  int port_;
};

} // namespace chess
