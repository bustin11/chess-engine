
#pragma once

#include <functional>
#include <utility>

#include "move.hpp"

namespace chess {

class Player {

  friend class Game;

protected:
  using callback_t = std::function<void(const Move &move)>;
  callback_t on_move_chosen_;

public:
  virtual void Update() {}

  virtual void NotifyTurnToMove() {}

  virtual void ChoseMove(const Move &move) {
    on_move_chosen_(move); // onMoveChosen
  }

  explicit Player(callback_t callback) : on_move_chosen_(std::move(std::move(callback))) {}
  virtual ~Player() = default;
};

}; // namespace chess