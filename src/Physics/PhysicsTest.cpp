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
			rigidbody->SetPosition(glm::vec2(position.x - 2, position.y));
			left = false;
			cout << "player position: x=" << transform->position.x << " y=" << transform->position.y << " z=" << transform->position.z << endl;
			cout << "rb x position: " << position.x << endl;
		}
		if (right)
		{
			rigidbody->SetPosition(glm::vec2(position.x + 2, position.y));
			right = false;
			cout << "player position: x=" << transform->position.x << " y=" << transform->position.y << " z=" << transform->position.z << endl;

		}
		if (up)
		{
			rigidbody->SetPosition(glm::vec2(position.x, position.y + 2));
			up = false;
			cout << "player position: x=" << transform->position.x << " y=" << transform->position.y << " z=" << transform->position.z << endl;

		}
		if (down)
		{
			rigidbody->SetPosition(glm::vec2(position.x, position.y - 2));
			down = false;
			cout << "player position: x=" << transform->position.x << " y=" << transform->position.y << " z=" << transform->position.z << endl;
		}
	}
	/*
	ComponentHandle<Player> players;
	for (Entity entity : es.entities_with_components(players)) {
		ComponentHandle<Rigidbody> rigidbody = entity.component<Rigidbody>();
		ComponentHandle<Transform> transform = entity.component<Transform>();
		ComponentHandle<GameObject> gameObject = entity.component<GameObject>();
		glm::vec2 position = rigidbody->GetPosition();
		if (left)
		{
			rigidbody->SetPosition(glm::vec2(position.x - 1, position.y));
			left = false;
			cout << "player position: x=" << transform->position.x << " y=" << transform->position.y << " z=" << transform->position.z << endl;
			cout << "rb x position: " << position.x << endl;
		}
		if (right)
		{
			rigidbody->SetPosition(glm::vec2(position.x + 1, position.y));
			right = false;
			cout << "player position: x=" << transform->position.x << " y=" << transform->position.y << " z=" << transform->position.z << endl;

		}
		if (up)
		{
			rigidbody->SetPosition(glm::vec2(position.x, position.y + 1));
			up = false;
			cout << "player position: x=" << transform->position.x << " y=" << transform->position.y << " z=" << transform->position.z << endl;

		}
		if (down)
		{
			rigidbody->SetPosition(glm::vec2(position.x, position.y - 1));
			down = false;
			cout << "player position: x=" << transform->position.x << " y=" << transform->position.y << " z=" << transform->position.z << endl;

		}
	}

	ComponentHandle<Rigidbody> rbs;
	for (Entity entity : es.entities_with_components(rbs)) {
		ComponentHandle<Rigidbody> rb = entity.component<Rigidbody>();
		ComponentHandle<GameObject> go = entity.component<GameObject>();
		glm::vec2 position = rb->GetPosition();
		//cout << go->name << " rigidbody position: x=" << position.x << " y=" << position.y << endl;
	}*/
}

void PhysicsTest::configure(EventManager& em) {
	em.subscribe<MoveInput>(*this);
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
