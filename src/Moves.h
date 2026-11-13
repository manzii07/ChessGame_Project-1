#pragma once
#include "Board.h"

bool isValidPawnMove(int r1, int c1, int r2, int c2, bool white);
bool isValidRookMove(int r1, int c1, int r2, int c2);
bool isValidBishopMove(int r1, int c1, int r2, int c2);
bool isValidQueenMove(int r1, int c1, int r2, int c2);
bool isValidKnightMove(int r1, int c1, int r2, int c2);
bool isValidKingMove(int r1, int c1, int r2, int c2);
bool isValidCastling(int r1, int c1, int r2, int c2, bool white);
bool isValidMove(int r1, int c1, int r2, int c2, bool checkCastling = true);