#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>
using namespace std;
class Actor;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
	~StudentWorld();
    virtual int init();
    virtual int move();
	void drawLevel();
    virtual void cleanUp();
	bool isBlocked(Actor* obj, double destX, double destY);
	bool isOverlap(Actor* obj);
	void finishLevel();
	void addZombie(double startX, double startY);
private:
	vector<Actor*> m_v;
	bool levelFinished;
};

#endif // STUDENTWORLD_H_
