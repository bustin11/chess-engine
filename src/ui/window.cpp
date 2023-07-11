
#include <stdio.h>
#include <stdlib.h>

#include <GL/glut.h>
#include <SOIL/SOIL.h> // Include SOIL library for loading image textures

#define linear_index(x,y) y * NUM_ROW + x
#include "window.hpp"

// TODO: add castling, en passant, and promotion to queen

// https://stackoverflow.com/questions/14115550/opengl-with-c-classes
namespace gui { 

Game* game;

map<int,GLuint> piece_texture_map;

const int screenWidth = 800;
const int screenHeight = 800;

GLuint blackPawnTexture, whitePawnTexture, blackKnightTexture, whiteKnightTexture,
    blackBishopTexture, whiteBishopTexture, blackRookTexture, whiteRookTexture,
    blackKingTexture, whiteKingTexture, blackQueenTexture, whiteQueenTexture;

void loadTexture(const char* filePath, GLuint& texture) {
    texture = SOIL_load_OGL_texture(
        filePath,
        SOIL_LOAD_RGBA,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_INVERT_Y
    );
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void drawSquare(int x, int y, float r, float g, float b, float a = 1.0f) {
    glBegin(GL_QUADS);
    glColor4f(r, g, b, a);
    glVertex2f(x, y);
    glVertex2f(x + 100, y);
    glVertex2f(x + 100, y + 100);
    glVertex2f(x, y + 100);
    glEnd();
}

void drawChessPiece(int x, int y, GLuint texture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Set the color to white with full alpha
    glTexCoord2f(0.0, 0.0);
    glVertex2f(x, y);
    glTexCoord2f(1.0, 0.0);
    glVertex2f(x + 100, y);
    glTexCoord2f(1.0, 1.0);
    glVertex2f(x + 100, y + 100);
    glTexCoord2f(0.0, 1.0);
    glVertex2f(x, y + 100);
    glEnd();

    // glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

color_t promotionColor;
bool selectingPromotionPiece = false;
int move_from, move_to;

bool isPieceSelected = false;
int selectedPieceX, selectedPieceY;
void handleMouseClick(int button, int state, int x, int y) {

  // if game is in promotion: 

  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

    auto& board = game->get_board()._board;
    auto color_to_move = game->get_board()._color_to_move;
    int boardX = x / 100;
    int boardY = 7 - (y / 100); // Flip the y-coordinate to match the board layout
    LOG_TRACE("%d, %d", boardX, boardY);
    if (!selectingPromotionPiece) {
      // Convert mouse coordinates to board coordinates
      if (!isPieceSelected) {
        // Select a piece if the clicked square contains one
        if (get_color(board[linear_index(boardX, boardY)]) == color_to_move) {
          selectedPieceX = boardX;
          selectedPieceY = boardY;
          isPieceSelected = true;
          game->set_piece_selected(linear_index(boardX, boardY), board[linear_index(boardX, boardY)]);
          for (auto& moves : game->get_moves()) {
            LOG_TRACE("%s", moves.to_str().c_str());
          }
          LOG_TRACE("selected");
        }
      } else if (selectedPieceX != boardX || selectedPieceY != boardY) {
        // Move the selected piece to the clicked square
        // Update the piece's position and update the board state accordingly
        try {
          move_from = linear_index(selectedPieceX, selectedPieceY);
          move_to = linear_index(boardX, boardY);
          game->make_move(move_from, move_to);
        } catch (const std::logic_error &exc) { // pawn promotion
          LOG_TRACE("Pawn promotion, please select valid piece to promote to");
          selectingPromotionPiece = true;
          game->undo_move();
          promotionColor = color_to_move;
        }
        selectedPieceX = boardX;
        selectedPieceY = boardY;
        isPieceSelected = false;
        LOG_TRACE("unselected");
      }
    } else { // need to select piece promotion
      // handle where the promotion pieces area will be and click on that area
      if (boardY == 4) {
        switch (boardX) {
          case 2: // TODO: handle all cases
            if (promotionColor == chess::BLACK) {
              game->make_move(move_from, move_to, chess::BLACK | chess::KNIGHT);
            } else {
              game->make_move(move_from, move_to, chess::WHITE | chess::KNIGHT);
            }
            break;
          case 3: // TODO: handle all cases
            if (promotionColor == chess::BLACK) {
              game->make_move(move_from, move_to, chess::BLACK | chess::BISHOP);
            } else {
              game->make_move(move_from, move_to, chess::WHITE | chess::BISHOP);
            }
            break;
          case 4: // TODO: handle all cases
            if (promotionColor == chess::BLACK) {
              game->make_move(move_from, move_to, chess::BLACK | chess::ROOK);
            } else {
              game->make_move(move_from, move_to, chess::WHITE | chess::ROOK);
            }
            break;
          case 5: // TODO: handle all cases
            if (promotionColor == chess::BLACK) {
              game->make_move(move_from, move_to, chess::BLACK | chess::QUEEN);
            } else {
              game->make_move(move_from, move_to, chess::WHITE | chess::QUEEN);
            }
            break;
          default:
            break;
        }
      }
      selectingPromotionPiece = false;
    }
    glutPostRedisplay();
  }
  
}

void handleKeyboardPress(unsigned char key, int x, int y) {
  switch (key) {
    case 'u':
      game->undo_move();
      break;
  }
  glutPostRedisplay();
}

void drawCheckeredBoard() {
  bool isLightSquare = true;

  float alpha = 1.0f;
  if (selectingPromotionPiece) {
    alpha = 0.8f;
  }

  // board itself
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      if (isLightSquare) {
        drawSquare(col * 100, row * 100, 0.463f, 0.588f, 0.337f, alpha); // Light square: #769656
      } else {
        drawSquare(col * 100, row * 100, 0.933f, 0.933f, 0.823f, alpha); // Dark square: #eeeed2
      }
      isLightSquare = !isLightSquare;
    }
    isLightSquare = !isLightSquare;
  }

  // pawn promotion
  if (selectingPromotionPiece) {
    int sx = move_from % 8 * 100;
    int sy = move_from / 8 * 100;
    int tx = move_to % 8 * 100;
    int ty = move_to / 8 * 100;
    drawSquare(sx, sy, 0.952f, 0.666f, 0.376f, .8);
    drawSquare(tx, ty, 0.952f, 0.666f, 0.376f, .8);
  }

  // board selection
  if (isPieceSelected) {
    drawSquare(selectedPieceX * 100, selectedPieceY * 100, 0.952f, 0.666f, 0.376f, .8);
    auto& moves = game->get_moves();
    for (auto& move : moves) {
      drawSquare(move._to % NUM_ROW * 100, move._to / NUM_ROW * 100, 1.0f, 0.607f, 0.607f, .8f);
      LOG_TRACE("%s", move.to_str().c_str());
    }
  }

  // board history
  auto& board = game->get_board();
  if (board._fullMoves > 1) {
    auto& last_move = board._move_history.back();
    drawSquare(last_move._from % NUM_ROW * 100, last_move._from / NUM_ROW * 100, 0.549f, 0.753f, 0.870f, .8);
    drawSquare(last_move._to % NUM_ROW * 100, last_move._to / NUM_ROW * 100, 0.549f, 0.753f, 0.870f, .8);
    LOG_DEBUG("last move: %s", last_move.to_str().c_str());
  }

}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  drawCheckeredBoard();

  auto& board = game->get_board()._board;
  for (int square = 0; square < chess::NUM_SQUARES; square++) {
    if (board[square] == chess::EMPTY) continue;
    int x = square % 8 * 100; 
    int y = square / 8 * 100;
    drawChessPiece(x, y, piece_texture_map[board[square]]);
  }

  if (selectingPromotionPiece) {
    drawSquare(2 * 100, 4 * 100, 0.9f, 0.9f, 0.9f);
    drawSquare(3 * 100, 4 * 100, 0.9f, 0.9f, 0.9f); 
    drawSquare(4 * 100, 4 * 100, 0.9f, 0.9f, 0.9f); 
    drawSquare(5 * 100, 4 * 100, 0.9f, 0.9f, 0.9f); 
    if (promotionColor == BLACK) {
      drawChessPiece(2 * 100, 4 * 100, blackKnightTexture);
      drawChessPiece(3 * 100, 4 * 100, blackBishopTexture);
      drawChessPiece(4 * 100, 4 * 100, blackRookTexture);
      drawChessPiece(5 * 100, 4 * 100, blackQueenTexture);
    } else {
      drawChessPiece(2 * 100, 4 * 100, whiteKnightTexture);
      drawChessPiece(3 * 100, 4 * 100, whiteBishopTexture);
      drawChessPiece(4 * 100, 4 * 100, whiteRookTexture);
      drawChessPiece(5 * 100, 4 * 100, whiteQueenTexture);
    }
  }
  glutSwapBuffers();
  // glFlush();
}

void myInit() {
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, screenWidth, 0, screenHeight);

  game = new StandardGame();
  game->set_board_fen(chess::FEN_START);


  // Load image textures for chess pieces
  loadTexture("imgs/black_pawn.png", blackPawnTexture);
  loadTexture("imgs/white_pawn.png", whitePawnTexture);
  loadTexture("imgs/black_knight.png", blackKnightTexture);
  loadTexture("imgs/white_knight.png", whiteKnightTexture);
  loadTexture("imgs/black_bishop.png", blackBishopTexture);
  loadTexture("imgs/white_bishop.png", whiteBishopTexture);
  loadTexture("imgs/black_rook.png", blackRookTexture);
  loadTexture("imgs/white_rook.png", whiteRookTexture);
  loadTexture("imgs/black_king.png", blackKingTexture);
  loadTexture("imgs/white_king.png", whiteKingTexture);
  loadTexture("imgs/black_queen.png", blackQueenTexture);
  loadTexture("imgs/white_queen.png", whiteQueenTexture);

  piece_texture_map[BLACK | PAWN] = blackPawnTexture;
  piece_texture_map[BLACK | BISHOP] = blackBishopTexture;
  piece_texture_map[BLACK | KNIGHT] = blackKnightTexture;
  piece_texture_map[BLACK | ROOK] = blackRookTexture;
  piece_texture_map[BLACK | QUEEN] = blackQueenTexture;
  piece_texture_map[BLACK | KING] = blackKingTexture;

  piece_texture_map[WHITE | PAWN] = whitePawnTexture;
  piece_texture_map[WHITE | BISHOP] = whiteBishopTexture;
  piece_texture_map[WHITE | KNIGHT] = whiteKnightTexture;
  piece_texture_map[WHITE | ROOK] = whiteRookTexture;
  piece_texture_map[WHITE | QUEEN] = whiteQueenTexture;
  piece_texture_map[WHITE | KING] = whiteKingTexture;

}

int window_run(int argc, char** argv) {
    // glutInit(&argc, argv);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - screenWidth) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - screenHeight) / 2);
    glutCreateWindow("Checkered Board with Chess Pieces");

    glutDisplayFunc(display);
    myInit();

    glutMouseFunc(handleMouseClick);
    glutKeyboardFunc(handleKeyboardPress);

    glutMainLoop();
    delete game;
    return 0;
}

}; // namespace chess