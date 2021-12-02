#include "RenderSystem.h"

UISystem& ui = UISystem::getInstance(); // Reference the UISystem instance (ensure the name is unique) 
int healthText, scoreText; // Create int IDs for each of the text elements you want to render
int totalScore, playerHealth;

void RenderSystem::configure(EventManager& em) {
	em.subscribe<ScoreUpdate>(*this);
	em.subscribe<PlayerHealthUpdate>(*this);
}

void RenderSystem::update(EntityManager& es, EventManager& ev, TimeDelta dt)
{
	if (gameState == PREPARING) {
		return;
	}
	// Create component handles to filter components
	ComponentHandle<Color> hcolor;
	ComponentHandle<Window> hwindow;
	ComponentHandle<Model3D> hmodel;
	ComponentHandle<Transform> htransform;
	ComponentHandle<Camera> hCamera;
	ComponentHandle<Rigidbody> hRigidBody;

	// Needed to write player health to ui
	ComponentHandle<Health> playerHealth;

	// Handle for getting point light component within game object that has it.
	ComponentHandle<PointLight> hPointLight;

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe Rendering
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Fill Rendering
	glEnable(GL_DEPTH_TEST);
	glm::vec2 playerPosition;
	//get player position to update camera position
	for (Entity entity : es.entities_with_components(hRigidBody)) {
		ComponentHandle<GameObject> object = entity.component<GameObject>();
		ComponentHandle<Rigidbody> rigidbody = entity.component<Rigidbody>();
		if (object && object->name == "player") {
			playerPosition = rigidbody->GetPosition();
			playerHealth = entity.component<Health>();
		}
	}

	//the main camera
	Camera* camera = &Camera();

	//loop through camera entities. This only works with one camera.
	auto cameraEntities = es.entities_with_components(hCamera, htransform);
	for (auto entity = cameraEntities.begin(); entity != cameraEntities.end(); ++entity)
	{
		camera = (*entity).component<Camera>().get();
		Transform* transform = (*entity).component<Transform>().get();
		camera->Position = transform->position;
		camera->Position = glm::vec3(playerPosition.x, camera->Position.y, playerPosition.y + 2.0);
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

	auto lightEntities = es.entities_with_components(htransform, hPointLight);
	for (auto entity = lightEntities.begin(); entity != lightEntities.end(); ++entity) {
		// update light information functions
		PointLight* pointLight = (*entity).component<PointLight>().get();
		Transform* transform = (*entity).component<Transform>().get();
		ComponentHandle<GameObject> object = (*entity).component<GameObject>();
		if (object && object->name == "player") {
			//Update lights
			updatePlayerLight(pointLight, transform);
			numPointLights++;
		}
		if (object && object->name == "treasure") {
			//Update all light position and stuff when it knows it is treasure
			updateTreasureLight(pointLight, transform);
			numPointLights++;
		}
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

	// Text rendering using the UI System
	// Enable these properties so text can be properly rendered
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!ui.configured) { // Initialize FreeType and VAO/VBOs + adds text elements to be rendered
		ui.setup();
		healthText = ui.NewTextElement("Health: " + to_string(playerHealth->curHealth) + '/' + to_string(playerHealth->maxHealth), 15.0f, 565.0f, 0.75f, glm::vec3(1.0, 1.0f, 1.0f), true);
		scoreText = ui.NewTextElement("Score: 0000", 585.0f, 565.0f, 0.75f, glm::vec3(1.0, 1.0f, 1.0f), true);
		ui.configured = true;
	}
	else {
		ui.textElements[scoreText].value = "Score: " + to_string(totalScore);
		ui.textElements[healthText].value = "Health: " + to_string(playerHealth->curHealth) + '/' + to_string(playerHealth->maxHealth);
		ui.RenderAll(); // Render all text elements which are set as active
		if (gameState == MENU)
			ui.RenderMenuText();
	}

	// This is broken up, unfortunately, since the swapBuffers call must be after the Draw Call.
	for (auto entity = en.begin(); entity != en.end(); ++entity)
	{
		// Disable these properties once finished rendering text so models can be properly rendered
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		//update color buffer (a 2D buffer that contains color values for each pixel) to render during this iteration and show it as output to the screen.
		glfwSwapBuffers((*entity).component<Window>().get()->window);
	}
	resetCount();
}

void RenderSystem::updatePlayerLight(PointLight* pointLightCompnent, Transform* transformComponent) {
	playerPointLight = pointLightCompnent;
	playerPointLight->position = transformComponent->position;
}

void RenderSystem::updateTreasureLight(PointLight* pointLightCompnent, Transform* transformComponent) {
	PointLight* tpl = pointLightCompnent;
	tpl->position = transformComponent->position;
	treasurePointLights.push_back(tpl);
}

void RenderSystem::resetCount() {
	numPointLights = 0;
	treasurePointLights.clear();
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
	//modelComponent->shader_program.setVec3("light.ambient", 0.3f, 0.2f, 0.2f);

	// Default diffuse lights
	//sets the position of the diffuse lighting
	//modelComponent->shader_program.setVec3("light.position", 0.0f, 0.0f, 1.0f);
	//sets the diffuse light value in shader (r, g, b)
	//modelComponent->shader_program.setVec3("light.diffuse", 0.2f + 171.0/255, 0.2f + 138.0/255, 0.2f + 126.0/255);
	//sets the specular light value of light in shader (r, g, b)
	//modelComponent->shader_program.setVec3("light.specular", 2.8f + 161.0 / 255, 2.8f + 69.0 / 255, 2.8f + 81.0 / 255);

	//glm::vec3 pointLightPositions[] = {
	//	glm::vec3(playerPosition.x, 0, playerPosition.y), //right
	//	glm::vec3(-4, 0, 1.0f), //left
	//	glm::vec3(-4.0f, 4.0f, 6.0f), //upper right
	//	glm::vec3(4.0f, 4.0f, 6.0f) //bottom right
	//};
	
	// Distance Constant Linear Quadratic
	// 7		1.0		0.7			1.8	
	// 13		1.0		0.35		0.44	
	// 20		1.0		0.22		0.20	
	// 32		1.0		0.14		0.07	
	// 50		1.0		0.09		0.032	
	// 65		1.0		0.07		0.017
	// 100		1.0		0.045		0.0075	
	// 160		1.0		0.027		0.0028	
	// 100		1.0		0.014		0.0007	

	//Point lights
	modelComponent->shader_program.setInt("numPointLights", numPointLights );
	int pointLightOffset = 0;
	//Player light
	for (int pointlights = 0; pointlights < 1; pointlights++) {
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointlights) + "].position", playerPointLight->position.x, 0, playerPointLight->position.z);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointlights) + "].ambient", playerPointLight->ambient.x, playerPointLight->ambient.y, playerPointLight->ambient.z);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointlights) + "].diffuse", playerPointLight->diffuse.x, playerPointLight->diffuse.y, playerPointLight->diffuse.z);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointlights) + "].specular", playerPointLight->specular.x, playerPointLight->specular.y, playerPointLight->specular.z);
		modelComponent->shader_program.setFloat("pointLights[" + to_string(pointlights) + "].constant", playerPointLight->constant);
		modelComponent->shader_program.setFloat("pointLights[" + to_string(pointlights) + "].linear", playerPointLight->linear);
		modelComponent->shader_program.setFloat("pointLights[" + to_string(pointlights) + "].quadratic", playerPointLight->quadratic);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointlights) + "].color", playerPointLight->color.x, playerPointLight->color.y, playerPointLight->color.z);
		pointLightOffset++;
	}
	

	//Treasure lights
	for (int pointlights = pointLightOffset, i = 0; pointlights < numPointLights; pointlights++, i++) {
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointlights) + "].position", treasurePointLights.at(i)->position.x, 0, treasurePointLights.at(i)->position.z);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointlights) + "].ambient", treasurePointLights.at(i)->ambient.x, treasurePointLights.at(i)->ambient.y, treasurePointLights.at(i)->ambient.z);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointlights) + "].diffuse", treasurePointLights.at(i)->diffuse.x, treasurePointLights.at(i)->diffuse.y, treasurePointLights.at(i)->diffuse.z);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointlights) + "].specular", treasurePointLights.at(i)->specular.x, treasurePointLights.at(i)->specular.y, treasurePointLights.at(i)->specular.z);
		modelComponent->shader_program.setFloat("pointLights[" + to_string(pointlights) + "].constant", treasurePointLights.at(i)->constant);
		modelComponent->shader_program.setFloat("pointLights[" + to_string(pointlights) + "].linear", treasurePointLights.at(i)->linear);
		modelComponent->shader_program.setFloat("pointLights[" + to_string(pointlights) + "].quadratic", treasurePointLights.at(i)->quadratic);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointlights) + "].color", treasurePointLights.at(i)->color.x, treasurePointLights.at(i)->color.y, treasurePointLights.at(i)->color.z);
		pointLightOffset++;
	}
	////point light 1
	//modelComponent->shader_program.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
	//modelComponent->shader_program.setVec3("pointLights[0].ambient", 0.2f, 0.1f, 0.1f);
	//modelComponent->shader_program.setVec3("pointLights[0].diffuse", 0.6f + 170.0 / 255, 0.6f + 148.0 / 255, 0.6f + 136.0 / 255);
	//modelComponent->shader_program.setVec3("pointLights[0].specular", 2.8f + 161.0 / 255, 2.8f + 69.0 / 255, 2.8f + 81.0 / 255);
	//modelComponent->shader_program.setFloat("pointLights[0].constant", 1.0f);
	//modelComponent->shader_program.setFloat("pointLights[0].linear", 0.14);
	//modelComponent->shader_program.setFloat("pointLights[0].quadratic", 0.07);
	//modelComponent->shader_program.setVec3("pointLights[0].color", 1.75, 1.75, 0.6);
	//
	////point light 2
	//modelComponent->shader_program.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
	//modelComponent->shader_program.setVec3("pointLights[1].ambient", 0.3f, 0.2f, 0.2f);
	//modelComponent->shader_program.setVec3("pointLights[1].diffuse", 0.6f + 170.0 / 255, 0.6f + 148.0 / 255, 0.6f + 136.0 / 255);
	//modelComponent->shader_program.setVec3("pointLights[1].specular", 2.8f + 161.0 / 255, 2.8f + 69.0 / 255, 2.8f + 81.0 / 255);
	//modelComponent->shader_program.setFloat("pointLights[1].constant", 1.0f);
	//modelComponent->shader_program.setFloat("pointLights[1].linear", 0.14);
	//modelComponent->shader_program.setFloat("pointLights[1].quadratic", 0.07);
	//modelComponent->shader_program.setVec3("pointLights[1].color", 0.6, 2.25, 2.25);
	//
	////point light 3
	//modelComponent->shader_program.setVec3("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
	//modelComponent->shader_program.setVec3("pointLights[2].ambient", 0.3f, 0.2f, 0.2f);
	//modelComponent->shader_program.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	//modelComponent->shader_program.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	//modelComponent->shader_program.setFloat("pointLights[2].constant", 1.0f);
	//modelComponent->shader_program.setFloat("pointLights[2].linear", 0.22);
	//modelComponent->shader_program.setFloat("pointLights[2].quadratic", 0.20);
	//modelComponent->shader_program.setVec3("pointLights[2].color", 0.6, 2.25, 2.25);
	//point light 4
	/*modelComponent->shader_program.setVec3("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
	modelComponent->shader_program.setVec3("pointLights[3].ambient", 0.3f, 0.2f, 0.2f);
	modelComponent->shader_program.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	modelComponent->shader_program.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	modelComponent->shader_program.setFloat("pointLights[3].constant", 1.0f);
	modelComponent->shader_program.setFloat("pointLights[3].linear", 0.22);
	modelComponent->shader_program.setFloat("pointLights[3].quadratic", 0.20);*/


	// Default material properties
	//sets the specular light value of the material in shader (r, g, b)
	modelComponent->shader_program.setVec3("material.specular", 0.0f, 0.0f, 0.0f);
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

void RenderSystem::receive(const ScoreUpdate& event) {
	totalScore += event.score;
}

void RenderSystem::receive(const PlayerHealthUpdate& event) {
	playerHealth = event.health;
}
