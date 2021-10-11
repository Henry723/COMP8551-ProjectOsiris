#include "InputSystem.h"

// Set up the 'groups' of keys using the keycodes
std::vector<int> movementKeys = { 87, 65, 83, 68 }; // W A S D
std::vector<int> attackKeys = { 263, 265, 262, 264 }; // Left Up Right Down arrow keys

// Set up variables to assign the type of action taken
bool isMovement = false;
bool isAttack = false;

void InputSystem::update(EntityManager& es, EventManager& ev, TimeDelta dt)
{
  // Create component handles to filter components
  ComponentHandle<Window> hwindow;

  // Loop through components
  auto en = es.entities_with_components(hwindow);
  for (auto entity = en.begin(); entity != en.end(); ++entity)
  {
    Window* window = (*entity).component<Window>().get();
    glfwSetKeyCallback(window->window, keyCallback); // set the function to be run when a keyboard event occurs

    // If Escape key is pressed, send the "Close Window" signal to GLFW and break the Render Loop.
    if (glfwGetKey(window->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // GLFW_RELEASE is the opposite of GLFW_PRESS, like KeyUp and KeyDown
      glfwSetWindowShouldClose(window->window, true);

    // Test for scene manager
    if (glfwGetKey(window->window, GLFW_KEY_1) == GLFW_PRESS)
      SceneManager::getInstance().setScene("Scene 1");
    else if (glfwGetKey(window->window, GLFW_KEY_2) == GLFW_PRESS)
      SceneManager::getInstance().setScene("Scene 2");
    else if (glfwGetKey(window->window, GLFW_KEY_3) == GLFW_PRESS)
      SceneManager::getInstance().setScene("Scene 3");

    // checks if any events are triggered, updates the window state, and calls the corresponding functions (which we can register via callback methods)
    glfwPollEvents();
  }
}

void InputSystem::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) // function is called whenever a keyboard input is detected
{
    // reset values to false at function start
    isMovement = false;
    isAttack = false;

    // Identify which type of action was taken
    if (std::find(movementKeys.begin(), movementKeys.end(), key) != movementKeys.end()) {
        isMovement = true;
    }

    if (std::find(attackKeys.begin(), attackKeys.end(), key) != attackKeys.end()) {
        isAttack = true;
    }

    // Determine what to do based on the type of action
    if (!isMovement && !isAttack) { // currently only care about movement and attack types (will add more as required)
        return;
    }
    else {
        switch (action)
        {
        case GLFW_PRESS: // decide what to do when the key is pressed
            cout << (isMovement ? "Movement: " : "") << (isAttack ? "Attack: " : "") << key << " pressed" << endl;
            break;
        case GLFW_RELEASE: // decide what to do when the key is released
            cout << (isMovement ? "Movement: " : "") << (isAttack ? "Attack: " : "") << key << " released" << endl;
        }
    }
}
