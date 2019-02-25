#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class Goodie;
class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
	Actor(int imageID, double x, double y, Direction dir, int depth, StudentWorld* w);

	// Action to perform for each tick.
	virtual void doSomething() = 0;

	// Is this actor dead?
	bool isDead() const;

	// Mark this actor as dead.
	void setDead();

	// Get this actor's world
	StudentWorld* world() const;

	// If this is an activated object, perform its effect on a (e.g., for an
	// Exit have a use the exit).
	virtual void activateIfAppropriate(Actor* a);

	// If this object uses exits, use the exit.
	virtual void useExitIfAppropriate();

	// If this object can die by falling into a pit or burning, die.
	virtual void dieByFallOrBurnIfAppropriate();

	// If this object can be infected by vomit, get infected.
	virtual void beVomitedOnIfAppropriate();

	// If this object can pick up goodies, pick up g
	virtual bool pickUpGoodieIfAppropriate(Goodie* g);

	// Can this object cause a zombie to vomit?
	virtual bool triggersZombieVomit() const;

	// Does this object block agent movement?
	virtual bool blocksMovement() const;

	// Does this object block flames?
	virtual bool blocksFlame() const;

	// Does this object trigger landmines only when they're active?
	virtual bool triggersOnlyActiveLandmines() const;

private:

	StudentWorld* m_sw;
	
	bool m_isDead;

};

class Wall : public Actor {
public:
	// Walls only need to know where to start
	Wall(double startX, double startY);

	// Walls do nothing
	void doSomething();

	// Walls block flames
	bool blocksFlame() const;
};

class ActivatingObject : public Actor {
public:
	
	ActivatingObject(int imageID, double startX, double startY, Direction dir, int depth, StudentWorld* w);

	bool blocksMovement() const;

	bool blocksFlame() const;

};

class Flame : public ActivatingObject {
public:
	// Flames need to know which direction to start in
	Flame(double startX, double startY, Direction dir, StudentWorld* w);

	void doSomething();

	void activateIfAppropriate(Actor* a);

private:

	int m_ticksAlive = 0;

};

class Vomit : public ActivatingObject {
public:

	Vomit(double startX, double startY, Direction dir, StudentWorld* w);

	void doSomething();

	void activateIfAppropriate(Actor* a);

private:

	int m_ticksAlive = 0;

};

class Landmine : public ActivatingObject {
public:
	Landmine(double startX, double startY, StudentWorld* w);

	void doSomething();

	void activateIfAppropriate(Actor* a);

	void dieByFallOrBurnIfAppropriate();

private:
	int m_safetyTicks = 30;

	bool m_active = false;
};

class Exit : public ActivatingObject {
public:

	Exit(double startX, double startY, StudentWorld* w);

	void activateIfAppropriate(Actor* a);

	bool blocksFlame() const;

	void doSomething();
};

class Biped : public Actor {
public:

	Biped(int imageID, double startX, double startY, StudentWorld* w);

	void lived();

	bool paralyzed();

	bool triggersOnlyActiveLandmines() const;

	void dieByFallOrBurnIfAppropriate();

	virtual void specializedDeath() = 0;

private:

	int m_ticksAlive = 0;
};

class Human : public Biped {
public:

	Human(int imageID, double startX, double startY, StudentWorld* w);

	void beVomitedOnIfAppropriate();

	bool isInfected();

	void getInfected();

	void getCured();

	void infectionSpreads();

	int infectionCount();

	bool triggersZombieVomit() const;

private:

	bool m_infected = false;
	
	int m_infectionCounter = 0;
};

class Penelope : public Human {
public:
	Penelope(double startX, double startY, StudentWorld* w);

	void useExitIfAppropriate();

	bool pickUpGoodieIfAppropriate(Goodie* g);

	void doSomething();

	int getFlames();

	int getMines();

	int getVacc();

	void receiveFlames();

	void receiveMines();

	void receiveVaccines();

	void specializedDeath();

private:
	// How many landmines does Penelope have?
	int m_landmines = 0;

	// How many flames does Penelope have?
	int m_flames = 0;

	// How many vaccines does Penelope have?
	int m_vaccines = 0;
};

class Citizen : public Human {
public:

	Citizen(double startX, double startY, StudentWorld* w);

	void doSomething();

	void specializedDeath();

	void useExitIfAppropriate();

};


class Zombie : public Biped {
public:

	Zombie(double startX, double startY, StudentWorld* w);

	int movement();

	void setMovement(int num);

	void moved();

	void decideToVomit();

private:

	int m_movementPlanDistance = 0;

};

class DumbZombie : public Zombie {
public:
	DumbZombie(double startX, double startY, StudentWorld* w);

	void doSomething();

	void specializedDeath();

private:
	bool m_hasVaccine;

};

class SmartZombie : public Zombie {
public:
	SmartZombie(double startX, double startY, StudentWorld* w);

	void specializedDeath();

	void doSomething();
};

class Goodie : public ActivatingObject {
public:

	Goodie(int imageID, double startX, double startY, StudentWorld* w);

	bool blocksMovement();

	void activateIfAppropriate(Actor* a);

	virtual void changeGameScore() = 0;

	void dieByFallOrBurnIfAppropriate();
	
};

class GasCan : public Goodie {
public:

	GasCan(double startX, double startY, StudentWorld* w);

	void doSomething();

	void changeGameScore();

};

class Vaccine : public Goodie {
public:

	Vaccine(double startX, double startY, StudentWorld* w);

	void doSomething();

	void changeGameScore();

};

class LandmineGoodie : public Goodie {
public:

	LandmineGoodie(double startX, double startY, StudentWorld* w);

	void doSomething();

	void changeGameScore();
};

#endif // ACTOR_H_
