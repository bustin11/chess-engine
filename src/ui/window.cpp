
#include <GL/glut.h>
#include <SOIL/SOIL.h>  // Include SOIL library for loading image textures

#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "window.hpp"


// https://stackoverflow.com/questions/14115550/opengl-with-c-classes
namespace gui {

chess::Game *game;
chess::MoveGenerator *move_generator;
const int SQUARE_WIDTH = 100;
std::map<int, GLuint> piece_texture_map;
std::vector<chess::Move> all_valid_moves_for_piece_selected;

struct BoardUI {
  inline static chess::Coord knight_promotion_coord{2, 4};
  inline static chess::Coord bishop_promotion_coord{3, 4};
  inline static chess::Coord rook_promotion_coord{4, 4};
  inline static chess::Coord queen_promotion_coord{5, 4};

  inline static int move_from = -1;
  inline static int move_to = -1;
  inline static int promotion_color;
  inline static int selected_piece_x, selected_piece_y;
  inline static int selected_square_index;
  inline static int selected_piece;

  inline static bool enable_attacking_squares = true;
  inline static bool selecting_promotion_piece = false;
  inline static bool is_piece_selected = false;
};

namespace textures {
GLuint black_pawn_texture;
GLuint white_pawn_texture;
GLuint black_knight_texture;
GLuint white_knight_texture;
GLuint black_bishop_texture;
GLuint white_bishop_texture;
GLuint black_rook_texture;
GLuint white_rook_texture;
GLuint black_king_texture;
GLuint white_king_texture;
GLuint black_queen_texture;
GLuint white_queen_texture;
}  // namespace textures

// TODO(justin): Probably not useful for now

void DrawXinSquare(int x, int y, float r, float g, float b) {
  // Calculate the center coordinates of the specified square
  int center_x = x * 100 + 50;
  int center_y = y * 100 + 50;

  // Set the color of the lines
  glColor3f(r, g, b);  // Red color

  // Set the line width
  glLineWidth(2.0);

  // Draw the lines to create an "X"
  glBegin(GL_LINES);
  glVertex2f(center_x - 30, center_y - 30);
  glVertex2f(center_x + 30, center_y + 30);
  glVertex2f(center_x - 30, center_y + 30);
  glVertex2f(center_x + 30, center_y - 30);
  glEnd();
}

void DrawAinSquare(int x, int y, float r, float g, float b) {
  // Calculate the center coordinates of the specified square
  int center_x = x * 100 + 50;
  int center_y = y * 100 + 50;

  // Set the color of the lines and polygons
  glColor3f(r, g, b);  // Blue color

  // Set the radius of the circle
  float radius = 40.0F;

  // Set the number of segments for the circle
  int num_segments = 100;

  // Draw the circle using line segments
  glBegin(GL_LINE_LOOP);
  for (int i = 0; i < num_segments; ++i) {
    float theta = 2.0F * M_PI * static_cast<float>(i) / static_cast<float>(num_segments);
    float x = radius * std::cos(theta);
    float y = radius * std::sin(theta);
    glVertex2f(center_x + x, center_y + y);
  }
  glEnd();
}

auto GetMove() -> chess::Move {
  for (auto &move : all_valid_moves_for_piece_selected) {
    if (move.Start() == BoardUI::move_from && move.End() == BoardUI::move_to) {
      return move;
    }
  }
  return chess::Move::InvalidMove();
}

/**
 * event handling
 */

void HandleMouseClick(int button, int state, int x, int y) {
  // if game is in promotion:
  auto &board = *(game->board_);
  auto &player_to_move = *(game->player_to_move_);
  int color_to_move = game->board_->color_to_move_;

  int board_x = x / SQUARE_WIDTH;
  int board_y = 7 - y / SQUARE_WIDTH;  // Flip the y-coordinate to match the board layout
  int square_index = linear_index(board_x, board_y);

  bool left_clicked = button == GLUT_LEFT_BUTTON && state == GLUT_DOWN;
  bool moving_color_is_selected = chess::piece::Color(board[square_index]) == color_to_move;
  bool selected_same_square = BoardUI::selected_piece_x == board_x && BoardUI::selected_piece_y == board_y;

  if (!left_clicked) {
    return;
  }

  // handle promotion
  if (BoardUI::selecting_promotion_piece) {
    // handle where the promotion pieces area will be and click on that area
    if (BoardUI::knight_promotion_coord.IsEqual(board_x, board_y)) {
      auto move = chess::Move(BoardUI::move_from, BoardUI::move_to, chess::Move::Type::promote_knight);
      player_to_move.ChoseMove(move);
    } else if (BoardUI::bishop_promotion_coord.IsEqual(board_x, board_y)) {
      auto move = chess::Move(BoardUI::move_from, BoardUI::move_to, chess::Move::Type::promote_bishop);
      player_to_move.ChoseMove(move);
    } else if (BoardUI::rook_promotion_coord.IsEqual(board_x, board_y)) {
      auto move = chess::Move(BoardUI::move_from, BoardUI::move_to, chess::Move::Type::promote_rook);
      player_to_move.ChoseMove(move);
    } else if (BoardUI::queen_promotion_coord.IsEqual(board_x, board_y)) {
      auto move = chess::Move(BoardUI::move_from, BoardUI::move_to, chess::Move::Type::promote_queen);
      player_to_move.ChoseMove(move);
    }
    BoardUI::selecting_promotion_piece = false;
    BoardUI::selected_piece = 0;
    glutPostRedisplay();
    return;
  }

  // handle moving piece
  if (!BoardUI::is_piece_selected && moving_color_is_selected) {
    // Select a piece if the clicked square contains one
    BoardUI::selected_piece_x = board_x;
    BoardUI::selected_piece_y = board_y;
    BoardUI::selected_square_index = linear_index(board_x, board_y);
    BoardUI::is_piece_selected = true;
    LOG_TRACE("selected");
  } else if (BoardUI::is_piece_selected) {
    if (!selected_same_square) {
      // Move the selected piece to the clicked square
      BoardUI::move_from = BoardUI::selected_square_index;
      BoardUI::move_to = linear_index(board_x, board_y);
      auto move = GetMove();  // to get the type of the move

      if (!move.IsInvalid()) {
        // valid move
        if (move.IsPromotion()) {
          BoardUI::promotion_color = color_to_move;
          BoardUI::selecting_promotion_piece = true;
        } else {
          player_to_move.ChoseMove(move);
        }
        BoardUI::selected_piece_x = board_x;
        BoardUI::selected_piece_y = board_y;
      }
    }
    BoardUI::is_piece_selected = false;
    LOG_TRACE("unselected");
  }

  glutPostRedisplay();
}

void HandleKeyboardPress(unsigned char key, int x, int y) {
  switch (key) {
    case 'u':
      game->UndoMoveRecentMove();
      BoardUI::is_piece_selected = false;
      BoardUI::selecting_promotion_piece = false;
      break;
  }
  glutPostRedisplay();
}

/**
 * rendering functions
 */

void RenderSquare(int x, int y, float r, float g, float b, float a = 1.0F) {
  glBegin(GL_QUADS);
  glColor4f(r, g, b, a);
  glVertex2f(x * SQUARE_WIDTH, y * SQUARE_WIDTH);
  glVertex2f(x * SQUARE_WIDTH + SQUARE_WIDTH, y * SQUARE_WIDTH);
  glVertex2f(x * SQUARE_WIDTH + SQUARE_WIDTH, y * SQUARE_WIDTH + SQUARE_WIDTH);
  glVertex2f(x * SQUARE_WIDTH, y * SQUARE_WIDTH + SQUARE_WIDTH);
  glEnd();
}

void RenderPiece(const chess::Coord &coord, GLuint texture) {
  int x = coord.x_;
  int y = coord.y_;
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBegin(GL_QUADS);
  glColor4f(1.0F, 1.0F, 1.0F, 1.0F);  // Set the color to white with full alpha
  glTexCoord2f(0.0, 0.0);
  glVertex2f(x * SQUARE_WIDTH, y * SQUARE_WIDTH);
  glTexCoord2f(1.0, 0.0);
  glVertex2f(x * SQUARE_WIDTH + SQUARE_WIDTH, y * SQUARE_WIDTH);
  glTexCoord2f(1.0, 1.0);
  glVertex2f(x * SQUARE_WIDTH + SQUARE_WIDTH, y * SQUARE_WIDTH + SQUARE_WIDTH);
  glTexCoord2f(0.0, 1.0);
  glVertex2f(x * SQUARE_WIDTH, y * SQUARE_WIDTH + SQUARE_WIDTH);
  glEnd();

  // glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
}

void RenderCheckeredBoard() {
  auto render_light_square = [](int x, int y, float a = 1.0F) { RenderSquare(x, y, 0.463F, 0.588F, 0.337F, a); };

  auto render_dark_square = [](int x, int y, float a = 1.0F) { RenderSquare(x, y, 0.933F, 0.933F, 0.823F, a); };

  auto render_selected_square = [](int x, int y, float a = 1.0F) { RenderSquare(x, y, 0.952F, 0.666F, 0.376F, a); };

  auto render_last_move_square = [](int x, int y, float a = 1.0F) { RenderSquare(x, y, 0.549F, 0.753F, 0.870F, a); };

  auto render_all_move_square = [](int x, int y, float a = 1.0F) {
    RenderSquare(x, y, 1.0F, 0.607F, 0.607F, a);
  };

  float selecting_alpha = BoardUI::is_piece_selected ? 0.8F : 1.0F;
  float promotion_alpha = BoardUI::selecting_promotion_piece ? 0.8F : 1.0F;

  // board itself
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      if (chess::BoardModel::IsLightSquare(file, rank)) {
        render_light_square(file, rank, promotion_alpha);
      } else {
        render_dark_square(file, rank, promotion_alpha);
      }
    }
  }

  // board selection
  if (BoardUI::is_piece_selected) {
    render_selected_square(BoardUI::selected_piece_x, BoardUI::selected_piece_y, selecting_alpha);
    auto all_moves = move_generator->GenerateMoves(game->board_);  // TODO(justin): change this
    all_valid_moves_for_piece_selected =
        move_generator->FilterMovesWithStartSquare(all_moves, BoardUI::selected_square_index);
    for (auto &move : all_valid_moves_for_piece_selected) {
      LOG_DEBUG("From: %d,%d ---> %d,%d", move.Start() / 8, move.Start() % 8, move.End() / 8, move.End() % 8);
      render_all_move_square(move.End() % 8, move.End() / 8, selecting_alpha);
    }
  }

  // pawn promotion
  if (BoardUI::selecting_promotion_piece) {
    int sx = BoardUI::move_from % 8;
    int sy = BoardUI::move_from / 8;
    int tx = BoardUI::move_to % 8;
    int ty = BoardUI::move_to / 8;
    render_selected_square(sx, sy, promotion_alpha);
    render_selected_square(tx, ty, promotion_alpha);
  }

  // board history
  auto &game_moves = game->game_moves_;
  if (!game_moves.empty()) {
    auto &last_move = game_moves.back();
    int start = last_move.Start();
    int end = last_move.End();

    render_last_move_square(start % 8, start / 8, .8F);
    render_last_move_square(end % 8, end / 8, .8F);
  }

  // if (enable_attacking_squares && is_piece_selected) {
  //   auto attacking_counts = game->get_attacking_counts();
  //   auto xray_counts = game->get_xray_counts();
  //   for (auto square = 0; square < chess::NUM_SQUARES; square++) {
  //     if (attacking_counts[square] > 0) {
  //       xy_index(x, y, square);
  //       drawAinSquare(x, y, 1.0f, 0, 0);
  //     }
  //     if (xray_counts[square] > 0) {
  //       xy_index(x, y, square);
  //       drawXinSquare(x, y, 0, 1.0f, 0);
  //     }
  //   }
  // }
}

void RenderChessPieces() {
  auto &board = *(game->board_);
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      int square_index = linear_index(file, rank);
      if (board[square_index] == chess::piece::EMPTY) {
        continue;
      }
      RenderPiece(chess::Coord(file, rank), piece_texture_map[board[square_index]]);
    }
  }
}

void Render() {
  glClear(GL_COLOR_BUFFER_BIT);

  RenderCheckeredBoard();
  RenderChessPieces();

  if (BoardUI::selecting_promotion_piece) {
    auto render_grey_square = [](const chess::Coord &coord) {
      int x = coord.x_;
      int y = coord.y_;
      float promotion_alpha = .9f;
      RenderSquare(x, y, 0.9F, 0.9F, 0.9F, promotion_alpha);
    };

    // squares
    render_grey_square(BoardUI::knight_promotion_coord);
    render_grey_square(BoardUI::bishop_promotion_coord);
    render_grey_square(BoardUI::rook_promotion_coord);
    render_grey_square(BoardUI::queen_promotion_coord);

    // pieces on top of squares
    if (BoardUI::promotion_color == chess::piece::BLACK) {
      RenderPiece(BoardUI::knight_promotion_coord, textures::black_knight_texture);
      RenderPiece(BoardUI::bishop_promotion_coord, textures::black_bishop_texture);
      RenderPiece(BoardUI::rook_promotion_coord, textures::black_rook_texture);
      RenderPiece(BoardUI::queen_promotion_coord, textures::black_queen_texture);
    } else {
      RenderPiece(BoardUI::knight_promotion_coord, textures::white_knight_texture);
      RenderPiece(BoardUI::bishop_promotion_coord, textures::white_bishop_texture);
      RenderPiece(BoardUI::rook_promotion_coord, textures::white_rook_texture);
      RenderPiece(BoardUI::queen_promotion_coord, textures::white_queen_texture);
    }
  }

  glutSwapBuffers();
}

void StartNewGame() { 
  game->load_custom_position_ = true;
  game->custom_position_ = "r3k2r/p1ppqpb1/1n2pnp1/3PN3/1p2P3/2N1BQ1p/PPPKbPPP/R6R b kq - 0 2";
  game->NewGame(chess::Game::PlayerType::Human, chess::Game::PlayerType::Human); 
}

void InitTextures() {
  auto load_texture = [](const char *file_path, GLuint &texture) {
    texture = SOIL_load_OGL_texture(file_path, SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID,
                                    SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_INVERT_Y);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  };

  // Load image textures for chess pieces
  load_texture("imgs/black_pawn.png", textures::black_pawn_texture);
  load_texture("imgs/white_pawn.png", textures::white_pawn_texture);
  load_texture("imgs/black_knight.png", textures::black_knight_texture);
  load_texture("imgs/white_knight.png", textures::white_knight_texture);
  load_texture("imgs/black_bishop.png", textures::black_bishop_texture);
  load_texture("imgs/white_bishop.png", textures::white_bishop_texture);
  load_texture("imgs/black_rook.png", textures::black_rook_texture);
  load_texture("imgs/white_rook.png", textures::white_rook_texture);
  load_texture("imgs/black_king.png", textures::black_king_texture);
  load_texture("imgs/white_king.png", textures::white_king_texture);
  load_texture("imgs/black_queen.png", textures::black_queen_texture);
  load_texture("imgs/white_queen.png", textures::white_queen_texture);

  piece_texture_map[chess::piece::BLACK | chess::piece::PAWN] = textures::black_pawn_texture;
  piece_texture_map[chess::piece::BLACK | chess::piece::BISHOP] = textures::black_bishop_texture;
  piece_texture_map[chess::piece::BLACK | chess::piece::KNIGHT] = textures::black_knight_texture;
  piece_texture_map[chess::piece::BLACK | chess::piece::ROOK] = textures::black_rook_texture;
  piece_texture_map[chess::piece::BLACK | chess::piece::QUEEN] = textures::black_queen_texture;
  piece_texture_map[chess::piece::BLACK | chess::piece::KING] = textures::black_king_texture;

  piece_texture_map[chess::piece::WHITE | chess::piece::PAWN] = textures::white_pawn_texture;
  piece_texture_map[chess::piece::WHITE | chess::piece::BISHOP] = textures::white_bishop_texture;
  piece_texture_map[chess::piece::WHITE | chess::piece::KNIGHT] = textures::white_knight_texture;
  piece_texture_map[chess::piece::WHITE | chess::piece::ROOK] = textures::white_rook_texture;
  piece_texture_map[chess::piece::WHITE | chess::piece::QUEEN] = textures::white_queen_texture;
  piece_texture_map[chess::piece::WHITE | chess::piece::KING] = textures::white_king_texture;
}

auto WindowRun(int argc, char **argv) -> int {
  // window
  int screen_width = SQUARE_WIDTH * 8;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB);
  glutInitWindowSize(screen_width, screen_width);
  glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - screen_width) / 2,
                         (glutGet(GLUT_SCREEN_HEIGHT) - screen_width) / 2);

  int win_id = glutCreateWindow("Checkered Board with Chess Pieces");

  glutDisplayFunc(Render);
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, screen_width, 0, screen_width);

  // init game
  game = new chess::Game();
  game->Init();
  StartNewGame();

  // init move generator
  move_generator = new chess::MoveGenerator();

  // event handlers
  glutMouseFunc(HandleMouseClick);
  glutKeyboardFunc(HandleKeyboardPress);

  InitTextures();

  glutMainLoop();
  glutDestroyWindow(win_id);

  delete move_generator;
  delete game;

  return 0;
}

};  // namespace gui
