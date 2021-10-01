#include "GameControl.h"

GameControl::GameControl(GLFWwindow* window, string filename)
{
  // Create systems
  systems.add<Input>();
  systems.add<Render>();
  systems.configure();

  // TODO make it easier to create entities
  // Create entities
  entityx::Entity entity = entities.create();
  entity.assign<Window>(window);
  entity.assign<Color>(0.2f, 0.3f, 0.3f, 1.0f);
}

void GameControl::Update(TimeDelta dt)
{
  systems.update<Input>(dt);
  systems.update<Render>(dt);
}
