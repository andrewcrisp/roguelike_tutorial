#include "libtcod.hpp"
#include "Actor.hpp"
#include "Map.hpp"
#include "Engine.hpp"

Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP), fovRadius(10), screenWidth(screenWidth), screenHeight(screenHeight) {
	TCODConsole::initRoot(80,50,"libtcod C++ tutorial",false);
	player = new Actor(40,25,'@',"player",TCODColor::white);
	player->destructible=new PlayerDestructible(30,2,"your cadaver");
	player->attacker=new Attacker(5);
	player->ai = new playerAi();
	actors.push(player);
	map = new Map(80,45);
}

Engine::~Engine() {
	actors.clearAndDelete();
	delete map;
}

void Engine::update() {
	if ( gameStatus == STARTUP ) map->computeFov();
	gameStatus=IDLE;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS,&lastKey,NULL);
	player->update()
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
