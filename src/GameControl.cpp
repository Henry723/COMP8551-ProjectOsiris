#include "GameControl.h"

// TESTING -- Start --
#include "MemoryManager/CMMPoolTester.h"
// TESTING -- End --

// TEMP - These will likely be adapted into an asset system?
const char* modelSource = "./src/Renderer3D/Models/wayfarer.obj";
const char* vertSource = "./src/Renderer3D/Shaders/Default.vert";
const char* fragSource = "./src/Renderer3D/Shaders/Default.frag";
const char* texSource = "./src/Renderer3D/Textures/container.jpg";

GameControl::GameControl(GLFWwindow* window, string filename)
{
  // Create systems
  systems.add<InputSystem>();
  systems.add<RenderSystem>();
  systems.add<PhysicsEngine>();
  systems.add<UISystem>();
  systems.configure();

  //Testing Scene Manager
  glfwSetWindowTitle(window, filename.c_str());

  // TESTING -- Start --
  {
      CMMPoolTester class_test;
      class_test.run_test();
      int fail_count = class_test.failed();
      int pass_count = class_test.passed();
      bool CMMPoolTest_success = class_test.tests_all_passed();
  }
  // TESTING -- End --

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
  //systems.update<UISystem>(dt); Currently disabled as rendering UI within the UI System was causing issues
  systems.update<PhysicsEngine>(dt);
}
