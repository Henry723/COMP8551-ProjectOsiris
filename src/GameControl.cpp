#include "GameControl.h"

// TEMP - Handled by Asset System?
const char* modelSource = "./Models/Wayfarer.obj";
const char* vertSource = "./Shaders/Default.vert";
const char* fragSource = "./Shaders/Default.frag";
const char* texSource = "./Textures/container.jpg";

GameControl::GameControl(GLFWwindow* window, string filename)
{
  // Create systems
  systems.add<InputSystem>();
  systems.add<RenderSystem>();
  systems.configure();

  // TODO make it easier to create entities
  // Create entities
  entityx::Entity entity = entities.create();
  entity.assign<Window>(window);
  entity.assign<Color>(0.2f, 0.3f, 0.3f, 1.0f);

  // TEST - Create a 3D model entity for the RenderSystem to use
  entityx::Entity playerEntity = entities.create();
  playerEntity.assign<Model3D>(modelSource, vertSource, fragSource, texSource);
}

void GameControl::Update(TimeDelta dt)
{
  systems.update<InputSystem>(dt);
  systems.update<RenderSystem>(dt);
}
