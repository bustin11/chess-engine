/**
 * TODO:
 * 1. copy and move constructors
 * 2. dump_fen to dump board to fen
 *
 */
#pragma once

#include <stdio.h>
#include <array>
#include <vector>
#include <list>
#include <set>

#include "piece.hpp"



using chess::piece_t, chess::color_t;
using std::string, std::array, std::vector, std::pair, std::set;

namespace chess {

template <typename T> class Board {

struct Event {
  // const Move _move;
  const __uint8_t castling_rights_; // after move
  const piece_t captured_piece_; // after move
  const Move move_;
  const square_t en_passant_square_; // after move
  // Event(const Move& move, __uint8_t castling_rights, piece_t captured_piece, square_t en_passant_square) :
  //   from_(move.from_), to_(move.to_), castling_rights_(castling_rights), captured_piece_(captured_piece), 
  //     en_passant_square_(en_passant_square) {}
  Event(const Move& move, __uint8_t castling_rights, piece_t captured_piece, square_t en_passant_square) :
    move_(move), castling_rights_(castling_rights), captured_piece_(captured_piece), 
      en_passant_square_(en_passant_square) {}
  // Event(square_t from, square_t to, __uint8_t castling_rights, piece_t captured_piece) :
  //   _from(from), _to(to), _castling_rights(castling_rights), _captured_piece(captured_piece) {}
};

struct MetaData {
  set<square_t> piece_location_ = {}; // 0/1 := black/white

  square_t king_position_ = {};
  array<square_t, NUM_SQUARES> knp_attacking_counts_ = {}; // double check/check, castling
  array<square_t, NUM_SQUARES> qrb_attacking_counts_ = {}; // blocking, castling
  array<square_t, NUM_SQUARES> xray_counts_ = {}; // pins, breaking pins, en passants
  // capturing is done with the board itself
};

public:
  Board(){};
  friend class Game;
  friend class StandardGame;

  void load_from_fen(const string &fen);

  inline bool can_castle_king_side(color_t color) const {
    if (color == WHITE) {
      return (castling_rights_ & (1 << 3)) && board_[5] == EMPTY && board_[6] == EMPTY;
    }
    return (castling_rights_ & (1 << 1)) && board_[61] == EMPTY && board_[62] == EMPTY;
  }

  inline bool can_castle_queen_side(color_t color) const {
    if (color == WHITE) {
      return (castling_rights_ & (1 << 2)) && board_[3] == EMPTY && board_[2] == EMPTY && board_[1] == EMPTY;
    }
    return (castling_rights_ & (1 << 0)) && board_[59] == EMPTY && board_[58] == EMPTY && board_[57] == EMPTY;
  }


  void dump_data();

  bool is_pinned_left_diagonal(square_t start_square, bool hypothetical = false); 
  bool is_pinned_right_diagonal(square_t start_square, bool hypothetical = false); 
  bool is_pinned_row(square_t start_square, bool hypothetical = false) ; 
  bool is_pinned_col(square_t start_square, bool hypothetical = false);

  array<T, NUM_SQUARES> board_ = {}; // The chessboard
  color_t color_to_move_ = WHITE;  // Whose turn it is ('w' for white, 'b' for black)
  __uint8_t castling_rights_ = {};  // Castling rights (e.g., "KQkq"), 1101, ie, means KQq
  square_t en_passant_square_ = EMPTY; // En passant target square (e.g., "e3")
  int half_moves_{};          // Half-moves since last pawn move or capture
  int full_moves_{};          // Full moves counter
  vector<Event> move_history_ = {};
  array<MetaData, 2> meta_data_ = {};

private:
  bool is_pinned_on_line(int diag_id, square_t start_square, bool hypothetical = false);
  inline bool king_in_sliding_check(square_t target_square = -1) {
    auto king_pos = target_square == -1 ? 
      meta_data_[color_to_move_ == WHITE].king_position_ : target_square;
    auto& qrb = meta_data_[color_to_move_ == BLACK].qrb_attacking_counts_;
    return qrb[king_pos] > 0; 
  }
  inline bool king_in_non_sliding_check(square_t target_square = -1) {
    auto king_pos = target_square == -1 ? 
      meta_data_[color_to_move_ == WHITE].king_position_ : target_square;
    auto& knp = meta_data_[color_to_move_ == BLACK].knp_attacking_counts_;
    return knp[king_pos] > 0; 
  }
  bool king_in_check(square_t target_square = -1, bool check_xrays = false);
  bool still_check_without(square_t start_square, piece_t piece);
  bool still_check_with(square_t start_square, piece_t piece);
  bool king_in_double_check();

  __uint8_t original_castling_rights_ = {};  // Castling rights (e.g., "KQkq"), 1101, ie, means KQq
  square_t original_en_passant_square_ = EMPTY; // En passant target square (e.g., "e3")

};



}; // namespace chess
