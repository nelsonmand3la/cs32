#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include "Level.h"
#include "Actor.h"
#include <iostream>
#include <sstream>
#include <iomanip>
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
	int result = drawLevel();
	if (result == 1) {
		return GWSTATUS_PLAYER_WON;
	}
	if (result == -1) {
		return GWSTATUS_LEVEL_ERROR;
	}
	levelFinished = false;
	updateGameStats();
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	for (list<Actor*>::iterator it = m_v.begin(); it != m_v.end(); it++) {
		(*it)->doSomething();
	}
	m_hero->doSomething();

	for (list<Actor*>::iterator it = m_v.begin(); it != m_v.end(); it++) {
		if ((*it)->isDead()) {
			it--;
			list<Actor*>::iterator temp = it;
			it++;
			delete (*it);
			m_v.erase(it);
			it = temp;
			break;
		}
	}

	if (m_hero->isDead()) {
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	if (levelFinished) {
		levelFinished = false;
		playSound(SOUND_LEVEL_FINISHED);
		return GWSTATUS_FINISHED_LEVEL;
	}
	updateGameStats();
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	while (m_v.size() != 0) {
		list<Actor*>::iterator it = m_v.begin();
		delete *it;
		m_v.erase(it);
	}
	delete m_hero;
}

int StudentWorld::drawLevel()
{
	Level lev(assetPath());
	ostringstream oss;
	oss << "level0";
	oss << getLevel();
	oss << ".txt";
	string levelFile = oss.str();
	Level::LoadResult result = lev.loadLevel(levelFile);
	if (result == Level::load_fail_file_not_found)
		return 1;
	else if (result == Level::load_fail_bad_format)
		return -1;
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
				case Level::citizen: {
					Actor* temp = new Citizen(j * 16, i * 16, this);
					m_citizens++;
					m_v.push_back(temp);
					break;
				}
				case Level::smart_zombie: {
					Actor* temp = new SmartZombie(j * 16, i * 16, this);
					m_v.push_back(temp);
					break;
				}
				case Level::dumb_zombie: {
					Actor* temp = new DumbZombie(j * 16, i * 16, this);
					m_v.push_back(temp);
					break;
				}
				case Level::player: {
					m_hero = new Penelope(j * 16, i * 16, this);
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
				case Level::gas_can_goodie: {
					Actor* temp = new GasCan(j * 16, i * 16, this);
					m_v.push_back(temp);
					break;
				}
				case Level::vaccine_goodie: {
					Actor* temp = new Vaccine(j * 16, i * 16, this);
					m_v.push_back(temp);
					break;
				}
				case Level::landmine_goodie: {
					Actor* temp = new LandmineGoodie(j * 16, i * 16, this);
					m_v.push_back(temp);
					break;
				}
				}
			}

		}
	}
	return 0;
}

bool StudentWorld::blocked(Actor* a, double destX, double destY) {
	for (list<Actor*>::iterator it = m_v.begin(); it != m_v.end(); it++) {
		if ((*it)->blocksMovement()) {
			double otherObjX = (*it)->getX();
			double otherObjY = (*it)->getY();
			if (a == (*it) || destX + SPRITE_WIDTH - 1 < otherObjX || otherObjX + SPRITE_WIDTH - 1 < destX || destY > otherObjY + SPRITE_HEIGHT - 1 || destY + SPRITE_HEIGHT - 1 < otherObjY)
				continue;
			return true;
		}
	}

	if (a == (m_hero) || destX + SPRITE_WIDTH - 1 < getHeroX() || getHeroX() + SPRITE_WIDTH - 1 < destX || destY > getHeroY() + SPRITE_HEIGHT - 1 || destY + SPRITE_HEIGHT - 1 < getHeroY())
		return false;

	return true;
}

void StudentWorld::activateOnOverlap(Actor* a) {
	for (list<Actor*>::iterator it = m_v.begin(); it != m_v.end(); it++) {
		if ((*it) == a || (*it)->isDead())
			continue;
		if (isOverlap(a->getX(), a->getY(), (*it)->getX(), (*it)->getY()))
			a->activateIfAppropriate(*it);
	}

	if (isOverlap(a->getX(), a->getY(), getHeroX(), getHeroY()))
		a->activateIfAppropriate(m_hero);
}

bool StudentWorld::isFlameBlocked(double startX, double startY) {
	for (list<Actor*>::iterator it = m_v.begin(); it != m_v.end(); it++) {
		if ((*it)->blocksFlame()) {
			if (isOverlap(startX, startY, (*it)->getX(), (*it)->getY()))
				return true;
		}
	}
	return false;
}

bool StudentWorld::isOverlap(double objX, double objY, double otherObjX, double otherObjY) {
	double objCenterX = objX + SPRITE_WIDTH / 2;
	double objCenterY = objY + SPRITE_HEIGHT / 2;
	double otherObjCenterX = otherObjX + SPRITE_WIDTH / 2;
	double otherObjCenterY = otherObjY + SPRITE_HEIGHT / 2;
	double deltaX = objCenterX - otherObjCenterX;
	double deltaY = objCenterY - otherObjCenterY;
	if (deltaX * deltaX + deltaY * deltaY <= 100) {
		return true;
	}
	return false;
}

void StudentWorld::finishLevel() {
	levelFinished = true;
}

void StudentWorld::lostCitizen() {
	m_citizens--;
}

bool StudentWorld::citizensGone() {
	return m_citizens == 0;
}

void StudentWorld::addActor(Actor* a) {
	m_v.push_back(a);
}

double StudentWorld::distSquared(double aX, double aY, double bX, double bY) {
	double cmAx = aX + SPRITE_WIDTH / 2;
	double cmAy = aY + SPRITE_HEIGHT / 2;
	double cmBx = bX + SPRITE_WIDTH / 2;
	double cmBy = bY + SPRITE_HEIGHT / 2;
	double deltaX = cmBx - cmAx;
	double deltaY = cmBy - cmAy;
	return ((deltaX * deltaX) + (deltaY * deltaY));
}

double StudentWorld::distSqPenelope(Actor* a) {
	double aX = a->getX();
	double aY = a->getY();
	return distSquared(aX, aY, getHeroX(), getHeroY());
}

double StudentWorld::distSqNearestZombie(Actor* a, double aX, double aY) {
	double minDist = -1;
	list<Actor*>::iterator it = m_v.begin();
	while (it != m_v.end()) {
		Actor* b = *it;

		if (a == b) {
			it++;
			continue;
		}

		if (b->triggersZombieVomit()) {
			it++;
			continue;
		}

		if (b->blocksFlame()) {
			it++;
			continue;
		}

		if (!b->triggersOnlyActiveLandmines()) {
			it++;
			continue;
		}

		double newDist = distSquared(aX, aY, b->getX(), b->getY());

		if (minDist == -1)
			minDist = newDist;

		if (minDist > newDist)
			minDist = newDist;

		it++;
	}
	return minDist;
}

Actor* StudentWorld::nearestHuman(Actor* a, double aX, double aY) {
	double minDist = distSqPenelope(a);
	Actor* closestHuman = m_hero;
	list<Actor*>::iterator it = m_v.begin();
	while (it != m_v.end()) {
		Actor* b = *it;

		if (a == b) {
			it++;
			continue;
		}

		if (!b->triggersOnlyActiveLandmines()) {
			it++;
			continue;
		}

		double newDist = distSquared(aX, aY, b->getX(), b->getY());

		if (minDist > newDist) {
			minDist = newDist;
			closestHuman = b;
		}

		it++;
	}
	return closestHuman;
}

double StudentWorld::getHeroX() {
	return m_hero->getX();
}

double StudentWorld::getHeroY() {
	return m_hero->getY();
}

bool StudentWorld::isVaccineBlocked(double startX, double startY) {
	for (list<Actor*>::iterator it = m_v.begin(); it != m_v.end(); it++) {
		Actor* a = (*it);
		if (isOverlap(startX, startY, a->getX(), a->getY())) {
			return true;
		}
	}
	return false;
}

bool StudentWorld::someoneToVomitOn(double x, double y) {
	if (distSquared(x, y, getHeroX(), getHeroY()) <= 10 * 10) {
		return true;
	}

	for (list<Actor*>::iterator it = m_v.begin(); it != m_v.end(); it++) {
		Actor* b = (*it);
		if (b->triggersZombieVomit()) {
			double bX = b->getX();
			double bY = b->getY();
			double distanceSquared = distSquared(x, y, bX, bY);
			if (distanceSquared <= 10 * 10) {
				return true;
			}
		}
	}
	return false;
}

void StudentWorld::giveFlames() {
	m_hero->receiveFlames();
	updateGameStats();
}

void StudentWorld::giveMines() {
	m_hero->receiveMines();
	updateGameStats();
}

void StudentWorld::giveVaccines() {
	m_hero->receiveVaccines();
	updateGameStats();
}

void StudentWorld::updateGameStats() {
	//Score: 004500  Level: 27  Lives: 3  Vaccines: 2  Flames: 16  Mines: 1  Infected: 0
	ostringstream gameStats;
	gameStats << "Score: ";
	gameStats.fill('0');
	gameStats << setw(6) << getScore();
	gameStats << "  Level: ";
	gameStats << getLevel();
	gameStats << "  Lives: ";
	gameStats << getLives();
	gameStats << "  Vaccines: ";
	gameStats << m_hero->getVacc();
	gameStats << "  Flames: ";
	gameStats << m_hero->getFlames();
	gameStats << "  Mines: ";
	gameStats << m_hero->getMines();
	gameStats << "  Infected: ";
	gameStats << m_hero->infectionCount();
	setGameStatText(gameStats.str());
}