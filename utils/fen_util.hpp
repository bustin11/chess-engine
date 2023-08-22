
#pragma once

#include <iostream>
#include <map>
#include <string>

#include "board_model.hpp"
#include "piece.hpp"

namespace chess {

struct ChessBoardInfo {
  std::array<int, 64> squares = {};
  int full_moves;
  int half_move_count;
  char en_passant_file;
  bool white_kingside_castle;
  bool white_queenside_castle;
  bool black_kingside_castle;
  bool black_queenside_castle;
  bool white_to_move;
};

inline ChessBoardInfo *loadChessBoardInfo(const std::string &fen) {

  ChessBoardInfo *info = new ChessBoardInfo();

  // TODO: Fix this
  const std::map<char, int> char_piece_map{
      {'P', piece::WHITE | piece::PAWN},   {'N', piece::WHITE | piece::KNIGHT},
      {'B', piece::WHITE | piece::BISHOP}, {'R', piece::WHITE | piece::ROOK},
      {'Q', piece::WHITE | piece::QUEEN},  {'K', piece::WHITE | piece::KING},

      {'p', piece::BLACK | piece::PAWN},   {'n', piece::BLACK | piece::KNIGHT},
      {'b', piece::BLACK | piece::BISHOP}, {'r', piece::BLACK | piece::ROOK},
      {'q', piece::BLACK | piece::QUEEN},  {'k', piece::BLACK | piece::KING},
  };

  // place pieces on board
  int file = 0;
  int rank = 7;
  for (char c : fen) {
    if (c == ' ')
      break;
    if (c == '/') {
      rank--;
      file = 0;
    } else if (c >= '1' && c <= '8') {
      int numEmptySquares = c - '0';
      file += numEmptySquares;
    } else {
      info->squares[file + rank * 8] = char_piece_map.at(c);
      file++;
    }
  }

  // Parse additional information
  size_t pos = fen.find(' ');
  CHESS_ASSERT(pos != std::string::npos, "need whose turn it is"); // 1
  std::string additional_info = fen.substr(pos + 1);
  info->white_to_move = (additional_info[0] == 'w');

  CHESS_ASSERT(fen.size() >= 3, "need castling rights"); // 2
  additional_info = additional_info.substr(2);
  info->white_kingside_castle =
      (additional_info.find('K') != std::string::npos);
  info->white_queenside_castle =
      (additional_info.find('Q') != std::string::npos);
  info->black_kingside_castle =
      (additional_info.find('k') != std::string::npos);
  info->black_queenside_castle =
      (additional_info.find('q') != std::string::npos);

  pos = additional_info.find(' ');
  if (pos != std::string::npos) {
    additional_info = additional_info.substr(pos + 1);
    size_t index = BoardModel::FILE_NAMES.find(additional_info[0]);
    if (index != std::string::npos) {
      info->en_passant_file = index + 1; // 3 en passant file
    }
    pos = additional_info.find(' ');
    if (pos != std::string::npos) {
      additional_info = additional_info.substr(pos + 1);
      info->half_move_count = stoi(additional_info); // 4 half moves
      pos = additional_info.find(' ');
      if (pos != std::string::npos) {
        additional_info = additional_info.substr(pos + 1);
        info->full_moves = stoi(additional_info); // 5 half moves
      }
    }
  }

  return info;
}

inline bool bitboardContainsSquare(u64 bitboard, int square) {
  return ((bitboard >> square) & 1) != 0;
}

}; // namespace chess
