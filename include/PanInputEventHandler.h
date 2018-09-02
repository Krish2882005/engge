#pragma once
#include "SFML/Graphics.hpp"
#include "Game.h"

namespace gg
{
class PanInputEventHandler : public InputEventHandler
{
  public:
    PanInputEventHandler(sf::RenderWindow &window)
        : _window(window),
          _view(sf::FloatRect(0, 0, 320, 180)),
          _isPressed(false)
    {
    }

    virtual void run(sf::Event event)
    {
        switch (event.type)
        {
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Button::Left)
            {
                _isPressed = true;
                _pos = sf::Mouse::getPosition();
            }
            break;
        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Button::Left)
            {
                _isPressed = false;
            }
            break;
        case sf::Event::MouseMoved:
        {
            if (!_isPressed)
                break;
            auto pos2 = sf::Mouse::getPosition(_window);
            auto delta = pos2 - _pos;
            if (abs(delta.x) < 100)
            {
                _view.move(-delta.x, -delta.y);
            }
            _window.setView(_view);
            _pos = pos2;
        }
        break;
        default:
            break;
        }
    }

  private:
    sf::RenderWindow &_window;
    sf::View _view;
    bool _isPressed;
    sf::Vector2i _pos = sf::Mouse::getPosition();
};

class GGEngine;

class EngineShortcutsInputEventHandler : public InputEventHandler
{
  public:
    EngineShortcutsInputEventHandler(gg::GGEngine &engine, sf::RenderWindow &window)
        : _engine(engine), _window(window)
    {
    }

    virtual void run(sf::Event event)
    {
        switch (event.type)
        {
        case sf::Event::KeyPressed:
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            {
                _engine.getRoom().showDrawWalkboxes(!_engine.getRoom().areDrawWalkboxesVisible());
                break;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O))
            {
                _engine.getRoom().showObjects(!_engine.getRoom().areObjectsVisible());
                break;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::L))
            {
                _engine.getRoom().showLayers(!_engine.getRoom().areLayersVisible());
                break;
            }
            break;
        case sf::Event::MouseMoved:
        {
            auto pos = sf::Mouse::getPosition(_window);
            auto worldPos = (sf::Vector2i)_window.mapPixelToCoords(pos);
            auto &objects = _engine.getRoom().getObjects();
            for (auto &obj : objects)
            {
                auto rect = obj.get()->getRealHotspot();
                obj.get()->setHotspotVisible(rect.contains(worldPos));
            }
        }
        break;
        default:
            break;
        }
    }

  private:
    gg::GGEngine &_engine;
    sf::RenderWindow &_window;
};
} // namespace gg
