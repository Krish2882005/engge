#include "Game.h"
#include "Screen.h"
#include "imgui-SFML.h"
#include "imgui.h"

namespace ng
{
Game::Game() : _window(sf::VideoMode(Screen::Width, Screen::Height), "Engge")
{
    _window.setSize(sf::Vector2u(Screen::Width, Screen::Height));
    _window.setFramerateLimit(60);
    _window.setMouseCursorVisible(false);
    ImGui::SFML::Init(_window);
}

Game::~Game() { ImGui::SFML::Shutdown(); }

void Game::setEngine(Engine *pEngine)
{
    _pEngine = pEngine;
    _pEngine->setWindow(_window);
    _optionsDialog.setEngine(pEngine);
}

void Game::run()
{
    sf::Clock clock;
    while (_window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        update(elapsed);
        processEvents();
        render();
    }
}

void Game::processEvents()
{
    sf::Event event{};
    while (_window.pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(event);
        for (auto &eh : _inputEventHandlers)
        {
            eh->run(event);
        }
        switch (event.type)
        {
            case sf::Event::Closed:
                _window.close();
                break;
            default:
                break;
        }
    }
}

void Game::update(const sf::Time &elapsed)
{
    ImGui::SFML::Update(_window, elapsed);
    _pEngine->update(elapsed);
    _optionsDialog.update(elapsed);
}

void Game::render()
{
    _window.clear();
    _pEngine->draw(_window);
    _window.draw(_optionsDialog);
    ImGui::SFML::Render(_window);
    _window.display();
}
} // namespace ng
