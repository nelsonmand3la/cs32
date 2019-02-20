#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include "Level.h"
#include "Actor.h"
#include <iostream>
#include <sstream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

StudentWorld::~StudentWorld() {
	cleanUp();
}

int StudentWorld::init()
{
	drawLevel();
	levelFinished = false;
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	for (vector<Actor*>::iterator it = m_v.begin(); it != m_v.end(); it++) {
		(*it)->doSomething();
	}
	if (levelFinished) {
		levelFinished = false;
		return GWSTATUS_FINISHED_LEVEL;
	}
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	while (m_v.size() != 0) {
		vector<Actor*>::iterator it = m_v.begin();
		delete *it;
		m_v.erase(it);
	}
}

void StudentWorld::drawLevel()
{
	Level lev(assetPath());
	ostringstream oss;
	oss << "level0";
	oss << getLevel();
	oss << ".txt";
	string levelFile = oss.str();
	Level::LoadResult result = lev.loadLevel(levelFile);
	if (result == Level::load_fail_file_not_found)
		cerr << "Cannot find level01.txt data file" << endl;
	else if (result == Level::load_fail_bad_format)
		cerr << "Your level was improperly formatted" << endl;
	else if (result == Level::load_success)
	{
		cerr << "Successfully loaded level" << endl;
		for (int i = 0; i < LEVEL_HEIGHT; i++) {
			for (int j = 0; j < LEVEL_WIDTH; j++) {
				Level::MazeEntry ge = lev.getContentsOf(j, i); // level_x=5, level_y=10
				switch (ge) // so x=80 and y=160
				{
				case Level::empty:
					cout << "Location 80,160 is empty" << endl;
					break;
				case Level::smart_zombie:
					cout << "Location 80,160 starts with a smart zombie" << endl;
					break;
				case Level::dumb_zombie: {
					Actor* temp = new DumbZombie(j * 16, i * 16, this);
					m_v.push_back(temp);
					break;
				}
				case Level::player: {
					Actor* temp = new Penelope(j * 16, i * 16, this);
					m_v.push_back(temp);
					break;
				}
				case Level::exit: {
					Actor* temp = new Exit(j * 16, i * 16, this);
					m_v.push_back(temp);
					break;
				}
				case Level::wall: {
					Actor* temp = new Wall(j * 16, i * 16);
					m_v.push_back(temp);
					break;
				}
				case Level::pit:
					cout << "Location 80,160 has a pit in the ground" << endl;
					break;
					// etc…
				}
			}
		}
	}
}

bool StudentWorld::isBlocked(Actor* obj, double destX, double destY) {
	for (vector<Actor*>::iterator it = m_v.begin(); it != m_v.end(); it++) {
		if (!(*it)->blockable()) {
			continue;
		}
		double otherObjX = (*it)->getX();
		double otherObjY = (*it)->getY();
		if (obj == (*it) || destX + SPRITE_WIDTH - 1 < otherObjX || otherObjX + SPRITE_WIDTH - 1 < destX || destY > otherObjY + SPRITE_HEIGHT - 1 || destY + SPRITE_HEIGHT - 1 < otherObjY)
			continue;
		return true;
	}
	return false;
}

bool StudentWorld::isOverlap(Actor* obj) {
	double objCenterX = obj->getX() + SPRITE_WIDTH / 2;
	double objCenterY = obj->getY() + SPRITE_HEIGHT / 2;
	for (vector<Actor*>::iterator it = m_v.begin(); it != m_v.end(); it++) {
		if (*it == obj)
			continue;
		double otherObjCenterX = (*it)->getX() + SPRITE_WIDTH / 2;
		double otherObjCenterY = (*it)->getY() + SPRITE_HEIGHT / 2;
		double deltaX = objCenterX - otherObjCenterX;
		double deltaY = objCenterY - otherObjCenterY;
		if (deltaX * deltaX + deltaY * deltaY <= 100)
			return true;
	}
	return false;
}

void StudentWorld::finishLevel() {
	levelFinished = true;
}

void StudentWorld::addZombie(double startX, double startY) {
	if (randInt(1, 10) >= 7) {
		return;
	}
	Actor* temp = new DumbZombie(startX, startY, this);
	m_v.push_back(temp);
}