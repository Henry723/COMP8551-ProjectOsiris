#pragma once
#include "../components.hpp"
#include "Camera.h"
#include <glm/glm.hpp>
#include "../UI/UI.h"
#include "../Events/EventSystem.h"
#include "../gamestate.h"

class RenderSystem : public System<RenderSystem>, public Receiver<RenderSystem>
{
public:
	//Camera mainCamera(glm::vec3(0.0f, 0.0f, 3.0f));
	void update(EntityManager&, EventManager&, TimeDelta) override;
	void configure(EventManager& em) override;
	void receive(const ScoreUpdate& event);
	void receive(const PlayerHealthUpdate& event);
	void receive(const TimerUpdate& event);

	~RenderSystem();

private:
  void draw(Model3D* modelComponent, Camera* cameraComponent);
  void updatePlayerLight(PointLight* pointLightComponent, Transform* transformComponent);
  void updateTreasureLight(PointLight* pointLightComponent, Transform* transformComponent);
  void updateKeyLight(PointLight* pointLightComponent, Transform* transformComponent);
  void updateDoorLight(PointLight* pointLightComponent, Transform* transformComponent);
  void resetCount();
  
  EntityManager* entityManager;
  int numPointLights, numPlayer, numTreasure, numKey, numDoor;
  PointLight *playerPointLight, *keyPointLight, *doorPointLight;
  vector<PointLight*> treasurePointLights;
};

