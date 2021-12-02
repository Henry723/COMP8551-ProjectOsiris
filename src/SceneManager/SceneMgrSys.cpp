#include "SceneMgrSys.h"
#include "SceneManager.h"

#include "../gamestate.h"

void SceneMgrSystem::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
    if (X_pressed)
    {
        // Forward to the scene manager
        X_pressed = false;
        scmgr.keyPressNotification(SceneManager::TKeyPress::START);
    }
 
    if (Y_pressed)
    {
        // Forward to the scene manager
        Y_pressed = false;
        scmgr.keyPressNotification(SceneManager::TKeyPress::MENU);
    }
    
    if (SPC_pressed)
    {
        // Forward to the scene manager
        SPC_pressed = false;
        scmgr.keyPressNotification(SceneManager::TKeyPress::RESTART);
    }
}

// Subscribes to each input event
void SceneMgrSystem::configure(EventManager& em)
{
    em.subscribe<ControlInput>(*this);
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

