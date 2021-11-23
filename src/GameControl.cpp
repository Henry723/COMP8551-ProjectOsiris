#include "GameControl.h"

// TEMP - These will likely be adapted into an asset system
const char* src_keyModel = "./src/Renderer3D/Models/key_totem.obj";
const char* src_keyTexture = "./src/Renderer3D/Textures/tex_key_totem.jpg";

const char* vertSource = "./src/Renderer3D/Shaders/Default.vert";
const char* fragSource = "./src/Renderer3D/Shaders/Default.frag";

GameControl::GameControl(GLFWwindow* window, string filename)
{
  gameState = PREPARING;
  // Create systems
  systems.add<InputSystem>();
  systems.add<RenderSystem>();
  systems.add<PhysicsEngine>();
  systems.add<PhysicsTest>();
  systems.add<EnemySystem>();
  systems.add<TurnEvents>();
  systems.add<ScoreTest>();
  systems.add<UISystem>();
  systems.add<InputEventTester>();
  systems.add<AudioSystem>();
  systems.add<ExampleEmitterSystem>();
  systems.add<ExampleListenerSystem>();
  systems.configure();

  //Testing Scene Manager
  glfwSetWindowTitle(window, filename.c_str());

  // TEST -- Memory Manager Pool Test -- Start --
  {
      CMMPoolTester class_test;
      class_test.run_test(CMMPoolTester::report::verbose);
  }
  // TESTING -- Memory Manager Pool Test -- End --

  // TEST -- Memory Manager Pool Test -- Start --
  {
      CCfgMgrAppTester class_test;
      class_test.run_test(CCfgMgrAppTester::report::verbose);
  }
  // TESTING -- Memory Manager Pool Test -- End --

  // TODO make it easier to create entities
  // Create entities
  entityx::Entity entity = entities.create();
  entity.assign<Window>(window);
  entity.assign<Color>(0.2f, 0.3f, 0.3f, 1.0f);

  entityx::Entity cameraEntity = entities.create();
  //change the YAW and PITCH here in the 3rd and 4th argument
  cameraEntity.assign<Camera>(glm::vec3(0.0f, 0.0f, -6.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -80.0f);
  //change the camera's position here in the first argument
  cameraEntity.assign<Transform>(glm::vec3(0, -8.0f, -2.0f) * -1.0f, glm::vec4(1.0f), glm::vec3(1.0f));

  //CCfgMgrApplication cfgManager = CCfgMgrApplication();
  //cfgManager.loadConfig("./src/Game.xml", entities);

  CCfgMgrApplication cfgManager_level = CCfgMgrApplication();
  cfgManager_level.loadLevel("./src/Level01.xml", entities);
}

void GameControl::Update(TimeDelta dt)
{
  systems.update<InputSystem>(dt);
  systems.update<UISystem>(dt); //Currently disabled as rendering UI within the UI System was causing issues
  systems.update<PhysicsEngine>(dt);
  systems.update<PhysicsTest>(dt);
  systems.update<ScoreTest>(dt);
  systems.update<ExampleEmitterSystem>(dt);
  systems.update<TurnEvents>(dt);
  systems.update<EnemySystem>(dt);
  systems.update<RenderSystem>(dt);

}
