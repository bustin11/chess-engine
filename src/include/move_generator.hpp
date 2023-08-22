
#pragma once

#include <vector>

#include "board.hpp"
#include "board_model.hpp"
#include "macros.hpp"
#include "move.hpp"

namespace chess {

class MoveGenerator {

private:
  enum class PromotionMode { All, QueenOnly };
  PromotionMode promotion_mode_;
  std::vector<Move> moves_;
  Board *board_;
  bool gen_quiets_;
  bool in_double_check_;
  bool in_check_;
  bool pins_in_position_;

  u64 check_ray_bit_mask_;
  u64 pin_ray_bit_mask_;

  bool white_to_move_;
  int friendly_color_;
  int opponent_color_;
  int friendly_king_square_;
  int friendly_color_index_;
  int opponent_color_index_;

  u64 opponent_sliding_attack_map_;
  u64 opponent_knight_attacks_map_;
  u64 opponent_pawn_attack_map_;
  u64 opponent_attack_map_no_pawns_;
  u64 opponent_attack_map_;

public:
  auto GenerateMoves(Board *board,
                                  bool include_quiet_moves = true) -> std::vector<Move>;
  void GenerateSlidingMoves();
  void GenerateKingMoves();
  void GenerateKnightMoves();
  void GenerateSlidingPieceMoves();
  void GeneratePawnMoves();
  void GenerateSlidingPieceMoves(int square, int start, int end);
  auto FilterMovesWithStartSquare(const std::vector<Move>& moves,
                                               int start_square) -> std::vector<Move>;

  void Init();

  void CalculateAttackData();

  auto InCheck() -> bool;
  auto SquareIsAttacked(int square) -> bool;
  auto IsMovingAlongRay(int offset, int start, int end) -> bool;
  auto HasQueenSideCastleRights() -> bool;
  auto HasKingSideCastleRights() -> bool;
  auto IsPinned(int square) -> bool;
  auto SquareIsInCheckRay(int square) -> bool;
  auto InCheckAfterEnPassant(int start_square, int target_square,
                             int ep_captured_pawn_square) -> bool;

  void MakePromotionMoves(int start_square, int target_square);
  void GenSlidingAttackMap();
  void UpdateSlidingAttackPiece(int start_square, int start, int end);
  auto SquareAttackedAfterEpCapture(int ep_capture_square,
                                    int capturing_pawn_start_square) -> bool;
};

}; // namespace chess
