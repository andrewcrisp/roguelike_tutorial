#include "main.hpp"

Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP), player(NULL), map(NULL), fovRadius(10), screenWidth(screenWidth), screenHeight(screenHeight),level(1) {
	TCODConsole::initRoot(80,50,"libtcod C++ tutorial",false);
	gui = new Gui();

}

Engine::~Engine() {
	term();
	delete gui;
}

void Engine::init() {
	player = new Actor(40,25,'@',"player",TCODColor::white);
	player->destructible = new PlayerDestructible(30,2,"your cadaver",0);
	player->attacker = new Attacker(5);
	player->ai = new PlayerAi();
	player->container = new Container(26);
	actors.push(player);
	stairs = new Actor(0,0,'>',"stairs",TCODColor::white);
	stairs->blocks=false;
	stairs->fovOnly=false;
	actors.push(stairs);
	map = new Map(80,43);
	map->init(true);
	gui->message(TCODColor::red,"Welcome to the dungeon.");
	gameStatus=STARTUP;
}

void Engine::nextLevel() {
	level++;
	gui->message(TCODColor::lightViolet,"You find the time to rest and recover.");
	player->destructible->heal(player->destructible->maxHp/2);
	gui->message(TCODColor::red,"You descend further into the dungeon.");
	delete map;
	for (Actor **it=actors.begin(); it!=actors.end(); it++) {
		if ( *it != player && *it != stairs ) {
			delete *it;
			it = actors.remove(it);
		}
	}
	map = new Map(80,43);
	map->init(true);
	gameStatus=STARTUP;
}

void Engine::term() {
	actors.clearAndDelete();
	if (map ) delete map;
	gui->clear();
}

void Engine::update() {
	if ( gameStatus == STARTUP ) map->computeFov();
	gameStatus=IDLE;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE,&lastKey,&mouse);
	if ( lastKey.vk == TCODK_ESCAPE ) {
		save();
		load();
	}
	player->update();
	if (gameStatus == NEW_TURN ) {
		for (Actor **iterator=actors.begin(); iterator != actors.end(); iterator++) {
			Actor *actor = *iterator;
			if ( actor != player ) {
				actor->update();
			}
		}
	}
}

void Engine::render() {
	TCODConsole::root->clear();
	map->render();
	player->render();
	gui->render();
	TCODConsole::root->print(1,screenHeight-2, "HP : %d/%d",(int)player->destructible->hp,(int)player->destructible->maxHp);
	for (Actor **iterator=actors.begin();iterator!= actors.end(); iterator++) {
		Actor *actor=*iterator;
		if ( actor != player
				&& ((!actor->fovOnly && map->isExplored(actor->x,actor->y))
					|| map->isInFov(actor->x,actor->y)) ) {
			actor->render();
		}
	}
		
}

void Engine::sendToBack(Actor *actor) {
	actors.remove(actor);
	actors.insertBefore(actor,0);
}

Actor *Engine::getClosestMonster(int x, int y, float range) const {
	Actor *closest=NULL;
	float bestDistance=1E6f;
	for (Actor **iterator=actors.begin();
			iterator != actors.end();
			iterator++) {
		Actor *actor = *iterator;
		if ( actor != player && actor->destructible && !actor->destructible->isDead() ) {
			float distance=actor->getDistance(x,y);
			if ( distance < bestDistance && ( distance <= range || range == 0.0f ) ) {
				bestDistance=distance;
				closest=actor;
			}
		}
	}
	return closest;
}

bool Engine::pickATile(int *x, int *y, float maxRange){
	while ( !TCODConsole::isWindowClosed() ) {
		render();
		for (int cx=0; cx < map->width; cx++) {
			for (int cy=0; cy < map->height; cy++) {
				if ( map->isInFov(cx,cy) 
						&& ( maxRange == 0 
							|| player->getDistance(cx,cy) <= maxRange) ) {
					TCODColor col=TCODConsole::root->getCharBackground(cx,cy);
					col = col * 1.2f;
					TCODConsole::root->setCharBackground(cx,cy,col);
				}
			}
		}
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE,&lastKey,&mouse);
		if ( map->isInFov(mouse.cx,mouse.cy)
				&& ( maxRange==0
					|| player->getDistance(mouse.cx,mouse.cy) <=maxRange )) {
			TCODConsole::root->setCharBackground(mouse.cx,mouse.cy,TCODColor::white);
			if ( mouse.lbutton_pressed ) {
				*x=mouse.cx;
				*y=mouse.cy;
				return true;
			}
		}
		if (mouse.rbutton_pressed || lastKey.vk != TCODK_NONE) {
			return false;
		}
		TCODConsole::flush();
	}
	return false;
}

Actor *Engine::getActor(int x, int y) const {
	for (Actor **iterator=actors.begin();
			iterator != actors.end(); iterator++) {
		Actor *actor=*iterator;
		if ( actor->x == x && actor->y == y && actor->destructible && ! actor->destructible->isDead()) {
			return actor;
		}
	}
	return NULL;
}

