
#pragma once

#include <GL/glut.h>
#include <SOIL/SOIL.h> // Include SOIL library for loading image textures

#include "board_model.hpp"
#include "coord.hpp"
#include "game.hpp"
#include "logger.hpp"
#include "player.hpp"

namespace gui {

auto WindowRun(int argc, char **argv) -> int;

}; // namespace gui