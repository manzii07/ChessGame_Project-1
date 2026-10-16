#include "Board.h"

char board[8][8];
bool whiteTurn = true;
int selectedRow = -1, selectedCol = -1;
bool pieceSelected = false;
int enPassantRow = -1, enPassantCol = -1;

bool whiteKingMoved = false, blackKingMoved = false;
bool whiteRookAMoved = false, whiteRookHMoved = false;
bool blackRookAMoved = false, blackRookHMoved = false;

stack<GameState> history;
vector<string> moveHistory;

void initBoard() {
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            board[r][c] = '.';

    board[0][0] = 'r'; board[0][1] = 'n'; board[0][2] = 'b'; board[0][3] = 'q';
    board[0][4] = 'k'; board[0][5] = 'b'; board[0][6] = 'n'; board[0][7] = 'r';
    for (int c = 0; c < 8; c++) board[1][c] = 'p';

    board[7][0] = 'R'; board[7][1] = 'N'; board[7][2] = 'B'; board[7][3] = 'Q';
    board[7][4] = 'K'; board[7][5] = 'B'; board[7][6] = 'N'; board[7][7] = 'R';
    for (int c = 0; c < 8; c++) board[6][c] = 'P';
}

bool isWhitePiece(char p) { return p >= 'A' && p <= 'Z'; }
bool isBlackPiece(char p) { return p >= 'a' && p <= 'z'; }
bool isEmpty(int r, int c) { return board[r][c] == '.'; }

void saveState() {
    GameState state;
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            state.board[r][c] = board[r][c];
    state.whiteTurn = whiteTurn;
    state.whiteKingMoved = whiteKingMoved;
    state.blackKingMoved = blackKingMoved;
    state.whiteRookAMoved = whiteRookAMoved;
    state.whiteRookHMoved = whiteRookHMoved;
    state.blackRookAMoved = blackRookAMoved;
    state.blackRookHMoved = blackRookHMoved;
	state.enPassantRow = enPassantRow;
	state.enPassantCol = enPassantCol;
    history.push(state);
}

void undoMove() {
    if (history.empty()) return;
    GameState state = history.top();
    history.pop();
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            board[r][c] = state.board[r][c];
    whiteTurn = state.whiteTurn;
    whiteKingMoved = state.whiteKingMoved;
    blackKingMoved = state.blackKingMoved;
    whiteRookAMoved = state.whiteRookAMoved;
    whiteRookHMoved = state.whiteRookHMoved;
    blackRookAMoved = state.blackRookAMoved;
    blackRookHMoved = state.blackRookHMoved;
    enPassantRow = state.enPassantRow;
    enPassantCol = state.enPassantCol;
    if (!moveHistory.empty()) moveHistory.pop_back();
}

void resetGame() {
    while (!history.empty()) history.pop();
    moveHistory.clear();
    whiteTurn = true;
    enPassantRow = -1; enPassantCol = -1;
    selectedRow = -1; selectedCol = -1;
    pieceSelected = false;
    whiteKingMoved = false; blackKingMoved = false;
    whiteRookAMoved = false; whiteRookHMoved = false;
    blackRookAMoved = false; blackRookHMoved = false;
    initBoard();
}