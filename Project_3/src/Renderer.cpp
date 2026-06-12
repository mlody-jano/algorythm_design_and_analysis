#include "Renderer.h"
#include <string>
#include <cmath>

/**
 * constants colors and sizes used in rendering
 */
static const sf::Color BG        {15,  15,  23};
static const sf::Color GRID_COL  {55,  55,  75};
static const sf::Color X_COL     {242, 100, 90};
static const sf::Color O_COL     {90,  200, 170};
static const sf::Color WIN_COL   {255, 220, 80};
static const sf::Color TEXT_COL  {210, 210, 230};
static const sf::Color DIM_COL   {100, 100, 120};

static const float STATUS_H = 70.f;
static const float PADDING   = 24.f;

/**
 * parametrized constructor of @class Renderer
 * @param window reference to the SFML RenderWindow to draw on
 * @param font reference to the SFML Font to use for text
 */
Renderer::Renderer(sf::RenderWindow& window, const sf::Font& font) : window_(window), font_(font) {}

/**
 * private method to compute the layout of the grid and cells based on the current window size and board size
 * @param boardSize size of board (number of cells in one dimension)
 */
void Renderer::computeLayout(int boardSize) {
    float w         = static_cast<float>(window_.getSize().x);
    float h         = static_cast<float>(window_.getSize().y) - STATUS_H;
    float available = std::min(w, h) - 2.f * PADDING;
    cellSize_ = available / static_cast<float>(boardSize);
    gridLeft_ = (w - cellSize_ * boardSize) / 2.f;
    gridTop_  = PADDING + (h - cellSize_ * boardSize) / 2.f;
}

/**
 * public method to convert pixel position to board cell
 * @param pixel pixel position in window coordinates
 * @param boardSize size of board (number of cells in one dimension)
 * @returns Move struct representing which row and column to fill
 */
Move Renderer::pixelToCell(sf::Vector2i pixel, int boardSize) const {
    float px = static_cast<float>(pixel.x);
    float py = static_cast<float>(pixel.y);
    if (px < gridLeft_ || py < gridTop_) { return {-1, -1}; }
    int col = static_cast<int>((px - gridLeft_) / cellSize_);
    int row = static_cast<int>((py - gridTop_)  / cellSize_);
    if (row < 0 || row >= boardSize || col < 0 || col >= boardSize) { return {-1, -1}; }
    return {row, col};
}

/**
 * public method for using @class Renderer
 * @param game reference to the valid game object
 * @param aiThinking bool marker when AI is performing a search for best move
 */
void Renderer::draw(const Game& game, bool aiThinking) {
    window_.clear(BG);
    computeLayout(game.getBoardSize());
    drawGrid(game.getBoardSize());
    drawCells(game);
    drawWinLine(game);
    drawStatusBar(game, aiThinking);
    window_.display();
}

/**
 * private method for drawing a valid grid for given board size
 * @param boardSize size of board (number of cells in one dimension)
 */
void Renderer::drawGrid(int boardSize) {
    const float thickness = 2.f;
    float total = cellSize_ * boardSize;

    for (int i{1}; i < boardSize; ++i) {
        // Vertical line
        sf::RectangleShape vline({thickness, total});
        vline.setFillColor(GRID_COL);
        vline.setPosition({gridLeft_ + i * cellSize_ - thickness / 2.f, gridTop_});
        window_.draw(vline);

        // Horizontal line
        sf::RectangleShape hline({total, thickness});
        hline.setFillColor(GRID_COL);
        hline.setPosition({gridLeft_, gridTop_ + i * cellSize_ - thickness / 2.f});
        window_.draw(hline);
    }
}

/**
 * private method for drawing X in the grid
 * @param cx coordinates in X dim
 * @param cy coordinates in Y dim
 */
void Renderer::drawX(float cx, float cy) {
    float arm   = cellSize_ * 0.30f;
    float thick = std::max(3.f, cellSize_ * 0.08f);

    for (int sign : {-1, 1}) {
        sf::RectangleShape bar({arm * 2.f * std::sqrt(2.f), thick});
        bar.setFillColor(X_COL);
        bar.setOrigin({bar.getSize().x / 2.f, bar.getSize().y / 2.f});
        bar.setPosition({cx, cy});
        bar.setRotation(sf::degrees(45.f * static_cast<float>(sign)));
        window_.draw(bar);
    }
}

/**
 * private method for drawing O in the grid
 * @param cx coordinates in X dim
 * @param cy coordinates in Y dim
 */
void Renderer::drawO(float cx, float cy) {
    float radius = cellSize_ * 0.30f;
    float thick  = std::max(3.f, cellSize_ * 0.08f);

    sf::CircleShape ring(radius, 48);
    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineColor(O_COL);
    ring.setOutlineThickness(-thick);
    ring.setOrigin({radius, radius});
    ring.setPosition({cx, cy});
    window_.draw(ring);
}

/**
 * private method for drawing all cells on the grid
 * @param game reference to the game object
 */
void Renderer::drawCells(const Game& game) {
    int n = game.getBoardSize();
    for (int r{0}; r < n; ++r) {
        for (int c{0}; c < n; ++c) {
            float cx = gridLeft_ + (c + 0.5f) * cellSize_;
            float cy = gridTop_  + (r + 0.5f) * cellSize_;
            Cell cell = game.getBoard().getCell(r, c);
            if      (cell == Cell::X) { drawX(cx, cy); }
            else if (cell == Cell::O) { drawO(cx, cy); }
        }
    }
}

/**
 * private method for drawing the winning line
 * @param game reference to the game object
 */
void Renderer::drawWinLine(const Game& game) {
    GameState gs = game.getGameState();
    if (gs != GameState::XWins && gs != GameState::OWins) { return; }

    Cell winner = (gs == GameState::XWins) ? Cell::X : Cell::O;
    const Board& board = game.getBoard();
    int n = board.getSize();

    auto check = [&](const std::vector<std::pair<int,int>>& coords) -> bool {
        for (auto& [r, c] : coords)
            if (board.getCell(r, c) != winner) { return false; }
        return true;
    };

    std::vector<std::pair<int,int>> line;

    for (int r{0}; r < n && line.empty(); ++r) {
        std::vector<std::pair<int,int>> coords;
        for (int c{0}; c < n; ++c) coords.push_back({r, c});
        if (check(coords)) { line = coords; }
    }
    for (int c{0}; c < n && line.empty(); ++c) {
        std::vector<std::pair<int,int>> coords;
        for (int r{0}; r < n; ++r) coords.push_back({r, c});
        if (check(coords)) { line = coords; }
    }
    if (line.empty()) {
        std::vector<std::pair<int,int>> d1, d2;
        for (int i{0}; i < n; ++i) { d1.push_back({i, i}); d2.push_back({i, n - 1 - i}); }
        if      (check(d1)) { line = d1; }
        else if (check(d2)) { line = d2; }
    }
    if (line.empty()) { return; }

    auto cellCenter = [&](std::pair<int,int> rc) -> sf::Vector2f {
        return {gridLeft_ + (rc.second + 0.5f) * cellSize_, gridTop_  + (rc.first  + 0.5f) * cellSize_};
    };

    sf::Vector2f start = cellCenter(line.front());
    sf::Vector2f end   = cellCenter(line.back());
    sf::Vector2f dir   = end - start;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len < 1.f) { return; }
    sf::Vector2f unit = dir / len;
    float thick = std::max(4.f, cellSize_ * 0.07f);

    start -= unit * (cellSize_ * 0.35f);
    end   += unit * (cellSize_ * 0.35f);
    len = std::sqrt((end - start).x * (end - start).x + (end - start).y * (end - start).y);

    sf::RectangleShape wline({len, thick});
    wline.setFillColor(WIN_COL);
    wline.setOrigin({0.f, thick / 2.f});
    wline.setPosition(start);
    float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159265f;
    wline.setRotation(sf::degrees(angle));
    window_.draw(wline);
}

/**
 * private method for drawing the status of game while running
 * @param game reference to the game object
 * @param aiThinking bool marker when AI is performing a search for best move
 */
void Renderer::drawStatusBar(const Game& game, bool aiThinking) {
    float winW = static_cast<float>(window_.getSize().x);
    float winH = static_cast<float>(window_.getSize().y);

    // Separator line
    sf::RectangleShape sep({winW, 1.f});
    sep.setFillColor(GRID_COL);
    sep.setPosition({0.f, winH - STATUS_H});
    window_.draw(sep);

    std::string msg;
    sf::Color   col = TEXT_COL;

    switch (game.getGameState()) {
        case GameState::Playing:
            if (aiThinking) {
                msg = "Computer is thinking...";
                col = DIM_COL;
            } else if (game.getTurn() == Turn::Player) {
                msg = "Your move  (X)";
            } else {
                msg = "Computer move  (O)";
                col = DIM_COL;
            }
            break;
        case GameState::XWins:
            msg = "You won!";
            col = X_COL;
            break;
        case GameState::OWins:
            msg = "Computer won.";
            col = O_COL;
            break;
        case GameState::Draw:
            msg = "Draw.";
            col = DIM_COL;
            break;
    }

    // SFML 3: sf::Text requires font in constructor
    sf::Text statusText(font_, msg, 22);
    statusText.setFillColor(col);
    sf::FloatRect bounds = statusText.getLocalBounds();
    statusText.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
    statusText.setPosition({winW / 2.f, winH - STATUS_H / 2.f});
    window_.draw(statusText);

    sf::Text hint(font_, "R - new game", 14);
    hint.setFillColor(DIM_COL);
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setOrigin({hb.position.x + hb.size.x, hb.position.y + hb.size.y});
    hint.setPosition({winW - PADDING, winH - 12.f});
    window_.draw(hint);
}