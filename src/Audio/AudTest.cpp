#include "AudTest.h"

AudTest::AudTest() {
	aud = new AudioSystem();
}

AudTest::~AudTest()
{
	delete aud;
}

void AudTest::configure(EventManager& em)
{
	em.subscribe<AttackInput>(*this);
}

void AudTest::receive(const AttackInput& event)
{
	
}