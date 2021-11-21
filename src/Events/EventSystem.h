#pragma once
#include "../components.hpp"
#include <entityx/Event.h>

using entityx::EventManager;
using entityx::Event;
using entityx::Receiver;

// Custom event for Movement Inputs
// InpDir - Enum used to indicate which direction input was detected
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

// Custom event for Attack Inputs
// InpDir - Enum used to indicate which direction input was detected
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

// Custom event for Interact Inputs
struct InteractInput {
	explicit InteractInput(){}
};

// Custom event for Enemy movement debugging
struct EnemyDebugInput {
	enum InpDir {
		UP,
		LEFT,
		DOWN,
		RIGHT
	};
	explicit EnemyDebugInput(InpDir dir) : dir(dir){}
	InpDir dir;
};

// Custom event for Game State Control
struct ControlInput {
	enum Cmd {
		X,
		Y,
		SPACE,
	};
	explicit ControlInput(Cmd cmd) : cmd(cmd) {}
	Cmd cmd;
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
	}

	void update(EntityManager& es, EventManager& events, TimeDelta dt) override {};

	void receive(const ExampleEvent& event) {
		total += event.a * event.b;
		cout << "Event system :" << total << endl;
	}
};