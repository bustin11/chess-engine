/*
 * TODO: the desturctor is very important
 */

#include "board.hpp"

namespace chess {

Board::Board() { 
  precomputed_move_data::Init();
  Init(); 
}

Board::~Board() {
  for (auto pl : pawns_) delete pl;
  for (auto pl : knights_) delete pl;
  for (auto pl : bishops_) delete pl;
  for (auto pl : rooks_) delete pl;
  for (auto pl : queens_) delete pl;
  delete all_piece_lists_[0];
}

auto Board::GetPieceList(int piece_type, int color_index) const -> PieceList *{

  return all_piece_lists_[color_index * 8 + piece_type];
  // switch (piece_type) {
  //   case 2:
  //     return pawns_[color_index];
  //   case 3:
  //     return knights_[color_index];
  //   case 5:
  //     return bishops_[color_index];
  //   case 6:
  //     return rooks_[color_index];
  //   case 7:
  //     return queens_[color_index];
  //   default:
  //     std::cout << piece_type << " " << color_index << std::endl;
  //     CHESS_ASSERT(false, "not possible");
  // }
}

void Board::MakeMove(const Move &move, bool searching) {
  u32 old_en_passant_file = (current_game_state_ >> 4) & 15;
  u32 original_castling_rights = current_game_state_ & 15;
  u32 new_castling_rights = original_castling_rights;
  current_game_state_ = 0;

  int opponent_color_index = 1 - color_to_move_index_;
  int move_from = move.Start();
  int move_to = move.End();

  int captured_piece_type = piece::PieceType(square_[move_to]);
  int move_piece = square_[move_from];
  int move_piece_type = piece::PieceType(move_piece);

  auto move_type = move.MoveType();
  bool is_promotion = move.IsPromotion();
  bool is_en_passant = move_type == Move::Type::en_passant;

  // handle captures
  current_game_state_ |= static_cast<u16>(captured_piece_type << 8);
  if (captured_piece_type != piece::EMPTY && !is_en_passant) {
    // TODO(justin): handle zobrist hashing here
    GetPieceList(captured_piece_type, opponent_color_index)->RemovePieceFromSquare(move_to);
  }

  if (move_piece_type == piece::KING) {
    king_square_[color_to_move_index_] = move_to;
    new_castling_rights &= (white_to_move_) ? white_castle_mask_ : black_castle_mask_;
  } else if (move_piece_type != piece::EMPTY) {
    GetPieceList(move_piece_type, color_to_move_index_)->MovePiece(move_from, move_to);
  }

  int piece_on_target_square = move_piece;

  // handle promotion
  if (is_promotion) {
    int promote_type = 0;
    switch (static_cast<Move::Type>(move_type)) {
      case Move::Type::promote_queen:
        promote_type = piece::QUEEN;
        queens_[color_to_move_index_]->AddPieceToSquare(move_to);
        break;
      case Move::Type::promote_rook:
        promote_type = piece::ROOK;
        rooks_[color_to_move_index_]->AddPieceToSquare(move_to);
        break;
      case Move::Type::promote_knight:
        promote_type = piece::KNIGHT;
        knights_[color_to_move_index_]->AddPieceToSquare(move_to);
        break;
      case Move::Type::promote_bishop:
        promote_type = piece::BISHOP;
        bishops_[color_to_move_index_]->AddPieceToSquare(move_to);
        break;
      default:
        CHESS_ASSERT(false, "Can't promote to requested piece type");
    }
    piece_on_target_square = promote_type | color_to_move_;
    pawns_[color_to_move_index_]->RemovePieceFromSquare(move_to);
  } else {
    // en passant or caslting
    switch (static_cast<Move::Type>(move_type)) {
      case Move::Type::en_passant: {
        int ep_pawn_square = move_to + ((color_to_move_ == piece::WHITE)   ? -8 : 8);
        current_game_state_ |= static_cast<u16>(square_[ep_pawn_square] << 8);
        square_[ep_pawn_square] = piece::EMPTY;
        pawns_[opponent_color_index]->RemovePieceFromSquare(ep_pawn_square);
        // TODO(justin): handle zobrist;
        break;
      }
      case Move::Type::castle: {
        bool kingside = move_to == BoardModel::G1 || (move_to == BoardModel::G8);
        int castling_rook_from_index = (kingside) ? move_to + 1 : move_to - 2;
        int castling_rook_to_index = (kingside) ? move_to - 1 : move_to + 1;

        square_[castling_rook_from_index] = piece::EMPTY;
        square_[castling_rook_to_index] = piece::ROOK | color_to_move_;

        rooks_[color_to_move_index_]->MovePiece(castling_rook_from_index, castling_rook_to_index);
        // TODO(justin): handle zobrist
        break;
      }
      default:
        break;
    }
  }

  // update the board move
  square_[move_to] = piece_on_target_square;
  square_[move_from] = piece::EMPTY;

  // pawn 2x push
  if (move_type == Move::Type::pawn_jump) {
    int file = BoardModel::FileIndex(move_to) + 1;
    current_game_state_ |= static_cast<u16>(file << 4);
    // TODO(justin): handle zobrist hashing
  }

  if (original_castling_rights != 0) {
    if (move_to == BoardModel::H1 || move_from == BoardModel::H1) {
      new_castling_rights &= white_castle_kingside_mask_;
    } else if (move_to == BoardModel::A1 || move_from == BoardModel::A1) {
      new_castling_rights &= white_castle_queenside_mask_;
    }
    if (move_to == BoardModel::H8 || move_from == BoardModel::H8) {
      new_castling_rights &= black_castle_kingside_mask_;
    } else if (move_to == BoardModel::A8 || move_from == BoardModel::A8) {
      new_castling_rights &= black_castle_queenside_mask_;
    }
  }

  // TODO(justin): handle zobrist hashing

  if (old_en_passant_file != 0) {
    // TODO(justin): handle zobrist hashing
  }

  if (new_castling_rights != original_castling_rights) {
    // TODO(justin): handle zobrist hashing
  }

  current_game_state_ |= new_castling_rights;
  current_game_state_ |= static_cast<u32>(fifty_move_counter_) << 14;
  history_.push(current_game_state_);

  // change side to move
  white_to_move_ = !white_to_move_;
  color_to_move_ = (white_to_move_) ? piece::WHITE : piece::BLACK;
  opponent_color_ = (white_to_move_) ? piece::BLACK : piece::WHITE;
  color_to_move_index_ = 1 - color_to_move_index_;
  ply_count_++;
  fifty_move_counter_++;

  if (!searching) {
    if (move_piece_type == piece::PAWN || captured_piece_type != piece::EMPTY) {
      repetition_position_history_.clear();
      repetition_position_stack_.clear();
      fifty_move_counter_ = 0;
    } else {
      // repetition_position_history(zobrist_key);
    }
  }

  // if (!AssertPieceList()) {
  //   std::cout << "Making move: " << move.Start() << " " << move.End() << std::endl;
  //   CHESS_ASSERT(false, "piece list is inconsistent with board");
  // }

}

void Board::UndoMove(const Move &move, bool searching) {
  
  int opponent_color_index = color_to_move_index_;
  bool undoing_white_move = opponent_color_ == piece::WHITE;
  color_to_move_ = opponent_color_;
  opponent_color_ = (undoing_white_move) ? piece::BLACK : piece::WHITE;
  color_to_move_index_ = 1 - color_to_move_index_;
  white_to_move_ = !white_to_move_;

  u32 original_castle_state = current_game_state_ & 0b1111;

  int captured_piece_type = (static_cast<int>(current_game_state_) >> 8) & 63;
  int captured_piece = (captured_piece_type == piece::EMPTY) ? 0 : (captured_piece_type | opponent_color_);

  int moved_from = move.Start();
  int moved_to = move.End();
  auto move_type = move.MoveType();
  bool is_en_passant = move_type == Move::Type::en_passant;
  bool is_promotion = move.IsPromotion();

  int to_square_piece_type = piece::PieceType(square_[moved_to]);
  
  int moved_piece_type = is_promotion ? piece::PAWN : to_square_piece_type;

  // TODO(justin): handle zobrist;

  u32 old_en_passant_file = (current_game_state_ >> 4) & 15;
  if (old_en_passant_file != 0) {
    // TODO(justin): handle zobrist;
  }

  if (captured_piece_type != piece::EMPTY && !is_en_passant) {
    // TODO(justin): handle zobrist;
    GetPieceList(captured_piece_type, opponent_color_index)->AddPieceToSquare(moved_to);
  }

  // updating the piece list
  if (moved_piece_type == piece::KING) {
    king_square_[color_to_move_index_] = moved_from;
  } else if (!is_promotion) {
    GetPieceList(moved_piece_type, color_to_move_index_)->MovePiece(moved_to, moved_from);
  }

  square_[moved_from] = moved_piece_type | color_to_move_;
  square_[moved_to] = captured_piece;  // 0 if no piece was captured

  if (is_promotion) {
    pawns_[color_to_move_index_]->AddPieceToSquare(moved_from);
    switch (static_cast<Move::Type>(move_type)) {
      case Move::Type::promote_queen:
        queens_[color_to_move_index_]->RemovePieceFromSquare(moved_to);
        break;
      case Move::Type::promote_rook:
        rooks_[color_to_move_index_]->RemovePieceFromSquare(moved_to);
        break;
      case Move::Type::promote_bishop:
        bishops_[color_to_move_index_]->RemovePieceFromSquare(moved_to);
        break;
      case Move::Type::promote_knight:
        knights_[color_to_move_index_]->RemovePieceFromSquare(moved_to);
        break;
      default:
        CHESS_ASSERT(false, "Impossible piece there");
    }
  } else if (is_en_passant) {
    int ep_index = moved_to + ((color_to_move_ == piece::WHITE) ? -8 : 8);
    square_[moved_to] = piece::EMPTY;
    square_[ep_index] = captured_piece;
    pawns_[opponent_color_index]->AddPieceToSquare(ep_index);
    // TODO(justin): handle zobrist;
  } else if (move_type == Move::Type::castle) {
    bool kingside = moved_to == BoardModel::G1 || moved_to == BoardModel::G8;
    int castling_rook_from_index = (kingside) ? moved_to + 1 : moved_to - 2;
    int castling_rook_to_index = (kingside) ? moved_to - 1 : moved_to + 1;

    square_[castling_rook_to_index] = 0;
    square_[castling_rook_from_index] = piece::ROOK | color_to_move_;

    rooks_[color_to_move_index_]->MovePiece(castling_rook_to_index, castling_rook_from_index);
    // TODO(justin): handle zobrist hashing
  }

  history_.pop();
  current_game_state_ = history_.top();

  fifty_move_counter_ = static_cast<int>(current_game_state_ & 4294950912) >> 14;
  int new_en_passant_file = static_cast<int>(current_game_state_ >> 4) & 15;
  if (new_en_passant_file != 0) {
    // TODO(justin): handle zobrist;
  }

  u32 new_castle_state = current_game_state_ & 0b1111;
  if (new_castle_state != original_castle_state) {
    // handle zobrist;
  }

  ply_count_--;

  if (!searching && !repetition_position_stack_.empty()) {
    u64 last_state = repetition_position_stack_.back();
    repetition_position_history_[last_state]--;
    repetition_position_stack_.pop_back();
  }

  // if (!AssertPieceList()) {
  //   std::cout << "Undoing move: " << move.Start() << " " << move.End() << std::endl;
  //   CHESS_ASSERT(false, "piece list is inconsistent with board");
  // }

  
}

void Board::LoadStartPosition() {
  const std::string fen_start = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  LoadPosition(fen_start);
}

void Board::LoadPosition(const std::string &fen) {
  // Init();
  auto loaded_position = loadChessBoardInfo(fen);
  for (int square_index = 0; square_index < 64; square_index++) {
    int piece = loaded_position->squares[square_index];
    square_[square_index] = piece;
    if (piece != piece::EMPTY) {
      int piece_type = piece::PieceType(piece);
      int piece_color_index = piece::IsColor(piece, piece::WHITE) ? WHITE_INDEX : BLACK_INDEX;
      if (piece_type == piece::KING) {
        king_square_[piece_color_index] = square_index;
      } else {
        GetPieceList(piece_type, piece_color_index)->AddPieceToSquare(square_index);
      }

      // if (piece::IsSlidingPiece(piece)) {
      //   if (piece_type == piece::QUEEN) {
      //     // queens_[piece_color_index].AddPieceToSquare(square_index);
      //     all_piece_lists_[piece_color_index *]
      //   } else if (piece_type == piece::ROOK) {
      //     // rooks_[piece_color_index].AddPieceToSquare(square_index);
      //   } else {
      //     CHESS_ASSERT(piece_type == piece::BISHOP, "piece type should be bishop");
      //     // bishops_[piece_color_index].AddPieceToSquare(square_index);
      //   }
      // } else if (piece_type == piece::KNIGHT) {
      //   // knights_[piece_color_index].AddPieceToSquare(square_index);
      // } else if (piece_type == piece::PAWN) {
      //   // pawns_[piece_color_index].AddPieceToSquare(square_index);
      // } else {
      //   CHESS_ASSERT(piece_type == piece::KING, "piece type should be king");
      //   king_square_[piece_color_index] = square_index;
      // }
    }

    // side to move
    white_to_move_ = loaded_position->white_to_move;
    color_to_move_ = white_to_move_ ? piece::WHITE : piece::BLACK;
    opponent_color_ = white_to_move_ ? piece::BLACK : piece::WHITE;
    color_to_move_index_ = white_to_move_ ? 0 : 1;

    // create game state
    int white_castle = ((loaded_position->white_kingside_castle) ? 1 << 0 : 0) |
                       ((loaded_position->white_queenside_castle) ? 1 << 1 : 0);
    int black_castle = ((loaded_position->black_kingside_castle) ? 1 << 2 : 0) |
                       ((loaded_position->black_queenside_castle) ? 1 << 3 : 0);
    int ep_state = loaded_position->en_passant_file << 4;
    u16 initial_game_state = static_cast<u16>(white_castle | black_castle | ep_state);

    history_.push(initial_game_state);
    current_game_state_ = initial_game_state;
    fifty_move_counter_ = loaded_position->half_move_count;  // TODO(justin): this is different from the code

    // TODO(justin): handle zobrist
  }

  delete loaded_position;
}

void Board::Init() {
  knights_ = {new PieceList(10), new PieceList(10)};
  bishops_ = {new PieceList(10), new PieceList(10)};
  rooks_ = {new PieceList(10), new PieceList(10)};
  queens_ = {new PieceList(9), new PieceList(9)};

  pawns_ = {new PieceList(8), new PieceList(8)};

  PieceList *empty_list = new PieceList(0);

  all_piece_lists_ = {empty_list,          empty_list,
                      pawns_[WHITE_INDEX], knights_[WHITE_INDEX],
                      empty_list,          bishops_[WHITE_INDEX],
                      rooks_[WHITE_INDEX], queens_[WHITE_INDEX],
                      empty_list,          empty_list,
                      pawns_[BLACK_INDEX], knights_[BLACK_INDEX],
                      empty_list,          bishops_[BLACK_INDEX],
                      rooks_[BLACK_INDEX], queens_[BLACK_INDEX]};
}

auto Board::DumpChessboardInfo() -> std::string {
  const std::map<int, char> piece_char_map{
      {piece::WHITE | piece::PAWN, 'P'}, {piece::WHITE | piece::KNIGHT, 'N'}, {piece::WHITE | piece::BISHOP, 'B'},
      {piece::WHITE | piece::ROOK, 'R'}, {piece::WHITE | piece::QUEEN, 'Q'},  {piece::WHITE | piece::KING, 'K'},

      {piece::BLACK | piece::PAWN, 'b'}, {piece::BLACK | piece::KNIGHT, 'n'}, {piece::BLACK | piece::BISHOP, 'b'},
      {piece::BLACK | piece::ROOK, 'r'}, {piece::BLACK | piece::QUEEN, 'q'},  {piece::BLACK | piece::KING, 'k'},
  };

  std::string fen{};
  for (int rank = 7; rank >= 0; rank--) {
    int num_empty_files = 0;
    for (int file = 0; file < 8; file++) {
      int index = rank * 8 + file;
      auto piece = square_[index];  // TODO(justin): create the board class next
      if (piece != piece::EMPTY) {
        if (num_empty_files != 0) {
          fen += std::to_string(num_empty_files);
          num_empty_files = 0;
        }
        char piece_char = piece_char_map.at(piece);
        fen += piece_char;
      } else {
        num_empty_files++;
      }
    }
    if (num_empty_files > 0) {
      fen += std::to_string(num_empty_files);
    }
    if (rank > 0) {
      fen += '/';
    }
  }

  // whose turn
  fen += ' ' + ((white_to_move_) ? 'w' : 'b');

  // castling rights
  auto white_kingside_castle = current_game_state_ & 1;
  auto white_queenside_castle = current_game_state_ & 0b10;
  auto black_kingside_castle = current_game_state_ & 0b100;
  auto black_queenside_castle = current_game_state_ & 0b1000;
  fen += ' ';
  fen += white_kingside_castle != 0U ? "K" : "";
  fen += white_queenside_castle != 0U ? "Q" : "";
  fen += black_kingside_castle != 0U ? "k" : "";
  fen += black_queenside_castle != 0U ? "q" : "";
  fen += ((current_game_state_ & 0b1111) == 0) ? "-" : "";

  // en-passant
  fen += ' ';
  int en_passant_file = (current_game_state_ >> 4) & 0b1111;
  if (en_passant_file == 0) {
    fen += '-';
  } else {
    char file_name = BoardModel::FILE_NAMES[en_passant_file - 1];
    int en_passant_rank = white_to_move_ ? 6 : 3;
    fen += std::to_string(file_name) + std::to_string(en_passant_rank);
  }

  // half clock
  fen += ' ';
  fen += std::to_string(fifty_move_counter_);

  // full-move count
  fen += ' ';
  fen += std::to_string(ply_count_);

  return fen;
}

auto Board::AssertPieceList() -> bool {
  int i = 0;
  int color = piece::WHITE; 
  for (auto& pl : all_piece_lists_) {
    if (pl->Count() != 0) {
      auto all_squares = pl->GetAllSquares();
      for (int square : all_squares) {
        if (i == 2 && square_[square] != (color | piece::PAWN)) {
          std::cout << "square @ " << square << " is " << square_[square] 
          << ", where piecelist is " << (color | piece::PAWN) << std::endl;
          return false;
        }
        if (i == 3 && square_[square] != (color | piece::KNIGHT)) {
          std::cout << "square @ " << square << " is " << square_[square] 
          << ", where piecelist is " << (color | piece::PAWN) << std::endl;
          return false;
        }
        if (i == 5 && square_[square] != (color | piece::BISHOP)) {
          std::cout << "square @ " << square << " is " << square_[square] 
          << ", where piecelist is " << (color | piece::PAWN) << std::endl;
          return false;
        }
        if (i == 6 && square_[square] != (color | piece::ROOK)) {
          std::cout << "square @ " << square << " is " << square_[square] 
          << ", where piecelist is " << (color | piece::PAWN) << std::endl;
          return false;
        }
        if (i == 7 && square_[square] != (color | piece::QUEEN)) {
          std::cout << "square @ " << square << " is " << square_[square] 
          << ", where piecelist is " << (color | piece::PAWN) << std::endl;
          return false;
        }
      }
    }
    i++;
    if (i == 8) {
      i = 0;
      color = piece::BLACK;
    }
  }
  return true;
}

// Board &Board::operator=(Board other) {
//   swap(*this, other);
//   return *this;
// }

}  // namespace chess
