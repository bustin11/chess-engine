

#include "game.hpp"

using chess::piece_t;
using std::string, std::swap;

namespace chess {

Game::Game() { set_board_fen(FEN_START); }

Game::~Game() {}

void Game::generate_pawn_moves(int start_square, piece_t piece) {}
void Game::generate_knight_moves(int start_square, piece_t piece) {}
void Game::generate_king_moves(int start_square, piece_t piece) {}
void Game::generate_sliding_moves(int start_square, piece_t piece) {}
void Game::generate_moves_for_piece(int start_square, piece_t piece) {}
void Game::generate_moves() {  }

void Game::make_move(const string &move, piece_t promotion_piece) {}
void Game::make_move(int from, int to, piece_t promotion_piece) {}
void Game::make_move(const chess::Move &move, piece_t promotion_piece) {}

void Game::undo_move() {}

bool Game::is_legal_move(const chess::Move &move) { return false; }

void Game::set_board_fen(string fen) {
  _board.load_from_fen(fen);
}

void Game::set_piece_selected(int start_square, piece_t piece) { 
  generate_moves_for_piece(start_square, piece);
}


}; // namespace chess