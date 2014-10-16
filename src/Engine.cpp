#include "main.hpp"

Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP), fovRadius(10), screenWidth(screenWidth), screenHeight(screenHeight) {
	TCODConsole::initRoot(80,50,"libtcod C++ tutorial",false);
	player = new Actor(40,25,'@',"player",TCODColor::white);
	player->destructible=new PlayerDestructible(30,2,"your cadaver");
	player->attacker=new Attacker(5);
	player->ai = new PlayerAi();
	player->container = new Container(26);
	actors.push(player);
	map = new Map(80,43);
	gui = new Gui();
	gui->message(TCODColor::red, "Welcome stranger!\n Prepare to die, again.");

}

Engine::~Engine() {
	actors.clearAndDelete();
	delete map;
	delete gui;
}

void Engine::update() {
	if ( gameStatus == STARTUP ) map->computeFov();
	gameStatus=IDLE;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE,&lastKey,&mouse);
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
		if ( map->isInFov(actor->x,actor->y) ) {
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

