#include <stdio.h>
#include <math.h>
#include "main.hpp"

static const int TRACKING_TURNS=3;

void PlayerAi::update(Actor *owner) {
	if ( owner->destructible && owner->destructible->isDead() ) {
		return;
	}
	int dx=0,dy=0;
	switch(engine.lastKey.vk) {
		case TCODK_UP : dy=-1; break;
		case TCODK_DOWN : dy=1; break;
		case TCODK_LEFT : dx=-1; break;
		case TCODK_RIGHT : dx=1;break;
		case TCODK_CHAR : handleActionKey(owner, engine.lastkey.c); break;
		default:break;
	}
	if (dx != 0 || dy != 0) {
		engine.gameStatus=Engine::NEW_TURN;
		if (moveOrAttack(owner, owner->x+dx,owner->y+dy)) {
			engine.map->computeFov();
		}
	}
}

bool PlayerAi::moveOrAttack(Actor *owner, int targetx, int targety) {
	if ( engine.map->isWall(targetx,targety) ) return false;
	for (Actor **iterator=engine.actors.begin();iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if ( actor->destructible && !actor->destructible->isDead() && actor->x == targetx && actor->y == targety ) {
			owner->attacker->attack(owner, actor);
			return false;
		}
	}
	for (Actor **iterator=engine.actors.begin();iterator != engine.actors.end(); iterator++) {
		Actor *actor=*iterator;
		bool corpseOrItem=(actor->destructible && actor->destructible->isDead()) || actor->pickable;
		if ( corpseOrItem && actor->destructible && actor->destructible->isDead() && actor->x == targetx && actor->y == targety ) {	
			engine.gui->message(TCODColor::lightGrey,"There is a %s here\n",actor->name);
		}
	}
	owner->x=targetx;
	owner->y=targety;
	return true;
}

void PlayerAi::handleActionKey(Actor *owner, int ascii) {
	switch(ascii) {
		case 'g' :
			{
				bool found=false;
				for (Actor **iterator=engine.actors.begin(); iterator != engine.actors.end(); iterator ++){
					actor *actor=*iterator;
					if (actor->pickable && actor->x == owner->x && actor->y == owner->y) {
						if (actor->pickable->pick(actor,owner)) {
							found=true;
							engine.gui->message(TCODColor::lightGrey,"You pick the %s.", actor->name);
							break;
						} else if ( ! found) {
							found=true;
							engine.gui->message(TCODColor::red, "Your inventory is full.");
						}
					}
				}
				if (!found) {
					engine.gui->message(TCODColor::lightGrey,"There is nothing here that you can pick up.");
				}
				engine.gameStatus=Engine::NEW_TURN;
			}
			break;
	}
}

MonsterAi::MonsterAi() : moveCount(0) {
}

void MonsterAi::update(Actor *owner) {
	if ( owner->destructible && owner->destructible->isDead() ) {
		return;
	}
	if ( engine.map->isInFov(owner->x,owner->y) ) {
		moveCount=TRACKING_TURNS;
	} else {
		moveCount--;
	}
	if ( moveCount > 0 ) {
		moveOrAttack(owner, engine.player->x, engine.player->y);
	}
}

void MonsterAi::moveOrAttack(Actor *owner, int targetx, int targety) {
	int dx = targetx - owner->x;
	int dy = targety - owner->y;
	int stepdx = (dx > 0 ? 1:-1);
	int stepdy = (dy > 0 ? 1:-1);
	float distance=sqrtf(dx*dx+dy*dy);
	if ( distance >= 2 ) {
		dx = (int)(round(dx/distance));
		dy = (int)(round(dy/distance));
		if ( engine.map->canWalk(owner->x+dx,owner->y+dy) ) {
			owner->x += dx;
			owner->y += dy;
		} else if ( engine.map->canWalk(owner->x+stepdx,owner->y) ) {
			owner->x += stepdx;
		} else if ( engine.map->canWalk(owner->x,owner->y+stepdy) ) {
			owner->y += stepdy;
		}
	} else if ( owner->attacker ) {
		owner->attacker->attack(owner, engine.player);
	}
}

