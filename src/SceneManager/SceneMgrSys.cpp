#include "SceneMgrSys.h"
#include "SceneManager.h"

#include "../gamestate.h"

void SceneMgrSystem::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
    if (X_pressed)
    {
        // Forward to the scene manager
        X_pressed = false;
        scmgr.cmdNotification(SceneManager::TCmd::START);
    }
 
    if (Y_pressed)
    {
        // Forward to the scene manager
        Y_pressed = false;
        scmgr.cmdNotification(SceneManager::TCmd::MENU);
    }
    
    if (SPC_pressed)
    {
        // Forward to the scene manager
        SPC_pressed = false;
        scmgr.cmdNotification(SceneManager::TCmd::RESTART);
    }

    if (GameOver_rcd)
    {
        // Forward to the scene manager
        GameOver_rcd = false;
        scmgr.cmdNotification(SceneManager::TCmd::GAMEOVER);
    }
}

// Subscribes to each input event
void SceneMgrSystem::configure(EventManager& em)
{
    em.subscribe<ControlInput>(*this);
    em.subscribe<GameOver>(*this);
}

// Used to pickup menu keys
void SceneMgrSystem::receive(const ControlInput& event)
{
    ControlInput::Cmd cmd = event.cmd;
    switch (cmd) 
    {
    case ControlInput::X:
        X_pressed = true;
        break;
    case ControlInput::Y:
        Y_pressed = true;
        break;
    case ControlInput::SPACE:
        SPC_pressed = true;
        break;
    }
}

// Used to pickup menu keys
void SceneMgrSystem::receive(const GameOver& event)
{
    GameOver_rcd = true;
}