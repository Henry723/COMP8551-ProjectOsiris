#include "PhysicsTest.h"

void PhysicsTest::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	ComponentHandle<Rigidbody> rigidbody;
	for (Entity entity : es.entities_with_components(rigidbody))
	{
		ComponentHandle<Rigidbody> rigidbody = entity.component<Rigidbody>();
		ComponentHandle<Transform> transform = entity.component<Transform>();
		ComponentHandle<GameObject> object = entity.component<GameObject>();
		//Only do this for rigidbodies that are the player
		//Probably poor practice, but works for testing
		if (object && object->name == "player")
		{
			//First get current position
			glm::vec2 position = rigidbody->GetPosition();
			//For each direction, check if the movement flag is set and if you can move in that direction
			//First reset move flags before moving so future collisions update them correctly.
			//Then move the Rigidbody which will update the Transform as well
			//Velocity probably needs to be better scaled in-engine
			//Spaces between centers of tiles is 2.
			if (left && canMoveLeft) {
				transform->rotation = glm::vec4(0, -1, 0, 90);
				ResetMoveFlags();
				rigidbody->MoveToPosition(glm::vec2(position.x - 2, position.y), 0.5);
			}

			else if (right && canMoveRight) {
				transform->rotation = glm::vec4(0, 1, 0, 90);
				ResetMoveFlags();
				rigidbody->MoveToPosition(glm::vec2(position.x + 2, position.y), 0.5);
			}

			else if (up && canMoveUp) {
				transform->rotation = glm::vec4(0, 1, 0, 110);
				ResetMoveFlags();
				rigidbody->MoveToPosition(glm::vec2(position.x, position.y - 2), 0.5);
			}

			else if (down && canMoveDown) {
				transform->rotation = glm::vec4(0, 1, 0, 0);
				ResetMoveFlags();
				rigidbody->MoveToPosition(glm::vec2(position.x, position.y + 2), 0.5);
			}

			if (attackLeft && leftEntity)
			{
				//ComponentHandle<GameObject> targetObj = leftEntity->component<GameObject>();
				ComponentHandle<Health> targetH = leftEntity->component<Health>();
				if (!--targetH->curHealth)
				{
					ComponentHandle<Rigidbody> targetRb = leftEntity->component<Rigidbody>();
					targetRb->DeleteBody();
					leftEntity->destroy();
					leftEntity = nullptr;
					canMoveLeft = true;
				}
			}
			if (attackRight && rightEntity)
			{
				//ComponentHandle<GameObject> targetObj = rightEntity->component<GameObject>();
				ComponentHandle<Health> targetH = rightEntity->component<Health>();
				if (!--targetH->curHealth)
				{
					ComponentHandle<Rigidbody> targetRb = rightEntity->component<Rigidbody>();
					targetRb->DeleteBody();
					rightEntity->destroy();
					rightEntity = nullptr;
					canMoveRight = true;
				}
			}
			if (attackUp && upEntity)
			{
				//ComponentHandle<GameObject> targetObj = upEntity->component<GameObject>();
				ComponentHandle<Health> targetH = upEntity->component<Health>();
				if (!--targetH->curHealth)
				{
					ComponentHandle<Rigidbody> targetRb = upEntity->component<Rigidbody>();
					targetRb->DeleteBody();
					upEntity->destroy();
					upEntity = nullptr;
					canMoveUp = true;
				}
			}
			if (attackDown && downEntity)
			{
				//ComponentHandle<GameObject> targetObj = downEntity->component<GameObject>();
				ComponentHandle<Health> targetH = downEntity->component<Health>();
				if (!--targetH->curHealth)
				{
					ComponentHandle<Rigidbody> targetRb = downEntity->component<Rigidbody>();
					targetRb->DeleteBody();
					downEntity->destroy();
					downEntity = nullptr;
					canMoveDown = true;
				}
			}
			//Reset input flags, needed here in case the movement didn't fire (moves would stack otherwise)
			right = false;
			up = false;
			down = false;
			left = false;
			attackRight = false;
			attackUp = false;
			attackDown = false;
			attackLeft = false;
		}
	}
}

//Convenience function for reset collision flags
void PhysicsTest::ResetMoveFlags()
{
	canMoveLeft = true;
	canMoveRight = true;
	canMoveUp = true;
	canMoveDown = true;
	//ResetCollisionEntities(); //Will have to reset detected entities on move as well
}

//Convenience function for resetting detected entities
void PhysicsTest::ResetCollisionEntities()
{
	this->leftEntity = nullptr;
	this->rightEntity = nullptr;
	this->upEntity = nullptr;
	this->downEntity = nullptr;
}

void PhysicsTest::configure(EventManager& em) {
	em.subscribe<MoveInput>(*this);
	em.subscribe<Collision>(*this);
	em.subscribe<EndCollision>(*this);
	em.subscribe<AttackInput>(*this);
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
				if (playerCollider == "left")
				{
					if (otherName == "enemy") leftEntity = other;
					canMoveLeft = false;
				}
				if (playerCollider == "right") {
					if (otherName == "enemy") rightEntity = other;
					canMoveRight = false;
				}
				if (playerCollider == "top") {
					if (otherName == "enemy") upEntity = other;
					canMoveUp = false;
				}
				if (playerCollider == "bottom") {
					if (otherName == "enemy") downEntity = other;
					canMoveDown = false;
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
				if (playerCollider == "left") {
					if (otherName == "enemy") leftEntity = nullptr;
					canMoveLeft = true;
				}
				else if (playerCollider == "right") {
					if (otherName == "enemy") rightEntity = nullptr;
					canMoveRight = true;
				}
				else if (playerCollider == "top") {
					if (otherName == "enemy") upEntity = nullptr;
					canMoveUp = true;
				}
				else if (playerCollider == "bottom") {
					if (otherName == "enemy") downEntity = nullptr;
					canMoveDown = true;
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
