

#include "piece_list.hpp"
#include <iostream>
#include "logger.hpp"
#include "macros.hpp"

namespace chess {

PieceList::PieceList(int max_piece_count) { occupied_squares_.resize(max_piece_count); }

void PieceList::AddPieceToSquare(int square) {
  occupied_squares_[num_pieces_] = square;
  square_index_map_[square] = num_pieces_;
  num_pieces_++;
}

void PieceList::RemovePieceFromSquare(int square) {
  int target_index = square_index_map_[square];
  occupied_squares_[target_index] = occupied_squares_[num_pieces_ - 1];
  square_index_map_[occupied_squares_[target_index]] = target_index;
  num_pieces_--;
}

void PieceList::MovePiece(int start_square, int target_square) {
  int index = square_index_map_[start_square];
  // if (!(0 <= index && index < num_pieces_)) {
  //   CHESS_ASSERT(false, "piece not in square index map");
  // }
  occupied_squares_[index] = target_square;
  square_index_map_[target_square] = index;
}

auto PieceList::operator[](int index) -> int { return occupied_squares_[index]; }

auto PieceList::GetAllSquares() -> std::vector<int> {
  std::vector<int> squares;
  for (int i = 0; i < num_pieces_; i++) {
    squares.push_back(occupied_squares_[i]);
  }
  return squares;
}

};  // namespace chess
