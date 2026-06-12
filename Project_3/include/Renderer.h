#pragma once
#include <SFML/Graphics.hpp>
#include "Game.h"

/**
 * @class Renderer
 * responsible for rendering a game UI
 * uses SFML library to create a separate interactive window containing the game 
 */
class Renderer {
public:
    Renderer(sf::RenderWindow&, const sf::Font&);

    // Draw complete frame
    void                        draw(const Game&, bool);

    // Convert pixel position to board cell; returns {-1,-1} if outside grid
    Move                        pixelToCell(sf::Vector2i, int) const;

private:
    sf::RenderWindow&   window_;
    const sf::Font&     font_;

    // Layout helpers (recomputed each draw based on window & board size)
    float               cellSize_  = 0.f;
    float               gridLeft_  = 0.f;
    float               gridTop_   = 0.f;

    void                        computeLayout(int);
    void                        drawGrid(int);
    void                        drawCells(const Game&);
    void                        drawX(float, float);
    void                        drawO(float, float);
    void                        drawWinLine(const Game&);
    void                        drawStatusBar(const Game&, bool);
};
