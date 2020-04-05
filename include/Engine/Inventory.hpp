#pragma once
#include <array>
#include "SFML/Graphics.hpp"
#include "Engine/ActorIconSlot.hpp"
#include "Verb.hpp"
#include "Graphics/SpriteSheet.hpp"

namespace ng {
class Engine;
class Object;
class Hud;

class Inventory : public sf::Drawable {
public:
  Inventory(std::array<ActorIconSlot, 6> &actorsIconSlots,
            Hud &hud,
            Actor *&pCurrentActor);

  void setEngine(Engine *pEngine);
  bool update(const sf::Time &elapsed);

  void setMousePosition(const sf::Vector2f &pos) { _mousePos = pos; }
  Object *getCurrentInventoryObject() { return _pCurrentInventoryObject; }
  sf::Vector2f getPosition(Object *pObject) const;

private:
  void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
  void drawUpArrow(sf::RenderTarget &target) const;
  void drawDownArrow(sf::RenderTarget &target) const;
  bool hasUpArrow() const;
  bool hasDownArrow() const;

private:
  int getCurrentActorIndex() const;

private:
  Engine *_pEngine{nullptr};
  std::array<ActorIconSlot, 6> &_actorsIconSlots;
  Hud &_hud;
  SpriteSheet _gameSheet, _inventoryItems;
  Actor *&_pCurrentActor;
  std::array<sf::IntRect, 8> _inventoryRects;
  Object *_pCurrentInventoryObject{nullptr};
  sf::Vector2f _mousePos;
  float _jiggleTime{0};
};
} // namespace ng
