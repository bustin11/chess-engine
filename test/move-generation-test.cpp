

#include <cstdio>
#include <random>
#include <string>

#include "fens.hpp"
#include "standard_game.hpp"
#include "gtest/gtest.h"


namespace chess {

// TODO: fix this with ::fmt
void test_print_move(square_t from, square_t to) {
  int from_x = from % NUM_ROW;
  int from_y = from / NUM_ROW;

  int to_x = to % NUM_ROW;
  int to_y = to / NUM_ROW;
  string s = "(" + to_string(from_x) + ", " + to_string(from_y) + ")";
  s += " -> ";
  s += "(" + to_string(to_x) + ", " + to_string(to_y) + ")";
  printf("%s", s.c_str());
}

void test_print_move(const Move& move) {
  print_move(move.from_, move.to_);
}

// Check if game is correct, ie, all moves are legal, and all legal moves are present
TEST(MoveGenerationTest, DISABLED_CountLegalMoves) {
  // TODO: fix any bugs that come with move generation

  vector<long long> ply(4);

  for (int i = 1; i < ply.size(); i++) {
    Game* game = new StandardGame();
    game->set_board_fen(fen::FEN_POSITION_5);

    std::function<long long(int, int)> count_moves = [&game, &count_moves](int depth, int total) {

      if (depth == total) return 1LL;

      auto color = (depth & 1) ? BLACK : WHITE;

      long long count = 0;
      game->generate_moves();
      auto all_moves = game->get_moves();
      for (auto& move : all_moves) {
        try {
          
          game->make_move(move);
          count += count_moves(depth + 1, total);
          game->undo_move();
        } catch (std::exception& e) {
          game->make_move(move, color | QUEEN);
          count += count_moves(depth + 1, total);
          game->undo_move();

          game->make_move(move, color | ROOK);
          count += count_moves(depth + 1, total);
          game->undo_move();

          game->make_move(move, color | BISHOP);
          count += count_moves(depth + 1, total);
          game->undo_move();

          game->make_move(move, color | KNIGHT);
          count += count_moves(depth + 1, total);
          game->undo_move();
        }
        
      }

      return count;

    };

    ply[i] = count_moves(0, i);
    printf("[%d-ply count] -- %lld\n", i, ply[i]);
    delete game;
  }

  ASSERT_TRUE(true);

}

// Check if game is correct, ie, all moves are legal, and all legal moves are present
TEST(MoveGenerationTest, DISABLED_Position4) {
  // TODO: fix any bugs that come with move generation

  vector<long long> ply(4);
  
  for (int i = 3; i < ply.size(); i++) {
    Game* game = new StandardGame();
    game->set_board_fen(fen::FEN_POSITION_4);

    std::function<long long(int, int)> count_moves = [&game, &count_moves](int depth, int total) {

      if (depth == total) return 1LL;

      auto color = (depth & 1) ? BLACK : WHITE;
      // auto color = (depth & 1) ? WHITE : BLACK;

      long long count = 0;
      game->generate_moves();
      auto all_moves = game->get_moves();
      for (auto& move : all_moves) {
        try {
          game->make_move(move);
          count += count_moves(depth + 1, total);
          game->undo_move();
        } catch (std::exception& e) {
          game->make_move(move, color | QUEEN);
          count += count_moves(depth + 1, total);
          game->undo_move();

          game->make_move(move, color | ROOK);
          count += count_moves(depth + 1, total);
          game->undo_move();

          game->make_move(move, color | BISHOP);
          count += count_moves(depth + 1, total);
          game->undo_move();

          game->make_move(move, color | KNIGHT);
          count += count_moves(depth + 1, total);
          game->undo_move();
        }
        
      }

      return count;

    };

    ply[i] = count_moves(0, 3);
    printf("[%d-ply count] -- %lld\n", i, ply[i]);
    delete game;
  }

  ASSERT_TRUE(true);

}

TEST(MoveGenerationTest, Position5) {
  // TODO: fix any bugs that come with move generation

  vector<long long> ply(4);
  
  for (int i = 3; i < ply.size(); i++) {
    Game* game = new StandardGame();
    game->set_board_fen(fen::FEN_POSITION_5);
    // game->make_move("e1g1", WHITE | KNIGHT);
    // game->make_move("e8d8");

    std::function<long long(int, int)> count_moves = [&game, &count_moves](int depth, int total) {

      if (depth == total) return 1LL;

      auto color = (depth & 1) ? BLACK : WHITE;
      // auto color = (depth & 1) ? WHITE : BLACK;

      long long count = 0;
      game->generate_moves();
      auto all_moves = game->get_moves();
      // if (all_moves.size() == 0) return 1LL;
      for (auto& move : all_moves) {
        try {
          game->make_move(move);
          count += count_moves(depth + 1, total);
          game->undo_move();
        } catch (std::exception& e) {
          game->make_move(move, color | QUEEN);
          count += count_moves(depth + 1, total);
          game->undo_move();

          game->make_move(move, color | ROOK);
          count += count_moves(depth + 1, total);
          game->undo_move();

          game->make_move(move, color | BISHOP);
          count += count_moves(depth + 1, total);
          game->undo_move();

          game->make_move(move, color | KNIGHT);
          count += count_moves(depth + 1, total);
          game->undo_move();
        }
        
      }

      return count;

    };

    ply[i] = count_moves(0, 3);
    printf("[%d-ply count] -- %lld\n", i, ply[i]);
    // ASSERT_TRUE(ply[i] == 2103487);
    delete game;
  }

  ASSERT_TRUE(true);

}

}  // namespace bustub