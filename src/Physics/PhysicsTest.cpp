#include "PhysicsTest.h"

void PhysicsTest::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	if (gameState != RUNNING) return; //Make sure game is running, if not, return.
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
				/////////////////////
				//ATTACK CHECKS START: Check for attack input and if there's an entity to attack.
				if (attackLeft && leftEntity)
				{
					leftEntity->destroy(); //Destroy entity
					leftEntity = nullptr; //Nullify tracker
					timeUntilNextOrder = 0; //Attacked, end timer.
				}
				//Next three blocks are the same for different directions.
				if (attackRight && rightEntity)
				{
					rightEntity->destroy();
					rightEntity = nullptr;
					timeUntilNextOrder = 0;
				}
				if (attackUp && upEntity)
				{
					upEntity->destroy();
					upEntity = nullptr;
					timeUntilNextOrder = 0;
				}
				if (attackDown && downEntity)
				{
					downEntity->destroy();
					downEntity = nullptr;
					timeUntilNextOrder = 0;
				}
				//ATTACK CHECKS END

				//Reset input flags, needed here in case the movement didn't fire (moves would stack otherwise)
				right = false;
				up = false;
				down = false;
				left = false;
				attackRight = false;
				attackUp = false;
				attackDown = false;
				attackLeft = false;

				if (!isMoving) //If the player isn't moving, decrement timer.
					timeUntilNextOrder -= dt;
			}
			else
			{
				cout << "end of player turn" << endl;
				events.emit<PlayerTurnEnd>();
			}
		}
	}

}

void PhysicsTest::configure(EventManager& em) {
	em.subscribe<MoveInput>(*this);
	em.subscribe<Collision>(*this);
	em.subscribe<EndCollision>(*this);
	em.subscribe<AttackInput>(*this);
	em.subscribe<EnemyTurnEnd>(*this);
}

void PhysicsTest::receive(const Collision& event)
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

			//If we hit a treasure, destroy it.
			if (otherName == "treasure" && playerCollider == "body") other->destroy();
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

void PhysicsTest::receive(const EndCollision& event)
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
void PhysicsTest::receive(const MoveInput& event) {
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

//Receive function just sets boolean flags to be picked up by update loop
void PhysicsTest::receive(const AttackInput& event) {
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

void PhysicsTest::receive(const EnemyTurnEnd& event)
{
	timeUntilNextOrder = timeInterval;
}
