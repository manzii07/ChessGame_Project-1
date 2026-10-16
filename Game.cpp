#include "Game.h"

bool isInCheck(bool white) {
    char king = white ? 'K' : 'k';
    int kingRow = -1, kingCol = -1;
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            if (board[r][c] == king) { kingRow = r; kingCol = c; }

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            char piece = board[r][c];
            if (piece == '.') continue;
            if (white && isWhitePiece(piece)) continue;
            if (!white && isBlackPiece(piece)) continue;
            bool savedTurn = whiteTurn;
            whiteTurn = !white;
            bool canAttack = isValidMove(r, c, kingRow, kingCol, false);
            whiteTurn = savedTurn;
            if (canAttack) return true;
        }
    }
    return false;
}

bool hasAnyLegalMove(bool white) {
    for (int r1 = 0; r1 < 8; r1++) {
        for (int c1 = 0; c1 < 8; c1++) {
            char piece = board[r1][c1];
            if (piece == '.') continue;
            if (white && !isWhitePiece(piece)) continue;
            if (!white && !isBlackPiece(piece)) continue;

            for (int r2 = 0; r2 < 8; r2++) {
                for (int c2 = 0; c2 < 8; c2++) {
                    char target = board[r2][c2];
                    if (white && isWhitePiece(target)) continue;
                    if (!white && isBlackPiece(target)) continue;

                    bool savedTurn = whiteTurn;
                    whiteTurn = white;
                    bool valid = isValidMove(r1, c1, r2, c2);
                    whiteTurn = savedTurn;
                    if (!valid) continue;

                    board[r2][c2] = piece;
                    board[r1][c1] = '.';
                    bool stillInCheck = isInCheck(white);
                    board[r1][c1] = piece;
                    board[r2][c2] = target;

                    if (!stillInCheck) return true;
                }
            }
        }
    }
    return false;
}

bool isCheckmate(bool white) {
    return isInCheck(white) && !hasAnyLegalMove(white);
}

bool isStalemate(bool white) {
    return !isInCheck(white) && !hasAnyLegalMove(white);
}

void checkPawnPromotion() {
    for (int c = 0; c < 8; c++) {
        if (board[0][c] == 'P') board[0][c] = 'Q';
        if (board[7][c] == 'p') board[7][c] = 'q';
    }
}

bool isInsufficientMaterial() {
    vector<char> whitePieces, blackPieces;

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            char p = board[r][c];
            if (p == '.' || p == 'K' || p == 'k') continue;
            if (isWhitePiece(p)) whitePieces.push_back(tolower(p));
            else blackPieces.push_back(tolower(p));
        }
    }

    if (whitePieces.empty() && blackPieces.empty()) return true;

    if (whitePieces.empty() && blackPieces.size() == 1 &&
        (blackPieces[0] == 'b' || blackPieces[0] == 'n')) return true;
    if (blackPieces.empty() && whitePieces.size() == 1 &&
        (whitePieces[0] == 'b' || whitePieces[0] == 'n')) return true;

    return false;
}

void handleClick(int mouseX, int mouseY, string& statusMsg) {
    int col = mouseX / TILE_SIZE;
    int row = mouseY / TILE_SIZE;

    if (row < 0 || row > 7 || col < 0 || col > 7) return;

    char clicked = board[row][col];

    if (!pieceSelected) {
        if (clicked == '.') { statusMsg = "No piece there!"; return; }
        if (whiteTurn && !isWhitePiece(clicked)) { statusMsg = "That's not your piece!"; return; }
        if (!whiteTurn && !isBlackPiece(clicked)) { statusMsg = "That's not your piece!"; return; }
        selectedRow = row;
        selectedCol = col;
        pieceSelected = true;
        statusMsg = (whiteTurn ? "White" : "Black");
        statusMsg += " - piece selected. Click destination.";
    }
    else {
        if (row == selectedRow && col == selectedCol) {
            pieceSelected = false;
            statusMsg = (whiteTurn ? "White" : "Black");
            statusMsg += "'s turn";
            return;
        }

        char piece = board[selectedRow][selectedCol];
        char target = board[row][col];

        if (whiteTurn && isWhitePiece(clicked)) { statusMsg = "Can't capture your own piece!"; pieceSelected = false; return; }
        if (!whiteTurn && isBlackPiece(clicked)) { statusMsg = "Can't capture your own piece!"; pieceSelected = false; return; }

        if (!isValidMove(selectedRow, selectedCol, row, col)) {
            statusMsg = "Illegal move!";
            pieceSelected = false;
            return;
        }

        string moveStr = "";
        moveStr += (char)('a' + selectedCol);
        moveStr += (char)('0' + (8 - selectedRow));
        moveStr += "-";
        moveStr += (char)('a' + col);
        moveStr += (char)('0' + (8 - row));
        moveHistory.push_back((whiteTurn ? "W: " : "B: ") + moveStr);

        saveState();

        // save en passant target before clearing it
        int prevEPRow = enPassantRow;
        int prevEPCol = enPassantCol;

        // set new en passant target if pawn moved two squares
        if (tolower(piece) == 'p' && abs(row - selectedRow) == 2) {
            enPassantRow = (row + selectedRow) / 2;
            enPassantCol = col;
        }
        else {
            enPassantRow = -1;
            enPassantCol = -1;
        }

        // make the move
        board[row][col] = piece;
        board[selectedRow][selectedCol] = '.';

        // remove en passant captured pawn
        if (tolower(piece) == 'p' && col == prevEPCol && row == prevEPRow) {
            int capturedPawnRow = whiteTurn ? row + 1 : row - 1;
            board[capturedPawnRow][col] = '.';
        }

        // handle castling
        if (tolower(piece) == 'k') {
            int row1 = whiteTurn ? 7 : 0;
            if (selectedCol == 4 && col == 6) {
                board[row1][5] = board[row1][7];
                board[row1][7] = '.';
            }
            else if (selectedCol == 4 && col == 2) {
                board[row1][3] = board[row1][0];
                board[row1][0] = '.';
            }
        }

        // pawn promotion
        checkPawnPromotion();

        // check if move leaves own king in check
        if (isInCheck(whiteTurn)) {
            board[selectedRow][selectedCol] = piece;
            board[row][col] = target;
            statusMsg = "That move leaves your king in check!";
            pieceSelected = false;
            return;
        }

        // update moved flags
        if (piece == 'K') whiteKingMoved = true;
        if (piece == 'k') blackKingMoved = true;
        if (piece == 'R' && selectedCol == 0) whiteRookAMoved = true;
        if (piece == 'R' && selectedCol == 7) whiteRookHMoved = true;
        if (piece == 'r' && selectedCol == 0) blackRookAMoved = true;
        if (piece == 'r' && selectedCol == 7) blackRookHMoved = true;

        pieceSelected = false;
        whiteTurn = !whiteTurn;

        if (isCheckmate(whiteTurn)) {
            statusMsg = (whiteTurn ? "Black" : "White");
            statusMsg += " wins by checkmate!";
        }
        else if (isStalemate(whiteTurn)) {
            statusMsg = "Stalemate! It's a draw!";
        }
        else if (isInsufficientMaterial()) {
            statusMsg = "Draw! Insufficient material!";
        }
        else if (isInCheck(whiteTurn)) {
            statusMsg = "Check! ";
            statusMsg += (whiteTurn ? "White" : "Black");
            statusMsg += "'s turn";
        }
        else {
            statusMsg = (whiteTurn ? "White" : "Black");
            statusMsg += "'s turn";
        }
    }
}