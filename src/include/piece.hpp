
#pragma once

#include <array>
#include <iostream>
#include <map>
#include <string>

#include "logger.hpp"
#include "macros.hpp"

namespace chess {

namespace piece {

const int EMPTY = 0;
const int KING = 1;
const int PAWN = 2;
const int KNIGHT = 3;

const int BISHOP = 5;
const int ROOK = 6;
const int QUEEN = 7; // BISHOP | ROOK

const int WHITE = 8;
const int BLACK = 16;

const int MASK_TYPE = 0b00111;
const int MASK_BLACK = 0b10000;
const int MASK_WHITE = 0b01000;
const int MASK_COLOR = MASK_BLACK | MASK_WHITE;

inline auto IsColor(int piece, int color) -> bool { return (piece & MASK_COLOR) == color; }

inline auto Color(int piece) -> int { return piece & MASK_COLOR; }

inline auto PieceType(int piece) -> int { return piece & MASK_TYPE; }

inline auto IsRookOrQueen(int piece) -> bool { return (piece & ROOK) == ROOK; }

inline auto IsBishopOrQueen(int piece) -> bool { return (piece & BISHOP) == BISHOP; }

inline auto IsSlidingPiece(int piece) -> bool { return (piece & 0b100) != 0; }

}; // namespace piece

}; // namespace chess
