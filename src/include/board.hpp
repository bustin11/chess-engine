/**
 * TODO:
 * 1. copy and move constructors
 * 2. dump_fen to dump board to fen
 *
 */
#pragma once

#include <array>
#include <vector>

#include "piece.hpp"

using chess::piece_t, chess::color_t;
using std::string, std::array, std::vector, std::pair;

namespace chess {

const string FEN_START =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

template <typename T> class Board {

  struct Event {
    const int _index; // index of occurance (1 indexed)
    const int _square; // en_passant or capture square or castling (64, 65, 66, 67)
    const T _captured_piece; // -1 if no capture
    Event(int index, int square, T capture_piece) : 
      _index(index), _square(square), _captured_piece(capture_piece) {}
    Event(int index, int square) :
       _index(index), _square(square), _captured_piece(EMPTY) {}
    Event(int index) :
       _index(index), _square(-1), _captured_piece(EMPTY) {}
  };

public:
  Board(){};
  friend class Game;
  friend class StandardGame;

  void load_from_fen(const string &fen) {

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
        _board[index] = char_piece_map.at(c);
        index++;
      }
    }

    // fen starts upper left to bottom right
    // my board starts from bottom left to upper right
    for (int square = 0; square < NUM_SQUARES / 2; square++) {
      int row = NUM_ROW - 1 - (square / NUM_ROW);
      int col = square % NUM_ROW;
      int target_square = row * NUM_ROW + col;
      swap(_board[square], _board[target_square]);
    }

    // Parse additional information
    size_t pos = fen.find(' ');
    if (pos != string::npos && pos + 1 < fen.size()) {
      string additionalInfo = fen.substr(pos + 1);
      pos = additionalInfo.find(' ');

      if (pos != string::npos && pos + 1 < additionalInfo.size()) {
        _color_to_move = additionalInfo[0] == 'w' ? WHITE : BLACK;
        additionalInfo = additionalInfo.substr(pos + 1);

        pos = additionalInfo.find(' ');
        if (pos != string::npos && pos + 1 < additionalInfo.size()) {
          string castling = additionalInfo.substr(0, pos);
          string KQkq = "KQkq";
          for (int i = 3; i >= 0; i--) {
            if (castling.find(KQkq[i]) != string::npos) {
              _castling_rights |= (1 << i);
            }
          }
          additionalInfo = additionalInfo.substr(pos + 1);

          pos = additionalInfo.find(' ');
          if (pos != string::npos && pos + 1 < additionalInfo.size()) {
            if (additionalInfo.substr(0, pos) != "-")
              _en_passant_square = move_index_map.at(additionalInfo.substr(0, pos));

            additionalInfo = additionalInfo.substr(pos + 1);

            pos = additionalInfo.find(' ');
            if (pos != string::npos && pos + 1 < additionalInfo.size()) {
              _halfMoves = stoi(additionalInfo.substr(0, pos));
              additionalInfo = additionalInfo.substr(pos + 1);

              pos = additionalInfo.find(' ');
              if (pos != string::npos && pos + 1 < additionalInfo.size()) {
                _fullMoves = stoi(additionalInfo.substr(0, pos));
              }
            }
          }
        }
      }
    }
  }



  string _fen;
  array<T, NUM_SQUARES> _board; // The chessboard
  color_t _color_to_move;  // Whose turn it is ('w' for white, 'b' for black)
  __uint8_t _castling_rights;  // Castling rights (e.g., "KQkq"), 1101, ie, means KQq
  int _en_passant_square; // En passant target square (e.g., "e3")
  int _halfMoves;          // Half-moves since last pawn move or capture
  int _fullMoves;          // Full moves counter
  vector<Move> _move_history;
  vector<Event> _event_history;

};

}; // namespace chess
