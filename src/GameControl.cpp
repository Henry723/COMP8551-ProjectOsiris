#include "GameControl.h"

// TEMP - These will likely be adapted into an asset system?
const char* src_playerModel = "./src/Renderer3D/Models/wayfarer.obj";
const char* src_playerTexture = "./src/Renderer3D/Textures/tex_wayfarer.jpg";

const char* src_enemyModel = "./src/Renderer3D/Models/scarab.obj";
const char* src_enemyTexture = "./src/Renderer3D/Textures/tex_scarab.jpg";

const char* src_keyModel = "./src/Renderer3D/Models/key_totem.obj";
const char* src_keyTexture = "./src/Renderer3D/Textures/tex_key_totem.jpg";

const char* src_treasureModel = "./src/Renderer3D/Models/treasure_pile.obj";
const char* src_treasureTexture = "./src/Renderer3D/Textures/tex_treasurePile.jpg";

const char* src_floorTile1Model = "./src/Renderer3D/Models/stone_tile_1.obj";
const char* src_floorTile1Texture = "./src/Renderer3D/Textures/tex_floorTile1.jpg";

const char* src_floorTile2Model = "./src/Renderer3D/Models/stone_tile_2.obj";
const char* src_floorTile2Texture = "./src/Renderer3D/Textures/tex_floorTile2.jpg";

const char* src_floorTile3Model = "./src/Renderer3D/Models/stone_tile_3.obj";
const char* src_floorTile3Texture = "./src/Renderer3D/Textures/tex_floorTile3.jpg";

const char* vertSource = "./src/Renderer3D/Shaders/Default.vert";
const char* fragSource = "./src/Renderer3D/Shaders/Default.frag";

GameControl::GameControl(GLFWwindow* window, string filename)
{
  // Create systems
  systems.add<InputSystem>();
  systems.add<RenderSystem>();
  systems.add<PhysicsEngine>();
  systems.add<PhysicsTest>();
  systems.add<UISystem>();
  systems.add<InputEventTester>();
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

  // Create a 3D model entity for the RenderSystem to use
  // Player
  entityx::Entity playerEntity = entities.create();
  playerEntity.assign<Model3D>(src_playerModel, vertSource, fragSource, src_playerTexture);
  playerEntity.assign<Transform>(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec4(0, 1.0, 0, 0), glm::vec3(1.0f));
  playerEntity.assign<Rigidbody>(Rigidbody::BodyShape::CIRCLE, Rigidbody::ColliderType::PLAYER, 0.5, glm::vec2(0.0f, 0.0f));
  playerEntity.assign<GameObject>("player");

  // Enemy
  entityx::Entity enemyEntity = entities.create();
  enemyEntity.assign<Model3D>(src_enemyModel, vertSource, fragSource, src_enemyTexture);
  enemyEntity.assign<Transform>(glm::vec3(-2.0f, -2.0f, 0), glm::vec4(0.0, 1.0, 0, -90), glm::vec3(1.0f));
  enemyEntity.assign<Rigidbody>(Rigidbody::BodyShape::CIRCLE, Rigidbody::ColliderType::ENEMY, 0.5, glm::vec2(-2.0f, 0.0f));
  enemyEntity.assign<GameObject>("enemy");

  // Treasure Pile
  entityx::Entity treasureEntity = entities.create();
  treasureEntity.assign<Model3D>(src_treasureModel, vertSource, fragSource, src_treasureTexture);
  treasureEntity.assign<Transform>(glm::vec3(2.0f, -2.0f, 0), glm::vec4(0.0, 1.0, 0, 0), glm::vec3(1.0f));
  treasureEntity.assign<Rigidbody>(Rigidbody::BodyShape::CIRCLE, Rigidbody::ColliderType::COLLECTIBLE, 0.5, glm::vec2(2.0f, 0.0f));
  treasureEntity.assign<GameObject>("treasure");

  // Key
  entityx::Entity keyEntity = entities.create();
  keyEntity.assign<Model3D>(src_keyModel, vertSource, fragSource, src_keyTexture);
  keyEntity.assign<Transform>(glm::vec3(0.0f, 0.5f, 0), glm::vec4(0.0, 1.0, 0, 0), glm::vec3(1.0f));

  // Floor tiles
  entityx::Entity floorTile1Entity = entities.create();
  floorTile1Entity.assign<Model3D>(src_floorTile1Model, vertSource, fragSource, src_floorTile1Texture);
  floorTile1Entity.assign<Transform>(glm::vec3(0.0f, -2.0f, 0), glm::vec4(0.0, 1.0, 0, 0), glm::vec3(1.0f, 1.0f, 1.0f));

  entityx::Entity floorTile2Entity = entities.create();
  floorTile2Entity.assign<Model3D>(src_floorTile2Model, vertSource, fragSource, src_floorTile2Texture);
  floorTile2Entity.assign<Transform>(glm::vec3(2.0f, -2.0f, 0), glm::vec4(0.0, 1.0, 0, 110), glm::vec3(1.0f, 1.0f, 1.0f));

  entityx::Entity floorTile3Entity = entities.create();
  floorTile3Entity.assign<Model3D>(src_floorTile3Model, vertSource, fragSource, src_floorTile3Texture);
  floorTile3Entity.assign<Transform>(glm::vec3(-2.0f, -2.0f, 0), glm::vec4(0.0, 1.0, 0, -110), glm::vec3(1.0f, 1.0f, 1.0f));

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
  //systems.update<UISystem>(dt); Currently disabled as rendering UI within the UI System was causing issues
  systems.update<PhysicsEngine>(dt);
  systems.update<PhysicsTest>(dt);
  systems.update<ExampleEmitterSystem>(dt);
  
}
