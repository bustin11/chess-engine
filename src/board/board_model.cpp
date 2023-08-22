
#include "board_model.hpp"

namespace chess {

auto BoardModel::RankIndex(int square_index) -> int { return square_index >> 3; }

auto BoardModel::FileIndex(int square_index) -> int { return square_index & (0b000111); }

auto BoardModel::IndexFromCoord(int file_index, int rank_index) -> int { return file_index + rank_index * 8; }

auto BoardModel::IndexFromCoord(const Coord &coord) -> int {
  return IndexFromCoord(coord.FileIndex(), coord.RankIndex());
}

auto BoardModel::CoordFromIndex(int square_index) -> Coord {
  return {FileIndex(square_index), RankIndex(square_index)};
}

auto BoardModel::IsLightSquare(int file_index, int rank_index) -> bool { return (file_index + rank_index) % 2 == 0; }

auto BoardModel::SquareNameFromIndex(int square_index) -> std::string {
  return SquareNameFromCoordinate(FileIndex(square_index), RankIndex(square_index));
}

auto BoardModel::SquareNameFromCoordinate(const Coord &coord) -> std::string {
  return SquareNameFromCoordinate(coord.FileIndex(), coord.RankIndex());
}

auto BoardModel::SquareNameFromCoordinate(int file_index, int rank_index) -> std::string {
  return FILE_NAMES[file_index] + std::to_string(rank_index + 1);
}

};  // namespace chess
