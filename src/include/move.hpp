
#pragma once

#include "board_model.hpp"
#include "macros.hpp"
#include "piece.hpp"

namespace chess {

class Move {
public:
  enum class Type {
    none = 0,
    pawn_jump,  // creates an en passant square
    en_passant, // remembers where to restore the captured pawn
    castle,     // if caslting occurred (though can be checked with k moving 2
                // squares)
    // promotion, but can check 8th rank
    promote_queen,
    promote_rook,
    promote_bishop,
    promote_knight
  };

  // b2b4d8d7

  const u16 mask_start_ = 0b0000000000111111;
  const u16 mask_end_ = 0b0000111111000000;
  const u16 mask_type_ = 0b1111000000000000;
  const u16 move_;

  explicit Move(u16 val);
  Move(int from, int to);
  Move(int from, int to, Type type);

  auto operator==(const Move &other) -> bool { return move_ == other.move_; }

  auto Start() const -> int;
  auto End() const -> int;

  auto IsPromotion() const -> bool;

  auto MoveType() const -> Type;
  auto PromotionPieceType() const -> int;

  static auto InvalidMove() -> Move;
  auto IsInvalid() const -> bool;

  auto Get() const -> u16;

  auto ToString() const -> std::string;

};

}; // namespace chess