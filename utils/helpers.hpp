/**
 * TODO:
 * 1. throw exception for bad fen string
 *
 */

#pragma once

#include <array>
#include <string>
#include <string_view>

#include "board.hpp"

using chess::char_piece_map, chess::NUM_SQUARES, chess::NUM_ROW;
using std::array, std::string, std::string_view;

namespace chess {

template <typename T>
void load_fen(Board<T> &chessboardInfo, const string &fen,
              map<char, T> char_piece_map = chess::char_piece_map) {

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
      chessboardInfo._board[index] = char_piece_map[c];
      index++;
    }
  }

  // fen starts upper left to bottom right
  // my board starts from bottom left to upper right
  for (int square = 0; square < NUM_SQUARES / 2; square++) {
    int row = NUM_ROW - 1 - (square / NUM_ROW);
    int col = square % NUM_ROW;
    int target_square = row * NUM_ROW + col;
    swap(chessboardInfo._board[square], chessboardInfo._board[target_square]);
  }

  // Parse additional information
  size_t pos = fen.find(' ');
  if (pos != string::npos && pos + 1 < fen.size()) {
    string additionalInfo = fen.substr(pos + 1);
    pos = additionalInfo.find(' ');

    if (pos != string::npos && pos + 1 < additionalInfo.size()) {
      chessboardInfo._color_to_move = additionalInfo[0];
      additionalInfo = additionalInfo.substr(pos + 1);

      pos = additionalInfo.find(' ');
      if (pos != string::npos && pos + 1 < additionalInfo.size()) {
        chessboardInfo._castlingRights = additionalInfo.substr(0, pos);
        additionalInfo = additionalInfo.substr(pos + 1);

        pos = additionalInfo.find(' ');
        if (pos != string::npos && pos + 1 < additionalInfo.size()) {
          chessboardInfo._enPassantSquare = additionalInfo.substr(0, pos);
          additionalInfo = additionalInfo.substr(pos + 1);

          pos = additionalInfo.find(' ');
          if (pos != string::npos && pos + 1 < additionalInfo.size()) {
            chessboardInfo._halfMoves = stoi(additionalInfo.substr(0, pos));
            additionalInfo = additionalInfo.substr(pos + 1);

            pos = additionalInfo.find(' ');
            if (pos != string::npos && pos + 1 < additionalInfo.size()) {
              chessboardInfo._fullMoves = stoi(additionalInfo.substr(0, pos));
            }
          }
        }
      }
    }
  }
}

}; // namespace chess
