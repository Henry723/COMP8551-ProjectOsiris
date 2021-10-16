#include "GameControl.h"

// TEMP - These will likely be adapted into an asset system?
//const char* modelSource = "./src/Renderer3D/Models/wayfarer.obj";
const char* modelSource = "./src/Renderer3D/Models/plane.obj";
const char* vertSource = "./src/Renderer3D/Shaders/Default.vert";
const char* fragSource = "./src/Renderer3D/Shaders/Default.frag";
const char* texSource = "./src/Renderer3D/Textures/container.jpg";

GameControl::GameControl(GLFWwindow* window, string filename)
{
  // Create systems
  systems.add<InputSystem>();
  systems.add<RenderSystem>();
  systems.add<PhysicsEngine>();
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

  // TODO make it easier to create entities
  // Create entities
  entityx::Entity entity = entities.create();
  entity.assign<Window>(window);
  entity.assign<Color>(0.2f, 0.3f, 0.3f, 1.0f);

  // TEST - Create a 3D model entity for the RenderSystem to use
  entityx::Entity playerEntity = entities.create();
  playerEntity.assign<Model3D>(modelSource, vertSource, fragSource, texSource);
  playerEntity.assign<Transform>(glm::vec3(1.0f), glm::vec4(1.0f), glm::vec3(1.0f));
  // Second Entity for reference
  entityx::Entity nemesisEntity = entities.create();
  nemesisEntity.assign<Model3D>(modelSource, vertSource, fragSource, texSource);
  nemesisEntity.assign<Transform>(glm::vec3(-3.0f, 0, 0), glm::vec4(0, 1.0, 0, 20), glm::vec3(3.0f));

  entityx::Entity cameraEntity = entities.create();
  //change the YAW and PITCH here in the 3rd and 4th argument
  cameraEntity.assign<Camera>(glm::vec3(0.0f, 0.0f, -6.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
  //change the camera's position here in the first argument
  cameraEntity.assign<Transform>(glm::vec3(0, 0.0f, -8.0f) * -1.0f, glm::vec4(1.0f), glm::vec3(1.0f));



  //TEST - Load up configuration file, run the test function, grab position data of specified object
  CCfgMgrPhysical txml("./src/Configuration/gameobjects.xml");
  txml.test();
  cout << txml.getAttribute("Cube 2", "position") << endl;
}

void GameControl::Update(TimeDelta dt)
{
  systems.update<InputSystem>(dt);
  systems.update<RenderSystem>(dt);
  systems.update<PhysicsEngine>(dt);
  systems.update<ExampleEmitterSystem>(dt);
  
}
