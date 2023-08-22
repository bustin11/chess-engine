
#pragma once

namespace chess {

struct Coord {
  int x_, y_;
  Coord(int file, int rank) {
    x_ = file;
    y_ = rank;
  }
  Coord() = default;

  auto Linear() -> int { return x_ + 8 * y_; }
  auto IsEqual(int x, int y) const -> bool { return this->x_ == x && this->y_ == y; }
  auto FileIndex() const -> int { return x_; }
  auto RankIndex() const -> int { return y_; }
};

} // namespace chess
