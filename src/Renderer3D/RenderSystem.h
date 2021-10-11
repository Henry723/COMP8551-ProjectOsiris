#pragma once
#include "../components.hpp"
#include "../UI/UI.h"

class RenderSystem : public System<RenderSystem>
{
public:
  void update(EntityManager&, EventManager&, TimeDelta) override;
};

