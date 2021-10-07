#pragma once
#include "../components.hpp"
#include "../SceneManager.hpp"

class InputSystem : public System<InputSystem>
{
public:
  void update(EntityManager&, EventManager&, TimeDelta) override;
};
