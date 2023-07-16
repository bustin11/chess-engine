
#pragma once

using std::string;


namespace fen {

  const string FEN_START =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  const string FEN_TEST[]{"r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2",
  "8/k1P5/8/1K6/8/8/8/8 w - - 0 1", "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"};

  const string FEN_PINS = "3r1n2/b5q1/1R1B1p2/4P3/3K1Q1r/8/1B3N2/q5b1 w - - 0 1";

  const string FEN_KNIGHT_PIN = "3r4/q5b1/8/2NNN3/r1NKN2r/2NNN3/8/q2r2b1 b - - 0 1";

  const string FEN_PAWN_PIN_DIAGONAL = "b5q1/3n4/2P1P3/1n1KP3/2P1P3/8/q2P4/7b w - - 0 1";

  const string FEN_PAWN_PIN_ROW_COL = "3r4/5p2/1b1P1b2/r1PKP2r/8/8/3P4/3r4 w - - 0 1";

  const string FEN_PAWN_PIN = "b2r2q1/5p2/2PPP3/r1PKP2r/2P1P3/8/q2P4/3r3b w - - 0 1";

  const string FEN_SLIDING_PIECE_PIN = "3r2br/1b6/2RRB3/3KB2r/3QQ3/8/1b4b1/3r4 w - - 0 1";

  const string FEN_SIMPLE_KNIGHT_CHECK = "3R4/5n2/B7/3k4/8/7Q/2K5/5N2 b - - 0 1";

  const string FEN_PAWN_BLOCK_CHECK = "8/4p2r/1K6/3P4/7r/3P4/8/2k3b1 w - - 0 1";

  const string FEN_POSITION_5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";

  const string FEN_POSITION_4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";


}; // namespace chess_fen