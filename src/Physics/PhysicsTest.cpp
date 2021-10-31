#include "PhysicsTest.h"

void PhysicsTest::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	ComponentHandle<Rigidbody> rigidbody;
	for (Entity entity : es.entities_with_components(rigidbody))
	{
		ComponentHandle<Rigidbody> rigidbody = entity.component<Rigidbody>();
		ComponentHandle<Transform> transform = entity.component<Transform>();
		glm::vec2 position = rigidbody->GetPosition();
		if (left)
		{
			rigidbody->SetVelocity(glm::vec2(-0.5, 0));
			//rigidbody->SetPosition(glm::vec2(position.x - 2, position.y));
			left = false;
		}
		if (right)
		{
			rigidbody->SetVelocity(glm::vec2(0.5, 0));
			//rigidbody->SetPosition(glm::vec2(position.x + 2, position.y));
			right = false;

		}
		if (up)
		{
			rigidbody->SetVelocity(glm::vec2(0, 0.5));
			//rigidbody->SetPosition(glm::vec2(position.x, position.y + 2));
			up = false;

		}
		if (down)
		{
			rigidbody->SetVelocity(glm::vec2(0, 0.5));
			//rigidbody->SetPosition(glm::vec2(position.x, position.y - 2));
			down = false;
		}
	}
}

void PhysicsTest::configure(EventManager& em) {
	em.subscribe<MoveInput>(*this);
	em.subscribe<Collision>(*this);
}

void PhysicsTest::receive(const Collision& event)
{
	cout << "collision event received" << endl;
	if (event.a && event.b)
	{
		ComponentHandle<GameObject> objectA = event.a->component<GameObject>();
		ComponentHandle<GameObject> objectB = event.b->component<GameObject>();

		if (objectA && objectB)
		{
			if (objectA->name == "player" && objectB->name == "enemy")
			{
				event.a->destroy();
			}
		}
	}
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
