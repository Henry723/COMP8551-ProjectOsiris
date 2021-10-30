#include "InputEventTester.h"

void InputEventTester::configure(EventManager& em) {
	em.subscribe<MoveInput>(*this);
	em.subscribe<AttackInput>(*this);
}

void InputEventTester::receive(const MoveInput& event) {
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

void InputEventTester::receive(const AttackInput& event) {
	AttackInput::InpDir dir = event.dir;
	switch (dir) {
	case AttackInput::UP:
		cout << "EVENT SYSTEM: ATTACK INPUT - UP!" << endl;
		break;
	case AttackInput::LEFT:
		cout << "EVENT SYSTEM: ATTACK INPUT - LEFT!" << endl;
		break;
	case AttackInput::DOWN:
		cout << "EVENT SYSTEM: ATTACK INPUT - DOWN!" << endl;
		break;
	case AttackInput::RIGHT:
		cout << "EVENT SYSTEM: ATTACK INPUT - RIGHT!" << endl;
		break;
	}
}
