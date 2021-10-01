#pragma once
#include "../components.hpp"

class InputSystem : public System<InputSystem>
{
public:
  void update(EntityManager&, EventManager&, TimeDelta) override;
};
