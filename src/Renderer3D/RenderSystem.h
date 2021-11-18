#pragma once
#include "../components.hpp"
#include "Camera.h"
#include <glm/glm.hpp>
#include "../UI/UI.h"

#include "../gamestate.h"


class RenderSystem : public System<RenderSystem>
{
public:
  //Camera mainCamera(glm::vec3(0.0f, 0.0f, 3.0f));
  void update(EntityManager&, EventManager&, TimeDelta) override;
  

private:
  void draw(Model3D* modelComponent, Camera* cameraComponent);
};

