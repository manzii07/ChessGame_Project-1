#pragma once
#include <string>
#include <stack>
#include <vector>
using namespace std;

extern char board[8][8];
extern bool whiteTurn;
extern int selectedRow, selectedCol;
extern bool pieceSelected;

extern bool whiteKingMoved, blackKingMoved;
extern bool whiteRookAMoved, whiteRookHMoved;
extern bool blackRookAMoved, blackRookHMoved;

const int TILE_SIZE = 80;
const int WINDOW_SIZE = 640;
const int PANEL_WIDTH = 200;

struct GameState {
    char board[8][8];
    bool whiteTurn;
    bool whiteKingMoved, blackKingMoved;
    bool whiteRookAMoved, whiteRookHMoved;
    bool blackRookAMoved, blackRookHMoved;
	int enPassantRow, enPassantCol;
};

extern stack<GameState> history;
extern vector<string> moveHistory;
extern int enPassantRow, enPassantCol;

void initBoard();
bool isWhitePiece(char p);
bool isBlackPiece(char p);
bool isEmpty(int r, int c);
void saveState();
void undoMove();
void resetGame();