#include "Actor.h"
#include "StudentWorld.h"

// Implementation of Actor functions
Actor::Actor(int imageID, double startX, double startY, Direction dir, int depth, bool block, StudentWorld* sw) : GraphObject(imageID, startX, startY, dir, depth, 1.0) {
	m_sw = sw;
	m_isAlive = true;
	m_isBlockable = block;
}

StudentWorld* Actor::getWorld() const {
	return m_sw;
}

void Actor::playSound(int soundID) {
	m_sw->playSound(soundID);
}

void Actor::die() {
	m_isAlive = false;
}

bool Actor::blockable() const {
	return m_isBlockable;
}

bool Actor::alive() const {
	return m_isAlive;
}

void Actor::changeScore(int amount) {
	m_sw->increaseScore(amount);
}

// Implementation of MovableObject functions
MovableObject::MovableObject(int imageID, double startX, double startY, StudentWorld* sw) : Actor(imageID, startX, startY, right, 0, true, sw) {}

// Implementation of ImmovableObject functions
ImmovableObject::ImmovableObject(int imageId, double startX, double startY, int depth, bool block, StudentWorld* sw) : Actor(imageId, startX, startY, right, depth, block, sw) {}

// Implementation of Human functions
Human::Human(int imageID, double startX, double startY, StudentWorld* sw) : MovableObject(imageID, startX, startY, sw) {}

bool Human::infected() {
	return m_isInfected;
}

void Human::infectionSpreads() {
	m_infectionCounter++;
}

int Human::infectionCount() {
	return m_infectionCounter;
}

// Implementation of Wall functions
Wall::Wall(double startX, double startY) : ImmovableObject(IID_WALL, startX, startY, 0, true, nullptr) {}

void Wall::doSomething() {}

// Implementation of Exit functions
Exit::Exit(double startX, double startY, StudentWorld* sw) : ImmovableObject(IID_EXIT, startX, startY, 1, false, sw) {}

void Exit::doSomething() {
	if (getWorld()->isOverlap(this)) {
		getWorld()->finishLevel();
	}
}

// Implementation of Zombie functions
Zombie::Zombie(double startX, double startY, StudentWorld* sw) : MovableObject(IID_ZOMBIE, startX, startY, sw) {}

// Implementation of Penelope functions
Penelope::Penelope(double startX, double startY, StudentWorld* sw) : Human(IID_PLAYER, startX, startY, sw) {}

void Penelope::doSomething() {
	int ch;
	if (getWorld()->getKey(ch)) {
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			setDirection(left);
			if (getWorld()->isBlocked(this, getX() - 4, getY()))
				break;
			moveTo(getX() - 4, getY());
			break;
		case KEY_PRESS_RIGHT:
			setDirection(right);
			if (getWorld()->isBlocked(this, getX() + 4, getY()))
				break;
			moveTo(getX() + 4, getY());
			break;
		case KEY_PRESS_UP:
			setDirection(up);
			if (getWorld()->isBlocked(this, getX(), getY() + 4))
				break;
			moveTo(getX(), getY() + 4);
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);
			if (getWorld()->isBlocked(this, getX(), getY() - 4))
				break;
			moveTo(getX(), getY() - 4);
			break;
		}
	}
}

// Implementation of Citizen functions
Citizen::Citizen(double startX, double startY) : Human(IID_CITIZEN, startX, startY, nullptr) {}

void Citizen::doSomething() {
	if (!alive())
		return;
	if (infected()) {
		if (infectionCount() >= 500) {
			die();
			playSound(SOUND_ZOMBIE_BORN);
			changeScore(-1000);
			getWorld()->addZombie(getX(), getY());
		}
		infectionSpreads();
	}
}

DumbZombie::DumbZombie(double startX, double startY, StudentWorld* sw) : Zombie(startX, startY, sw) {}

void DumbZombie::doSomething() {}