#include "PhysicsTest.h"

void PhysicsTest::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	ComponentHandle<Rigidbody> rigidbody;
	for (Entity entity : es.entities_with_components(rigidbody))
	{
		ComponentHandle<Rigidbody> rigidbody = entity.component<Rigidbody>();
		ComponentHandle<Transform> transform = entity.component<Transform>();
		ComponentHandle<GameObject> object = entity.component<GameObject>();
		if (object && object->name == "player")
		{
			glm::vec2 position = rigidbody->GetPosition();
			if (left && canMoveLeft)

			{
				canMoveLeft = true;
				canMoveRight = true;
				canMoveUp = true;
				canMoveDown = true;
				
				rigidbody->MoveToPosition(glm::vec2(position.x - 2, position.y), glm::vec2(-0.5, 0));

			}
			if (right && canMoveRight)
			{
				canMoveLeft = true;
				canMoveRight = true;
				canMoveUp = true;
				canMoveDown = true;
				
				rigidbody->MoveToPosition(glm::vec2(position.x + 2, position.y), glm::vec2(0.5, 0));
			}
			if (up && canMoveUp)
			{
				canMoveLeft = true;
				canMoveRight = true;
				canMoveUp = true;
				canMoveDown = true;
				
				rigidbody->MoveToPosition(glm::vec2(position.x, position.y - 2), glm::vec2(0, -0.5));
			}
			if (down && canMoveDown)
			{
				canMoveLeft = true;
				canMoveRight = true;
				canMoveUp = true;
				canMoveDown = true;
				
				rigidbody->MoveToPosition(glm::vec2(position.x, position.y + 2), glm::vec2(0, 0.5));
			}	
			right = false;
			up = false;
			down = false;
			left = false;
		}
	}
}

void PhysicsTest::configure(EventManager& em) {
	em.subscribe<MoveInput>(*this);
	em.subscribe<Collision>(*this);
}

void PhysicsTest::receive(const Collision& event)
{
	cout << "collision event received for " << event.fA << " and " << event.fB << endl;
	
	if (event.fA == "left" || event.fB == "left") this->canMoveLeft = false;
	if (event.fA == "right" || event.fB == "right") this->canMoveRight = false;
	if (event.fA == "top" || event.fB == "top") this->canMoveUp = false;
	if (event.fA == "bottom" || event.fB == "bottom") this->canMoveDown = false;
	//if (event.a && event.b)
	//{
	//	ComponentHandle<GameObject> objectA = event.a->component<GameObject>();
	//	ComponentHandle<GameObject> objectB = event.b->component<GameObject>();

	//	if (objectA && objectB)
	//	{
	//		if (objectA->name == "player" && objectB->name == "enemy")
	//		{
	//			event.a->destroy();
	//		}
	//		else if (objectA->name == "enemy" && objectB->name == "player")
	//		{
	//			//event.b->destroy();
	//		}
	//		else if (objectA->name == "player" && objectB->name == "treasure")
	//		{
	//			//event.b->destroy();
	//		}
	//		else if (objectA->name == "treasure" && objectB->name == "player")
	//		{
	//			//event.a->destroy();
	//		}
	//	}
	//}
	cout << "can move left " << canMoveLeft << endl;
}

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
