#include "RenderSystem.h"

UISystem ui;

void RenderSystem::update(EntityManager& es, EventManager& ev, TimeDelta dt)
{
	// Create component handles to filter components
	ComponentHandle<Color> hcolor;
	ComponentHandle<Window> hwindow;
	ComponentHandle<Model3D> hmodel;

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe Rendering
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Fill Rendering

	// Loop through window components (there will likely only be one.)
	auto en = es.entities_with_components(hwindow, hcolor);
	for (auto entity = en.begin(); entity != en.end(); ++entity)
	{
		Color *col = (*entity).component<Color>().get();
		// rendering commands
		// ...
		glClearColor(col->red, col->green, col->blue, col->alpha); // state-setting function of OpenGL
		glClear(GL_COLOR_BUFFER_BIT); // state-using function. Uses the current state defined to retrieve the clearing color.
	}

	// Loop through Model3D components
	auto modelEntities = es.entities_with_components(hmodel);
	for (auto entity = modelEntities.begin(); entity != modelEntities.end(); ++entity) {
		Model3D* model = (*entity).component<Model3D>().get();

		//cout << "Drawing model " << model.name << endl;
		model->Draw();
	}

	// Text rendering using the UI System
	// Enable these properties so text can be properly rendered
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!ui.configured) { // Initialize FreeType and VAO/VBOs + adds text elements to be rendered
		ui.setup();
		ui.NewTextElement("text 1", 200.0f, 100.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f), true);
		ui.NewTextElement("text 2", 500.0f, 550.0f, 0.8f, glm::vec3(0.2, 0.7f, 0.9f), true);
		ui.configured = true;
	}

	ui.RenderAll();

	// This is broken up, unfortunately, since the swapBuffers call must be after the Draw Call.
	for (auto entity = en.begin(); entity != en.end(); ++entity)
	{
		// Disable these properties once finished rendering text so models can be properly rendered
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		//update color buffer (a 2D buffer that contains color values for each pixel) to render during this iteration and show it as output to the screen.
		glfwSwapBuffers((*entity).component<Window>().get()->window);
	}
}
