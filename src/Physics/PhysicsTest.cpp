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
			if (left && canMoveLeft)
			{
				ResetMoveFlags();
				rigidbody->MoveToPosition(glm::vec2(position.x - 2, position.y), 0.5);
			}
			if (right && canMoveRight)
			{
				ResetMoveFlags();
				rigidbody->MoveToPosition(glm::vec2(position.x + 2, position.y), 0.5);
			}
			if (up && canMoveUp)
			{
				ResetMoveFlags();
				rigidbody->MoveToPosition(glm::vec2(position.x, position.y - 2), 0.5);
			}
			if (down && canMoveDown)
			{
				ResetMoveFlags();
				rigidbody->MoveToPosition(glm::vec2(position.x, position.y + 2), 0.5);
			}
			//Reset input flags, needed here in case the movement didn't fire (moves would stack otherwise)
			right = false;
			up = false;
			down = false;
			left = false;
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
}

void PhysicsTest::configure(EventManager& em) {
	em.subscribe<MoveInput>(*this);
	em.subscribe<Collision>(*this);
}

void PhysicsTest::receive(const Collision& event)
{
	cout << "collision event received for " << event.fA << " and " << event.fB << endl;
	//Collision is just checking for any directional sensors being hit and telling the player if they can or cant move.
	//Can be further customised to check specifically for Treasure/Enemy/Walls/etc
	if (event.fA == "left" || event.fB == "left") this->canMoveLeft = false;
	if (event.fA == "right" || event.fB == "right") this->canMoveRight = false;
	if (event.fA == "top" || event.fB == "top") this->canMoveUp = false;
	if (event.fA == "bottom" || event.fB == "bottom") this->canMoveDown = false;
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
