#include "RenderSystem.h"

void RenderSystem::update(EntityManager& es, EventManager& ev, TimeDelta dt)
{
	// Create component handles to filter components
	ComponentHandle<Color> hcolor;
	ComponentHandle<Window> hwindow;
	ComponentHandle<Model3D> hmodel;
	ComponentHandle<Transform> htransform;
	ComponentHandle<Camera> hCamera;

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe Rendering
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Fill Rendering
	glEnable(GL_DEPTH_TEST);


	//the main camera
	Camera* camera;

	//loop through camera entities. This only works with one camera.
	auto cameraEntities = es.entities_with_components(hCamera, htransform);
	for (auto entity = cameraEntities.begin(); entity != cameraEntities.end(); ++entity)
	{
		camera = (*entity).component<Camera>().get();
		Transform* transform = (*entity).component<Transform>().get();
		camera->Position = transform->position;
	}
	
	// Loop through window components (there will likely only be one.)
	auto en = es.entities_with_components(hwindow, hcolor);
	for (auto entity = en.begin(); entity != en.end(); ++entity)
	{
		Color *col = (*entity).component<Color>().get();
		// rendering commands`
		// ...
		glClearColor(col->red, col->green, col->blue, col->alpha); // state-setting function of OpenGL
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // state-using function. Uses the current state defined to retrieve the clearing color.
	}

	// Loop through Model3D components
	auto modelEntities = es.entities_with_components(hmodel, htransform);
	for (auto entity = modelEntities.begin(); entity != modelEntities.end(); ++entity) {
		Model3D* model = (*entity).component<Model3D>().get();
		Transform* transform = (*entity).component<Transform>().get();
		model->translate(transform->position);
		model->scale(transform->scale);
		model->rotate(glm::vec3(transform->rotation.r, transform->rotation.g, transform->rotation.b), transform->rotation.a);
		
		//cout << "Drawing model " << model.name << endl;
		//model->Draw();
		draw(model, camera);	
	}

	// This is broken up, unfortunately, since the swapBuffers call must be after the Draw Call.
	for (auto entity = en.begin(); entity != en.end(); ++entity)
	{
		//update color buffer (a 2D buffer that contains color values for each pixel) to render during this iteration and show it as output to the screen.
		glfwSwapBuffers((*entity).component<Window>().get()->window);
	}
}

void RenderSystem::draw(Model3D* modelComponent, Camera* cameraComponent)
{
	// TEST - Changing uniforms over time.
	float timeValue = glfwGetTime();
	float green = (sin(timeValue) / 2.0f) + 0.5f;

	int vertColorLocation = glGetUniformLocation(modelComponent->shader_program.ID, "ourColor"); // Get the location of the uniform

	// ..:: Drawing code (called in render loop) :: ..
	//		This is called FOR EACH object we want to draw this frame.
	// 1. Choose the shader to use
	modelComponent->shader_program.use();

	//Camera
	//sets the camera location which calculates the camera position relative to the models
	modelComponent->shader_program.setVec3("positionOffset", cameraComponent->Position.x, cameraComponent->Position.y, cameraComponent->Position.z);

	//Default ambient lights
	//sets the ambient light value in shader (r, g, b)
	modelComponent->shader_program.setVec3("light.ambient", 0.3f, 0.2f, 0.2f);

	// Default diffuse lights
	//sets the position of the diffuse lighting
	modelComponent->shader_program.setVec3("light.position", 0.0f, 0.0f, 1.0f);
	//sets the diffuse light value in shader (r, g, b)
	modelComponent->shader_program.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
	//sets the specular light value of light in shader (r, g, b)
	modelComponent->shader_program.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	// Default material properties
	//sets the specular light value of the material in shader (r, g, b)
	modelComponent->shader_program.setVec3("material.specular", 1.0f, 1.0f, 1.0f);
	//sets the material shininess value in shader, the higher the value the less "shiny" it is.
	modelComponent->shader_program.setFloat("material.shininess", 10.0f);

	//create perspective matrix and set it in shader
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);
	modelComponent->shader_program.setMat4("projection", projection);

	//get view matrix from camera and set it in shader
	glm::mat4 view = cameraComponent->GetViewMatrix();
	modelComponent->shader_program.setMat4("view", view);

	//get model matrix and set in shader
	glm::mat4 model = modelComponent->getModelMatrix();
	modelComponent->shader_program.setMat4("model", model);

	// Now we have the location, we can set the shaders uniform globally.
	// This must be done AFTER "using" the program.
	glUniform4f(vertColorLocation, 0.0f, green, 0.0f, 1.0f);

	// 2. Bind the VAO of the object we want to draw.
	glBindVertexArray(modelComponent->vao);

	// 3. Bind the texture to the object
	glBindTexture(GL_TEXTURE_2D, modelComponent->texture);

	// 4. Draw the object.
	//		Use DrawArrays for ordered, and DrawElements for indexed.
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawElements(GL_TRIANGLES, modelComponent->numIndices, GL_UNSIGNED_INT, 0);
	// 5. Unbind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	modelComponent->resetModelMatrix();
}
