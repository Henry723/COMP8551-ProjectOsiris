#pragma once
#include "../components.hpp"

class RenderSystem : public System<RenderSystem>
{
public:
  void update(EntityManager&, EventManager&, TimeDelta) override;

private:
  void draw(Model3D* modelComponent);
};

