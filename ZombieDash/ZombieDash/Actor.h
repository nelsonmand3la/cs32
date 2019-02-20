#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
public:
	Actor(int imageID, double startX, double startY, Direction dir, int depth, bool block, StudentWorld* sw = nullptr);
	virtual void doSomething() = 0;
	bool blockable() const;
	bool alive() const;
	void die();
	StudentWorld* getWorld() const;
	void playSound(int soundID);
	void changeScore(int amount);
private:
	bool m_isBlockable;
	StudentWorld* m_sw;
	bool m_isAlive = true;
};

class MovableObject : public Actor {
public:
	MovableObject(int imageID, double startX, double startY, StudentWorld* sw);
};

class ImmovableObject : public Actor {
public:
	ImmovableObject(int imageID, double startX, double startY, int depth, bool block, StudentWorld* sw);
};

class Human : public MovableObject {
public:
	Human(int imageID, double startX, double startY, StudentWorld* sw);
	bool infected();
	void infectionSpreads();
	int infectionCount();
private:
	bool m_isInfected = false;
	int m_infectionCounter = 0;
};

class Zombie : public MovableObject {
public:
	Zombie(double startX, double startY, StudentWorld* sw);
private:
	int m_movementPlanDistance = 0;
};

class Wall : public ImmovableObject {
public:
	Wall(double startX, double startY);
	void doSomething();
};

class Exit : public ImmovableObject {
public:
	Exit(double startX, double startY, StudentWorld* sw);
	void doSomething();
};

class Penelope : public Human {
public:
	Penelope(double startX, double startY, StudentWorld* sw);
	void doSomething();
private:
	int m_landmines = 0;
	int m_flames = 0;
	int m_vaccines = 0;
};

class Citizen : public Human {
public:
	Citizen(double startX, double startY);
	void doSomething();
};

class DumbZombie : public Zombie {
public:
	DumbZombie(double startX, double startY, StudentWorld* sw);
	void doSomething();
};
#endif // ACTOR_H_
