
#include <iostream>
#include "move_generator.hpp"

namespace chess {

namespace Pre = precomputed_move_data;
using BM = BoardModel;

auto MoveGenerator::GenerateMoves(Board *board, bool include_quiet_moves) -> std::vector<Move> {
  this->board_ = board;
  gen_quiets_ = include_quiet_moves;
  Init();

  CalculateAttackData();

  GenerateKingMoves();

  if (in_double_check_) {
    return moves_;
  }

  GenerateSlidingMoves();
  GenerateKnightMoves();
  GeneratePawnMoves();

  return moves_;
}

void MoveGenerator::Init() {
  moves_.clear();
  pins_in_position_ = in_double_check_ = in_check_ = false;
  check_ray_bit_mask_ = pin_ray_bit_mask_ = 0;

  white_to_move_ = board_->white_to_move_;
  friendly_color_ = board_->color_to_move_;
  opponent_color_ = board_->opponent_color_;
  friendly_king_square_ = board_->king_square_[board_->color_to_move_index_];
  friendly_color_index_ = board_->color_to_move_index_;
  opponent_color_index_ = 1 - friendly_color_index_;
}

void MoveGenerator::GenerateKingMoves() {
  for (auto target_square : Pre::king_moves[friendly_king_square_]) {
    int piece_on_target_square = board_->square_[target_square];
    if (piece::IsColor(piece_on_target_square, friendly_color_)) {
      continue;
    }
    bool is_capture = piece::IsColor(piece_on_target_square, opponent_color_);
    if (!is_capture) {
      if (!gen_quiets_ || SquareIsInCheckRay(target_square)) {
        continue;
      }
    }
    if (!SquareIsAttacked(target_square)) {
      moves_.emplace_back(friendly_king_square_, target_square);
      if (!in_check_ && !is_capture) {
        // kingside
        if ((target_square == BM::F1 || target_square == BM::F8) && HasKingSideCastleRights()) {
          int castle_kingside_square = target_square + 1;
          if (board_->square_[castle_kingside_square] == piece::EMPTY &&
              board_->square_[target_square] == piece::EMPTY) {
            if (!SquareIsAttacked(castle_kingside_square)) {
              moves_.emplace_back(friendly_king_square_, castle_kingside_square, Move::Type::castle);
            }
          }
        }
        // queenside
        if ((target_square == BM::D1 || target_square == BM::D8) && HasQueenSideCastleRights()) {
          int castle_queenside_square = target_square - 1;
          if (board_->square_[target_square] == piece::EMPTY && 
            board_->square_[castle_queenside_square] == piece::EMPTY &&
            board_->square_[castle_queenside_square-1] == piece::EMPTY) {
            if (!SquareIsAttacked(castle_queenside_square)) {
              moves_.emplace_back(friendly_king_square_, castle_queenside_square, Move::Type::castle);
            }
          }
        }
      }
    }
  }
}

void MoveGenerator::GenerateSlidingMoves() {
  auto &rooks = board_->rooks_[friendly_color_index_];
  for (int i = 0; i < rooks->Count(); i++) {
    GenerateSlidingPieceMoves((*rooks)[i], 0, 4);
  }
  auto &bishops = board_->bishops_[friendly_color_index_];
  for (int i = 0; i < bishops->Count(); i++) {
    GenerateSlidingPieceMoves((*bishops)[i], 4, 8);
  }
  auto &queens = board_->queens_[friendly_color_index_];
  for (int i = 0; i < queens->Count(); i++) {
    GenerateSlidingPieceMoves((*queens)[i], 0, 8);
  }
}

void MoveGenerator::GenerateSlidingPieceMoves(int start_square, int start, int end) {
  bool is_pinned = IsPinned(start_square);

  if (in_check_ && is_pinned) {
    return;
  }

  for (int dir_index = start; dir_index < end; dir_index++) {
    int current_dir_offset = Pre::direction_offsets[dir_index];
    if (is_pinned && !IsMovingAlongRay(current_dir_offset, friendly_king_square_, start_square)) {
      continue;
    }

    for (int n = 0; n < Pre::num_squares_to_edge[start_square][dir_index]; n++) {
      int target_square = start_square + current_dir_offset * (n + 1);
      int target_square_piece = board_->square_[target_square];
      if (piece::IsColor(target_square_piece, friendly_color_)) {
        break;
      }
      bool is_capture = target_square_piece != piece::EMPTY;
      bool move_prevents_check = SquareIsInCheckRay(target_square);
      if (move_prevents_check || !in_check_) {
        if (gen_quiets_ || is_capture) {
          moves_.emplace_back(start_square, target_square);
        }
      }
      if (is_capture || move_prevents_check) {
        break;
      }
    }
  }
}

void MoveGenerator::GenerateKnightMoves() {
  PieceList &knights = *board_->knights_[friendly_color_index_];
  for (int i = 0; i < knights.Count(); i++) {
    int start_square = knights[i];
    if (IsPinned(start_square)) {
      continue;
    }

    for (int target_square : Pre::knight_moves[start_square]) {
      int target_square_piece = board_->square_[target_square];
      bool is_capture = piece::IsColor(target_square_piece, opponent_color_);
      if (gen_quiets_ || is_capture) {
        if (piece::IsColor(target_square_piece, friendly_color_) || (InCheck() && !SquareIsInCheckRay(target_square))) {
          continue;
        }
        moves_.emplace_back(start_square, target_square);
      }
    }
  }
}

void MoveGenerator::GeneratePawnMoves() {
  PieceList &pawns = *(board_->pawns_[friendly_color_index_]);
  int pawn_offset = (friendly_color_ == piece::WHITE) ? 8 : -8;
  int start_rank = (board_->white_to_move_) ? 1 : 6;
  int final_rank_before_promotion = (board_->white_to_move_) ? 6 : 1;

  int en_passant_file = (static_cast<int>(board_->current_game_state_ >> 4) & 15) - 1;
  int en_passant_square = -1;
  if (en_passant_file != -1) {
    en_passant_square = 8 * ((board_->white_to_move_) ? 5 : 2) + en_passant_file;
  }

  for (int i = 0; i < pawns.Count(); i++) {
    int start_square = pawns[i];
    int rank = BM::RankIndex(start_square);
    bool one_step_from_promotion = (rank == final_rank_before_promotion);

    if (gen_quiets_) {
      int square_one_forward = start_square + pawn_offset;

      // Square ahead of pawn is empty: forward moves
      if (board_->square_[square_one_forward] == piece::EMPTY) {
        if (!IsPinned(start_square) || IsMovingAlongRay(pawn_offset, start_square, friendly_king_square_)) {
          if (!in_check_ || SquareIsInCheckRay(square_one_forward)) {
            if (one_step_from_promotion) {
              MakePromotionMoves(start_square, square_one_forward);
            } else {
              moves_.emplace_back(start_square, square_one_forward);
            }
          }

          if (rank == start_rank) {
            int square_two_forward = square_one_forward + pawn_offset;
            if (board_->square_[square_two_forward] == piece::EMPTY) {
              if (!in_check_ || SquareIsInCheckRay(square_two_forward)) {
                moves_.emplace_back(start_square, square_two_forward, Move::Type::pawn_jump);
              }
            }
          }
        }
      }
    }

    // Pawn captures.
    for (int j = 0; j < 2; j++) {
      if (Pre::num_squares_to_edge[start_square][Pre::PAWN_ATTACKING_DIRECTIONS[friendly_color_index_][j]] > 0) {
        // move in direction friendly pawns attack to get square from which
        // enemy pawn would attack
        int pawn_capture_dir = Pre::direction_offsets[Pre::PAWN_ATTACKING_DIRECTIONS[friendly_color_index_][j]];
        int target_square = start_square + pawn_capture_dir;
        int target_piece = board_->square_[target_square];

        // If piece is pinned, and the square it wants to move to is not on same
        // line as the pin, then skip this direction
        if (IsPinned(start_square) && !IsMovingAlongRay(pawn_capture_dir, friendly_king_square_, start_square)) {
          continue;
        }

        // Regular capture
        if (piece::IsColor(target_piece, opponent_color_)) {
          // If in check, and piece is not capturing/interposing the checking
          // piece, then skip to next square
          if (in_check_ && !SquareIsInCheckRay(target_square)) {
            continue;
          }
          if (one_step_from_promotion) {
            MakePromotionMoves(start_square, target_square);
          } else {
            moves_.emplace_back(start_square, target_square);
          }
        }

        // Capture en-passant
        if (target_square == en_passant_square) {
          int en_passant_captured_pawn_square = target_square + ((board_->white_to_move_) ? -8 : 8);
          if (!InCheckAfterEnPassant(start_square, target_square, en_passant_captured_pawn_square)) {
            moves_.emplace_back(start_square, target_square, Move::Type::en_passant);
          }
        }
      }
    }
  }
}

void MoveGenerator::MakePromotionMoves(int start_square, int target_square) {
  moves_.emplace_back(start_square, target_square, Move::Type::promote_queen);
  if (promotion_mode_ == PromotionMode::All) {
    moves_.emplace_back(start_square, target_square, Move::Type::promote_bishop);
    moves_.emplace_back(start_square, target_square, Move::Type::promote_knight);
    moves_.emplace_back(start_square, target_square, Move::Type::promote_rook);
  }
}

auto MoveGenerator::FilterMovesWithStartSquare(const std::vector<Move> &moves, int start_square) -> std::vector<Move> {
  std::vector<Move> filtered_moves;
  for (const auto &move : moves) {
    if (move.Start() == start_square) {
      filtered_moves.push_back(move);
    }
  }
  return filtered_moves;
}

auto MoveGenerator::IsMovingAlongRay(int offset, int start, int end) -> bool {
  int move_dir = Pre::direction_lookup[end - start + 63];
  
  return (offset == move_dir || -offset == move_dir);
}

auto MoveGenerator::IsPinned(int square) -> bool {
  return pins_in_position_ && ((pin_ray_bit_mask_ >> square) & 1) != 0;
}

auto MoveGenerator::HasQueenSideCastleRights() -> bool {
  int mask = board_->white_to_move_ ? 2 : 8;
  return (board_->current_game_state_ & mask) != 0;
}

auto MoveGenerator::HasKingSideCastleRights() -> bool {
  int mask = board_->white_to_move_ ? 1 : 4;
  return (board_->current_game_state_ & mask) != 0;
}

auto MoveGenerator::SquareIsInCheckRay(int square) -> bool {
  return in_check_ && bitboardContainsSquare(check_ray_bit_mask_, square);
}

auto MoveGenerator::InCheck() -> bool { return in_check_; }

void MoveGenerator::GenSlidingAttackMap() {
  opponent_sliding_attack_map_ = 0;

  PieceList &enemy_rooks = *(board_->rooks_[opponent_color_index_]);
  for (int i = 0; i < enemy_rooks.Count(); i++) {
    UpdateSlidingAttackPiece(enemy_rooks[i], 0, 4);
  }

  PieceList &enemy_queens = *(board_->queens_[opponent_color_index_]);
  for (int i = 0; i < enemy_queens.Count(); i++) {
    UpdateSlidingAttackPiece(enemy_queens[i], 0, 8);
  }

  PieceList &enemy_bishops = *(board_->bishops_[opponent_color_index_]);
  for (int i = 0; i < enemy_bishops.Count(); i++) {
    UpdateSlidingAttackPiece(enemy_bishops[i], 4, 8);
  }
}

void MoveGenerator::UpdateSlidingAttackPiece(int start_square, int start_dir_index, int end_dir_index) {
  for (int dir_index = start_dir_index; dir_index < end_dir_index; dir_index++) {
    int current_dir_offset = Pre::direction_offsets[dir_index];
    for (int n = 0; n < Pre::num_squares_to_edge[start_square][dir_index]; n++) {
      int target_square = start_square + current_dir_offset * (n + 1);
      int target_square_piece = board_->square_[target_square];
      opponent_sliding_attack_map_ |= 1UL << target_square;
      if (target_square != friendly_king_square_ && target_square_piece != piece::EMPTY) {
        break;
      }
    }
  }
}

void MoveGenerator::CalculateAttackData() {
  GenSlidingAttackMap();
  // Search squares in all directions around friendly king for checks/pins by
  // enemy sliding pieces (queen, rook, bishop)
  int start_dir_index = 0;
  int end_dir_index = 8;

  if (board_->queens_[opponent_color_index_]->Count() == 0) {
    start_dir_index = (board_->rooks_[opponent_color_index_]->Count() > 0) ? 0 : 4;
    end_dir_index = (board_->bishops_[opponent_color_index_]->Count() > 0) ? 8 : 4;
  }

  for (int dir = start_dir_index; dir < end_dir_index; dir++) {
    bool is_diagonal = dir > 3;

    int n = Pre::num_squares_to_edge[friendly_king_square_][dir];
    int direction_offset = Pre::direction_offsets[dir];
    bool is_friendly_piece_along_ray = false;
    u64 ray_mask = 0;

    for (int i = 0; i < n; i++) {
      int square_index = friendly_king_square_ + direction_offset * (i + 1);
      ray_mask |= 1UL << square_index;
      int piece = board_->square_[square_index];

      // This square contains a piece
      if (piece != piece::EMPTY) {
        if (piece::IsColor(piece, friendly_color_)) {
          // First friendly piece we have come across in this direction, so it
          // might be pinned
          if (!is_friendly_piece_along_ray) {
            is_friendly_piece_along_ray = true;
          } else {
            // This is the second friendly piece we've found in this direction,
            // therefore pin is not possible
            break;
          }
        } else {
          // This square contains an enemy piece
          int piece_type = piece::PieceType(piece);

          // Check if piece is in bitmask of pieces able to move in current
          // direction
          if ((is_diagonal && piece::IsBishopOrQueen(piece_type)) ||
              (!is_diagonal && piece::IsRookOrQueen(piece_type))) {
            // Friendly piece blocks the check, so this is a pin
            if (is_friendly_piece_along_ray) {
              pins_in_position_ = true;
              pin_ray_bit_mask_ |= ray_mask;
            } else {
              // No friendly piece blocking the attack, so this is a check
              check_ray_bit_mask_ |= ray_mask;
              in_double_check_ = in_check_;  // if already in check, then this is double check
              in_check_ = true;
            }
            break;
          }  // This enemy piece is not able to move in the current direction,
          // and so is blocking any checks/pins
          break;
        }
      }
    }
    // Stop searching for pins if in double check, as the king is the only piece
    // able to move in that case anyway
    if (in_double_check_) {
      break;
    }
  }

  // Knight attacks
  auto opponent_knights = board_->knights_[opponent_color_index_];
  opponent_knight_attacks_map_ = 0;
  bool is_knight_check = false;

  for (int knight_index = 0; knight_index < opponent_knights->Count(); knight_index++) {
    int start_square = (*opponent_knights)[knight_index];
    opponent_knight_attacks_map_ |= Pre::knight_attack_bitboards[start_square];

    if (!is_knight_check && bitboardContainsSquare(opponent_knight_attacks_map_, friendly_king_square_)) {
      is_knight_check = true;
      in_double_check_ = in_check_;  // if already in check, then this is double check
      in_check_ = true;
      check_ray_bit_mask_ |= 1UL << start_square;
    }
  }

  // Pawn attacks
  auto opponent_pawns = board_->pawns_[opponent_color_index_];
  opponent_pawn_attack_map_ = 0;
  bool is_pawn_check = false;

  for (int pawn_index = 0; pawn_index < opponent_pawns->Count(); pawn_index++) {
    int pawn_square = (*opponent_pawns)[pawn_index];
    u64 pawn_attacks = Pre::pawn_attack_bitboards[pawn_square][opponent_color_index_];
    opponent_pawn_attack_map_ |= pawn_attacks;

    if (!is_pawn_check && bitboardContainsSquare(pawn_attacks, friendly_king_square_)) {
      is_pawn_check = true;
      in_double_check_ = in_check_;  // if already in check, then this is double check
      in_check_ = true;
      check_ray_bit_mask_ |= 1UL << pawn_square;
    }
  }

  int enemy_king_square = board_->king_square_[opponent_color_index_];

  opponent_attack_map_no_pawns_ =
      opponent_sliding_attack_map_ | opponent_knight_attacks_map_ | Pre::king_attack_bitboards[enemy_king_square];
  opponent_attack_map_ = opponent_attack_map_no_pawns_ | opponent_pawn_attack_map_;
}

auto MoveGenerator::SquareIsAttacked(int square) -> bool {
  return bitboardContainsSquare(opponent_attack_map_, square);
}

auto MoveGenerator::InCheckAfterEnPassant(int start_square, int target_square, int ep_captured_pawn_square) -> bool {
  // Update board to reflect en-passant capture
  board_->square_[target_square] = board_->square_[start_square];
  board_->square_[start_square] = piece::EMPTY;
  board_->square_[ep_captured_pawn_square] = piece::EMPTY;

  bool in_check_after_ep_capture = false;
  if (SquareAttackedAfterEpCapture(ep_captured_pawn_square, start_square)) {
    in_check_after_ep_capture = true;
  }

  // Undo change to board
  board_->square_[target_square] = piece::EMPTY;
  board_->square_[start_square] = piece::PAWN | friendly_color_;
  board_->square_[ep_captured_pawn_square] = piece::PAWN | opponent_color_;
  return in_check_after_ep_capture;
}

auto MoveGenerator::SquareAttackedAfterEpCapture(int ep_capture_square, int capturing_pawn_start_square) -> bool {
  if (bitboardContainsSquare(opponent_attack_map_no_pawns_, friendly_king_square_)) {
    // the captured pawn could be checking the king
    return true;
  }
  // Loop through the horizontal direction towards ep capture to see if any
  // enemy piece now attacks king
  int dir_index = (ep_capture_square < friendly_king_square_) ? 2 : 3;
  for (int i = 0; i < Pre::num_squares_to_edge[friendly_king_square_][dir_index]; i++) {
    int square_index = friendly_king_square_ + Pre::direction_offsets[dir_index] * (i + 1);
    int piece = board_->square_[square_index];
    if (piece != piece::EMPTY) {
      // Friendly piece is blocking view of this square from the enemy.
      if (piece::IsColor(piece, friendly_color_)) {
        break;
      }  // This square contains an enemy piece
      if (piece::IsRookOrQueen(piece)) {
        return true;
      }  // This piece is not able to move in the current direction, and is
      // therefore blocking any checks along this line
      break;
    }
  }

  return false;
}

}  // namespace chess
