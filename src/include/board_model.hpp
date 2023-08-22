

#pragma once

#include <string>

#include "coord.hpp"

namespace chess {

class BoardModel {
public:
  inline static const std::string FILE_NAMES = "abcdefgh";
  inline static const std::string RANK_NAMES = "12345678";

  static const int A1 = 0;
  static const int B1 = 1;
  static const int C1 = 2;
  static const int D1 = 3;
  static const int E1 = 4;
  static const int F1 = 5;
  static const int G1 = 6;
  static const int H1 = 7;

  static const int A8 = 56;
  static const int B8 = 57;
  static const int C8 = 58;
  static const int D8 = 59;
  static const int E8 = 60;
  static const int F8 = 61;
  static const int G8 = 62;
  static const int H8 = 63;

  static auto RankIndex(int square_index) -> int;
  static auto FileIndex(int square_index) -> int;
  static auto IndexFromCoord(int file_index, int rank_index) -> int;
  static auto IndexFromCoord(const Coord &coord) -> int;
  static auto CoordFromIndex(int square_index) -> Coord;

  static auto IsLightSquare(int file_index, int rank_index) -> bool;

  static auto SquareNameFromIndex(int square_index) -> std::string;
  static auto SquareNameFromCoordinate(const Coord &coord) -> std::string;
  static auto SquareNameFromCoordinate(int file_index, int rank_index) -> std::string;
};

} // namespace chess