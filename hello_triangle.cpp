
#include <stdio.h>

#include <GL/glut.h>
#include <SOIL/SOIL.h> // Include SOIL library for loading image textures

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

void drawSquare(int x, int y, float r, float g, float b) {
    glBegin(GL_QUADS);
    glColor3f(r, g, b);
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

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

bool isPieceSelected = false;
int selectedPieceX, selectedPieceY;
void handleMouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Convert mouse coordinates to board coordinates
        int boardX = x / 100;
        int boardY = 7 - (y / 100); // Flip the y-coordinate to match the board layout
        printf("%d, %d\n", boardX, boardY);
        // if (!isPieceSelected) {
        //     // Select a piece if the clicked square contains one
        //     if (/* Logic to check if the clicked square has a piece */) {
        //         selectedPieceX = boardX;
        //         selectedPieceY = boardY;
        //         isPieceSelected = true;
        //     }
        // } else {
        //     // Move the selected piece to the clicked square
        //     // Update the piece's position and update the board state accordingly

        //     selectedPieceX = boardX;
        //     selectedPieceY = boardY;
        //     isPieceSelected = false;
        // }
    }
}


void drawCheckeredBoard() {
    bool isLightSquare = true;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (isLightSquare) {
                drawSquare(col * 100, row * 100, 0.463f, 0.588f, 0.337f); // Light square: #769656
            } else {
                drawSquare(col * 100, row * 100, 0.933f, 0.933f, 0.823f); // Dark square: #eeeed2
            }

            isLightSquare = !isLightSquare;
        }

        isLightSquare = !isLightSquare;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawCheckeredBoard();


    // Draw chess pieces using the image textures
    drawChessPiece(0, 0, blackRookTexture);
    drawChessPiece(100, 0, blackKnightTexture);
    drawChessPiece(200, 0, blackBishopTexture);
    drawChessPiece(300, 0, blackQueenTexture);
    drawChessPiece(400, 0, blackKingTexture);
    drawChessPiece(500, 0, blackBishopTexture);
    drawChessPiece(600, 0, blackKnightTexture);
    drawChessPiece(700, 0, blackRookTexture);

    drawChessPiece(0, 100, blackPawnTexture);
    drawChessPiece(100, 100, blackPawnTexture);
    drawChessPiece(200, 100, blackPawnTexture);
    drawChessPiece(300, 100, blackPawnTexture);
    drawChessPiece(400, 100, blackPawnTexture);
    drawChessPiece(500, 100, blackPawnTexture);
    drawChessPiece(600, 100, blackPawnTexture);
    drawChessPiece(700, 100, blackPawnTexture);

    drawChessPiece(0, 700, whiteRookTexture);
    drawChessPiece(100, 700, whiteKnightTexture);
    drawChessPiece(200, 700, whiteBishopTexture);
    drawChessPiece(300, 700, whiteQueenTexture);
    drawChessPiece(400, 700, whiteKingTexture);
    drawChessPiece(500, 700, whiteBishopTexture);
    drawChessPiece(600, 700, whiteKnightTexture);
    drawChessPiece(700, 700, whiteRookTexture);

    drawChessPiece(0, 600, whitePawnTexture);
    drawChessPiece(100, 600, whitePawnTexture);
    drawChessPiece(200, 600, whitePawnTexture);
    drawChessPiece(300, 600, whitePawnTexture);
    drawChessPiece(400, 600, whitePawnTexture);
    drawChessPiece(500, 600, whitePawnTexture);
    drawChessPiece(600, 600, whitePawnTexture);
    drawChessPiece(700, 600, whitePawnTexture);

    glFlush();
}

void myInit() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, screenWidth, 0, screenHeight);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - screenWidth) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - screenHeight) / 2);
    glutCreateWindow("Checkered Board with Chess Pieces");

    glutDisplayFunc(display);
    myInit();

    glutMouseFunc(handleMouseClick);


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

    glutMainLoop();

    return 0;
}
