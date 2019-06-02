#include "Game.h"
#include "Screen.h"

namespace ng
{
	Game::Game()
		: _window(sf::VideoMode(Screen::Width, Screen::Height), "Engge")
	{
		_window.setSize(sf::Vector2u(1024, 768));
		_window.setFramerateLimit(60);
		_window.setMouseCursorVisible(false);
	}

	Game::~Game() = default;

	void Game::setEngine(Engine* pEngine) 
	{
		_pEngine = pEngine;
		_pEngine->setWindow(_window);
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
			for (auto &eh : _inputEventHandlers)
			{
				eh->run(event);
			}
			switch (event.type)
			{
			case sf::Event::Closed:
				_window.close();
				break;
			case sf::Event::KeyPressed:
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
				{
					_window.close();
				}
				break;
			default:
				break;
			}
		}
	}

	void Game::update(const sf::Time &elapsed)
	{
		_pEngine->update(elapsed);
	}

	void Game::render()
	{
		_window.clear();
		_pEngine->draw(_window);
		_window.display();
	}
} // namespace ng
