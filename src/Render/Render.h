#pragma once
#include "../components.hpp"

class Render : public System<Render>
{
public:
  void update(EntityManager&, EventManager&, TimeDelta) override;
};

