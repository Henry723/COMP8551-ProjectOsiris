#include "PlayerSystem.h"

void PlayerSystem::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	if (gameState != GameState::RUNNING) return; //Make sure game is running, if not, return.
	ComponentHandle<GameObject> gameObjects; //GameObject component handle
	for (Entity entity : es.entities_with_components(gameObjects)) //Iterate over game objects
	{
		ComponentHandle<GameObject> object = entity.component<GameObject>(); //Get the game object
		if (object->name == "player") //If it's the player, process player update.
		{
			if (timeUntilNextOrder > 0) //If there's time left on the player's timer, they can act.
			{
				//Player rigidbody, transform, and position
				ComponentHandle<Rigidbody> rigidbody = entity.component<Rigidbody>();
				ComponentHandle<Transform> transform = entity.component<Transform>();
				ComponentHandle<Health> health = entity.component<Health>();

				glm::vec2 position = rigidbody->GetPosition();

				if (isMoving) //Check local moving flag
				{
					if (!rigidbody->moveBody) //Check rigidbody flag
					{
						isMoving = false; //Reset local moving flag
						timeUntilNextOrder = 0; //Was moving, now done, end timer.
					}
				}
				//MOVE CHECKS START: Not moving, check if we should
				else if (left && !rigidbody->IsCollidingWithBody("left")) { //Check if player pressed left and there are no left contacts
					transform->rotation = glm::vec4(0, -1, 0, 90); //Rotate left
					rigidbody->MoveToPosition(glm::vec2(position.x - 2, position.y), playerSpeed); //Start moving left
					isMoving = true; //Set local flag
				}
				//Next three blocks are the same for different directions.
				else if (right && !rigidbody->IsCollidingWithBody("right")) {
					transform->rotation = glm::vec4(0, 1, 0, 90);
					rigidbody->MoveToPosition(glm::vec2(position.x + 2, position.y), playerSpeed);
					isMoving = true;
				}
				else if (up && !rigidbody->IsCollidingWithBody("top")) {
					transform->rotation = glm::vec4(0, 1, 0, 110);
					rigidbody->MoveToPosition(glm::vec2(position.x, position.y - 2), playerSpeed);
					isMoving = true;
				}
				else if (down && !rigidbody->IsCollidingWithBody("bottom")) {
					transform->rotation = glm::vec4(0, 1, 0, 0);
					rigidbody->MoveToPosition(glm::vec2(position.x, position.y + 2), playerSpeed);
					isMoving = true;
				}
				//MOVE CHECKS END

				AttackChecks(transform, events); //Use flags to check if player can attack.
				ResetFlags(); //Reset movement and attack flags on player turn end.

				if (!isMoving) //If the player isn't moving, decrement timer.
				{
					timeUntilNextOrder -= dt;
					events.emit<TimerUpdate>(timeUntilNextOrder/timeInterval);
				}
					
			}
			else events.emit<PlayerTurnEnd>(); //Timer ran out, end player turn
		}
	}
}

//Check input and enemy flags to see if an attack should be performed.
void PlayerSystem::AttackChecks(ComponentHandle<Transform> transform, EventManager& events)
{
	//Arrays of attack flags and enemy pointers
	bool attackFlags[4] = { attackLeft, attackRight, attackDown, attackUp };
	Entity* enemyPointers[4] = { leftEntity, rightEntity, downEntity, upEntity };
	PlayerAttack::Dir attkDir = PlayerAttack::CENTER;
	for (int i = 0; i < 4; i++)
	{
		if (attackFlags[0]) {
			transform->rotation = glm::vec4(0, -1, 0, 90);
			attkDir = PlayerAttack::LEFT;
		}
		else if (attackFlags[1]) {
			transform->rotation = glm::vec4(0, 1, 0, 90);
			attkDir = PlayerAttack::RIGHT;
		}
		else if (attackFlags[2]) {
			transform->rotation = glm::vec4(0, 1, 0, 0);
			attkDir = PlayerAttack::DOWN;
		}
		else if (attackFlags[3]) {
			transform->rotation = glm::vec4(0, 1, 0, 110);
			attkDir = PlayerAttack::UP;
		}
		//If the flag and pointer are set...
		if (attackFlags[i] && enemyPointers[i] && enemyPointers[i]->valid()) 
		{
			events.emit<PlayerAttack>(attkDir); //Event for attack SFX
			//Grab the health component.
			ComponentHandle<Health> targetH = enemyPointers[i]->component<Health>();
			if (!--targetH->curHealth) //Decrement and check for 0
			{
				enemyPointers[i]->destroy(); //Destroy enemy.
				enemyPointers[i] = nullptr; //Nullify pointer.
			}
			timeUntilNextOrder = 0; //Reset timer.
		}
	}
}

//Reset input flags, needed here in case the movement didn't fire (moves would stack otherwise)
void PlayerSystem::ResetFlags()
{
	right = false;
	up = false;
	down = false;
	left = false;
	attackRight = false;
	attackUp = false;
	attackDown = false;
	attackLeft = false;
}

/***********************************************
*****************EVENT HANDLERS*****************
************************************************/

void PlayerSystem::configure(EventManager& em) {
	em.subscribe<MoveInput>(*this); 
	em.subscribe<AttackInput>(*this);
	em.subscribe<Collision>(*this);
	em.subscribe<EndCollision>(*this);
	em.subscribe<EnemyTurnEnd>(*this);
}

void PlayerSystem::receive(const Collision& event)
{
	//Check for valid entities
	if (event.a->valid() && event.b->valid())
	{
		//Only game objects should be involved in collisions, so should be safe to grab these components.
		ComponentHandle<GameObject> objectA = event.a->component<GameObject>();
		ComponentHandle<GameObject> objectB = event.b->component<GameObject>();
		if (!objectA || !objectB) return; //Invalid collision, gmae objects not found
		if (objectA->name == "player" || objectB->name == "player") //Player collision
		{
			//Store data for player
			Entity* player = objectA->name == "player" ? event.a : event.b;
			string playerCollider = objectA->name == "player" ? event.fA : event.fB;

			//Store data for other entity
			Entity* other = objectA->name == "player" ? event.b : event.a;
			string otherName = objectA->name == "player" ? objectB->name : objectA->name;
			string otherType = objectA->name == "player" ? event.fB : event.fA;

			//If we hit a treasure or key, destroy it.
			if ((otherName == "treasure" || otherName == "key") && playerCollider == "body") other->destroy();
			//If a sensor hit the body of a wall or enemy...
			else if ((otherName == "wall" || otherName == "enemy") && (otherType == "body" || otherType == "enemy_body") && playerCollider != "body")
			{
				//Set the directional entity if an enemy, set the move flags regardless.
				if (otherName == "enemy")
				{
					if (playerCollider == "left") leftEntity = other;
					else if (playerCollider == "right") rightEntity = other;
					else if (playerCollider == "top")  upEntity = other;
					else if (playerCollider == "bottom")downEntity = other;
				}
			}
		}
	}
}

void PlayerSystem::receive(const EndCollision& event)
{
	//Only game objects should be involved in collisions, so should be safe to grab these components.
	if (event.a->valid() && event.b->valid()) //Check for valid entities
	{
		ComponentHandle<GameObject> objectA = event.a->component<GameObject>();
		ComponentHandle<GameObject> objectB = event.b->component<GameObject>();
		if (!objectA || !objectB) return; //Invalid collision, gmae objects not found
		if (objectA->name == "player" || objectB->name == "player") //Player collision
		{
			//Store data for player
			Entity* player = objectA->name == "player" ? event.a : event.b;
			string playerCollider = objectA->name == "player" ? event.fA : event.fB;

			//Store data for other entity
			Entity* other = objectA->name == "player" ? event.b : event.a;
			string otherName = objectA->name == "player" ? objectB->name : objectA->name;
			string otherType = objectA->name == "player" ? event.fB : event.fA;

			//If we're colliding with a body a
			if (otherType == "body" || otherType == "enemy_body")
			{
				//Depending on the player collider, unset the directional entity and move flag.
				if (otherName == "enemy")
				{
					if (playerCollider == "left") leftEntity = nullptr;
					else if (playerCollider == "right") rightEntity = nullptr;
					else if (playerCollider == "top")  upEntity = nullptr;
					else if (playerCollider == "bottom")downEntity = nullptr;
				}
			}
		}
	}
}

//Receive function just sets boolean flags to be picked up by update loop
void PlayerSystem::receive(const MoveInput& event) {
	if (playerTurn)
	{
		MoveInput::InpDir dir = event.dir;
		switch (dir) {
		case MoveInput::UP:
			up = true;
			break;
		case MoveInput::LEFT:
			left = true;
			break;
		case MoveInput::DOWN:
			down = true;
			break;
		case MoveInput::RIGHT:
			right = true;
			break;
		}
	}
}

//Receive function just sets boolean flags to be picked up by update loop
void PlayerSystem::receive(const AttackInput& event) {
	if (playerTurn)
	{
		AttackInput::InpDir dir = event.dir;
		switch (dir) {
		case AttackInput::UP:
			attackUp = true;
			break;
		case AttackInput::LEFT:
			attackLeft = true;
			break;
		case AttackInput::DOWN:
			attackDown = true;
			break;
		case AttackInput::RIGHT:
			attackRight = true;
			break;
		}
	}
}

void PlayerSystem::receive(const EnemyTurnEnd& event)
{
	timeUntilNextOrder = timeInterval;
}
