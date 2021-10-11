#pragma once
#include "../components.hpp"
#include <entityx/Event.h>

using entityx::EventManager;
using entityx::Event;
using entityx::Receiver;

struct MoveInput {
	enum InpDir {
		UP,
		LEFT,
		DOWN,
		RIGHT
	};
	explicit MoveInput(InpDir dir) : dir(dir){}
	InpDir dir;
};

struct AttackInput {
	enum InpDir {
		UP,
		LEFT,
		DOWN,
		RIGHT
	};
	explicit AttackInput(InpDir dir) : dir(dir) {}
	InpDir dir;
};

//Create the custom events as such
struct ExampleEvent {
	explicit ExampleEvent(int a, int b) : a(a), b(b) {}
	int a, b;
};

//The emitter will be called using .emit<custom events>
class ExampleEmitterSystem : public System<ExampleEmitterSystem> {
	bool emitted = false;

public:
	void update(EntityManager& es, EventManager& events, TimeDelta dt) override {
		emitSample(events);
	};

	void emitSample(EventManager& em) {
		if (!emitted)
			em.emit<ExampleEvent>(4, 2);
		emitted = true;
	}
};

//The listener needs to subscribe .subscribe<custom event>() before enabling recieve the emitted events from .emit<custom event>()
struct ExampleListenerSystem : public System<ExampleListenerSystem>, public Receiver<ExampleListenerSystem> {
	int total = 0;

	void configure(EventManager& em) override {
		em.subscribe<ExampleEvent>(*this);
		em.subscribe<MoveInput>(*this);
		em.subscribe<AttackInput>(*this);
	}

	void update(EntityManager& es, EventManager& events, TimeDelta dt) override {};

	void receive(const ExampleEvent& event) {
		total += event.a * event.b;
		cout << "Event system :" << total << endl;
	}

	void receive(const MoveInput& event) {
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

	void receive(const AttackInput& event) {
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
};