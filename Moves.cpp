#include "Moves.h"
#include "Board.h"

// forward declaration
bool isInCheck(bool white);

bool isValidPawnMove(int r1, int c1, int r2, int c2, bool white) {
    int direction = white ? -1 : 1;
    int startRow = white ? 6 : 1;

    if (c2 == c1 && r2 == r1 + direction && isEmpty(r2, c2))
        return true;

    if (c2 == c1 && r1 == startRow
        && r2 == r1 + 2 * direction
        && isEmpty(r1 + direction, c1)
        && isEmpty(r2, c2))
        return true;

    // normal diagonal capture
    if (r2 == r1 + direction && abs(c2 - c1) == 1) {
        char target = board[r2][c2];
        if (white && isBlackPiece(target)) return true;
        if (!white && isWhitePiece(target)) return true;

        // en passant capture
        if (r2 == enPassantRow && c2 == enPassantCol) return true;
    }

    return false;
}

bool isValidRookMove(int r1, int c1, int r2, int c2) {
    if (r1 != r2 && c1 != c2) return false;
    if (r1 == r2) {
        int step = (c2 > c1) ? 1 : -1;
        for (int c = c1 + step; c != c2; c += step)
            if (!isEmpty(r1, c)) return false;
    }
    else {
        int step = (r2 > r1) ? 1 : -1;
        for (int r = r1 + step; r != r2; r += step)
            if (!isEmpty(r, c1)) return false;
    }
    return true;
}

bool isValidBishopMove(int r1, int c1, int r2, int c2) {
    if (abs(r2 - r1) != abs(c2 - c1)) return false;
    int rowStep = (r2 > r1) ? 1 : -1;
    int colStep = (c2 > c1) ? 1 : -1;
    int r = r1 + rowStep, c = c1 + colStep;
    while (r != r2 || c != c2) {
        if (!isEmpty(r, c)) return false;
        r += rowStep;
        c += colStep;
    }
    return true;
}

bool isValidQueenMove(int r1, int c1, int r2, int c2) {
    return isValidRookMove(r1, c1, r2, c2) || isValidBishopMove(r1, c1, r2, c2);
}

bool isValidKnightMove(int r1, int c1, int r2, int c2) {
    int dr = abs(r2 - r1), dc = abs(c2 - c1);
    return (dr == 2 && dc == 1) || (dr == 1 && dc == 2);
}

bool isValidKingMove(int r1, int c1, int r2, int c2) {
    int dr = abs(r2 - r1), dc = abs(c2 - c1);
    return dr <= 1 && dc <= 1 && !(dr == 0 && dc == 0);
}
bool isValidCastling(int r1, int c1, int r2, int c2, bool white) {
    int row = white ? 7 : 0;
    char king = white ? 'K' : 'k';
    bool kingMoved = white ? whiteKingMoved : blackKingMoved;
    bool rookAMoved = white ? whiteRookAMoved : blackRookAMoved;
    bool rookHMoved = white ? whiteRookHMoved : blackRookHMoved;

    // king must be on starting square
    if (r1 != row || c1 != 4) return false;
    if (kingMoved) return false;
    if (isInCheck(white)) return false;

    // kingside castling (king moves to g file, col 6)
    if (r2 == row && c2 == 6) {
        if (rookHMoved) return false;
        if (!isEmpty(row, 5) || !isEmpty(row, 6)) return false;

        // king cannot pass through attacked square
        board[row][5] = king; board[row][4] = '.';
        bool throughCheck = isInCheck(white);
        board[row][4] = king; board[row][5] = '.';
        if (throughCheck) return false;

        return true;
    }

    // queenside castling (king moves to c file, col 2)
    if (r2 == row && c2 == 2) {
        if (rookAMoved) return false;
        if (!isEmpty(row, 1) || !isEmpty(row, 2) || !isEmpty(row, 3)) return false;

        // king cannot pass through attacked square
        board[row][3] = king; board[row][4] = '.';
        bool throughCheck = isInCheck(white);
        board[row][4] = king; board[row][3] = '.';
        if (throughCheck) return false;

        return true;
    }

    return false;
}

bool isValidMove(int r1, int c1, int r2, int c2, bool checkCastling) {
    char piece = tolower(board[r1][c1]);
    if (piece == 'p') return isValidPawnMove(r1, c1, r2, c2, whiteTurn);
    if (piece == 'r') return isValidRookMove(r1, c1, r2, c2);
    if (piece == 'b') return isValidBishopMove(r1, c1, r2, c2);
    if (piece == 'q') return isValidQueenMove(r1, c1, r2, c2);
    if (piece == 'n') return isValidKnightMove(r1, c1, r2, c2);
    if (piece == 'k') return isValidKingMove(r1, c1, r2, c2) ||
        (checkCastling && isValidCastling(r1, c1, r2, c2, whiteTurn));
    return true;

}