#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>
using namespace std;
class Actor;
class Penelope;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
	virtual ~StudentWorld();

    virtual int init();
    virtual int move();
    virtual void cleanUp();

	int drawLevel();

	void finishLevel();

	void updateGameStats();

	bool blocked(Actor* a, double destX, double destY);

	void activateOnOverlap(Actor* a);

	bool isOverlap(double objX, double objY, double otherObjX, double otherObjY);

	void lostCitizen();

	bool citizensGone();

	void giveFlames();

	void giveMines();

	void giveVaccines();

	void addActor(Actor* a);

	bool isFlameBlocked(double startX, double startY);

	bool isVaccineBlocked(double startX, double startY);

	double distSquared(double aX, double aY, double bX, double bY);

	double distSqPenelope(Actor* a);

	double distSqNearestZombie(Actor* a, double aX, double aY);

	Actor* nearestHuman(Actor* a, double aX, double aY);

	double getHeroX();
	
	double getHeroY();

	bool someoneToVomitOn(double x, double y);


private:
	list<Actor*> m_v;
	Penelope* m_hero;
	bool levelFinished;
	int m_citizens = 0;
};

#endif // STUDENTWORLD_H_
