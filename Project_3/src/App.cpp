#include "../include/App.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <algorithm>

#include "../include/Game.h"
#include "../include/Renderer.h"

/**
 * public method of @class App to prompt user for board size at the start and after reset
 * @returns size of board chosen by user
 */
int App::promptBoardSize() {
    int n{0};
    while (true) {
        std::cout << "╔══════════════════════════════╗\n";
        std::cout << "║    KOLKO I KRZYZYK  -  AI    ║\n";
        std::cout << "╚══════════════════════════════╝\n";
        std::cout << "Podaj rozmiar planszy (3-10): ";
        std::string line;
        std::getline(std::cin, line);
        try {
            n = std::stoi(line);
            if (n >= 3 && n <= 10) break;
        } catch (...) {}
        std::cout << "Nieprawidlowy rozmiar. Wpisz liczbe calkowita od 3 do 10.\n\n";
    }
    return n;
}

/**
 * public method of @class App to run the game
 * uses SFML for rendering and event handling
 * uses <thread>, <atomic> and <mutex> to separately run AI calculations, and rendering so that the UI doesnt freeze while AI is thinking
 */
void App::run() {

    #ifdef _WIN32
        system("chcp 65001 > nul");
    #endif

    int boardSize = promptBoardSize();

    /** define window dimensions */
    const int MIN_WIN  = 420;
    const int MAX_WIN  = 800;
    const int STATUS_H = 70;

    auto windowPx = [&](int size) { return std::clamp(size * 100, MIN_WIN, MAX_WIN); };

    int gridPx = windowPx(boardSize);

    // SFML 3: VideoMode takes sf::Vector2u
    sf::RenderWindow window( sf::VideoMode({static_cast<unsigned>(gridPx), static_cast<unsigned>(gridPx + STATUS_H)}), "Kolko i Krzyzyk" );
    window.setFramerateLimit(60);

    // SFML 3: sf::Font::openFromFile (replaces loadFromFile)
    sf::Font font;
    bool fontLoaded =
        font.openFromFile("assets/font.ttf")                                    ||
        font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")    ||
        font.openFromFile("/System/Library/Fonts/Helvetica.ttc")                ||
        font.openFromFile("C:/Windows/Fonts/arial.ttf")                         ||
        font.openFromFile("C:/Windows/Fonts/segoeui.ttf");
    if (!fontLoaded) { std::cerr << "Ostrzezenie: nie mozna wczytac czcionki.\n"; }

    /** create game and renderer instances */

    Game                game(boardSize);
    Renderer            renderer(window, font);

    /** create AI and game mutex for synchronization */

    std::atomic<bool>   aiRunning{false};
    std::mutex          gameMutex;

    /**
     * helper function to trigger AI move in a separate thread
     * sets @var aiRunning to true to prevent multiple threads from being spawned, and to show "AI is thinking" status in the UI
     * after a short delay, it locks the game mutex and lets AI to make its move, then sets @var aiRunning back to false
     * after that, detaches the thread and lets it finish on its own
     */
    auto triggerAI = [&]() {
        if (aiRunning) { return; }
        aiRunning = true;
        std::thread([&]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(350));
            {
                std::lock_guard<std::mutex> lk(gameMutex);
                if (game.getGameState() == GameState::Playing && game.getTurn() == Turn::AI) { game.aiMove(); }
            }
            aiRunning = false;
        }).detach();
    };

    /** 
     * main game loop 
     * while window is open, catch any events (mouse clicked, key pressed, window closed) and react accordingly
     */
    while (window.isOpen()) {
        // SFML 3: event loop uses std::optional
        while (const std::optional<sf::Event> event = window.pollEvent()) {

            // Closed
            if (event->is<sf::Event::Closed>()) { window.close(); }

            // Key pressed
            if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
                // SFML 3: sf::Keyboard::Key::R
                if (kp->code == sf::Keyboard::Key::R && !aiRunning) {
                    int newSize = promptBoardSize();
                    {
                        std::lock_guard<std::mutex> lk(gameMutex);
                        game.reset(newSize);
                        int newPx = windowPx(newSize);
                        window.setSize({ static_cast<unsigned>(newPx), static_cast<unsigned>(newPx + STATUS_H) });
                        // SFML 3: sf::View takes sf::FloatRect with position+size
                        window.setView(sf::View(sf::FloatRect({0.f, 0.f}, {static_cast<float>(newPx), static_cast<float>(newPx + STATUS_H) } )));
                    }
                }
            }

            // Mouse click
            if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button == sf::Mouse::Button::Left && !aiRunning) {
                    std::lock_guard<std::mutex> lk(gameMutex);
                    if (game.getGameState() == GameState::Playing && game.getTurn() == Turn::Player)
                    {
                        Move cell = renderer.pixelToCell( {mb->position.x, mb->position.y}, game.getBoardSize());
                        if (cell.row != -1 && game.playerMove(cell.row, cell.col)) {
                            if (game.getGameState() == GameState::Playing) { triggerAI(); }
                        }
                    }
                }
            }
        }

        // Draw
        {
            std::lock_guard<std::mutex> lk(gameMutex);
            renderer.draw(game, aiRunning.load());
        } 
    }
}