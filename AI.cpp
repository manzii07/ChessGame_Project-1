#include "AI.h"
#include <cstdlib>
#include <ctime>
#include <climits>

// piece values for evaluation
int pieceValue(char p) {
    p = tolower(p);
    if (p == 'p') return 1;
    if (p == 'n') return 3;
    if (p == 'b') return 3;
    if (p == 'r') return 5;
    if (p == 'q') return 9;
	if (p == 'k') return 1000;
    return 0;
}

vector<AIMove> getAllLegalMoves(bool white) {
    vector<AIMove> moves;
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

                    // make sure it doesnt leave king in check
                    board[r2][c2] = piece;
                    board[r1][c1] = '.';
                    bool inCheck = isInCheck(white);
                    board[r1][c1] = piece;
                    board[r2][c2] = target;
                    if (inCheck) continue;

                    moves.push_back({ r1, c1, r2, c2 });
                }
            }
        }
    }
    return moves;
}

// evaluate board score (positive = good for white, negative = good for black)
int evaluateBoard() {
    int score = 0;
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++) {
            char p = board[r][c];
            if (p == '.') continue;
            if (isWhitePiece(p)) score += pieceValue(p);
            else score -= pieceValue(p);
        }
    return score;
}

// minimax with alpha beta pruning
int minimax(int depth, bool maximizing, int alpha, int beta) {
    if (depth == 0) return evaluateBoard();

    vector<AIMove> moves = getAllLegalMoves(maximizing);
    if (moves.empty()) {
        if (isInCheck(maximizing)) return maximizing ? -10000 : 10000;
        return 0; // stalemate
    }

    if (maximizing) {
        int best = INT_MIN;
        for (auto& m : moves) {
            char captured = board[m.r2][m.c2];
            board[m.r2][m.c2] = board[m.r1][m.c1];
            board[m.r1][m.c1] = '.';
            whiteTurn = false;
            int score = minimax(depth - 1, false, alpha, beta);
            board[m.r1][m.c1] = board[m.r2][m.c2];
            board[m.r2][m.c2] = captured;
            whiteTurn = true;
            best = max(best, score);
            alpha = max(alpha, best);
            if (beta <= alpha) break;
        }
        return best;
    }
    else {
        int best = INT_MAX;
        for (auto& m : moves) {
            char captured = board[m.r2][m.c2];
            board[m.r2][m.c2] = board[m.r1][m.c1];
            board[m.r1][m.c1] = '.';
            whiteTurn = true;
            int score = minimax(depth - 1, true, alpha, beta);
            board[m.r1][m.c1] = board[m.r2][m.c2];
            board[m.r2][m.c2] = captured;
            whiteTurn = false;
            best = min(best, score);
            beta = min(beta, best);
            if (beta <= alpha) break;
        }
        return best;
    }
}

void applyAIMove(AIMove m, string& statusMsg) {
    char piece = board[m.r1][m.c1];
    char target = board[m.r2][m.c2];

    string moveStr = "";
    moveStr += (char)('a' + m.c1);
    moveStr += (char)('0' + (8 - m.r1));
    moveStr += "-";
    moveStr += (char)('a' + m.c2);
    moveStr += (char)('0' + (8 - m.r2));
    moveHistory.push_back("B: " + moveStr);

    saveState();

    board[m.r2][m.c2] = piece;
    board[m.r1][m.c1] = '.';

    // handle castling
    if (tolower(piece) == 'k') {
        if (m.c1 == 4 && m.c2 == 6) {
            board[m.r1][5] = board[m.r1][7];
            board[m.r1][7] = '.';
        }
        else if (m.c1 == 4 && m.c2 == 2) {
            board[m.r1][3] = board[m.r1][0];
            board[m.r1][0] = '.';
        }
    }

    checkPawnPromotion();

    if (piece == 'k') blackKingMoved = true;
    if (piece == 'r' && m.c1 == 0) blackRookAMoved = true;
    if (piece == 'r' && m.c1 == 7) blackRookHMoved = true;

    whiteTurn = true;

    if (isCheckmate(whiteTurn)) {
        statusMsg = "Black wins by checkmate!";
    }
    else if (isStalemate(whiteTurn)) {
        statusMsg = "Stalemate! It's a draw!";
    }
    else if (isInsufficientMaterial()) {
        statusMsg = "Draw! Insufficient material!";
    }
    else if (isInCheck(whiteTurn)) {
        statusMsg = "Check! White's turn";
    }
    else {
        statusMsg = "White's turn";
    }
}

void makeAIMove(GameMode difficulty, string& statusMsg) {
    vector<AIMove> moves = getAllLegalMoves(false);
    if (moves.empty()) return;

    AIMove best = moves[0];

    if (difficulty == VS_AI_EASY) {
        // random move
        srand(time(0));
        best = moves[rand() % moves.size()];

    }
    else if (difficulty == VS_AI_MEDIUM) {
        // greedy — pick move that captures most valuable piece
        int bestScore = -1;
        for (auto& m : moves) {
            int score = pieceValue(board[m.r2][m.c2]);
            if (score > bestScore) {
                bestScore = score;
                best = m;
            }
        }

    }
    else if (difficulty == VS_AI_HARD) {
        // minimax 3 moves ahead
        int bestScore = INT_MAX;
        whiteTurn = false;
        for (auto& m : moves) {
            char captured = board[m.r2][m.c2];
            board[m.r2][m.c2] = board[m.r1][m.c1];
            board[m.r1][m.c1] = '.';
            whiteTurn = true;
            int score = minimax(2, true, INT_MIN, INT_MAX);
            board[m.r1][m.c1] = board[m.r2][m.c2];
            board[m.r2][m.c2] = captured;
            whiteTurn = false;
            if (score < bestScore) {
                bestScore = score;
                best = m;
            }
        }
        whiteTurn = false;
    }

    applyAIMove(best, statusMsg);
}