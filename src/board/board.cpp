

#include "board.hpp"

namespace chess {

// https://stackoverflow.com/questions/21488744/how-to-defined-constructor-outside-of-template-class
template <typename T> void Board<T>::load_from_fen(const string &fen) {

    LOG_TRACE("loading fen: %s", fen.c_str());
    // place pieces on board
    int index = 0;
    for (char c : fen) {
      if (c == ' ')
        break;
      if (c == '/') {
        continue;
      } else if (c >= '1' && c <= '8') {
        int numEmptySquares = c - '0';
        index += numEmptySquares;
      } else {
        board_[index] = char_piece_map.at(c);
        index++;
      }
    }


    // fen starts upper left to bottom right
    // my board starts from bottom left to upper right
    for (square_t square = 0; square < NUM_SQUARES / 2; square++) {
      square_t row = NUM_ROW - 1 - (square / NUM_ROW);
      square_t col = square % NUM_ROW;
      square_t target_square = row * NUM_ROW + col;
      swap(board_[square], board_[target_square]);
    }

    // Parse additional information
    size_t pos = fen.find(' ');
    if (pos != string::npos && pos + 1 < fen.size()) {
      string additionalInfo = fen.substr(pos + 1);
      pos = additionalInfo.find(' ');
      LOG_DEBUG("%s",additionalInfo.c_str());

      if (pos != string::npos && pos + 1 < additionalInfo.size()) {
        color_to_move_ = additionalInfo[0] == 'w' ? WHITE : BLACK;
        additionalInfo = additionalInfo.substr(pos + 1);
LOG_DEBUG("%s",additionalInfo.c_str());
        pos = additionalInfo.find(' ');
        if (pos != string::npos && pos + 1 < additionalInfo.size()) {
          string castling = additionalInfo.substr(0, pos);
          string KQkq = "KQkq";
          for (int i = 3; i >= 0; i--) {
            if (castling.find(KQkq[3-i]) != string::npos) {
              castling_rights_ |= (1 << i);
            }
          }
          original_castling_rights_ = castling_rights_;
          LOG_DEBUG("castling rights %s are %d", castling.c_str(), castling_rights_);
          additionalInfo = additionalInfo.substr(pos + 1);
LOG_DEBUG("%s",additionalInfo.c_str());
          pos = additionalInfo.find(' ');
          if (pos != string::npos && pos + 1 < additionalInfo.size()) {
            if (additionalInfo.substr(0, pos) != "-")
              en_passant_square_ = move_index_map.at(additionalInfo.substr(0, pos));
            original_en_passant_square_ = en_passant_square_;
            additionalInfo = additionalInfo.substr(pos + 1);
LOG_DEBUG("%s",additionalInfo.c_str());
            pos = additionalInfo.find(' ');
            if (pos != string::npos && pos + 1 < additionalInfo.size()) {
              half_moves_ = stoi(additionalInfo.substr(0, pos));
              additionalInfo = additionalInfo.substr(pos + 1);
LOG_DEBUG("%s",additionalInfo.c_str());
              pos = additionalInfo.find(' ');
              if (pos != string::npos && pos + 1 < additionalInfo.size()) {
                full_moves_ = stoi(additionalInfo.substr(0, pos));
              }
            }
          }
        }
      }
    }
    dump_data();
  }

// TODO fixed captures and updates
template <typename T> void Board<T>::dump_data() {
    
  for (int i = 0; i < 2; i++) {
    auto& meta_data = meta_data_[i];
    auto& location = meta_data.piece_location_;
    auto& knp = meta_data.knp_attacking_counts_;
    auto& qrb = meta_data.qrb_attacking_counts_;
    auto& xray = meta_data.xray_counts_;
    for (square_t start_square = 0; start_square < NUM_SQUARES; start_square++) {
      xray[start_square] = knp[start_square] = qrb[start_square] = 0;
    }
  }
  
  meta_data_[0].piece_location_.clear();
  meta_data_[1].piece_location_.clear();

  for (square_t start_square = 0; start_square < NUM_SQUARES; start_square++) {

    piece_t piece = board_[start_square];
    if (piece == EMPTY) continue;
    int meta_data_index = is_color(piece, WHITE);
    auto& meta_data = meta_data_[meta_data_index];
    auto& knp = meta_data.knp_attacking_counts_;
    auto& qrb = meta_data.qrb_attacking_counts_;
    auto& xray = meta_data.xray_counts_;
    auto friendly_color = get_color(piece);
    auto opponent_color = opposite_color(friendly_color);
    auto& location = meta_data.piece_location_;
    location.insert(start_square);

    if (is_sliding_piece(piece)) {
      print_piece(piece);
      auto start_dir_index = (is_type(piece, BISHOP)) ? 4 : 0;
      auto end_dir_index = (is_type(piece, ROOK)) ? 4 : 8;

      for (auto dir_index = start_dir_index; dir_index < end_dir_index;
          dir_index++) {
        bool xrayed = false;
        for (auto n = 0; n < num_squares_to_edge[start_square][dir_index]; n++) {

          auto target_square = start_square + direction_offsets[dir_index] * (n + 1);
          auto target_piece = board_[target_square];

          if (xrayed) {
            xray[target_square]++;
            if (target_piece != EMPTY) {
              break;
            }
          } else {
            qrb[target_square]++;
            if (target_piece != EMPTY) {
              xrayed = true;
            }
          }

        }
      }
    } else if (is_type(piece, PAWN)) {
      array<int, 2> capture_indexes{5 - meta_data_index, 7 - meta_data_index};
      for (auto index : capture_indexes) {
        auto target_square = start_square + direction_offsets[index];
        if (num_squares_to_edge[start_square][index] == 0) continue;
        knp[target_square]++;
      }
    } else if (is_type(piece, KNIGHT)) {
      xy_index(sx, sy, start_square)
      for (auto target_offset : knight_square_offsets) {
        auto target_square = start_square + target_offset;
        xy_index(tx, ty, target_square)
        if (in_bounds(target_square) && abs(tx - sx) + abs(ty - sy) == 3) { // TODO: CHANGE THIS LATER
          auto target_piece = board_[target_square];
          knp[target_square]++;
        }
      }
    } else { // KING
      meta_data.king_position_ = start_square;
      // moving 1 square
      for (int dir_index = 0; dir_index < 8; dir_index++) {
        if (num_squares_to_edge[start_square][dir_index] == 0) continue;
        auto target_square = start_square + direction_offsets[dir_index];
        auto target_piece = board_[target_square];
        knp[target_square]++;
      }
    }
  }
}

template <typename T> bool Board<T>::is_pinned_row(square_t start_square, bool hypothetical) {
  return is_pinned_on_line(1, start_square, hypothetical);
}

template <typename T> bool Board<T>::is_pinned_col(square_t start_square, bool hypothetical) {
  LOG_DEBUG("start square is %d", start_square);
  return is_pinned_on_line(0, start_square, hypothetical);
}

template <typename T> bool Board<T>::is_pinned_left_diagonal(square_t start_square, bool hypothetical) {
  return is_pinned_on_line(2, start_square, hypothetical);
}

template <typename T> bool Board<T>::is_pinned_right_diagonal(square_t start_square, bool hypothetical) {
  return is_pinned_on_line(3, start_square, hypothetical);
}

// helper
template <typename T> bool Board<T>::is_pinned_on_line(int dir_id, square_t start_square, bool hypothetical) {

  auto& meta_data = meta_data_[(color_to_move_ == BLACK)]; // your opponent
  auto& knp = meta_data.knp_attacking_counts_;
  auto& qrb = meta_data.qrb_attacking_counts_;
  auto& xray = meta_data.xray_counts_;

  auto king_position = meta_data_[color_to_move_ == WHITE].king_position_; // yourself
  auto friendly_color = color_to_move_;
  auto opponent_color = opposite_color(friendly_color);

  auto start_dir = 2 * dir_id;
  auto end_dir = start_dir + 1;

  if (on_line(dir_id, start_square, king_position)) {
    if ((hypothetical || xray[king_position] > 0) && qrb[start_square] > 0) {
      // candidate for possible pin, so we check (want to reduce these checks amap)
      bool king_is_directly_xrayed = false;
      bool sliding_piece_is_attacking = false;
      // TODO: change this later
      for (auto dir_index = start_dir; dir_index <= end_dir; dir_index++) {
        for (auto n = 0; n < num_squares_to_edge[start_square][dir_index]; n++) {
          auto target_square = start_square + direction_offsets[dir_index] * (n + 1);
          auto target_piece = board_[target_square];
          if (target_piece != EMPTY) {
            if (target_piece == (friendly_color | KING)) {
              king_is_directly_xrayed = true;
              break;
            }
            if (dir_id <= 1) {
              if (is_type(target_piece, QUEEN) || is_type(target_piece, ROOK)) {
                if (is_color(target_piece, opponent_color)) {
                  sliding_piece_is_attacking = true;
                  break;
                }
              }
            } else {
              if (is_type(target_piece, QUEEN) || is_type(target_piece, BISHOP)) {
                if (is_color(target_piece, opponent_color)) {
                  sliding_piece_is_attacking = true;
                  break;
                }
              }
            }
            break;
          }
        }
      }
      LOG_DEBUG("king %d, sliding %d", king_is_directly_xrayed, sliding_piece_is_attacking);
      return king_is_directly_xrayed && sliding_piece_is_attacking;
    }
  }
  return false;
}

template <typename T> inline bool Board<T>::king_in_double_check() {
  auto king_pos = meta_data_[color_to_move_ == WHITE].king_position_;
  auto& knp = meta_data_[color_to_move_ == BLACK].knp_attacking_counts_;
  auto& qrb = meta_data_[color_to_move_ == BLACK].qrb_attacking_counts_;
  LOG_DEBUG("how many pieces attacking the king %d and %d", knp[king_pos], qrb[king_pos]);
  return knp[king_pos] + qrb[king_pos] > 1; 
}

template <typename T> bool Board<T>::still_check_without(square_t start_square, piece_t piece) {

  CHESS_ASSERT(get_color(piece) == opposite_color(color_to_move_), "piece is wrong color or empty");

  int meta_data_index = is_color(piece, WHITE);
  auto& meta_data = meta_data_[meta_data_index];
  auto& knp = meta_data.knp_attacking_counts_;
  auto& qrb = meta_data.qrb_attacking_counts_;
  
  if (is_sliding_piece(piece)) {
    auto start_dir_index = (is_type(piece, BISHOP)) ? 4 : 0;
    auto end_dir_index = (is_type(piece, ROOK)) ? 4 : 8;
    // printf("let's go the distance\n");
    for (auto dir_index = start_dir_index; dir_index < end_dir_index;
        dir_index++) {
      for (auto n = 0; n < num_squares_to_edge[start_square][dir_index]; n++) {

        auto target_square = start_square + direction_offsets[dir_index] * (n + 1);
        auto target_piece = board_[target_square];

        if (target_piece != EMPTY) {
          if (target_piece == (color_to_move_ | KING)) return false;
          break;
        }

      }
    }
  } else if (is_type(piece, PAWN)) {
    array<int, 2> capture_indexes{5 - meta_data_index, 7 - meta_data_index};
    for (auto index : capture_indexes) {
      auto target_square = start_square + direction_offsets[index];
      auto target_piece = board_[target_square];
      if (num_squares_to_edge[start_square][index] == 0) continue;
      if (target_piece == (color_to_move_ | KING)) return false;
    }
  } else if (is_type(piece, KNIGHT)) {
    xy_index(sx, sy, start_square)
    for (auto target_offset : knight_square_offsets) {
      auto target_square = start_square + target_offset;
      xy_index(tx, ty, target_square)
      if (in_bounds(target_square) && abs(tx - sx) + abs(ty - sy) == 3) { // TODO: CHANGE THIS LATER
        auto target_piece = board_[target_square];
        if (target_piece == (color_to_move_ | KING)) return false;
      }
    }
  }

  return true;
}

template <typename T> bool Board<T>::still_check_with(square_t start_square, piece_t piece) {

  CHESS_ASSERT(get_color(piece) == color_to_move_, "piece is wrong color or empty");

  auto before = board_[start_square];
  board_[start_square] = piece;
  bool blocking = false;
  if (is_pinned_row(start_square, true) || is_pinned_col(start_square, true) 
    || is_pinned_left_diagonal(start_square, true) || is_pinned_right_diagonal(start_square, true)) {
      blocking = true;
  }
  board_[start_square] = before;
  return !blocking;
}

template <typename T> bool Board<T>::king_in_check(square_t target_square, bool check_xrays) {
  // CHESS_ASSERT(target_square == -1, "target square for king is non empty");
  auto king_pos = target_square == -1 ? 
    meta_data_[color_to_move_ == WHITE].king_position_ : target_square;
  auto& knp = meta_data_[color_to_move_ == BLACK].knp_attacking_counts_;
  auto& qrb = meta_data_[color_to_move_ == BLACK].qrb_attacking_counts_;\
  LOG_DEBUG("%d", check_xrays);
  if (check_xrays) {
    CHESS_ASSERT(board_[target_square] == EMPTY, "target square for king is non empty");
    auto start_square = meta_data_[color_to_move_ == WHITE].king_position_;
    board_[target_square] = color_to_move_ | KING;
    bool ok = knp[target_square] + qrb[target_square] > 0 || 
      (is_pinned_row(start_square, true) || is_pinned_col(start_square, true) 
      || is_pinned_left_diagonal(start_square, true) || is_pinned_right_diagonal(start_square, true)); 
    board_[target_square] = EMPTY;
    return ok;
  }
  return knp[king_pos] + qrb[king_pos] > 0; 
}

template class Board<piece_t>;

} // namespace chess
