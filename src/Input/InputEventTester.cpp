#include "InputEventTester.h"

UISystem& uiInputEvents = UISystem::getInstance(); // Reference the UISystem instance (ensure the name is unique)
int moveText, toggleText;

void InputEventTester::configure(EventManager& em) {
	em.subscribe<MoveInput>(*this);
	em.subscribe<AttackInput>(*this);
	moveText = uiInputEvents.NewTextElement("WASD to move", 250.0f, 150.0f, 0.75f, glm::vec3(1.0, 0.2f, 0.2f), true);
	toggleText = uiInputEvents.NewTextElement("UP ARROW to toggle", 300.0f, 450.0f, 0.7f, glm::vec3(0.0, 1.0f, 0.4f), true);
}

void InputEventTester::receive(const MoveInput& event) {
	MoveInput::InpDir dir = event.dir;
	switch (dir) {
	case MoveInput::UP:
		cout << "EVENT SYSTEM: MOVE INPUT - UP!" << endl;
		uiInputEvents.textElements[moveText].posY += 25.0f;
		break;
	case MoveInput::LEFT:
		cout << "EVENT SYSTEM: MOVE INPUT - LEFT!" << endl;
		uiInputEvents.textElements[moveText].posX -= 25.0f;
		break;
	case MoveInput::DOWN:
		cout << "EVENT SYSTEM: MOVE INPUT - DOWN!" << endl;
		uiInputEvents.textElements[moveText].posY -= 25.0f;
		break;
	case MoveInput::RIGHT:
		cout << "EVENT SYSTEM: MOVE INPUT - RIGHT!" << endl;
		uiInputEvents.textElements[moveText].posX += 25.0f;
		break;
	}
}

void InputEventTester::receive(const AttackInput& event) {
	AttackInput::InpDir dir = event.dir;
	switch (dir) {
	case AttackInput::UP:
		cout << "EVENT SYSTEM: ATTACK INPUT - UP!" << endl;
		uiInputEvents.textElements[toggleText].active = !uiInputEvents.textElements[toggleText].active;
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
