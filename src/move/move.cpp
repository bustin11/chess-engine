
#include <string>

#include "move.hpp"

namespace chess {

Move::Move(u16 val) : move_(val) {}
Move::Move(int from, int to) : move_(from | (to << 6)) {}
Move::Move(int from, int to, Type type) : move_(from | (to << 6) | (static_cast<int>(type) << 12)) {}

auto Move::Start() const -> int { return move_ & mask_start_; }
auto Move::End() const -> int { return (move_ & mask_end_) >> 6; }

auto Move::IsPromotion() const -> bool {
  auto type = MoveType();
  return type == Type::promote_bishop || type == Type::promote_knight || type == Type::promote_queen ||
         type == Type::promote_rook;
}

auto Move::MoveType() const -> Move::Type { return static_cast<Type>(move_ >> 12); }

auto Move::PromotionPieceType() const -> int {
  switch (MoveType()) {
    case Type::promote_bishop:
      return piece::BISHOP;
    case Type::promote_knight:
      return piece::KNIGHT;
    case Type::promote_queen:
      return piece::QUEEN;
    case Type::promote_rook:
      return piece::ROOK;
    default:
      CHESS_ASSERT(false, "not promotion");
      return piece::EMPTY;
  }
}

auto Move::InvalidMove() -> Move { return Move(static_cast<u16>(0)); }

auto Move::IsInvalid() const -> bool { return move_ == 0; }

auto Move::Get() const -> u16 { return move_; }

auto Move::ToString() const -> std::string {
  // TODO(justin): board representation
  return BoardModel::SquareNameFromIndex(Start()) + BoardModel::SquareNameFromIndex(End());
}

};  // namespace chess
