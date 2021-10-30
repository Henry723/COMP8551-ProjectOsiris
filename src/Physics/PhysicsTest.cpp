#include "PhysicsTest.h"

PhysicsTest::PhysicsTest()
{
}

void PhysicsTest::configure(EventManager& em) {
	
	em.subscribe<MoveInput>(*this);
}

void PhysicsTest::receive(const MoveInput& event) {
	MoveInput::InpDir dir = event.dir;
	switch (dir) {
	case MoveInput::UP:
		cout << "EVENT SYSTEM: MOVE INPUT - UP!" << endl;
		break;
	case MoveInput::LEFT:
		cout << "EVENT SYSTEM: MOVE INPUT - LEFT!" << endl;
		break;
	case MoveInput::DOWN:
		cout << "EVENT SYSTEM: MOVE INPUT - DOWN!" << endl;
		break;
	case MoveInput::RIGHT:
		cout << "EVENT SYSTEM: MOVE INPUT - RIGHT!" << endl;
		break;
	}
}
