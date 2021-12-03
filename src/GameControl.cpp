#include "GameControl.h"

//#define RUN_TESTING


GameControl::GameControl(GLFWwindow* window, string filename)
{ 
#ifdef RUN_TESTING
    // TESTING -- Memory Manager Pool Test -- Start --
    {
        CMMPoolTester class_test;
        class_test.run_test(CMMPoolTester::report::verbose);
    }
    // TESTING -- Memory Manager Pool Test -- End --

    // TESTING -- Configuration Manager Custom Test -- Start --
    {
        CCfgMgrAppTester class_test;
        class_test.run_test(CCfgMgrAppTester::report::verbose);
    }
    // TESTING -- Configuration Manager Custom -- End --
#endif

	// Create systems
	systems.add<InputSystem>();
	systems.add<RenderSystem>();
	systems.add<PhysicsEngine>();
	//systems.add<PhysicsTest>();
	systems.add<PlayerSystem>();
	systems.add<EnemySystem>();
	systems.add<TurnEvents>();
	systems.add<ScoreTest>();
	systems.add<UISystem>();
	systems.add<AudioSystem>();
	systems.add<SceneMgrSystem>();
	systems.add<AnimationSystem>();

	systems.configure();

    // Now that systems are added use the scene manager to help loading appropriate scene
    SceneManager& scene = SceneManager::getInstance();

    // Set the main window to the new scene name
    glfwSetWindowTitle(window, scene.getSceneName().c_str());

	// Create entities
	entityx::Entity entity = entities.create();
	entity.assign<Window>(window);
	entity.assign<Color>(0.0f, 0.0f, 0.0f, 1.0f);

    entityx::Entity cameraEntity = entities.create();
    //change the YAW and PITCH here in the 3rd and 4th argument
    cameraEntity.assign<Camera>(glm::vec3(0.0f, 0.0f, -6.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -80.0f);
    //change the camera's position here in the first argument
    cameraEntity.assign<Transform>(glm::vec3(0, -8.0f, -2.0f) * -1.0f, glm::vec4(1.0f), glm::vec3(1.0f));

    string lfName = scene.getSceneLoadFile();
    if (lfName != "")
    {
        CCfgMgrApplication cfgManager_level = CCfgMgrApplication();
        cfgManager_level.loadLevel(lfName.c_str(), entities);

        // Everything is loaded for entities so switch to run
        gameState = GameState::RUNNING;
    }
}

void GameControl::Update(TimeDelta dt)
{

	systems.update<InputSystem>(dt);
	systems.update<UISystem>(dt); //Currently disabled as rendering UI within the UI System was causing issues
	systems.update<PhysicsEngine>(dt);
	systems.update<PlayerSystem>(dt);
	//systems.update<PhysicsTest>(dt);
	systems.update<ScoreTest>(dt);
	systems.update<TurnEvents>(dt);
	systems.update<EnemySystem>(dt);
	systems.update<AnimationSystem>(dt);
	systems.update<RenderSystem>(dt);
	systems.update<AudioSystem>(dt);
	systems.update<SceneMgrSystem>(dt);
}
