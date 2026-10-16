#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include "Board.h"
#include "Game.h"
#include "Menu.h"
#include "AI.h"
using namespace std;

map<char, sf::Texture> textures;

bool loadTextures() {
    map<char, string> files = {
        {'P', "ChessPieces/wP.png"}, {'R', "ChessPieces/wR.png"},
        {'N', "ChessPieces/wN.png"}, {'B', "ChessPieces/wB.png"},
        {'Q', "ChessPieces/wQ.png"}, {'K', "ChessPieces/wK.png"},
        {'p', "ChessPieces/bP.png"}, {'r', "ChessPieces/bR.png"},
        {'n', "ChessPieces/bN.png"}, {'b', "ChessPieces/bB.png"},
        {'q', "ChessPieces/bQ.png"}, {'k', "ChessPieces/bK.png"}
    };

    for (auto& pair : files) {
        if (!textures[pair.first].loadFromFile(pair.second)) {
            return false;
        }
    }
    return true;
}

void drawBoard(sf::RenderWindow& window, sf::Font& font, string statusMsg) {
    sf::Color lightColor(240, 217, 181);
    sf::Color darkColor(181, 136, 99);
    sf::Color highlightColor(100, 200, 100, 180);

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            sf::RectangleShape square(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            square.setPosition(sf::Vector2f(c * TILE_SIZE, r * TILE_SIZE));
            square.setFillColor((r + c) % 2 == 0 ? lightColor : darkColor);
            window.draw(square);

            if (pieceSelected && r == selectedRow && c == selectedCol) {
                square.setFillColor(highlightColor);
                window.draw(square);
            }

            char piece = board[r][c];
            if (piece != '.') {
                sf::Sprite sprite(textures[piece]);
                sf::Vector2u texSize = textures[piece].getSize();
                float scaleX = (float)TILE_SIZE / texSize.x;
                float scaleY = (float)TILE_SIZE / texSize.y;
                sprite.setScale(sf::Vector2f(scaleX, scaleY));
                sprite.setPosition(sf::Vector2f(c * TILE_SIZE, r * TILE_SIZE));
                window.draw(sprite);
            }
        }
    }

    for (int i = 0; i < 8; i++) {
        sf::Text rankText(font, to_string(8 - i), 16);
        rankText.setPosition(sf::Vector2f(2, i * TILE_SIZE + 2));
        rankText.setFillColor(sf::Color::Yellow);
        window.draw(rankText);

        sf::Text fileText(font, string(1, 'a' + i), 16);
        fileText.setPosition(sf::Vector2f(i * TILE_SIZE + 65, 622));
        fileText.setFillColor(sf::Color::Yellow);
        window.draw(fileText);
    }

    sf::RectangleShape bar(sf::Vector2f(WINDOW_SIZE, 30));
    bar.setPosition(sf::Vector2f(0, WINDOW_SIZE - 30));
    bar.setFillColor(sf::Color(30, 30, 30, 200));
    window.draw(bar);

    sf::Text status(font, statusMsg, 18);
    status.setPosition(sf::Vector2f(10, WINDOW_SIZE - 28));
    status.setFillColor(sf::Color::White);
    window.draw(status);
}

void drawPanel(sf::RenderWindow& window, sf::Font& font) {
    sf::RectangleShape panel(sf::Vector2f(PANEL_WIDTH, WINDOW_SIZE));
    panel.setPosition(sf::Vector2f(WINDOW_SIZE, 0));
    panel.setFillColor(sf::Color(30, 30, 30));
    window.draw(panel);

    sf::Text title(font, "Move History", 18);
    title.setPosition(sf::Vector2f(WINDOW_SIZE + 10, 10));
    title.setFillColor(sf::Color::Yellow);
    window.draw(title);

    sf::RectangleShape line(sf::Vector2f(PANEL_WIDTH - 10, 2));
    line.setPosition(sf::Vector2f(WINDOW_SIZE + 5, 35));
    line.setFillColor(sf::Color(100, 100, 100));
    window.draw(line);

    int startIdx = max(0, (int)moveHistory.size() - 24);
    for (int i = startIdx; i < (int)moveHistory.size(); i++) {
        sf::Text moveText(font, moveHistory[i], 15);
        moveText.setPosition(sf::Vector2f(WINDOW_SIZE + 10, 45 + (i - startIdx) * 24));
        moveText.setFillColor(moveHistory[i][0] == 'W' ? sf::Color::White : sf::Color(180, 180, 180));
        window.draw(moveText);
    }

    sf::RectangleShape btn(sf::Vector2f(PANEL_WIDTH - 20, 35));
    btn.setPosition(sf::Vector2f(WINDOW_SIZE + 10, WINDOW_SIZE - 50));
    btn.setFillColor(sf::Color(180, 50, 50));
    window.draw(btn);

    sf::Text btnText(font, "Restart", 18);
    btnText.setPosition(sf::Vector2f(WINDOW_SIZE + 45, WINDOW_SIZE - 46));
    btnText.setFillColor(sf::Color::White);
    window.draw(btnText);
}

int main() {
    sf::RenderWindow window(sf::VideoMode({
        (unsigned int)(WINDOW_SIZE + PANEL_WIDTH),
        (unsigned int)WINDOW_SIZE + 30 }), "Chess Game");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("C:\\Windows\\Fonts\\arial.ttf")) return -1;
    if (!loadTextures()) return -1;

    initBoard();
    string statusMsg = "White's turn";
    GameMode gameMode = MENU;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Z && gameMode != MENU) {
                    undoMove();
                    // undo twice if vs AI so it's still white's turn
                    if (gameMode != VS_PLAYER) undoMove();
                    statusMsg = "White's turn";
                }
            }

            if (auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    int mx = mouseEvent->position.x;
                    int my = mouseEvent->position.y;

                    if (gameMode == MENU) {
                        GameMode selected = handleMenuClick(mx, my);
                        if (selected != MENU) {
                            gameMode = selected;
                            resetGame();
                            statusMsg = "White's turn";
                        }
                    }
                    else {
                        // restart button
                        if (mx >= WINDOW_SIZE + 10 && mx <= WINDOW_SIZE + PANEL_WIDTH - 10
                            && my >= WINDOW_SIZE - 50 && my <= WINDOW_SIZE - 15) {
                            resetGame();
                            gameMode = MENU;
                            statusMsg = "White's turn";
                        }
                        else {
                            // only allow clicks when it's white's turn in AI mode
                            if (gameMode == VS_PLAYER || whiteTurn) {
                                handleClick(mx, my, statusMsg);
                            }
                        }
                    }
                }
            }
        }

        // AI makes its move if it's black's turn
        if (gameMode != MENU && gameMode != VS_PLAYER && !whiteTurn) {
            makeAIMove(gameMode, statusMsg);
        }

        window.clear(sf::Color(50, 50, 50));
        if (gameMode == MENU) {
            drawMenu(window, font);
        }
        else {
            drawBoard(window, font, statusMsg);
            drawPanel(window, font);
        }
        window.display();
    }

    return 0;
}