#pragma once
#include "Board.h"
#include "Moves.h"
#include "Game.h"
#include "Menu.h"
#include <vector>
using namespace std;

struct AIMove {
    int r1, c1, r2, c2;
};

void makeAIMove(GameMode difficulty, string& statusMsg);