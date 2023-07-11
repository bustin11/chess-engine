
/**
 * https://stackoverflow.com/questions/16758234/which-is-the-difference-between-declaring-a-constructor-private-and-delete#:~:text=The%20constructor%20still%20exists%2C%20but,delete%20is%20what%20you%20want.
 *
 * TODO: support the ability to switch between game modes?
 * 1) use setter function
 * 2)
 */

#pragma once

#include <string.h>

#include "httplib/httplib.h"
#include "logger.hpp"
#include "macros.hpp"
#include "standard_game.hpp"

namespace chess {

const int DEFAULT_PORT = 8080;

class Web {
public:
  Web(int port = DEFAULT_PORT);
  void run();

  Web(const Web &) = delete;
  Web &operator=(const Web &) = delete;
  Web(Web &&) = delete;
  Web &operator=(Web &&) = delete;

private:
  Game _game;
  int _port;
};

} // namespace chess
