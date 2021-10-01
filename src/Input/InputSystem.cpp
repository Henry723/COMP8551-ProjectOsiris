#include "InputSystem.h"

void InputSystem::update(EntityManager& es, EventManager& ev, TimeDelta dt)
{
  // Create component handles to filter components
  ComponentHandle<Window> hwindow;

  // Loop through components
  auto en = es.entities_with_components(hwindow);
  for (auto entity = en.begin(); entity != en.end(); ++entity)
  {
    // If Escape key is pressed, send the "Close Window" signal to GLFW and break the Render Loop.
    if (glfwGetKey((*entity).component<Window>().get()->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // GLFW_RELEASE is the opposite of GLFW_PRESS, like KeyUp and KeyDown
      glfwSetWindowShouldClose((*entity).component<Window>().get()->window, true);

    // checks if any events are triggered, updates the window state, and calls the corresponding functions (which we can register via callback methods)
    glfwPollEvents();
  }
}
