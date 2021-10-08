#include "GameControl.h"

GameControl::GameControl(GLFWwindow* window, string filename)
{
  // Create systems
  systems.add<InputSystem>();
  systems.add<RenderSystem>();
  systems.add<ExampleEmitterSystem>();
  systems.add<ExampleListenerSystem>();
  systems.configure();

  //Testing Scene Manager
  glfwSetWindowTitle(window, filename.c_str());

  // TODO make it easier to create entities
  // Create entities
  entityx::Entity entity = entities.create();
  entity.assign<Window>(window);
  entity.assign<Color>(0.2f, 0.3f, 0.3f, 1.0f);
}

void GameControl::Update(TimeDelta dt)
{
  //systems.update<InputSystem>(dt);
  //systems.update<RenderSystem>(dt);
  //systems.update<ExampleListenerSystem>(dt);
    systems.update_all(dt);
}
