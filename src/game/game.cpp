

#include "game.hpp"

namespace chess {

Game::Game() = default;

Game::~Game() {
  delete board_;
  delete search_board_;
  delete white_player_;
  delete black_player_;
}

void Game::CreatePlayer(PlayerType white_player_type, PlayerType black_player_type) {
  if (white_player_type == PlayerType::Human) {
    white_player_ = new HumanPlayer([this](const Move &move) { this->OnMoveChosen(move); });
  } else {
    // TODO(justin): create an ai player
  }
  if (black_player_type == PlayerType::Human) {
    black_player_ = new HumanPlayer([this](const Move &move) { this->OnMoveChosen(move); });
  } else {
    // TODO(justin): create an ai player
  }
}

void Game::NewGame(PlayerType white_player_type, PlayerType black_player_type) {
  game_moves_.clear();
  if (load_custom_position_) {
    board_->LoadPosition(custom_position_);
    search_board_->LoadPosition(custom_position_);
  } else {
    board_->LoadStartPosition();
    search_board_->LoadStartPosition();
  }

  CreatePlayer(white_player_type, black_player_type);
  game_result_ = Result::Playing;

  NotifyPlayerToMove();
  // TODO(justin): more stuff
}

void Game::ExportGame() {
  // TODO(justin): Fix this
  // string pgn = PGNCreater.createPGN(game_moves);
  // string base_URL = "https://www.lichess.org/paste?pgn=";
  // string escaped_URL = url_encode(base_URL);
  // //https://stackoverflow.com/questions/154536/encode-decode-urls-in-c string
  // url = base_URL + escaped_URL;
}

void Game::NotifyPlayerToMove() {
  game_result_ = GetGameState();
  PrintGameResult();
  if (game_result_ == Result::Playing) {
    player_to_move_ = (board_->white_to_move_) ? white_player_ : black_player_;
    player_to_move_->NotifyTurnToMove();
  } else {
    LOG_INFO("Game over!");
  }
}

auto Game::GetGameState() -> Game::Result {
  auto *move_generator = new MoveGenerator();
  auto moves = move_generator->GenerateMoves(board_);

  if (moves.empty()) {
    if (move_generator->InCheck()) {
      return board_->white_to_move_ ? Result::WhiteIsMated : Result::BlackIsMated;
    }
    return Result::Stalemate;
  }

  if (board_->fifty_move_counter_ >= 100) {
    return Result::FiftyMoveRule;
  }

  int rep_count = 0;
  // int rep_count = board->repetition_position_history(); // TODO: count
  // zobrist key 3 states
  if (rep_count == 3) {
    return Result::Repetition;
  }

  // Look for insufficient material (not all cases implemented yet)
  int num_pawns = board_->pawns_[Board::WHITE_INDEX]->Count() + board_->pawns_[Board::BLACK_INDEX]->Count();
  int num_rooks = board_->rooks_[Board::WHITE_INDEX]->Count() + board_->rooks_[Board::BLACK_INDEX]->Count();
  int num_queens = board_->queens_[Board::WHITE_INDEX]->Count() + board_->queens_[Board::BLACK_INDEX]->Count();
  int num_knights = board_->knights_[Board::WHITE_INDEX]->Count() + board_->knights_[Board::BLACK_INDEX]->Count();
  int num_bishops = board_->bishops_[Board::WHITE_INDEX]->Count() + board_->bishops_[Board::BLACK_INDEX]->Count();

  if (num_pawns + num_rooks + num_queens == 0) {
    if (num_knights == 1 || num_bishops == 1) {
      return Result::InsufficientMaterial;
    }
  }

  delete move_generator;
  return Result::Playing;
}

void Game::PrintGameResult() {
  std::string text{};
  if (game_result_ == Result::Playing) {
    text = "Playing";
  } else if (game_result_ == Result::WhiteIsMated || game_result_ == Result::BlackIsMated) {
    text = "Checkmate!";
  } else if (game_result_ == Result::FiftyMoveRule) {
    text = "Draw";
    text += "\n(50 move rule)";
  } else if (game_result_ == Result::Repetition) {
    text = "Draw";
    text += "\n(3-fold repetition)";
  } else if (game_result_ == Result::Stalemate) {
    text = "Draw";
    text += "\n(Stalemate)";
  } else if (game_result_ == Result::InsufficientMaterial) {
    text = "Draw";
    text += "\n(Insufficient material)";
  }
  LOG_INFO("%s", text.c_str());
}

void Game::Init() {
  // TODO(justin): init stuff
  board_ = new Board();
  search_board_ = new Board();
}

void Game::Update() {
  // tell the player to update
  // update the clocks when the time comes
  if (game_result_ == Result::Playing) {

    // player_to_move_->Update ();
    if (use_clocks_) {
      // whiteClock.isTurnToMove = board.WhiteToMove;
    }
  }
}

void Game::OnMoveChosen(const Move &move) {
  board_->MakeMove(move);
  search_board_->MakeMove(move);

  LOG_TRACE("%s's turn", board_->white_to_move_ ? "white" : "black");


  game_moves_.push_back(move);
  // TODO(justin): update the opponents turn
  Update();
  NotifyPlayerToMove();
}

void Game::UndoMoveRecentMove() {
  if (!game_moves_.empty()) {
    board_->UndoMove(game_moves_.back());
    search_board_->UndoMove(game_moves_.back());
    game_moves_.pop_back(); // TODO(justin): don't pop back, keep a trailing pointer
  }
}

};  // namespace chess
