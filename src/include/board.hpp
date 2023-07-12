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

#define linear_index(x,y) y * NUM_ROW + x
#define xy_index(x,y,square) square_t x = square % NUM_ROW; \
                             square_t y = square / NUM_ROW;

using chess::piece_t, chess::color_t;
using std::string, std::array, std::vector, std::pair, std::set;

namespace chess {

const string FEN_START =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

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

  array<T, NUM_SQUARES> knp_attacking_counts_ = {}; // double check/check
  array<T, NUM_SQUARES> qrb_attacking_counts_ = {}; // blocking
  array<T, NUM_SQUARES> xray_counts_ = {}; // pins
  // capturing is done with the board itself
};

public:
  Board(){};
  friend class Game;
  friend class StandardGame;

  void load_from_fen(const string &fen) {

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
          printf("%d ", board_[index]);
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
            if (castling.find(KQkq[i]) != string::npos) {
              castling_rights_ |= (1 << i);
            }
          }
          LOG_DEBUG("castling rights are %d", castling_rights_);
          additionalInfo = additionalInfo.substr(pos + 1);
LOG_DEBUG("%s",additionalInfo.c_str());
          pos = additionalInfo.find(' ');
          if (pos != string::npos && pos + 1 < additionalInfo.size()) {
            if (additionalInfo.substr(0, pos) != "-")
              en_passant_square_ = move_index_map.at(additionalInfo.substr(0, pos));

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
  }

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

  array<T, NUM_SQUARES> board_ = {}; // The chessboard
  color_t color_to_move_ = WHITE;  // Whose turn it is ('w' for white, 'b' for black)
  __uint8_t castling_rights_ = 15;  // Castling rights (e.g., "KQkq"), 1101, ie, means KQq
  square_t en_passant_square_ = EMPTY; // En passant target square (e.g., "e3")
  int half_moves_{};          // Half-moves since last pawn move or capture
  int full_moves_{};          // Full moves counter
  vector<Event> move_history_ = {};
  array<MetaData, 2> meta_data_;

};

}; // namespace chess
