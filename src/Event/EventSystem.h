#pragma once
#include "entityx/Event.h"
#include <iostream>

using namespace std;
using entityx::EventManager;
using entityx::Event;
using entityx::Receiver;

struct ExampleEvent {
	explicit ExampleEvent(int a, int b) :a(a) , b(b) {}
	int a, b;
};

class ExampleEmitterSystem {
	EventManager em;
public:
	void emitSample() {
		em.emit<ExampleEvent>(1, 2);
	}
};

class ExampleListenerSystem : public Receiver<ExampleListenerSystem> {
	EventManager em;
	int total;

	ExampleListenerSystem() {
		em.subscribe<ExampleEvent>(*this);
	}

	void recieve(const ExampleEvent &ee) {
		total += ee.a * ee.b;
		cout << total << endl;
	}
};