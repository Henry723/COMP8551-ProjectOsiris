#pragma once
#include "../headers.h"
#include "../SceneManager/SceneManager.h"
#include "../UI/UI.h"
#include "../Events/EventSystem.h"
#include <entityx/Event.h>


struct SceneMgrSystem : public System<SceneMgrSystem>, public Receiver<SceneMgrSystem>, EntityX
{
    static SceneMgrSystem& instance()
    {
        static SceneMgrSystem instance;
        return instance;
    }

public:
    static SceneMgrSystem& getInstance()
    {
        static SceneMgrSystem instance;
        return instance;
    }

    void update(EntityManager&, EventManager&, TimeDelta) override; 
    void configure(EventManager& em) override;  // Subscribes to each input event
    void receive(const ControlInput& event);    // Used to pickup menu keys
    void receive(const GameOver& event);

    enum class TSndEvt { GAMEWON };
    void sendEvent(TSndEvt toSend);

private:
    SceneManager& scmgr = SceneManager::getInstance();
    bool X_pressed = false;
    bool Y_pressed = false;
    bool SPC_pressed = false;
    bool GameOver_rcd = false;
    bool GameWon_rcd = false;
};