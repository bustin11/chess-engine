
#pragma once

#include <map>
#include <string>
#include <array>
#include <iostream>

#include "logger.hpp"

using std::map, std::swap, std::min, std::string, std::to_string;

namespace chess {

using square_t = __int8_t;
using piece_t = __uint8_t;
using color_t = __uint8_t;

constexpr int NUM_SQUARES = 64;
constexpr int NUM_ROW = 8;
constexpr int MOST_MOVES = 256; // actually 218

const piece_t PROMOTION = 128;
const piece_t EMPTY = 0;
const piece_t KING = 1;
const piece_t PAWN = 2;
const piece_t KNIGHT = 3;
const piece_t BISHOP = 4;
const piece_t ROOK = 5;
const piece_t QUEEN = 6;

const color_t WHITE = 8;
const color_t BLACK = 16;

const map<char, piece_t> char_piece_map{
    {'P', WHITE | PAWN}, {'N', WHITE | KNIGHT}, {'B', WHITE | BISHOP},
    {'R', WHITE | ROOK}, {'Q', WHITE | QUEEN},  {'K', WHITE | KING},

    {'p', BLACK | PAWN}, {'n', BLACK | KNIGHT}, {'b', BLACK | BISHOP},
    {'r', BLACK | ROOK}, {'q', BLACK | QUEEN},  {'k', BLACK | KING},
};

const map<string, square_t> move_index_map = {
    {"a1", 0},  {"b1", 1},  {"c1", 2},  {"d1", 3},  {"e1", 4},  {"f1", 5},
    {"g1", 6},  {"h1", 7},  {"a2", 8},  {"b2", 9},  {"c2", 10}, {"d2", 11},
    {"e2", 12}, {"f2", 13}, {"g2", 14}, {"h2", 15}, {"a3", 16}, {"b3", 17},
    {"c3", 18}, {"d3", 19}, {"e3", 20}, {"f3", 21}, {"g3", 22}, {"h3", 23},
    {"a4", 24}, {"b4", 25}, {"c4", 26}, {"d4", 27}, {"e4", 28}, {"f4", 29},
    {"g4", 30}, {"h4", 31}, {"a5", 32}, {"b5", 33}, {"c5", 34}, {"d5", 35},
    {"e5", 36}, {"f5", 37}, {"g5", 38}, {"h5", 39}, {"a6", 40}, {"b6", 41},
    {"c6", 42}, {"d6", 43}, {"e6", 44}, {"f6", 45}, {"g6", 46}, {"h6", 47},
    {"a7", 48}, {"b7", 49}, {"c7", 50}, {"d7", 51}, {"e7", 52}, {"f7", 53},
    {"g7", 54}, {"h7", 55}, {"a8", 56}, {"b8", 57}, {"c8", 58}, {"d8", 59},
    {"e8", 60}, {"f8", 61}, {"g8", 62}, {"h8", 63}};

class Move {
public:

  friend class Game;
  friend class StandardGame;
  friend class Move;

  Move() : from_(-1), to_(-1) {}
  Move(int from, int to) : from_(from), to_(to) {}
  Move(const Move& other) : to_(other.to_), from_(other.from_) {}

  // [NEEDED] checking legal moves in hashed set
  bool operator==(const Move& other) const {
    return from_ == other.from_ && to_ == other.to_;
  }

  const square_t from_;
  const square_t to_;
};

// TODO: fix this with ::fmt
static void print_move(square_t from, square_t to) {
  int from_x = from % NUM_ROW;
  int from_y = from / NUM_ROW;

  int to_x = to % NUM_ROW;
  int to_y = to / NUM_ROW;
  string s = "(" + to_string(from_x) + ", " + to_string(from_y) + ")";
  s += " -> ";
  s += "(" + to_string(to_x) + ", " + to_string(to_y) + ")";
  LOG_DEBUG("%s", s.c_str());
}

static void print_move(const Move& move) {
  print_move(move.from_, move.to_);
}

// n,s,e,w,nw,se,ne,sw
const __int8_t direction_offsets[8]{8, -8, 1, -1, 7, -7, 9, -9};
const __int8_t num_squares_to_edge[NUM_SQUARES][8]{
{7,0,7,0,0,0,7,0 },
{7,0,6,1,1,0,6,0 },
{7,0,5,2,2,0,5,0 },
{7,0,4,3,3,0,4,0 },
{7,0,3,4,4,0,3,0 },
{7,0,2,5,5,0,2,0 },
{7,0,1,6,6,0,1,0 },
{7,0,0,7,7,0,0,0 },
{6,1,7,0,0,1,6,0 },
{6,1,6,1,1,1,6,1 },
{6,1,5,2,2,1,5,1 },
{6,1,4,3,3,1,4,1 },
{6,1,3,4,4,1,3,1 },
{6,1,2,5,5,1,2,1 },
{6,1,1,6,6,1,1,1 },
{6,1,0,7,6,0,0,1 },
{5,2,7,0,0,2,5,0 },
{5,2,6,1,1,2,5,1 },
{5,2,5,2,2,2,5,2 },
{5,2,4,3,3,2,4,2 },
{5,2,3,4,4,2,3,2 },
{5,2,2,5,5,2,2,2 },
{5,2,1,6,5,1,1,2 },
{5,2,0,7,5,0,0,2 },
{4,3,7,0,0,3,4,0 },
{4,3,6,1,1,3,4,1 },
{4,3,5,2,2,3,4,2 },
{4,3,4,3,3,3,4,3 },
{4,3,3,4,4,3,3,3 },
{4,3,2,5,4,2,2,3 },
{4,3,1,6,4,1,1,3 },
{4,3,0,7,4,0,0,3 },
{3,4,7,0,0,4,3,0 },
{3,4,6,1,1,4,3,1 },
{3,4,5,2,2,4,3,2 },
{3,4,4,3,3,4,3,3 },
{3,4,3,4,3,3,3,4 },
{3,4,2,5,3,2,2,4 },
{3,4,1,6,3,1,1,4 },
{3,4,0,7,3,0,0,4 },
{2,5,7,0,0,5,2,0 },
{2,5,6,1,1,5,2,1 },
{2,5,5,2,2,5,2,2 },
{2,5,4,3,2,4,2,3 },
{2,5,3,4,2,3,2,4 },
{2,5,2,5,2,2,2,5 },
{2,5,1,6,2,1,1,5 },
{2,5,0,7,2,0,0,5 },
{1,6,7,0,0,6,1,0 },
{1,6,6,1,1,6,1,1 },
{1,6,5,2,1,5,1,2 },
{1,6,4,3,1,4,1,3 },
{1,6,3,4,1,3,1,4 },
{1,6,2,5,1,2,1,5 },
{1,6,1,6,1,1,1,6 },
{1,6,0,7,1,0,0,6 },
{0,7,7,0,0,7,0,0 },
{0,7,6,1,0,6,0,1 },
{0,7,5,2,0,5,0,2 },
{0,7,4,3,0,4,0,3 },
{0,7,3,4,0,3,0,4 },
{0,7,2,5,0,2,0,5 },
{0,7,1,6,0,1,0,6 },
{0,7,0,7,0,0,0,7 },
};

const std::array<__int8_t, 8> knight_square_offsets{17, 10, -6, -15, -17, -10, 6, 15};


// ======================= helpers =============================

inline bool is_color(piece_t piece, color_t color) { return piece & color; }

inline bool is_sliding_piece(piece_t piece) {
  return (piece & QUEEN) | (piece & ROOK) | (piece | BISHOP);
}

inline bool is_type(piece_t piece, piece_t type) { return (piece & 7) == type; }

inline color_t opposite_color(color_t color) {
  return (color ^ 24); // 11000 ^ (WHITE or BLACK)
}

inline color_t get_color(piece_t piece) { return piece & 24; } 

inline bool on_second_rank(square_t square) { return 8 <= square && square < 16; }

inline bool on_seventh_rank(square_t square) { return 48 <= square && square < 56; }

inline bool on_eigth_rank(square_t square) { return 56 <= square && square < 64; }

inline bool on_first_rank(square_t square) { return 0 <= square && square < 8; }

inline bool in_bounds(square_t square) {
  return 0 <= square && square < 64; 
}

inline bool is_promotion_piece(piece_t piece) {
  return is_type(piece, KNIGHT) || is_type(piece, BISHOP) || is_type(piece, ROOK) || is_type(piece, QUEEN);
}

inline bool is_en_passant_capture(piece_t piece, piece_t target_piece, square_t from, square_t to) {
  return is_type(piece, PAWN) && target_piece == EMPTY && abs(abs(from - to) - NUM_ROW) == 1;
}

inline bool is_en_passant_capture(piece_t piece, piece_t target_piece, const Move& move) {
  return is_en_passant_capture(piece, target_piece, move.from_, move.to_);
}


inline bool is_king_side_castling(piece_t piece, const Move& move) {
  return is_type(piece, KING) && move.to_ - move.from_ == 2;
}

inline bool is_queen_side_castling(piece_t piece, const Move& move) {
  return is_type(piece, KING) && move.to_ - move.from_ == -2;
}

inline bool is_double_square_pawn_push(piece_t piece, const Move& move) {
  bool white_double = on_second_rank(move.from_) && is_color(piece, WHITE);
  bool black_double = on_seventh_rank(move.from_) && is_color(piece, BLACK);
  return (white_double || black_double) && abs(move.from_ - move.to_) == 2 * NUM_ROW;
}

}; // namespace chess
