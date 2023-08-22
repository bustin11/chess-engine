/**
 * TODO: add ai and clocks
 */

#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include "human_player.hpp"
#include "logger.hpp"
#include "move_generator.hpp"

namespace chess {

class Game {

public:
  enum class Result {
    Playing,
    WhiteIsMated,
    BlackIsMated,
    Stalemate,
    Repetition,
    FiftyMoveRule,
    InsufficientMaterial
  };
  enum class PlayerType { Human, AI };

  bool use_clocks_{};
  bool load_custom_position_{};
  std::string custom_position_ =
      "1rbq1r1k/2pp2pp/p1n3p1/2b1p3/R3P3/1BP2N2/1P3PPP/1NBQ1RK1 w - - 0 1";

  PlayerType white_player_type_;
  PlayerType black_player_type_;

  Result game_result_;

  Player *white_player_;
  Player *black_player_;
  Player *player_to_move_;

  std::vector<Move> game_moves_;

  Board *board_;
  Board *search_board_;

  Game();
  ~Game();

  void CreatePlayer(PlayerType white_player_type, PlayerType black_player_type);

  void NewGame(PlayerType white_player_type, PlayerType black_player_type);
  void ExportGame();
  void NotifyPlayerToMove();
  auto GetGameState() -> Result;
  void PrintGameResult();
  void Init();
  void Update();
  void UndoMoveRecentMove();

  void OnMoveChosen(const Move &move);
};

} // namespace chess
