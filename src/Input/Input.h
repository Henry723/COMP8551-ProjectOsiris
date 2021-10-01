#pragma once
#include "../components.hpp"

class Input : public System<Input>
{
public:
  void update(EntityManager&, EventManager&, TimeDelta) override;
};
