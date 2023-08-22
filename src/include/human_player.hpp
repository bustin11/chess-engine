
#pragma once

#include <functional>

#include "player.hpp"

namespace chess {

class HumanPlayer : public Player {
public:
  explicit HumanPlayer(callback_t callback);

  void Update() override;

  void NotifyTurnToMove() override;
};

}; // namespace chess