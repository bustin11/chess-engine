// https://www.chessprogramming.org/Perft_Results

#include <stdio.h>
#include <ctime>

#include "fens.hpp"
#include "game.hpp"

#include "gtest/gtest.h"
namespace chess {

auto GetMoveString(int from, int to, bool is_promotion = false) -> std::string {
  int from_x = from % 8;
  int from_y = from / 8;

  int to_x = to % 8;
  int to_y = to / 8;

  BoardModel::SquareNameFromCoordinate(Coord(from_x, from_y));
  std::string s = BoardModel::SquareNameFromCoordinate(Coord(from_x, from_y));
  s += BoardModel::SquareNameFromCoordinate(Coord(to_x, to_y));
  if (is_promotion) {
    s += "*";
  }
  return s;
}

auto GetMoveString(const Move& move) -> std::string {
  return GetMoveString(move.Start(), move.End(), move.IsPromotion());
}

// TODO(justinhsu): fix this with ::fmt
void TestPrintMove(int from, int to) {
  printf("%s\n", GetMoveString(from, to).c_str());
}

void TestPrintMove(const Move &move) { 
  printf("%s\n", GetMoveString(move).c_str());
}

long long TestPositionWithPlyCount(const string& fen, int ply_count) {
  auto *board = new Board();
  board->LoadPosition(fen);
  auto move_generator = new chess::MoveGenerator();

  std::function<long long(int)> count_moves = 
    [&move_generator, &board, &count_moves](int depth) {
    
    if (depth == 0) return 1LL;

    long long count = 0;
    auto all_moves = move_generator->GenerateMoves(board);
    for (auto &move : all_moves) {
      board->MakeMove(move, true);
      count += count_moves(depth - 1);
      board->UndoMove(move, true);
    }

    return count;
  };

  long long counts = count_moves(ply_count);
  delete board;
  delete move_generator;
  return counts;
}

long long TestPositionWithPlyCountDebug(const string& fen, int ply_count) {
  auto *board = new Board();
  board->LoadPosition(fen);
  auto move_generator = new chess::MoveGenerator();

  std::function<long long(int, std::string)> count_moves = 
    [&move_generator, &board, &count_moves](int depth, string s = "") {
    // if (s.length() > 0)
      // printf("%s\n", s.c_str());
    
    if (depth == 0) return 1LL;

    long long count = 0;
    auto all_moves = move_generator->GenerateMoves(board);
    for (auto &move : all_moves) {
      board->MakeMove(move, true);
      count += count_moves(depth - 1, s + GetMoveString(move));
      board->UndoMove(move, true);
    }
    // TODO(justin): change me!
    if (s.length() < 6)
      printf("%s: %lld\n", s.c_str(), count);

    return count;
  };

  long long counts = count_moves(ply_count, "");
  delete board;
  delete move_generator;
  return counts;
}

void TestWrapper(const char* fen_name, const string& fen,
                  std::vector<long long> &correct_values, bool debug = false) {
  int ply_count = 1;
  for (auto val : correct_values) {
    int tt = clock();
    long long counts = 0;
    if (debug) counts = TestPositionWithPlyCountDebug(fen, ply_count);
    else counts = TestPositionWithPlyCount(fen, ply_count);
    int elapsed_time = 1000 * (clock() - tt) / CLOCKS_PER_SEC;
    printf("[%s(%d ms)]: ply count of %d = %lld\n", fen_name, elapsed_time, ply_count, counts);
    fflush(stdout);
    if (!debug) ASSERT_TRUE(counts == val);
    ply_count++;
  }
}




// =========================== STARTING POSITION
TEST(MoveGenerationTest, Position1_TEST) {
  const char* fen_name = "Position 2";
  std::vector<long long> correct_values{20, 400, 8902, 197281, 4865609, 119060324}; 
  std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  TestWrapper(fen_name, fen, correct_values);
}

// Check if game is correct, ie, all moves are legal, and all legal moves are
TEST(MoveGenerationTest, Position2_TEST) {
  const char* fen_name = "Position 2";
  std::vector<long long> correct_values{48, 2039, 97862, 4085603, 193690690}; 
  std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
  TestWrapper(fen_name, fen, correct_values);
}

TEST(MoveGenerationTest, Position3_TEST) {
  const char* fen_name = "Position 3";
  std::vector<long long> correct_values{14, 191, 2812, 43238, 674624, 11030083, 178633661}; 
  std::string fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";
  TestWrapper(fen_name, fen, correct_values);
}

TEST(MoveGenerationTest, Position4_TEST) {
  const char* fen_name = "Position 4";
  std::vector<long long> correct_values{6, 264, 9467, 422333, 15833292};
  TestWrapper(fen_name, fen::FEN_POSITION_4, correct_values);
}

TEST(MoveGenerationTest, Position5_TEST) {
  const char* fen_name = "Position 5";
  std::vector<long long> correct_values{44, 1486, 62379, 2103487, 89941194};
  TestWrapper(fen_name, fen::FEN_POSITION_5, correct_values);
}

TEST(MoveGenerationTest, Position6_TEST) {
  const char* fen_name = "Position 6";
  std::vector<long long> correct_values{46, 2079, 89890, 3894594, 164075551};
  TestWrapper(fen_name, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", correct_values);
}

}  // namespace chess