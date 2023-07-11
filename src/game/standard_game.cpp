

#include "standard_game.hpp"

// TODO: check for color and capturing in generating moves

// TODO: should move have a capture (should be as small as possible if performing
// move generation), so we should keep a state of which pieces have been captured 
// on what time in _moves_history

namespace chess {

StandardGame::StandardGame() {}
StandardGame::~StandardGame() {
  // TODO: cleanup board
}

void StandardGame::generate_sliding_moves(int start_square, piece_t piece) {

  color_t friendly_color = get_color(piece);
  color_t opponent_color = opposite_color(friendly_color);

  int start_dir_index = (is_type(piece, BISHOP)) ? 4 : 0;
  int end_dir_index = (is_type(piece, ROOK)) ? 4 : 8;

  for (int dir_index = start_dir_index; dir_index < end_dir_index;
       dir_index++) {
    for (int n = 0; n < num_squares_to_edge[start_square][dir_index]; n++) {

      int target_square = start_square + direction_offsets[dir_index] * (n + 1);
      piece_t target_piece = _board._board[target_square];

      if (is_color(target_piece, friendly_color)) {
        break;
      }
      _moves.emplace_back(start_square, target_square);
      _moves_set.emplace(start_square, target_square);

      if (is_color(target_piece, opponent_color)) {
        break;
      }
    }
  }
}

void StandardGame::generate_pawn_moves(int start_square, piece_t piece) {

  int dir_index = is_color(piece, BLACK);
  int target_square = start_square + direction_offsets[dir_index];
  if (in_bounds(target_square) && _board._board[target_square] == EMPTY) {

    _moves.emplace_back(start_square, target_square);
    _moves_set.emplace(start_square, target_square);

    target_square = start_square + 2*direction_offsets[dir_index];
    
    bool white_double = on_second_rank(start_square) && is_color(piece, WHITE);
    bool black_double = on_seventh_rank(start_square) && is_color(piece, BLACK);
    if ((white_double || black_double) && _board._board[target_square] == EMPTY) {
      _moves.emplace_back(start_square, target_square);
      _moves_set.emplace(start_square, target_square);
    }
  }

  vector<int> capture_indexes{4 + is_color(piece, BLACK), 6 + is_color(piece, BLACK)};

  for (auto index : capture_indexes) {
    target_square = start_square + direction_offsets[index];
    if (!in_bounds(target_square)) continue;
    if (num_squares_to_edge[start_square][index] == 0) continue;
    if (_board._board[target_square] == EMPTY && _board._en_passant_square != target_square) continue;
    if (get_color(_board._board[target_square]) == get_color(piece)) continue;
    _moves.emplace_back(start_square, target_square);
    _moves_set.emplace(start_square, target_square);
  }

}

void StandardGame::generate_king_moves(int start_square, piece_t piece) {

  for (int dir_index = 0; dir_index < 8; dir_index++) {
    if (num_squares_to_edge[start_square][dir_index] == 0) continue;
    int target_square = start_square + direction_offsets[dir_index];
    piece_t target_piece = _board._board[target_square];
    bool same_team = get_color(piece) == get_color(target_piece);
    if (!same_team) {
      _moves.emplace_back(start_square, target_square);
      _moves_set.emplace(start_square, target_square);
    }
  }

  // castling
  int shift_offset = 2 * is_color(piece, WHITE);

  if (_board._castling_rights & (1 << (shift_offset + 1))) {
    if (_board._board[start_square+1] == EMPTY && _board._board[start_square+2] == EMPTY) {
      _moves.emplace_back(start_square, start_square+2);
      _moves_set.emplace(start_square, start_square+2);
    }
  } else if (_board._castling_rights & (1 << shift_offset)) {
    if (_board._board[start_square-1] == EMPTY && _board._board[start_square-2] == EMPTY) {
      _moves.emplace_back(start_square, start_square-2);
      _moves_set.emplace(start_square, start_square-2);
    }
  }

}

void StandardGame::generate_knight_moves(int start_square, piece_t piece) {

  int sx = start_square % NUM_ROW;
  int sy = start_square / NUM_ROW;
  for (auto target_offset : knight_square_offsets) {
    int target_square = start_square + target_offset;
    int tx = target_square % NUM_ROW;
    int ty = target_square / NUM_ROW;
    if (abs(tx - sx) + abs(ty - sy) == 3) { // TODO: CHANGE THIS LATER
      piece_t target_piece = _board._board[target_square];
      bool same_team = get_color(piece) == get_color(target_piece);
      if (!same_team) {
        _moves.emplace_back(start_square, target_square);
        _moves_set.emplace(start_square, target_square);
      }
    }
  }

}

void StandardGame::generate_moves_for_piece(int start_square, piece_t piece) {
  _moves.clear();
  _moves_set.clear();
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
  for (int start_square = 0; start_square < NUM_SQUARES; start_square++) {
    piece_t piece = _board._board[start_square];
    if (is_color(piece, _board._color_to_move)) {
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
  LOG_DEBUG("selected %s", move.to_str().c_str());
  return _moves_set.count(move);
}

void StandardGame::make_move(const Move &move, piece_t promotion_piece) {

  // 1) assert turn is correct
  if (_board._color_to_move != get_color(_board._board[move._from])) {
    LOG_WARN("Player not allowed to make move, color: %d", _board._color_to_move);
    return;
  }

  // 2.5) validate move:
  if (is_legal_move(move)) {

    piece_t piece_moved = _board._board[move._from];
    if (is_type(piece_moved, PAWN) && _board._board[move._to] == EMPTY) { // en passant capture
      // TODO: change this when flipping the board (actually can implement in the GUI)
      if (abs(abs(move._from - move._to) - NUM_ROW) == 1) { // diagonal movement
        int dir_index = is_color(piece_moved, WHITE);
        int target_square = move._to + direction_offsets[dir_index];
        _board._event_history.emplace_back(_board._fullMoves+1, 
                                                target_square,
                                                _board._board[target_square]);
        _board._board[target_square] = EMPTY; 
        LOG_DEBUG("[en passant capture] @ %d", _board._fullMoves+1);
      }
    }

    if (_board._board[move._to] != EMPTY) {
      _board._event_history.emplace_back(_board._fullMoves+1, move._to, _board._board[move._to]);
      LOG_DEBUG("[capture] @ %d", _board._fullMoves);
      if (is_type(_board._board[move._to], ROOK)) {
        switch(move._to) {
          case 0:
            _board._castling_rights &= 11;
            break;
          case 7:
            _board._castling_rights &= 7;
            break;
          case 56:
            _board._castling_rights &= 14;
            break;
          case 63:
            _board._castling_rights &= 13;
            break;
          default:
            // do nothing
            break;
        }
      }
    }

    swap(_board._board[move._from], _board._board[move._to]);

    _board._board[move._from] = EMPTY;
    _board._color_to_move = opposite_color(_board._color_to_move);
    _board._move_history.emplace_back(move._from, move._to);
    _board._fullMoves = _board._move_history.size();
    _board._en_passant_square = -1;

    if (is_type(piece_moved, ROOK)) {
      switch(move._from) {
        case 0:
          _board._castling_rights &= 11;
          break;
        case 7:
          _board._castling_rights &= 7;
          break;
        case 56:
          _board._castling_rights &= 14;
          break;
        case 63:
          _board._castling_rights &= 13;
          break;
        default:
          // do nothing
          break;
      }
    } else if (is_type(piece_moved, KING)) {
      if (move._to - move._from == 2) { // KING side castling
        swap(_board._board[move._to + 1], _board._board[move._from + 1]);
      } else if (move._to - move._from == -2) { // QUEEN side castling
        swap(_board._board[move._to - 2], _board._board[move._from - 1]);
      }
      // remove castling rights
      if (is_color(piece_moved, WHITE)) {
        _board._castling_rights &= 3;
      } else {
        _board._castling_rights &= 12;
      }
    } else if (is_type(piece_moved, PAWN)) {
      // recording whether en passant is possible
      bool white_double = on_second_rank(move._from) && is_color(piece_moved, WHITE);
      bool black_double = on_seventh_rank(move._from) && is_color(piece_moved, BLACK);
      if ((white_double || black_double) && abs(move._from - move._to) == 16) {
        _board._en_passant_square = move._to + direction_offsets[is_color(piece_moved, WHITE)];
        _board._event_history.emplace_back(_board._fullMoves, _board._en_passant_square);
        LOG_DEBUG("[en passant square] @ %d", _board._fullMoves);
      }
      // handle pawn promotion
      if (on_first_rank(move._to) || on_eigth_rank(move._to)) {
        if (!is_promotion_piece(promotion_piece))
          throw std::logic_error("Can't promote to requested piece type");
        _board._board[move._to] = get_color(piece_moved) | promotion_piece;
        _board._event_history.emplace_back(_board._fullMoves);
        LOG_DEBUG("[pawn promotion] @ %d", _board._fullMoves);
      }
    }
  } else {
    // TODO: throw exception
  }
}

void StandardGame::make_move(int from, int to, piece_t promotion_piece) {

  make_move(Move(from, to), promotion_piece);
}

void StandardGame::make_move(const string &move, piece_t promotion_piece) {
  // convert move to index position (from and to)
  size_t pos = move.find('-');
  auto from = chess::move_index_map.at(move.substr(0, pos));
  auto to = chess::move_index_map.at(move.substr(pos + 1));

  make_move(from, to, promotion_piece);
}

void StandardGame::undo_move() {
  if (_board._move_history.empty()) {
    LOG_WARN("Can't undo move: move history is empty");
    return;
  }

  auto& last_move = _board._move_history.back();
  swap(_board._board[last_move._from], _board._board[last_move._to]);
  piece_t last_moved_piece = _board._board[last_move._from];
  color_t last_move_color = get_color(last_moved_piece);

  if (is_type(last_moved_piece, KING)) {
    if (last_move._to - last_move._from == 2) {
      swap(_board._board[last_move._from + 1], _board._board[last_move._to + 1]);
    } else if (last_move._to - last_move._from == -2) {
      swap(_board._board[last_move._from - 1], _board._board[last_move._to - 2]);
    }
  }
  
  
  // order is pawn promotions
  if (!_board._event_history.empty()) {
    auto& last_event = _board._event_history.back();
    bool promotion = last_event._square == -1 && last_event._captured_piece == EMPTY;
    if (_board._move_history.size() == last_event._index) {
      if (promotion) {
        LOG_DEBUG("Undoing promotion");
        _board._board[last_move._from] = last_move_color | PAWN;
        _board._event_history.pop_back();
      }
    }
  }

  // then castling
  if (!_board._event_history.empty()) {
    auto& last_event = _board._event_history.back();
    bool castling = last_event._square == 64;
    if (castling) {
      LOG_DEBUG("Castling rights");
      _board._castling_rights = last_event._captured_piece; // previous caslting rights state
    }
  }

  

  // , then capture (see make_move function)
  if (!_board._event_history.empty()) {
    auto& last_event = _board._event_history.back();
    bool piece_captured = last_event._captured_piece != EMPTY && last_event._square != 64;
    LOG_DEBUG("move history size %zu, last_event index %d", _board._move_history.size(), last_event._index);
    if (_board._move_history.size() == last_event._index) {
      if (piece_captured) {
        LOG_DEBUG("Undoing piece capture");
        _board._board[last_event._square] = last_event._captured_piece;
        _board._event_history.pop_back();
      }
    }
  }


  // removing en passant square
  _board._en_passant_square = EMPTY;
  if (!_board._event_history.empty()) {

    auto& last_event = _board._event_history.back();
    bool en_passant_in_play = last_event._square != -1 && last_event._captured_piece == EMPTY;

    if (_board._move_history.size() == last_event._index && en_passant_in_play) {
      LOG_DEBUG("undoing a double pawn move");
      _board._event_history.pop_back();
    }

  }


  // TODO: create a function that identifies moves
  _board._move_history.pop_back();
  _board._fullMoves = _board._move_history.size();
  _board._color_to_move = opposite_color(_board._color_to_move);
  
  // checking valid en passsant square AFTER
  _board._en_passant_square = EMPTY;
  if (!_board._event_history.empty()) {

    auto& last_event = _board._event_history.back();
    bool en_passant_in_play = last_event._square != -1 && last_event._captured_piece == EMPTY;
    _board._en_passant_square = EMPTY;

    // LOG_DEBUG("[EN PASSANT SQUARE] move history size %zu, last_event index %d", _board._move_history.size(), last_event._index);
    if (_board._move_history.size() == last_event._index && en_passant_in_play) {
      _board._en_passant_square = last_event._square;
    }

  }
  
}

}; // namespace chess