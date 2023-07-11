
#pragma once

#include <GL/glut.h>
#include <SOIL/SOIL.h> // Include SOIL library for loading image textures

#include "logger.hpp"
#include "standard_game.hpp"

using namespace chess;

namespace gui {

int window_run(int argc, char** argv);

}; // namespace chess