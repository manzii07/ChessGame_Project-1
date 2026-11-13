#pragma once
#include <SFML/Graphics.hpp>
#include <string>
using namespace std;

enum GameMode { MENU, VS_PLAYER, VS_AI_EASY, VS_AI_MEDIUM, VS_AI_HARD };

GameMode handleMenuClick(int mx, int my);
void drawMenu(sf::RenderWindow& window, sf::Font& font);