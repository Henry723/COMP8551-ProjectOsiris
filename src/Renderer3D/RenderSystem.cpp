#include "RenderSystem.h"
#include <thread>

UISystem& ui = UISystem::getInstance(); // Reference the UISystem instance (ensure the name is unique) 
int healthText, scoreText; // Create int IDs for each of the text elements you want to render
int totalScore, playerHealth;

void RenderSystem::configure(EventManager& em) {
	em.subscribe<ScoreUpdate>(*this);
	em.subscribe<PlayerHealthUpdate>(*this);
	em.subscribe<TimerUpdate>(*this);
}

void RenderSystem::update(EntityManager& es, EventManager& ev, TimeDelta dt)
{

	if (entityManager == nullptr) {
		entityManager = &es;
	}
	
	if (!(   gameState == GameState::RUNNING
		  || gameState == GameState::GAMEOVER))
		return;

	// Create component handles to filter components
	ComponentHandle<Color> hcolor;
	ComponentHandle<Window> hwindow;
	ComponentHandle<Models3D> hmodels;
	ComponentHandle<Transform> htransform;
	ComponentHandle<Camera> hCamera;
	ComponentHandle<Rigidbody> hRigidBody;
	ComponentHandle<Animator> hAnimator;

	// Needed to write player health to ui
	ComponentHandle<Health> playerHealth;

	// Handle for getting point light component within game object that has it.
	ComponentHandle<PointLight> hPointLight;

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe Rendering
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Fill Rendering
	glEnable(GL_DEPTH_TEST);
	glm::vec2 playerPosition(0.0);
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


	//Grab Model Index
	auto animatorEntities = es.entities_with_components(hmodels, hAnimator);
	for (auto entity = animatorEntities.begin(); entity != animatorEntities.end(); ++entity) {
		Animator* ator = (*entity).component<Animator>().get();
		Models3D* model = (*entity).component<Models3D>().get();
		//returns the keyframes index 1-3
		if (ator->checkAnimating()) {		
			int frameIndex = ator->getCurrentFrameIndex();
			if (frameIndex != model->getModelIndex()) {
				model->updateIndex(frameIndex);
			}
		}
		else {
			//use default model
			model->updateIndex(0);
		}	
		
	}


	// Loop though all the entity that has light components to update 
	auto lightEntities = es.entities_with_components(htransform, hPointLight);
	for (auto entity = lightEntities.begin(); entity != lightEntities.end(); ++entity) {
		// update light information functions
		PointLight* pointLight = (*entity).component<PointLight>().get();
		Transform* transform = (*entity).component<Transform>().get();
		ComponentHandle<GameObject> object = (*entity).component<GameObject>();
		if (object && object->name == "player") {
			//Update lights
			numPlayer++;
			updatePlayerLight(pointLight, transform);
		}
		if (object && object->name == "treasure") {
			//Update all light position and stuff when it knows it is treasure
			numTreasure++;
			updateTreasureLight(pointLight, transform);
		}
		if (object && object->name == "key") {
			//Update light position and stuff when it knows it is a key
			numKey++;
			updateKeyLight(pointLight, transform);
		}
		if (object && object->name == "door" && numKey == 0) {
			//Update light position and stuff when it knows it is a door and check if key is being collected
			numDoor++;
			updateDoorLight(pointLight, transform);
		}
	}
		
	// Loop through Model3D components
	auto modelEntities = es.entities_with_components(hmodels, htransform);

	for (auto entity = modelEntities.begin(); entity != modelEntities.end(); ++entity) {
		Models3D* model = (*entity).component<Models3D>().get();
		Transform* transform = (*entity).component<Transform>().get();

		model->translate(transform->position);
		model->scale(transform->scale);
		model->rotate(glm::vec3(transform->rotation.r, transform->rotation.g, transform->rotation.b), transform->rotation.a);
		
		//cout << "Drawing model " << model.name << endl;
		//model->Draw();
		draw(model->getCurrentModel3D(), model->getModelMatrix(), camera);
		model->resetModelMatrix();


		//std::thread drawThread(&RenderSystem::draw, this, model, camera);
		//drawThread.join();
	}

	// Text rendering using the UI System
	// Enable these properties so text can be properly rendered
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	static int lastCurHealth = 0;
	static int lastMaxHealth = 0;
	if (!ui.configured) { // Initialize FreeType and VAO/VBOs + adds text elements to be rendered
		ui.setup();
		healthText = ui.NewTextElement("Health: " + to_string(playerHealth->curHealth) + '/' + to_string(playerHealth->maxHealth), 15.0f, 565.0f, 0.75f, glm::vec3(1.0, 1.0f, 1.0f), true);
		scoreText = ui.NewTextElement("Score: 0000", 585.0f, 565.0f, 0.75f, glm::vec3(1.0, 1.0f, 1.0f), true);
		lastCurHealth = playerHealth->curHealth;
		lastMaxHealth = playerHealth->maxHealth;
		ui.configured = true;
	}
	else {
		ui.textElements[scoreText].value = "Score: " + to_string(totalScore);
		if (playerHealth != NULL)
		{
			ui.textElements[healthText].value = "Health: " + to_string(playerHealth->curHealth) + '/' + to_string(playerHealth->maxHealth);
			lastCurHealth = playerHealth->curHealth;
			lastMaxHealth = playerHealth->maxHealth;
		}
		else
			ui.textElements[healthText].value = "Health: " + to_string(lastCurHealth) + '/' + to_string(lastMaxHealth);
		ui.RenderAll(); // Render all text elements which are set as active
		if (gameState == GameState::GAMEOVER)
			ui.RenderMenuText();
	}

	// This is broken up, unfortunately, since the swapBuffers call must be after the Draw Call.
	for (auto entity = en.begin(); entity != en.end(); ++entity)
	{
		// Disable these properties once finished rendering text so models can be properly rendered
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		//update color buffer (a 2D buffer that contains color values for each pixel) to render during this iteration and show it as output to the screen.
		//glfwSwapBuffers((*entity).component<Window>().get()->window);
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

void RenderSystem::updateKeyLight(PointLight* pointLightCompnent, Transform* transformComponent) {
	keyPointLight = pointLightCompnent;
	keyPointLight->position = transformComponent->position;
}

void RenderSystem::updateDoorLight(PointLight* pointLightCompnent, Transform* transformComponent) {
	doorPointLight = pointLightCompnent;
	doorPointLight->position = transformComponent->position;
}

void RenderSystem::resetCount() {
	numPlayer = 0;
	numTreasure = 0;
	numKey = 0;
	numDoor = 0;
	treasurePointLights.clear();
}

void RenderSystem::draw(Model3D* modelComponent, glm::mat4 transformationMatrix, Camera* cameraComponent)
{
	if (modelComponent == nullptr) {
		return;
	}
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

	//Point lights, based off of the xml file, every entity with point lights will reflect within the world
	modelComponent->shader_program.setInt("numPointLights", (numPlayer + numTreasure + numKey + numDoor) );
	int pointLightOffset = 0;

	//Player light
	if (numPlayer > 0) {
		modelComponent->shader_program.setVec3("pointLights[" + to_string(0) + "].position", playerPointLight->position.x, 0, playerPointLight->position.z);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(0) + "].ambient", playerPointLight->ambient.x, playerPointLight->ambient.y, playerPointLight->ambient.z);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(0) + "].diffuse", playerPointLight->diffuse.x, playerPointLight->diffuse.y, playerPointLight->diffuse.z);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(0) + "].specular", playerPointLight->specular.x, playerPointLight->specular.y, playerPointLight->specular.z);
		modelComponent->shader_program.setFloat("pointLights[" + to_string(0) + "].constant", playerPointLight->constant);
		modelComponent->shader_program.setFloat("pointLights[" + to_string(0) + "].linear", playerPointLight->linear);
		modelComponent->shader_program.setFloat("pointLights[" + to_string(0) + "].quadratic", playerPointLight->quadratic);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(0) + "].color", playerPointLight->color.x, playerPointLight->color.y, playerPointLight->color.z);
		pointLightOffset++;
	}

	//Treasure lights
	if (numTreasure > 0) {
		for (int pointlights = pointLightOffset, i = 0; pointlights < (numPlayer + numTreasure); pointlights++, i++) {
			modelComponent->shader_program.setVec3("pointLights[" + to_string(pointLightOffset) + "].position", treasurePointLights.at(i)->position.x, 0, treasurePointLights.at(i)->position.z);
			modelComponent->shader_program.setVec3("pointLights[" + to_string(pointLightOffset) + "].ambient", treasurePointLights.at(i)->ambient.x, treasurePointLights.at(i)->ambient.y, treasurePointLights.at(i)->ambient.z);
			modelComponent->shader_program.setVec3("pointLights[" + to_string(pointLightOffset) + "].diffuse", treasurePointLights.at(i)->diffuse.x, treasurePointLights.at(i)->diffuse.y, treasurePointLights.at(i)->diffuse.z);
			modelComponent->shader_program.setVec3("pointLights[" + to_string(pointLightOffset) + "].specular", treasurePointLights.at(i)->specular.x, treasurePointLights.at(i)->specular.y, treasurePointLights.at(i)->specular.z);
			modelComponent->shader_program.setFloat("pointLights[" + to_string(pointLightOffset) + "].constant", treasurePointLights.at(i)->constant);
			modelComponent->shader_program.setFloat("pointLights[" + to_string(pointLightOffset) + "].linear", treasurePointLights.at(i)->linear);
			modelComponent->shader_program.setFloat("pointLights[" + to_string(pointLightOffset) + "].quadratic", treasurePointLights.at(i)->quadratic);
			modelComponent->shader_program.setVec3("pointLights[" + to_string(pointLightOffset) + "].color", treasurePointLights.at(i)->color.x, treasurePointLights.at(i)->color.y, treasurePointLights.at(i)->color.z);
			pointLightOffset++;
		}
	}

	//The key light
	if (numKey > 0) {
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointLightOffset) + "].position", keyPointLight->position.x, 0, keyPointLight->position.z);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointLightOffset) + "].ambient", keyPointLight->ambient.x, keyPointLight->ambient.y, keyPointLight->ambient.z);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointLightOffset) + "].diffuse", keyPointLight->diffuse.x, keyPointLight->diffuse.y, keyPointLight->diffuse.z);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointLightOffset) + "].specular", keyPointLight->specular.x, keyPointLight->specular.y, keyPointLight->specular.z);
		modelComponent->shader_program.setFloat("pointLights[" + to_string(pointLightOffset) + "].constant", keyPointLight->constant);
		modelComponent->shader_program.setFloat("pointLights[" + to_string(pointLightOffset) + "].linear", keyPointLight->linear);
		modelComponent->shader_program.setFloat("pointLights[" + to_string(pointLightOffset) + "].quadratic", keyPointLight->quadratic);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointLightOffset) + "].color", keyPointLight->color.x, keyPointLight->color.y, keyPointLight->color.z);
		pointLightOffset++;
	}

	//The door, it needs to see if there is no keys to enable this door light
	if (numDoor > 0 && numKey == 0) {
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointLightOffset) + "].position", doorPointLight->position.x, 0, doorPointLight->position.z);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointLightOffset) + "].ambient", doorPointLight->ambient.x, doorPointLight->ambient.y, doorPointLight->ambient.z);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointLightOffset) + "].diffuse", doorPointLight->diffuse.x, doorPointLight->diffuse.y, doorPointLight->diffuse.z);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointLightOffset) + "].specular", doorPointLight->specular.x, doorPointLight->specular.y, doorPointLight->specular.z);
		modelComponent->shader_program.setFloat("pointLights[" + to_string(pointLightOffset) + "].constant", doorPointLight->constant);
		modelComponent->shader_program.setFloat("pointLights[" + to_string(pointLightOffset) + "].linear", doorPointLight->linear);
		modelComponent->shader_program.setFloat("pointLights[" + to_string(pointLightOffset) + "].quadratic", doorPointLight->quadratic);
		modelComponent->shader_program.setVec3("pointLights[" + to_string(pointLightOffset) + "].color", doorPointLight->color.x, doorPointLight->color.y, doorPointLight->color.z);
		pointLightOffset++;
	}
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
	//glm::mat4 model = modelComponent->getModelMatrix();
	modelComponent->shader_program.setMat4("model", transformationMatrix);

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
	//modelComponent->resetModelMatrix();
}

void RenderSystem::receive(const ScoreUpdate& event) {
	totalScore += event.score;
}

void RenderSystem::receive(const PlayerHealthUpdate& event) {
	playerHealth = event.health;
}

void RenderSystem::receive(const TimerUpdate& event) {
	if(event.ratio > 0.01) ui.SetTimer(event.ratio);
}

RenderSystem::~RenderSystem() {
	//cout << "Render system is being deallocated" << endl;

	ComponentHandle<Model3D> hmodel;

	// Loop through Model3D components
	auto modelEntities = entityManager->entities_with_components(hmodel);
	for (auto entity = modelEntities.begin(); entity != modelEntities.end(); ++entity) {
		Model3D* model = (*entity).component<Model3D>().get();
		//cout << "Deallocating " << model << endl;
		model->clear_buffers();
		//delete model;
	}
}
