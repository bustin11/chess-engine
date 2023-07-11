
#pragma once

#include <vector>

#include "game.hpp"
#include "macros.hpp"
#include "logger.hpp"

using std::vector;

namespace chess {

class StandardGame : public Game {
public:
  StandardGame();

  // // delete copy
  // StandardGame(const Game &game) = delete;
  // StandardGame operator=(const Game &game) = delete;

  // // delete move
  // StandardGame(Game &&game) = delete;
  // StandardGame operator=(Game &&game) = delete;

  ~StandardGame();

  void generate_pawn_moves(int start_square, piece_t piece) override;
  void generate_knight_moves(int start_square, piece_t piece) override;
  void generate_king_moves(int start_square, piece_t piece) override;
  void generate_sliding_moves(int start_square, piece_t piece) override;
  void generate_moves_for_piece(int start_square, piece_t piece) override;
  void generate_moves() override;

  virtual bool is_legal_move(const Move &move) override;

  virtual void make_move(int from, int to, piece_t promotion_piece = EMPTY) override;
  virtual void make_move(const string &move, piece_t promotion_piece = EMPTY) override;
  virtual void make_move(const Move &move, piece_t promotion_piece = EMPTY) noexcept(false) override;

  virtual void undo_move() override;

};

}; // namespace chess
