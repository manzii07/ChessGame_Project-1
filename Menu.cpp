#include "Menu.h"

void drawButton(sf::RenderWindow& window, sf::Font& font, string text, float x, float y, float w, float h, sf::Color color) {
    sf::RectangleShape btn(sf::Vector2f(w, h));
    btn.setPosition(sf::Vector2f(x, y));
    btn.setFillColor(color);
    btn.setOutlineColor(sf::Color(200, 200, 200));
    btn.setOutlineThickness(2);
    window.draw(btn);

    sf::Text label(font, text, 22);
    label.setFillColor(sf::Color::White);
    sf::FloatRect bounds = label.getLocalBounds();
    label.setPosition(sf::Vector2f(x + (w - bounds.size.x) / 2, y + (h - bounds.size.y) / 2 - 5));
    window.draw(label);
}

void drawMenu(sf::RenderWindow& window, sf::Font& font) {
    window.clear(sf::Color(20, 20, 20));

    // title
    sf::Text title(font, "CHESS", 80);
    title.setFillColor(sf::Color(240, 217, 181));
    title.setOutlineColor(sf::Color(100, 70, 40));
    title.setOutlineThickness(3);
    sf::FloatRect tb = title.getLocalBounds();
    title.setPosition(sf::Vector2f((840 - tb.size.x) / 2, 80));
    window.draw(title);

    // subtitle
    sf::Text sub(font, "Choose Game Mode", 24);
    sub.setFillColor(sf::Color(180, 180, 180));
    sf::FloatRect sb = sub.getLocalBounds();
    sub.setPosition(sf::Vector2f((840 - sb.size.x) / 2, 200));
    window.draw(sub);

    // buttons
    float btnW = 300, btnH = 55, btnX = (840 - btnW) / 2;

    drawButton(window, font, "Play vs Player", btnX, 280, btnW, btnH, sf::Color(60, 120, 60));
    drawButton(window, font, "vs AI - Easy", btnX, 360, btnW, btnH, sf::Color(60, 90, 160));
    drawButton(window, font, "vs AI - Medium", btnX, 440, btnW, btnH, sf::Color(160, 120, 40));
    drawButton(window, font, "vs AI - Hard", btnX, 520, btnW, btnH, sf::Color(160, 50, 50));
}

GameMode handleMenuClick(int mx, int my) {
    float btnW = 300, btnH = 55, btnX = (840 - btnW) / 2;

    if (mx >= btnX && mx <= btnX + btnW) {
        if (my >= 280 && my <= 335) return VS_PLAYER;
        if (my >= 360 && my <= 415) return VS_AI_EASY;
        if (my >= 440 && my <= 495) return VS_AI_MEDIUM;
        if (my >= 520 && my <= 575) return VS_AI_HARD;
    }
    return MENU;
}