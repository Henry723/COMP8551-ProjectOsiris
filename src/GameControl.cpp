#include "GameControl.h"
#include "gamestate.h"

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
    gameState = PREPARING;
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

  // Create UI entities
  entityx::Entity menuEntity = entities.create();
  menuEntity.assign<Window>(window);

  // Create a 3D model entity for the RenderSystem to use
  // Player
  /*
  entityx::Entity playerEntity = entities.create();
  playerEntity.assign<Model3D>(src_playerModel, vertSource, fragSource, src_playerTexture);
  playerEntity.assign<Transform>(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec4(0, 1.0, 0, 0), glm::vec3(1.0f));
  vector<Collider> playerColliders;
  playerColliders.push_back(Collider(Collider::Shape::CIRCLE, glm::vec2(0, 0), false, 0.5));
  playerColliders.push_back(Collider(Collider::Shape::CIRCLE, glm::vec2(2, 0), false, 0.5,"right"));
  playerColliders.push_back(Collider(Collider::Shape::CIRCLE, glm::vec2(-2, 0), false, 0.5, "left"));
  playerColliders.push_back(Collider(Collider::Shape::CIRCLE, glm::vec2(0, 2), false, 0.5, "bottom"));
  playerColliders.push_back(Collider(Collider::Shape::CIRCLE, glm::vec2(0, -2), false, 0.5, "top"));
  playerEntity.assign<Rigidbody>(playerColliders, Rigidbody::ColliderType::PLAYER, glm::vec2(0.0f, 0.0f));
  playerEntity.assign<GameObject>("player");
  */

  // Enemy
  entityx::Entity enemyEntity = entities.create();
  enemyEntity.assign<Model3D>(src_enemyModel, vertSource, fragSource, src_enemyTexture);
  enemyEntity.assign<Transform>(glm::vec3(-2.0f, -2.0f, 0), glm::vec4(0.0, 1.0, 0, -90), glm::vec3(1.0f));
  enemyEntity.assign<Rigidbody>(vector<Collider>{Collider(Collider::Shape::CIRCLE, glm::vec2(0, 0), true, 0.5) }, Rigidbody::ColliderType::ENEMY, glm::vec2(-2.0f, 0.0f));
  enemyEntity.assign<GameObject>("enemy");
  //
  entityx::Entity enemyEntity2 = entities.create();
  enemyEntity2.assign<Model3D>(src_enemyModel, vertSource, fragSource, src_enemyTexture);
  enemyEntity2.assign<Transform>(glm::vec3(-2.0f, -2.0f, -2.0f), glm::vec4(0.0, 1.0, 0, -90), glm::vec3(1.0f));
  enemyEntity2.assign<Rigidbody>(vector<Collider>{Collider(Collider::Shape::CIRCLE, glm::vec2(0, 0), true, 0.5) }, Rigidbody::ColliderType::ENEMY, glm::vec2(-2.0f, -2.0f));
  enemyEntity2.assign<GameObject>("enemy");
  //
  entityx::Entity enemyEntity3 = entities.create();
  enemyEntity3.assign<Model3D>(src_enemyModel, vertSource, fragSource, src_enemyTexture);
  enemyEntity3.assign<Transform>(glm::vec3(-2.0f, -2.0f, 2.0f), glm::vec4(0.0, 1.0, 0, -90), glm::vec3(1.0f));
  enemyEntity3.assign<Rigidbody>(vector<Collider>{Collider(Collider::Shape::CIRCLE, glm::vec2(0, 0), true, 0.5) }, Rigidbody::ColliderType::ENEMY, glm::vec2(-2.0f, 2.0f));
  enemyEntity3.assign<GameObject>("enemy");
  //
  entityx::Entity enemyEntity4 = entities.create();
  enemyEntity4.assign<Model3D>(src_enemyModel, vertSource, fragSource, src_enemyTexture);
  enemyEntity4.assign<Transform>(glm::vec3(4.0f, -2.0f, 0), glm::vec4(0.0, 1.0, 0, -90), glm::vec3(1.0f));
  enemyEntity4.assign<Rigidbody>(vector<Collider>{Collider(Collider::Shape::CIRCLE, glm::vec2(0, 0), true, 0.5) }, Rigidbody::ColliderType::ENEMY, glm::vec2(4.0f, 0.0f));
  enemyEntity4.assign<GameObject>("enemy");

  // Treasure Pile
  entityx::Entity treasureEntity = entities.create();
  treasureEntity.assign<Model3D>(src_treasureModel, vertSource, fragSource, src_treasureTexture);
  treasureEntity.assign<Transform>(glm::vec3(2.0f, -2.0f, 0), glm::vec4(0.0, 1.0, 0, 0), glm::vec3(1.0f));
  treasureEntity.assign<Rigidbody>(vector<Collider>{Collider(Collider::Shape::CIRCLE, glm::vec2(0, 0), true, 0.5) }, Rigidbody::ColliderType::COLLECTIBLE, glm::vec2(2.0f, 0.0f));
  treasureEntity.assign<GameObject>("treasure");
  //
  entityx::Entity treasureEntity2 = entities.create();
  treasureEntity2.assign<Model3D>(src_treasureModel, vertSource, fragSource, src_treasureTexture);
  treasureEntity2.assign<Transform>(glm::vec3(0.0f, -2.0f, 2.0f), glm::vec4(0.0, 1.0, 0, 0), glm::vec3(1.0f));
  treasureEntity2.assign<Rigidbody>(vector<Collider>{Collider(Collider::Shape::CIRCLE, glm::vec2(0, 0), true, 0.5) }, Rigidbody::ColliderType::COLLECTIBLE, glm::vec2(0.0f, 2.0f));
  treasureEntity2.assign<GameObject>("treasure");
  //
  entityx::Entity treasureEntity3 = entities.create();
  treasureEntity3.assign<Model3D>(src_treasureModel, vertSource, fragSource, src_treasureTexture);
  treasureEntity3.assign<Transform>(glm::vec3(2.0f, -2.0f, 2.0f), glm::vec4(0.0, 1.0, 0, 0), glm::vec3(1.0f));
  treasureEntity3.assign<Rigidbody>(vector<Collider>{Collider(Collider::Shape::CIRCLE, glm::vec2(0, 0), true, 0.5) }, Rigidbody::ColliderType::COLLECTIBLE, glm::vec2(2.0f, 2.0f));
  treasureEntity3.assign<GameObject>("treasure");

  // Key
  //entityx::Entity keyEntity = entities.create();
  //keyEntity.assign<Model3D>(src_keyModel, vertSource, fragSource, src_keyTexture);
  //keyEntity.assign<Transform>(glm::vec3(0.0f, 0.5f, 0), glm::vec4(0.0, 1.0, 0, 0), glm::vec3(1.0f));

  //Generate floor tiles
  int mapMax = 3;
  int mapMin = -2;
  for (int i = mapMin; i < mapMax; i++)
  {
      for (int j = mapMin; j < mapMax; j++)
      {
          entityx::Entity floorTile1Entity = entities.create();
          const char* src_Model = src_floorTile1Model;
          const char* src_Texture = src_floorTile1Texture;
          if ((i + j) % 3 == 1) src_Model = src_floorTile2Model;
          if ((i + j) % 3 == 1) src_Texture = src_floorTile2Texture;
          if ((i + j) % 3 == 2) src_Model = src_floorTile3Model;
          if ((i + j) % 3 == 2) src_Texture = src_floorTile3Texture;
          floorTile1Entity.assign<Model3D>(src_Model, vertSource, fragSource, src_Texture);
          floorTile1Entity.assign<Transform>(glm::vec3(i * 2, -2.0f, j * 2), glm::vec4(0.0, 1.0, 0, 0), glm::vec3(1.05f, 0.9f, 1.05f));
      }
  }

  entityx::Entity cameraEntity = entities.create();
  //change the YAW and PITCH here in the 3rd and 4th argument
  cameraEntity.assign<Camera>(glm::vec3(0.0f, 0.0f, -6.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -80.0f);
  //change the camera's position here in the first argument
  cameraEntity.assign<Transform>(glm::vec3(0, -8.0f, -2.0f) * -1.0f, glm::vec4(1.0f), glm::vec3(1.0f));

  //Load up game objects from configuration file. Currently loading player.
  CCfgMgrPhysical txml("./src/Game.xml");
  txml.LoadObjects(entities);
}

void GameControl::Update(TimeDelta dt)
{
    //if (GameMode == ModePreparing) {
    //    systems.update<InputSystem>(dt);
    //    systems.update<UISystem>(dt); // Currently disabled as rendering UI within the UI System was causing issues
    //    systems.update<ExampleEmitterSystem>(dt);
    //}
    //if (GameMode == ModeRunning) {
        systems.update<InputSystem>(dt);
        systems.update<RenderSystem>(dt);
        systems.update<UISystem>(dt); // Currently disabled as rendering UI within the UI System was causing issues
        systems.update<PhysicsEngine>(dt);
        systems.update<PhysicsTest>(dt);
        systems.update<ExampleEmitterSystem>(dt);
    //}
  
    //if (GameMode == ModeMenu) {
    //}
}
