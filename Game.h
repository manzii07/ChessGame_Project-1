#pragma once
#include "Board.h"
#include "Moves.h"
#include <string>


bool isInsufficientMaterial();
bool isInCheck(bool white);
bool isCheckmate(bool white);
bool isStalemate(bool white);
void checkPawnPromotion();
void handleClick(int mouseX, int mouseY, string& statusMsg);