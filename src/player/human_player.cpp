

#include "human_player.hpp"

#include <utility>

namespace chess {

HumanPlayer::HumanPlayer(callback_t callback) : Player(std::move(callback)) {}

void HumanPlayer::Update() {}

void HumanPlayer::NotifyTurnToMove() {}

}  // namespace chess
