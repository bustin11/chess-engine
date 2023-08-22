
/**
 * Fix the constexpr or static stuff
 *
 */

#pragma once
#include <array>

#include "board.hpp"
#include "board_model.hpp"
#include "macros.hpp"
#include "piece.hpp"

namespace chess::precomputed_move_data {

// variables
// orthogonal, diagonal                        N,  S,  W, E, NW, SE,NE, SW
inline std::array<int, 8> direction_offsets = {8, -8, -1, 1, 7, -7, 9, -9};
inline std::array<std::array<int, 8>, 64> num_squares_to_edge = {};

inline std::vector<u8> knight_moves[64];
inline std::vector<u8> king_moves[64];

const u8 PAWN_ATTACKING_DIRECTIONS[2][2] = {
    {4, 6}, // NW and NE
    {7, 5}  //  SW and SE
};

inline std::array<std::vector<int>, 64> pawn_attacks_white = {}; // 2 attacks
inline std::array<std::vector<int>, 64> pawn_attacks_black = {}; // 2 attacks
inline std::array<int, 127> direction_lookup = {};

inline std::array<u64, 64> king_attack_bitboards = {};
inline std::array<u64, 64> knight_attack_bitboards = {};
inline std::array<std::array<u64, 2>, 64> pawn_attack_bitboards = {};

inline std::array<u64, 64> rook_moves = {};
inline std::array<u64, 64> bishop_moves = {};
inline std::array<u64, 64> queen_moves = {};

inline std::array<std::array<u64, 64>, 64> orthogonal_distance = {};
inline std::array<std::array<u64, 64>, 64> king_distance = {};
inline std::array<u64, 64> center_manhattan_distance;

auto NumRookMovesToReachSquare(int start_square, int target_square) -> int;
auto NumKingMovesToReachSquare(int start_square, int target_square) -> int;

void Init();

}  // namespace chess::precomputed_move_data