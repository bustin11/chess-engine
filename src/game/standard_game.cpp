

#include "standard_game.hpp"
#include <stdio.h>

// TODO: Make sure to capture the last move, and the piece that last moved so 
// that we can store the check history and from there, check xrays

namespace chess {

StandardGame::StandardGame() {}
StandardGame::~StandardGame() {
  // TODO: cleanup board
}

void StandardGame::generate_sliding_moves(square_t start_square, piece_t piece) {

  // check double check first
  if (board_.king_in_double_check()) return;

  auto& board = board_.board_;

  auto friendly_color = get_color(piece);
  auto opponent_color = opposite_color(friendly_color);

  bool sliding_check = board_.king_in_sliding_check();
  bool non_sliding_check = board_.king_in_non_sliding_check();

  auto start_dir_index = (is_type(piece, BISHOP)) ? 4 : 0;
  auto end_dir_index = (is_type(piece, ROOK)) ? 4 : 8;

  for (auto dir_index = start_dir_index; dir_index < end_dir_index;
       dir_index++) {
    bool pinned = false;
    for (auto line_index = 0; line_index < 4; line_index++) {
      if (dir_index / 2 == line_index) continue;
      if (board_.is_pinned_on_line(line_index, start_square)) {
        pinned = true;
        break;
      }
    }
    if (!pinned) {
      for (auto n = 0; n < num_squares_to_edge[start_square][dir_index]; n++) {

        auto target_square = start_square + direction_offsets[dir_index] * (n + 1);
        auto target_piece = board[target_square];

        if (is_color(target_piece, friendly_color)) {
          break;
        }

        if (is_color(target_piece, opponent_color)) {
          if (sliding_check || non_sliding_check) {
            if (board_.still_check_without(target_square, target_piece)) break;
          }
          moves_.emplace_back(start_square, target_square);
          moves_set_.emplace(start_square, target_square);
          break;
        }
        
        if (non_sliding_check) continue;
        if (sliding_check && board_.still_check_with(target_square, piece)) continue;
        moves_.emplace_back(start_square, target_square);
        moves_set_.emplace(start_square, target_square);

        // LOGIC for xrays, qrb attacking squares

      }
    }
  }
}

void StandardGame::generate_pawn_moves(square_t start_square, piece_t piece) {

  // check double check first
  if (board_.king_in_double_check()) return;

  auto& board = board_.board_;
  auto friendly_color = board_.color_to_move_;
  bool sliding_check = board_.king_in_sliding_check();
  bool non_sliding_check = board_.king_in_non_sliding_check();

  if (board_.is_pinned_row(start_square)) {
    return;
  }

  // check pins first
  if (!(board_.is_pinned_left_diagonal(start_square) || board_.is_pinned_right_diagonal(start_square))) {

    auto dir_index = is_color(piece, BLACK);
    auto target_square = start_square + direction_offsets[dir_index];
    if (in_bounds(target_square) && board[target_square] == EMPTY) {

      if (!non_sliding_check) {
        LOG_DEBUG("target square is %d", target_square);
        if (!sliding_check || !board_.still_check_with(target_square, piece)) { 
          moves_.emplace_back(start_square, target_square);
          moves_set_.emplace(start_square, target_square);
        }

        target_square += direction_offsets[dir_index];
        
        bool white_double = on_second_rank(start_square) && is_color(piece, WHITE);
        bool black_double = on_seventh_rank(start_square) && is_color(piece, BLACK);
        if (in_bounds(target_square) && (white_double || black_double) && board[target_square] == EMPTY) {
          if (!sliding_check || !board_.still_check_with(target_square, piece)) { 
            moves_.emplace_back(start_square, target_square);
            moves_set_.emplace(start_square, target_square);
          }
        }
      }
    }
  }

  if (!board_.is_pinned_col(start_square)) {
    array<int, 2> capture_indexes{4 + is_color(piece, BLACK), 6 + is_color(piece, BLACK)};
    for (auto index : capture_indexes) {
      auto target_square = start_square + direction_offsets[index];
      if (!in_bounds(target_square)) continue;
      if (num_squares_to_edge[start_square][index] == 0) continue;
      if (board[target_square] == EMPTY && board_.en_passant_square_ != target_square) continue;
      if (get_color(board[target_square]) == get_color(piece)) continue;
      if (board[target_square] == EMPTY) {
        // en passant
        auto new_target_square = target_square + direction_offsets[friendly_color == WHITE];
        auto before = board[new_target_square];
        board[new_target_square] = EMPTY;
        if (board_.is_pinned_row(start_square, true)) {
          board[new_target_square] = before;
          // en passant pin
          continue;
        }
        board[new_target_square] = before;
        if (sliding_check && board_.still_check_with(target_square, board[start_square])) continue;
      }
      if (board_.is_pinned_on_line(5 - index / 2, start_square)) {
        // 5 since 2 and 3 are dir_id
        continue;
      }

      if ((sliding_check || non_sliding_check) && board_.still_check_without(target_square, board[target_square])) continue;

      moves_.emplace_back(start_square, target_square);
      moves_set_.emplace(start_square, target_square);
    }
  }
}

void StandardGame::generate_king_moves(square_t start_square, piece_t piece) {

  auto& board = board_.board_;
  auto color = board_.color_to_move_;

  // moving 1 square
  for (int dir_index = 0; dir_index < 8; dir_index++) {
    if (num_squares_to_edge[start_square][dir_index] == 0) continue;
    auto target_square = start_square + direction_offsets[dir_index];
    auto target_piece = board[target_square];
    if (is_color(target_piece, color)) continue;
    if (board[target_square] == EMPTY && board_.king_in_check(target_square, true)) continue; // TODO: change this later
    if (board_.king_in_check(target_square)) continue;
    moves_.emplace_back(start_square, target_square);
    moves_set_.emplace(start_square, target_square);
  }

  bool in_check = board_.king_in_check();

  // castling
  if (board_.can_castle_king_side(color)) {
    auto target_square = start_square + 2;
    if (!in_check && !board_.king_in_check(start_square + 1) &&
      !board_.king_in_check(start_square + 2)) {

      moves_.emplace_back(start_square, target_square);
      moves_set_.emplace(start_square, target_square);
    }
  }
  if (board_.can_castle_queen_side(color)) {
    auto target_square = start_square - 2;
    if (!in_check && !board_.king_in_check(start_square - 1) &&
      !board_.king_in_check(start_square - 2)) {

      moves_.emplace_back(start_square, target_square);
      moves_set_.emplace(start_square, target_square);
    }
  }

}

void StandardGame::generate_knight_moves(square_t start_square, piece_t piece) {

  // check double check first
  if (board_.king_in_double_check()) return;

  // check absolute pins second
  if (board_.is_pinned_left_diagonal(start_square) || board_.is_pinned_right_diagonal(start_square)) {
    return;
  }

  if (board_.is_pinned_col(start_square) || board_.is_pinned_row(start_square)) {
    return;
  }

  xy_index(sx, sy, start_square)
  auto& board = board_.board_;
  auto sliding_check = board_.king_in_sliding_check();
  auto non_sliding_check = board_.king_in_non_sliding_check();

  for (auto target_offset : knight_square_offsets) {
    auto target_square = start_square + target_offset;
    xy_index(tx, ty, target_square)
    if (in_bounds(target_square) && abs(tx - sx) + abs(ty - sy) == 3) { // TODO: CHANGE THIS LATER

      auto target_piece = board[target_square];
      if (get_color(target_piece) == board_.color_to_move_) continue; 

      if (non_sliding_check) {
        if (target_piece == EMPTY || board_.still_check_without(target_square, target_piece)) continue;
      }
      if (sliding_check) {
        if (target_piece != EMPTY) { // capture
          if (board_.still_check_without(target_square, target_piece)) continue;
        } else {
          if (board_.still_check_with(target_square, piece)) continue;
        }
      }

      moves_.emplace_back(start_square, target_square);
      moves_set_.emplace(start_square, target_square);
    }
  }

}

void StandardGame::generate_moves_for_piece(square_t start_square, piece_t piece) {
  moves_.clear();
  moves_set_.clear();
  if (is_type(piece, PAWN)) {
    generate_pawn_moves(start_square, piece);
  } else if (is_type(piece, KNIGHT)) {
    generate_knight_moves(start_square, piece);
  } else if (is_type(piece, KING)) {
    generate_king_moves(start_square, piece);
  } else {
    generate_sliding_moves(start_square, piece);
  }
}

// TOOD: test this function with 1-ply, 2-ply, etc. (need depth factor, color to move)
void StandardGame::generate_moves() {
  moves_.clear();
  // for (auto start_square : board_.meta_data_[board_.color_to_move_ == WHITE].piece_location_) {
  for (square_t start_square = 0; start_square < NUM_SQUARES; start_square++) {
    piece_t piece = board_.board_[start_square];
    if (is_color(piece, board_.color_to_move_)) {
      if (is_sliding_piece(piece)) {
        generate_sliding_moves(start_square, piece);
      } else if (is_type(piece, PAWN)) {
        generate_pawn_moves(start_square, piece);
      } else if (is_type(piece, KNIGHT)) {
        generate_knight_moves(start_square, piece);
      } else { // KING
        generate_king_moves(start_square, piece);
      }
    }
  }

}

bool StandardGame::is_legal_move(const Move &move) {
  // return true;
  return moves_set_.count(move);
}

void StandardGame::make_move(const Move& move, piece_t promotion_piece) {

  // 1) assert turn is correct
  if (board_.color_to_move_ != get_color(board_.board_[move.from_])) {
    LOG_WARN("Player not allowed to make move, color: %d", board_.color_to_move_);
    return;
  }

  // TODO: validate move

  print_move(move);

  if (is_legal_move(move)) {
    auto& board = board_.board_;
    auto piece = board[move.from_];
    piece_t target_piece = board[move.to_];
    piece_t captured_piece = EMPTY;

    board_.en_passant_square_ = -1;
    if (is_type(piece, PAWN)) {
      if (is_en_passant_capture(piece, target_piece, move)) { // en passant
        auto dir_index = is_color(piece, WHITE);
        auto target_square = move.to_ + direction_offsets[dir_index];
        // captured_piece = board[target_square]; // not needed
        board[target_square] = EMPTY; 
        LOG_DEBUG("[en passant capture] @ %d", board_.full_moves_+1);
      } else if (on_first_rank(move.to_) || on_eigth_rank(move.to_)) { 
        if (!is_promotion_piece(promotion_piece))
          throw std::logic_error("Can't promote to requested piece type");
        board[move.from_] = get_color(piece) | promotion_piece;
        captured_piece = PROMOTION; // indictes a promotion occurred
        LOG_DEBUG("[pawn promotion] @ %d", board_.full_moves_+1);
        LOG_DEBUG("[piece promoted to] %s", piece_string_map.at(promotion_piece).c_str());
      } else if (is_double_square_pawn_push(piece, move)) {
        board_.en_passant_square_ = move.to_ + direction_offsets[is_color(piece, WHITE)];
        LOG_DEBUG("[double square pawn push] @ %d", board_.full_moves_);
      }
    }
    captured_piece |= target_piece;

    if (is_type(target_piece, ROOK)) {  
      // TAKING ROOK
      switch(move.to_) {
        case 0:
          board_.castling_rights_ &= 11;
          break;
        case 7:
          board_.castling_rights_ &= 7;
          break;
        case 56:
          board_.castling_rights_ &= 14;
          break;
        case 63:
          board_.castling_rights_ &= 13;
          break;
        default:
          // do nothing
          break;
      }
      LOG_DEBUG("[captured rook] @ %d", board_.full_moves_ + 1);
    }
    
    
    swap(board[move.from_], board[move.to_]);
    board[move.from_] = EMPTY;

    // changes in castling by moving
    if (is_type(piece, ROOK)) { 
      // losing castling rights by moving the rook
      switch(move.from_) {
        case 0:
          board_.castling_rights_ &= 11;
          break;
        case 7:
          board_.castling_rights_ &= 7;
          break;
        case 56:
          board_.castling_rights_ &= 14;
          break;
        case 63:
          board_.castling_rights_ &= 13;
          break;
        default:
          // do nothing
          break;
      }
      LOG_DEBUG("[rook moved] @ %d", board_.full_moves_);
    } else if (is_type(piece, KING)) {
      if (is_king_side_castling(piece, move)) {
        // move rook
        swap(board[move.to_ + 1], board[move.from_ + 1]);
        LOG_DEBUG("[king side castling] @ %d", board_.full_moves_);

      } else if (is_queen_side_castling(piece, move)) {
        // move rook
        swap(board[move.to_ - 2], board[move.from_ - 1]);
        LOG_DEBUG("[queen side castling] @ %d", board_.full_moves_);
      } 
      board_.castling_rights_ &= (board_.color_to_move_ == WHITE ? 3 : 12);
    }

    
    board_.color_to_move_ = opposite_color(board_.color_to_move_);
    board_.move_history_.emplace_back(move, board_.castling_rights_, captured_piece, board_.en_passant_square_);
    board_.full_moves_ = board_.move_history_.size();

    LOG_DEBUG("caslting rights are %d", board_.castling_rights_);
    board_.dump_data();
  } else {

  }

}

void StandardGame::make_move(square_t from, square_t to, piece_t promotion_piece) {
  make_move(Move(from, to), promotion_piece);
}

void StandardGame::make_move(const string &move, piece_t promotion_piece) {
  // convert move to index position (from and to)
  // size_t pos = move.find('-');
  auto from = move_index_map.at(move.substr(0, 2));
  auto to = move_index_map.at(move.substr(2));

  make_move(from, to, promotion_piece);
}

void StandardGame::undo_move() {
  if (board_.move_history_.empty()) {
    LOG_WARN("Can't undo move: move history is empty");
    return;
  }

  auto& last_event = board_.move_history_.back();
  auto& last_move = last_event.move_;
  auto& board = board_.board_;
  piece_t last_moved_piece = board[last_move.to_];
  color_t last_move_color = get_color(last_moved_piece);
  
  swap(board[last_move.from_], board[last_move.to_]);

  // moving rook back
  if (is_king_side_castling(last_moved_piece, last_move)) {
    swap(board[last_move.from_ + 1], board[last_move.to_ + 1]);
  } else if (is_queen_side_castling(last_moved_piece, last_move)) {
    swap(board[last_move.from_ - 1], board[last_move.to_ - 2]);
  }

  // pawn promotion
  if (on_eigth_rank(last_move.to_) || on_first_rank(last_move.to_)) {
    if (last_event.captured_piece_ & PROMOTION) { // promotion
      board[last_move.from_] = last_move_color | PAWN;
      LOG_DEBUG("[undoing pawn promotion]");
    }
  }
  
  if ((last_event.captured_piece_ & ~PROMOTION) != EMPTY) {
    board[last_move.to_] = (last_event.captured_piece_ & ~PROMOTION);
  }
  
  // // TODO: create a function that identifies moves
  board_.move_history_.pop_back();
  board_.full_moves_ = board_.move_history_.size();
  board_.color_to_move_ = opposite_color(board_.color_to_move_);
  board_.en_passant_square_ = -1;
  
  if (board_.move_history_.size() > 0) {
    auto& curr_last_event = board_.move_history_.back();
    board_.en_passant_square_ = curr_last_event.en_passant_square_;
    board_.castling_rights_ = curr_last_event.castling_rights_;

    // restore captured pawn if en passant
    if (board_.en_passant_square_ == last_move.to_ && is_type(last_moved_piece, PAWN)) {
      auto target_square = last_move.to_ + direction_offsets[last_move_color == WHITE];
      board[target_square] = PAWN | opposite_color(last_move_color);
    }
  } else if (board_.move_history_.size() == 0) {
    board_.castling_rights_ = board_.original_castling_rights_;
    board_.en_passant_square_ = board_.original_en_passant_square_;
  }
  board_.dump_data();
}

}; // namespace chess