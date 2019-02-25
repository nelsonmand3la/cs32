#include "Actor.h"
#include "StudentWorld.h"

//Implementation of Actor functions////

Actor::Actor(int imageID, double startX, double startY, Direction dir, int depth, StudentWorld* w) : GraphObject(imageID, startX, startY, dir, depth, 1.0) {
	m_sw = w;
	m_isDead = false;
}

bool Actor::isDead() const {
	return m_isDead;
}

void Actor::setDead() {
	m_isDead = true;
}

StudentWorld* Actor::world() const {
	return m_sw;
}

void Actor::activateIfAppropriate(Actor* a) {
	return;
}

void Actor::useExitIfAppropriate() {
	return;
}

void Actor::dieByFallOrBurnIfAppropriate() {
	return;
}

void Actor::beVomitedOnIfAppropriate() {
	return;
}

bool Actor::pickUpGoodieIfAppropriate(Goodie* g) {
	return false;
}

bool Actor::triggersZombieVomit() const {
	return false;
}

bool Actor::blocksMovement() const {
	return true;
}

bool Actor::blocksFlame() const {
	return false;
}

bool Actor::triggersOnlyActiveLandmines() const {
	return false;
}

//////////////////////////////////////

//Implementation of Wall functions////

Wall::Wall(double startX, double startY) : Actor(IID_WALL, startX, startY, right, 0, nullptr) {}

// Walls block flames
bool Wall::blocksFlame() const {
	return true;
}

// Walls do nothing
void Wall::doSomething() {}

/////////////////////////////////////

// Implementation of Exit functions //

Exit::Exit(double startX, double startY, StudentWorld* sw) : ActivatingObject(IID_EXIT, startX, startY, right, 1, sw) {}

void Exit::activateIfAppropriate(Actor* a) {
	//If Actor a can use the Exit, let them use it
	a->useExitIfAppropriate();
}

void Exit::doSomething() {
	//If an object, that can activate the exit, overlaps with the exit, do something
	world()->activateOnOverlap(this);
}

bool Exit::blocksFlame() const {
	return true;
}

//////////////////////////////////////

// Implementation of ActivatingObject functions //

ActivatingObject::ActivatingObject(int imageID, double startX, double startY, Direction dir, int depth, StudentWorld* sw) : Actor(imageID, startX, startY, dir, depth, sw) {}

bool ActivatingObject::blocksMovement() const {
	return false;
}

bool ActivatingObject::blocksFlame() const {
	return false;
}

//////////////////////////////////////////////////

//Implementation of Flame functions//

Flame::Flame(double startX, double startY, Direction dir, StudentWorld* w) : ActivatingObject(IID_FLAME, startX, startY, dir, 0, w) {}

void Flame::activateIfAppropriate(Actor* a) {
	a->dieByFallOrBurnIfAppropriate();
}

void Flame::doSomething() {
	// If the Flame is dead, then do nothing
	if (isDead()) {
		return;
	}

	// If the Flame has been alive for two ticks, set the Flame to dead, do nothing
	if (m_ticksAlive == 2) {
		setDead();
		return;
	}

	// Damage objects around it
	world()->activateOnOverlap(this);

	m_ticksAlive++;
}

/////////////////////////////////////

// Implementation of Vomit functions //

Vomit::Vomit(double startX, double startY, Direction dir, StudentWorld* w) : ActivatingObject(IID_VOMIT, startX, startY, dir, 0, w) {}

void Vomit::activateIfAppropriate(Actor* a) {
	a->beVomitedOnIfAppropriate();
}

void Vomit::doSomething() {
	if (isDead())
		return;

	if (m_ticksAlive == 2) {
		setDead();
		return;
	}

	world()->activateOnOverlap(this);

	m_ticksAlive++;
}

//////////////////////////////////////

// Implementation of Landmine functions //

Landmine::Landmine(double startX, double startY, StudentWorld* w) : ActivatingObject(IID_LANDMINE, startX, startY, right, 1, w) {};

void Landmine::dieByFallOrBurnIfAppropriate() {
	setDead();
	world()->playSound(SOUND_LANDMINE_EXPLODE);

	double x = getX();
	double y = getY();

	world()->addActor(new Flame(x, y, up, world()));
	world()->addActor(new Flame(x - SPRITE_WIDTH, y + SPRITE_HEIGHT, up, world()));
	world()->addActor(new Flame(x, y + SPRITE_HEIGHT, up, world()));
	world()->addActor(new Flame(x + SPRITE_WIDTH, y + SPRITE_HEIGHT, up, world()));
	world()->addActor(new Flame(x - SPRITE_WIDTH, y, up, world()));
	world()->addActor(new Flame(x + SPRITE_WIDTH, y, up, world()));
	world()->addActor(new Flame(x - SPRITE_WIDTH, y - SPRITE_HEIGHT, up, world()));
	world()->addActor(new Flame(x, y - SPRITE_HEIGHT, up, world()));
	world()->addActor(new Flame(x + SPRITE_WIDTH, y + SPRITE_HEIGHT, up, world()));

	//Introduce pit
}

void Landmine::activateIfAppropriate(Actor* a) {
	if (a->triggersOnlyActiveLandmines()) {
		setDead();
		world()->playSound(SOUND_LANDMINE_EXPLODE);

		double x = getX();
		double y = getY();

		world()->addActor(new Flame(x, y, up, world()));
		world()->addActor(new Flame(x - SPRITE_WIDTH, y + SPRITE_HEIGHT, up, world()));
		world()->addActor(new Flame(x, y + SPRITE_HEIGHT, up, world()));
		world()->addActor(new Flame(x + SPRITE_WIDTH, y + SPRITE_HEIGHT, up, world()));
		world()->addActor(new Flame(x - SPRITE_WIDTH, y, up, world()));
		world()->addActor(new Flame(x + SPRITE_WIDTH, y, up, world()));
		world()->addActor(new Flame(x - SPRITE_WIDTH, y - SPRITE_HEIGHT, up, world()));
		world()->addActor(new Flame(x, y - SPRITE_HEIGHT, up, world()));
		world()->addActor(new Flame(x + SPRITE_WIDTH, y + SPRITE_HEIGHT, up, world()));

		//Introduce pit
	}
}

void Landmine::doSomething() {
	if (isDead())
		return;
	
	if (!m_active) {
		m_safetyTicks--;
		if (m_safetyTicks == 0)
			m_active = true;
		return;
	}

	world()->activateOnOverlap(this);
}

//////////////////////////////////////////

//Implementation of Goodie functions//

Goodie::Goodie(int imageID, double startX, double startY, StudentWorld* sw) : ActivatingObject(imageID, startX, startY, 1, false, sw) {}

void Goodie::activateIfAppropriate(Actor* a) {
	if (a->pickUpGoodieIfAppropriate(this)) {
		changeGameScore();
		world()->playSound(SOUND_GOT_GOODIE);
	}
}

void Goodie::dieByFallOrBurnIfAppropriate() {
	setDead();
}

bool Goodie::blocksMovement() {
	return false;
}

//////////////////////////////////////

// Implementation of GasCan functions ///

GasCan::GasCan(double startX, double startY, StudentWorld* sw) : Goodie(IID_GAS_CAN_GOODIE, startX, startY, sw) {}

void GasCan::changeGameScore() {
	world()->giveFlames();
}

void GasCan::doSomething() {
	if (isDead())
		return;

	world()->activateOnOverlap(this);
}

////////////////////////////////////////

// Implementation of Vaccine functions //

Vaccine::Vaccine(double startX, double startY, StudentWorld* w) : Goodie(IID_VACCINE_GOODIE, startX, startY, w) {}

void Vaccine::changeGameScore() {
	world()->giveVaccines();
}

void Vaccine::doSomething() {
	if (isDead())
		return;

	world()->activateOnOverlap(this);
}

////////////////////////////////////////

// Implementation of LandmineGoodie functions //

LandmineGoodie::LandmineGoodie(double startX, double startY, StudentWorld* w) : Goodie(IID_LANDMINE_GOODIE, startX, startY, w) {}

void LandmineGoodie::changeGameScore() {
	world()->giveMines();
}

void LandmineGoodie::doSomething() {
	if (isDead())
		return;

	world()->activateOnOverlap(this);
}

///////////////////////////////////////////////

// Implementation of Biped functions //

Biped::Biped(int imageID, double startX, double startY, StudentWorld* w) : Actor(imageID, startX, startY, right, 0, w) {}

void Biped::lived() {
	m_ticksAlive++;
}

bool Biped::paralyzed() {
	return m_ticksAlive % 2 == 0;
}

bool Biped::triggersOnlyActiveLandmines() const {
	return true;
}

void Biped::dieByFallOrBurnIfAppropriate() {
	setDead();
	specializedDeath();
}

// Implementation of Human functions //

Human::Human(int imageID, double startX, double startY, StudentWorld* w) : Biped(imageID, startX, startY, w) {}

bool Human::isInfected() {
	return m_infected;
}

void Human::getInfected() {
	m_infected = true;
}

void Human::getCured() {
	m_infected = false;
	m_infectionCounter = 0;
}

void Human::infectionSpreads() {
	m_infectionCounter++;
}

int Human::infectionCount() {
	return m_infectionCounter;
}

void Human::beVomitedOnIfAppropriate() {
	getInfected();
}

bool Human::triggersZombieVomit() const {
	return true;
}

//////////////////////////////////////

// Implementation of Penelope functions //
Penelope::Penelope(double startX, double startY, StudentWorld* sw) : Human(IID_PLAYER, startX, startY, sw) {}

bool Penelope::pickUpGoodieIfAppropriate(Goodie* g) {
	world()->increaseScore(50);
	g->setDead();
	world()->playSound(SOUND_GOT_GOODIE);
	return true;
}

void Penelope::useExitIfAppropriate() {
	//If there are no more citizens
	if (world()->citizensGone()) {
		world()->finishLevel();
	}
}

void Penelope::doSomething() {
	if (isDead())
		return;

	if (isInfected()) {
		infectionSpreads();
		if (infectionCount() == 500) {
			setDead();
			world()->playSound(SOUND_PLAYER_DIE);
			return;
		}
	}

	int ch;
	if (world()->getKey(ch)) {
		switch (ch)
		{
		case KEY_PRESS_SPACE:
			if (m_flames > 0) {
				m_flames--;
				world()->playSound(SOUND_PLAYER_FIRE);
				double flameX = getX();
				double flameY = getY();
				for (int i = 1; i < 4; i++) {
					switch (getDirection()) {
					case right: {
						flameX += SPRITE_WIDTH;
						break;
					}
					case up: {
						flameY += SPRITE_HEIGHT;
						break;
					}
					case left: {
						flameX -= SPRITE_WIDTH;
						break;
					}
					case down: {
						flameY -= SPRITE_HEIGHT;
						break;
					}
					}
					if (world()->isFlameBlocked(flameX, flameY))
						break;
					world()->addActor(new Flame(flameX, flameY, getDirection(), world()));
				}
			}
			break;
		case KEY_PRESS_TAB:
			if (m_landmines > 0) {
				world()->addActor(new Landmine(getX(), getY(), world()));
				m_landmines--;
			}
			break;
		case KEY_PRESS_ENTER:
			if (m_vaccines > 0) {
				getCured();
				m_vaccines--;
			}
			break;
		case KEY_PRESS_LEFT:
			setDirection(left);
			if (world()->blocked(this, getX() - 4, getY()))
				break;
			moveTo(getX() - 4, getY());
			break;
		case KEY_PRESS_RIGHT:
			setDirection(right);
			if (world()->blocked(this, getX() + 4, getY()))
				break;
			moveTo(getX() + 4, getY());
			break;
		case KEY_PRESS_UP:
			setDirection(up);
			if (world()->blocked(this, getX(), getY() + 4))
				break;
			moveTo(getX(), getY() + 4);
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);
			if (world()->blocked(this, getX(), getY() - 4))
				break;
			moveTo(getX(), getY() - 4);
			break;
		}
	}
}

int Penelope::getMines() {
	return m_landmines;
}

int Penelope::getVacc() {
	return m_vaccines;
}

int Penelope::getFlames() {
	return m_flames;
}

void Penelope::specializedDeath() {
	world()->playSound(SOUND_PLAYER_DIE);
}

void Penelope::receiveFlames() {
	m_flames += 5;
}

void Penelope::receiveMines() {
	m_landmines += 2;
}

void Penelope::receiveVaccines() {
	m_vaccines++;
}


///////////////////////////////////////////////

// Implementation of Citizen functions //

Citizen::Citizen(double startX, double startY, StudentWorld* w) : Human(IID_CITIZEN, startX, startY, w) {}

void Citizen::useExitIfAppropriate() {
	world()->increaseScore(500);
	setDead();
	world()->playSound(SOUND_CITIZEN_SAVED);
	world()->lostCitizen();
}

void Citizen::specializedDeath() {
	world()->playSound(SOUND_CITIZEN_DIE);
	world()->increaseScore(-1000);
	world()->lostCitizen();
}

void Citizen::doSomething() {
	if (isDead())
		return;

	if (isInfected()) {
		infectionSpreads();
		if (infectionCount() >= 500) {
			setDead();
			world()->playSound(SOUND_ZOMBIE_BORN);
			world()->increaseScore(-1000);
			if (randInt(0, 9) < 7) {
				world()->addActor(new DumbZombie(getX(), getY(), world()));
			}
			else {
				world()->addActor(new SmartZombie(getX(), getY(), world()));
			}
			world()->lostCitizen();
			return;
		}
	}

	lived();

	if (paralyzed()) {
		return;
	}

	double x = getX();
	double y = getY();

	double distSqP = world()->distSqPenelope(this);
	double distSqZ = world()->distSqNearestZombie(this, x, y);

	bool blockedUp = world()->blocked(this, x, y + 2);
	bool blockedDown = world()->blocked(this, x, y - 2);
	bool blockedLeft = world()->blocked(this, x - 2, y);
	bool blockedRight = world()->blocked(this, x + 2, y);

	if ((distSqZ == -1 || distSqP < distSqZ) && distSqP <= 80 * 80) {

		double pX = world()->getHeroX();
		double pY = world()->getHeroY();

		if (x == pX) {
			if (y < pY) {
				if (!blockedUp) {
					setDirection(up);
					moveTo(x, y + 2);
					return;
				}
			}
			else {
				if (!blockedDown) {
					setDirection(down);
					moveTo(x, y - 2);
					return;
				}
			}
		}
		else if (y == pY) {
			if (x < pX) {
				if (!blockedRight) {
					setDirection(right);
					moveTo(x + 2, y);
					return;
				}
			}
			else {
				if (!blockedLeft) {
					setDirection(left);
					moveTo(x - 2, y);
					return;
				}
			}
		}
		else {
			Direction horizontal = 0;
			Direction vertical = 0;

			if (x < pX)
				horizontal = right;
			if (x > pX)
				horizontal = left;

			if (y < pY)
				vertical = up;
			if (y > pY)
				vertical = down;



			if (randInt(0, 1) == 0) {
				if (horizontal == right) {
					if (!blockedRight) {
						setDirection(horizontal);
						moveTo(x + 2, y);
						return;
					}
				}
				if (horizontal == left) {
					if (!blockedLeft) {
						setDirection(horizontal);
						moveTo(x - 2, y);
						return;
					}
				}
				if (vertical == up) {
					if (!blockedUp) {
						setDirection(vertical);
						moveTo(x, y + 2);
						return;
					}
				}
				if (vertical == down) {
					if (!blockedDown) {
						setDirection(vertical);
						moveTo(x, y - 2);
						return;
					}
				}
			}
			else {
				if (vertical == up) {
					if (!blockedUp) {
						setDirection(vertical);
						moveTo(x, y + 2);
						return;
					}
				}
				if (vertical == down) {
					if (!blockedDown) {
						setDirection(vertical);
						moveTo(x, y - 2);
						return;
					}
				}
				if (horizontal == right) {
					if (!blockedRight) {
						setDirection(horizontal);
						moveTo(x + 2, y);
						return;
					}
				}
				if (horizontal == left) {
					if (!blockedLeft) {
						setDirection(horizontal);
						moveTo(x - 2, y);
						return;
					}
				}
			}
		}
	}

	if (distSqZ != -1 && distSqZ <= 80 * 80) {
		double distUp = 0;
		double distDown = 0;
		double distLeft = 0;
		double distRight = 0;

		if (!blockedUp) {
			distUp = world()->distSqNearestZombie(this, x, y + 2);
		}
		if (!blockedDown) {
			distDown = world()->distSqNearestZombie(this, x, y - 2);
		}
		if (!blockedRight) {
			distRight = world()->distSqNearestZombie(this, x + 2, y);
		}
		if (!blockedLeft) {
			distLeft = world()->distSqNearestZombie(this, x - 2, y);
		}

		double max = distUp;

		if (distDown > max)
			max = distDown;

		if (distRight > max)
			max = distRight;

		if (distLeft > max)
			max = distLeft;

		if (max <= distSqZ)
			return;

		if (max == distUp) {
			setDirection(up);
			moveTo(getX(), getY() + 2);
			return;
		}

		if (max == distDown) {
			setDirection(down);
			moveTo(getX(), getY() - 2);
			return;
		}

		if (max == distLeft) {
			setDirection(left);
			moveTo(getX() - 2, getY());
			return;
		}

		setDirection(right);
		moveTo(getX() + 2, getY());
		return;
	}
}

///////////////////////////////////////

/////////////////////////////////////////

// Implementation of Zombie functions //

Zombie::Zombie(double startX, double startY, StudentWorld* sw) : Biped(IID_ZOMBIE, startX, startY, sw) {}

int Zombie::movement() {
	return m_movementPlanDistance;
}

void Zombie::setMovement(int num) {
	m_movementPlanDistance = num;
}

void Zombie::moved() {
	m_movementPlanDistance--;
}

void Zombie::decideToVomit() {
	Direction dir = getDirection();

	double vomitX = getX();
	double vomitY = getY();

	switch (dir) {
	case right: {
		vomitX += SPRITE_WIDTH;
		break;
	}
	case up: {
		vomitY += SPRITE_HEIGHT;
		break;
	}
	case left: {
		vomitX -= SPRITE_WIDTH;
		break;
	}
	case down: {
		vomitY -= SPRITE_HEIGHT;
		break;
	}
	}

	if (world()->someoneToVomitOn(vomitX, vomitY)) {
		if (randInt(1, 3) == 1) {
			world()->addActor(new Vomit(vomitX, vomitY, dir, world()));
			world()->playSound(SOUND_ZOMBIE_VOMIT);
			return;
		}
	}
}

////////////////////////////////////////

// Implementation of DumbZombie functions //

DumbZombie::DumbZombie(double startX, double startY, StudentWorld* sw) : Zombie(startX, startY, sw) {
	//if (randInt(1, 10) == 1) {
	//	m_hasVaccine = true;
	//}
	//else {
	//	m_hasVaccine = false;
	//}
	m_hasVaccine = true;
}

void DumbZombie::doSomething() {
	if (isDead()) {
		return;
	}

	lived();

	if (paralyzed()) {
		return;
	}

	decideToVomit();

	if (movement() == 0) {
		setMovement(randInt(3, 10));
		int dir = randInt(1, 4);
		switch (dir) {
		case(1):
			setDirection(right);
			break;
		case(2):
			setDirection(up);
			break;
		case(3):
			setDirection(left);
			break;
		case(4):
			setDirection(down);
			break;
		}
	}

	double x = getX();
	double y = getY();

	switch (getDirection()) {
	case(right):
		if (!world()->blocked(this, x + 1, y)) {
			moveTo(x + 1, y);
			moved();
			return;
		}
		break;
	case(up):
		if (!world()->blocked(this, x, y + 1)) {
			moveTo(x, y + 1);
			moved();
			return;
		}
		break;
	case(left):
		if (!world()->blocked(this, x - 1, y)) {
			moveTo(x - 1, y);
			moved();
			return;
		}
		break;
	case(down):
		if (!world()->blocked(this, x, y - 1)) {
			moveTo(x, y - 1);
			moved();
			return;
		}
		break;
	}
	
	setMovement(0);

}

void DumbZombie::specializedDeath() {
	world()->playSound(SOUND_ZOMBIE_DIE);
	world()->increaseScore(1000);
	if (m_hasVaccine) {
		int randomDirection = randInt(1, 4);
		double vaccineX = getX();
		double vaccineY = getY();
		switch (randomDirection) {
		case 1:
			vaccineX += SPRITE_WIDTH;
			break;
		case 2:
			vaccineY += SPRITE_HEIGHT;
			break;
		case 3:
			vaccineX -= SPRITE_WIDTH;
			break;
		case 4:
			vaccineY -= SPRITE_HEIGHT;
			break;
		}
		if (world()->isVaccineBlocked(vaccineX, vaccineY)) {
			return;
		}
		world()->addActor(new Vaccine(vaccineX, vaccineY, world()));
	}
}

///////////////////////////////////////////

// Implementation of SmartZombie functions//

SmartZombie::SmartZombie(double startX, double startY, StudentWorld* w) : Zombie(startX, startY, w) {}

void SmartZombie::doSomething() {
	if (isDead())
		return;

	lived();

	if (paralyzed())
		return;

	decideToVomit();

	double x = getX();
	double y = getY();

	if (movement() == 0) {
		setMovement(randInt(3, 10));

		Actor* closestHuman = world()->nearestHuman(this, x, y);

		double hX = closestHuman->getX();
		double hY = closestHuman->getY();

		double distanceSquared = world()->distSquared(x, y, hX, hY);

		if (distanceSquared <= 80 * 80) {
			if (x == hX) {
				if (y < hY)
					setDirection(up);
				else
					setDirection(down);
			}
			else if (y == hY) {
				if (x < hX)
					setDirection(right);
				else
					setDirection(left);
			}
			else {
				Direction horizontal = 0;
				Direction vertical = 0;

				if (x < hX)
					horizontal = right;
				if (x > hX)
					horizontal = left;

				if (y < hY)
					vertical = up;
				if (y > hY)
					vertical = down;

				if (randInt(0, 1) == 0)
					setDirection(horizontal);
				else
					setDirection(vertical);
			}
		}
		else {
			int dir = randInt(1, 4);
			switch (dir) {
			case 1:
				setDirection(right);
				break;
			case 2:
				setDirection(up);
				break;
			case 3:
				setDirection(left);
				break;
			case 4:
				setDirection(down);
				break;
			}
		}

	}


	bool blockedUp = world()->blocked(this, x, y + 1);
	bool blockedDown = world()->blocked(this, x, y - 1);
	bool blockedLeft = world()->blocked(this, x - 1, y);
	bool blockedRight = world()->blocked(this, x + 1, y);

	Direction dir = getDirection();

	switch (dir) {
	case right: {
		if (!blockedRight) {
			moveTo(x + 1, y);
			moved();
			return;
		}
		setMovement(0);
		break;
	}
	case up: {
		if (!blockedUp) {
			moveTo(x, y + 1);
			moved();
			return;
		}
		setMovement(0);
		break;
	}
	case left: {
		if (!blockedLeft) {
			moveTo(x - 1, y);
			moved();
			return;
		}
		setMovement(0);
		break;
	}
	case down: {
		if (!blockedDown) {
			moveTo(x, y - 1);
			moved();
			return;
		}
		setMovement(0);
		break;
	}
	}

}

void SmartZombie::specializedDeath() {
	world()->playSound(SOUND_ZOMBIE_DIE);
	world()->increaseScore(2000);
}