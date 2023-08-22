/**
 * TODO:
 * 1. copy and move constructors
 * 2. dump_fen to dump board to fen
 *
 */
#pragma once

#include <array>
#include <cstdio>
#include <stack>
#include <unordered_map>

#include "fen_util.hpp"
#include "move.hpp"
#include "piece_list.hpp"
#include "precomputed_move_data.hpp"

namespace chess {

class Board {
public:
  static const int WHITE_INDEX = 0;
  static const int BLACK_INDEX = 1;

  std::array<int, 64> square_ = {};

  bool white_to_move_;
  int color_to_move_;
  int opponent_color_;
  int color_to_move_index_;

  // 0-3 castling rights
  // 4-7 en passant square, starting from file 1
  // 8-12 captured piece
  // 14-... fifty move counter
  std::stack<u32> history_ = {};
  u32 current_game_state_ = {};

  int ply_count_;
  int fifty_move_counter_;

  u64 zobrist_key_;
  std::unordered_map<u64, int> repetition_position_history_ = {};
  std::vector<u64> repetition_position_stack_ = {};

  std::array<int, 2> king_square_ = {};

  const u32 white_castle_kingside_mask_ = 0b1111111111111110;
  const u32 white_castle_queenside_mask_ = 0b1111111111111101;
  const u32 black_castle_kingside_mask_ = 0b1111111111111011;
  const u32 black_castle_queenside_mask_ = 0b1111111111110111;

  const u32 white_castle_mask_ =
      white_castle_kingside_mask_ & white_castle_queenside_mask_;
  const u32 black_castle_mask_ =
      black_castle_kingside_mask_ & black_castle_queenside_mask_;

  std::array<PieceList*, 16> all_piece_lists_;
  
  // 2 colors
  std::array<PieceList*, 2> rooks_;
  std::array<PieceList*, 2> bishops_;
  std::array<PieceList*, 2> queens_;
  std::array<PieceList*, 2> knights_;
  std::array<PieceList*, 2> pawns_;

  auto GetPieceList(int piece_type, int color_index) const -> PieceList *;
  void MakeMove(const Move &move,
                bool searching = false); // false record in game history
  void UndoMove(const Move &move, bool searching = false);

  void LoadStartPosition();
  void LoadPosition(const std::string &fen);

  auto operator[](int square_index) -> int & { return square_[square_index]; }

  auto DumpChessboardInfo() -> std::string;

  ~Board();

  // copy ctrs
  // Board &operator=(Board other);

  // friend void swap(Board &first, Board &second) {
  //   // enable ADL
  //   using std::swap;
  //   LOG_ERROR("Don't come here, it's not implemented");
  // }

  Board();

private:
  void Init();

  auto AssertPieceList() -> bool;
};

}; // namespace chess
