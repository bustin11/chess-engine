
#pragma once

#include <array>
#include <vector>

namespace chess {

class PieceList {
private:
  std::vector<int> occupied_squares_;
  int num_pieces_{};
  std::array<int, 64> square_index_map_ = {};

public:
  explicit PieceList(int max_piece_count = 16);
  inline auto Count() -> int;
  void AddPieceToSquare(int square);
  void RemovePieceFromSquare(int square);
  void MovePiece(int start_square, int target_square);
  auto operator[](int index) -> int;
  auto GetAllSquares() -> std::vector<int>;
};

auto PieceList::Count() -> int { return num_pieces_; }

} // namespace chess
