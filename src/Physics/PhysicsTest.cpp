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
				rigidbody->MoveToPosition(glm::vec2(position.x - 2, position.y), 0.5);
			}
				
			else if (right && canMoveRight) {
				transform->rotation = glm::vec4(0, 1, 0, 90);
				rigidbody->MoveToPosition(glm::vec2(position.x + 2, position.y), 0.5);
			}
				
			else if (up && canMoveUp) {
				transform->rotation = glm::vec4(0, 1, 0, 110);
				rigidbody->MoveToPosition(glm::vec2(position.x, position.y - 2), 0.5);
			}
				
			else if (down && canMoveDown) {
				transform->rotation = glm::vec4(0, 1, 0, 0);
				rigidbody->MoveToPosition(glm::vec2(position.x, position.y + 2), 0.5);
			}
			
			if (attackLeft && leftEntity)
			{
				ComponentHandle<GameObject> targetObj = leftEntity->component<GameObject>();
				ComponentHandle<Rigidbody> targetRb = leftEntity->component<Rigidbody>();
				targetRb->DeleteBody();
				leftEntity->destroy();
			}
			if (attackRight && rightEntity)
			{
				ComponentHandle<GameObject> targetObj = rightEntity->component<GameObject>();
				ComponentHandle<Rigidbody> targetRb = rightEntity->component<Rigidbody>();
				targetRb->DeleteBody();
				rightEntity->destroy();
			}
			if (attackUp && upEntity)
			{
				ComponentHandle<GameObject> targetObj = upEntity->component<GameObject>();
				ComponentHandle<Rigidbody> targetRb = upEntity->component<Rigidbody>();
				targetRb->DeleteBody();
				upEntity->destroy();
			}
			if (attackDown && downEntity)
			{
				ComponentHandle<GameObject> targetObj = downEntity->component<GameObject>();
				ComponentHandle<Rigidbody> targetRb = downEntity->component<Rigidbody>();
				targetRb->DeleteBody();
				downEntity->destroy();
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
	//Only game objects should be involved in collisions, so should be safe to grab these components.
	ComponentHandle<GameObject> objectA = event.a->component<GameObject>();
	ComponentHandle<GameObject> objectB = event.b->component<GameObject>();

	//cout << "Collision start for " << objectA->name << " " << event.fA << ", and " << objectB->name << " " << event.fB << endl;

	//We only care about collisions with at least one body for this implementation.
	if (event.fA == "body" || event.fB == "body" || event.fA == "enemy_body" || event.fB == "enemy_body")
	{
		//Logic specific to player
		//Colliding with a player body for left Object
		if (objectA->name == "player" && event.fA == "body") 
		{
			if (objectB->name == "treasure") event.b->destroy();		
		}

		//Colliding with a player body for right Object
		else if (objectB->name == "player" && event.fB == "body")
		{
			if (objectA->name == "treasure") event.a->destroy();
		}
		
		//Colliding with a player sensor for left Object
		else if (objectA->name == "player" && event.fA != "body")
		{
			//Sensor colliding with an enemy body
			if (objectB->name == "enemy" && event.fB == "enemy_body")
			{
				if (event.fA == "left")
				{
					this->canMoveLeft = false;
					leftEntity = event.b;
				}
				else if (event.fA == "right")
				{
					this->canMoveRight = false;
					rightEntity = event.b;
				}
				else if (event.fA == "top")
				{
					this->canMoveUp = false;
					upEntity = event.b;
				}
				else if (event.fA == "bottom")
				{
					this->canMoveDown = false;
					downEntity = event.b;
				}
			}
		}
		//Colliding with a player sensor for right Object
		else if (objectB->name == "player" && event.fB != "body")
		{
			//Sensor colliding with an enemy body
			if (objectA->name == "enemy" && event.fA == "enemy_body")
			{
				if (event.fB == "left")
				{
					this->canMoveLeft = false;
					leftEntity = event.a;
				}
				else if (event.fB == "right")
				{
					this->canMoveRight = false;
					rightEntity = event.a;
				}
				else if (event.fB == "top")
				{
					this->canMoveUp = false;
					upEntity = event.a;
				}
				else if (event.fB == "bottom")
				{
					this->canMoveDown = false;
					downEntity = event.a;
				}
			}
		}
	}
}

void PhysicsTest::receive(const EndCollision& event)
{
	//Only game objects should be involved in collisions, so should be safe to grab these components.
	if (event.a->valid())
	{
		ComponentHandle<GameObject> objectA = event.a->component<GameObject>();
		if (objectA->name == "player" && event.fA != "body")
		{
			if (event.fA == "left")
			{
				this->canMoveLeft = true;
				leftEntity = nullptr;
			}
			else if (event.fA == "right")
			{
				this->canMoveRight = true;
				rightEntity = nullptr;
			}
			else if (event.fA == "top")
			{
				this->canMoveUp = true;
				upEntity = nullptr;
			}
			else if (event.fA == "bottom")
			{
				this->canMoveDown = true;
				downEntity = nullptr;
			}
		}
	}
	if (event.b->valid())
	{
		ComponentHandle<GameObject> objectB = event.b->component<GameObject>();
		if (objectB->name == "player" && event.fB != "body")
		{
			if (event.fB == "left")
			{
				this->canMoveLeft = true;
				leftEntity = nullptr;
			}
			else if (event.fB == "right")
			{
				this->canMoveRight = true;
				rightEntity = nullptr;
			}
			else if (event.fB == "top")
			{
				this->canMoveUp = true;
				upEntity = nullptr;
			}
			else if (event.fB == "bottom")
			{
				this->canMoveDown = true;
				downEntity = nullptr;
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
