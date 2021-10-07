#include "InputSystem.h"

void InputSystem::update(EntityManager& es, EventManager& ev, TimeDelta dt)
{
  // Create component handles to filter components
  ComponentHandle<Window> hwindow;

  // Loop through components
  auto en = es.entities_with_components(hwindow);
  for (auto entity = en.begin(); entity != en.end(); ++entity)
  {
    Window* window = (*entity).component<Window>().get();
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
