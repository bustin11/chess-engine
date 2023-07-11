/**
 * TODO:
 * 1. copy-elision/copy and swap?
 * 2. throw exceptions when fen string is invalid
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_set>

#include "board.hpp"
#include "logger.hpp"

using chess::piece_t;
using std::string, std::vector, std::unordered_set;

namespace chess {

struct KeyHasher {
  std::size_t operator()(const Move& k) const {
    // [0, 31]
    return ((k._from << 6) | k._to); // unique
  }
};

class Game {
public:
  Game();
  virtual ~Game();

  
  void set_piece_selected(int start_square, piece_t piece);
  void set_board_fen(string fen);

  virtual void generate_pawn_moves(int start_square, piece_t piece);
  virtual void generate_knight_moves(int start_square, piece_t piece);
  virtual void generate_king_moves(int start_square, piece_t piece);
  virtual void generate_sliding_moves(int start_square, piece_t piece);
  virtual void generate_moves_for_piece(int start_square, piece_t piece);
  virtual void generate_moves();
  
  virtual void make_move(const string &move, piece_t promotion_piece = EMPTY);
  virtual void make_move(int from, int to, piece_t promotion_piece = EMPTY);
  virtual void make_move(const Move &move, piece_t promotion_piece = EMPTY);

  virtual void undo_move(); // TODO: implement undo move when the promotion piece is invalid

  virtual bool is_legal_move(const Move &move);

  Board<piece_t>& get_board() { return _board; }


  Game &operator=(Game &&other) {
    swap(_board, other._board);
    return *this;
  }

  vector<Move>& get_moves() { return _moves; }

protected:
  Board<piece_t> _board;
  vector<Move> _moves; // moves for the current color;
  unordered_set<Move, KeyHasher> _moves_set; // copy of _moves, but hashing
};

} // namespace chess
