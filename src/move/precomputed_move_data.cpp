
#include "precomputed_move_data.hpp"
#include <vector>
#include <iostream>
namespace chess {

auto precomputed_move_data::NumKingMovesToReachSquare(int start_square, int target_square) -> int {
  return king_distance[start_square][target_square];
}

auto precomputed_move_data::NumRookMovesToReachSquare(int start_square, int target_square) -> int {
  return orthogonal_distance[start_square][target_square];
}

void precomputed_move_data::Init() {
  std::vector<int> knight_hops = {15, 17, -17, -15, 10, -6, 6, -10};

  for (int square_index = 0; square_index < 64; square_index++) {
    int y = square_index / 8;
    int x = square_index % 8;

    int north = 7 - y;
    int south = y;
    int east = 7 - x;
    int west = x;

    num_squares_to_edge[square_index][0] = north;
    num_squares_to_edge[square_index][1] = south;
    num_squares_to_edge[square_index][2] = west;
    num_squares_to_edge[square_index][3] = east;
    num_squares_to_edge[square_index][4] = std::min(north, west);
    num_squares_to_edge[square_index][5] = std::min(south, east);
    num_squares_to_edge[square_index][6] = std::min(north, east);
    num_squares_to_edge[square_index][7] = std::min(south, west);

    // knight hops
    std::vector<u8> legal_knight_hops;
    u64 knight_bitboard = 0;
    for (auto jump_offset : knight_hops) {
      int knight_hop_square = square_index + jump_offset;
      if (in_bounds(knight_hop_square)) {
        xy_index(tx, ty, knight_hop_square);
        if (abs(tx - x) + abs(ty - y) == 3) {
          legal_knight_hops.push_back(knight_hop_square);
          knight_bitboard |= static_cast<u64>(1) << knight_hop_square;
        }
      }
    }
    knight_moves[square_index] = legal_knight_hops;
    // std::copy(legal_knight_hops.begin(), legal_knight_hops.end(),
    // knight_moves[square_index].begin());
    knight_attack_bitboards[square_index] = knight_bitboard;

    // king moves
    std::vector<u8> legal_king_moves;
    u64 king_bitboard = 0;
    for (int king_move_delta : direction_offsets) {
      int king_move_square = square_index + king_move_delta;
      if (in_bounds(king_move_square)) {
        xy_index(tx, ty, king_move_square);
        if (std::max(std::abs(tx - x), std::abs(ty - y)) == 1) {
          legal_king_moves.push_back(king_move_square);
          king_bitboard |= static_cast<u64>(1) << king_move_square;
        }
      }
    }
    king_moves[square_index] = legal_king_moves;
    // std::copy(legal_king_moves.begin(), legal_king_moves.end(),
    // king_moves[square_index].begin());
    king_attack_bitboards[square_index] = king_bitboard;

    std::vector<int> pawn_captures_white;
    std::vector<int> pawn_captures_black;
    if (x > 0) {
      if (y < 7) {
        pawn_captures_white.push_back(square_index + 7);
        pawn_attack_bitboards[square_index][Board::WHITE_INDEX] |= static_cast<u64>(1) << (square_index + 7);
      }
      if (y > 0) {
        pawn_captures_black.push_back(square_index - 9);
        pawn_attack_bitboards[square_index][Board::BLACK_INDEX] |= static_cast<u64>(1) << (square_index - 9);
      }
    }
    if (x < 7) {
      if (y < 7) {
        pawn_captures_white.push_back(square_index + 9);
        pawn_attack_bitboards[square_index][Board::WHITE_INDEX] |= static_cast<u64>(1) << (square_index + 9);
      }
      if (y > 0) {
        pawn_captures_black.push_back(square_index - 7);
        pawn_attack_bitboards[square_index][Board::BLACK_INDEX] |= static_cast<u64>(1) << (square_index - 7);
      }
    }

    pawn_attacks_white[square_index] = pawn_captures_white;
    pawn_attacks_black[square_index] = pawn_captures_black;

    // rook moves
    for (int i = 0; i < 4; i++) {
      int current_offset = direction_offsets[i];
      for (int n = 0; n < num_squares_to_edge[square_index][i]; n++) {
        int target_square = square_index + current_offset * (n + 1);
        rook_moves[square_index] |= static_cast<u64>(1) << target_square;
      }
    }

    // bishop moves
    for (int i = 4; i < 8; i++) {
      int current_offset = direction_offsets[i];
      for (int n = 0; n < num_squares_to_edge[square_index][i]; n++) {
        int target_square = square_index + current_offset * (n + 1);
        rook_moves[square_index] |= static_cast<u64>(1) << target_square;
      }
    }

    // queen moves
    queen_moves[square_index] = (rook_moves[square_index] | bishop_moves[square_index]);
  }

  // direction lookup
  for (int i = 0; i < 127; i++) {
    int offset = i - 63;
    int abs_offset = abs(offset);
    int abs_dir = 1;
    if (abs_offset % 9 == 0) {
      abs_dir = 9;
    } else if (abs_offset % 8 == 0) {
      abs_dir = 8;
    } else if (abs_offset % 7 == 0) {
      abs_dir = 7;
    }
    direction_lookup[i] = abs_dir * sign(offset);
  }

  // distance lookup
  for (int square1 = 0; square1 < 64; square1++) {
    Coord coord1 = BoardModel::CoordFromIndex(square1);
    int file_dist_from_center = std::max(3 - coord1.FileIndex(), coord1.FileIndex() - 4);
    int rank_dist_from_center = std::max(3 - coord1.RankIndex(), coord1.RankIndex() - 4);
    center_manhattan_distance[square1] = file_dist_from_center + rank_dist_from_center;
    for (int square2 = 0; square2 < 64; square2++) {
      Coord coord2 = BoardModel::CoordFromIndex(square2);
      int rank_distance = std::abs(coord1.RankIndex() - coord2.RankIndex());
      int file_distance = std::abs(coord1.FileIndex() - coord2.FileIndex());
      orthogonal_distance[square1][square2] = rank_distance + file_distance;
      king_distance[square1][square2] = std::max(file_distance, rank_distance);
    }
  }
}

};  // namespace chess
