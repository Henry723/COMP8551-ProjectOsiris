#include "GameControl.h"

// TESTING -- Start --
#include "MemoryManager/CMMPoolTester.h"
// TESTING -- End --

GameControl::GameControl(GLFWwindow* window, string filename)
{
  // Create systems
  systems.add<InputSystem>();
  systems.add<RenderSystem>();
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
}

void GameControl::Update(TimeDelta dt)
{
  systems.update<InputSystem>(dt);
  systems.update<RenderSystem>(dt);
}
