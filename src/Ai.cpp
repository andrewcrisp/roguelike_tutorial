#include <stdio.h>
#include <math.h>
#include "main.hpp"

static const int TRACKING_TURNS=3;

void PlayerAi::load(TCODZip &zip) {
}

void PlayerAi::save(TCODZip &zip) {
	zip.putInt(PLAYER);
}

Ai *Ai::create(TCODZip &zip) {
	AiType type=(AiType)zip.getInt();
	Ai *ai=NULL;
	switch(type) {
		case PLAYER : ai = new PlayerAi(); break;
		case MONSTER : ai = new MonsterAi();break;
		case CONFUSED_MONSTER : ai = new ConfusedMonsterAi(0,NULL); break;
	}
	ai->load(zip);
	return ai;
}

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
		case TCODK_CHAR : handleActionKey(owner, engine.lastKey.c); break;
		default:break;
	}
	if (dx != 0 || dy != 0) {
		engine.gameStatus=Engine::NEW_TURN;
		if (moveOrAttack(owner, owner->x+dx,owner->y+dy)) {
			engine.map->computeFov();
		}
	}
}

Actor *PlayerAi::choseFromInventory(Actor *owner) {
	static const int INVENTORY_WIDTH=50;
	static const int INVENTORY_HEIGHT=28;
	static TCODConsole con(INVENTORY_WIDTH,INVENTORY_HEIGHT);
	
	con.setDefaultForeground(TCODColor(200,180,50));
	con.printFrame(0,0,INVENTORY_WIDTH,INVENTORY_HEIGHT);
	con.setDefaultForeground(TCODColor::white);
	int shortcut='a';
	int y=1;
	for (Actor **it=owner->container->inventory.begin();
			it != owner->container->inventory.end();
			it++) {
		Actor *actor=*it;
		con.print(2,y,"(%c) %s", shortcut, actor->name);
		y++;
		shortcut++;
	}
	TCODConsole::blit(&con, 0,0,INVENTORY_WIDTH,INVENTORY_HEIGHT,TCODConsole::root,engine.screenWidth/2 - INVENTORY_WIDTH/2, engine.screenHeight/2-INVENTORY_HEIGHT/2);
	TCODConsole::flush();
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL,true);
	if ( key.vk == TCODK_CHAR ) {
		int actorIndex=key.c - 'a';
		if ( actorIndex >= 0 && actorIndex < owner->container->inventory.size() ) {
			return owner->container->inventory.get(actorIndex);
		}
	}
	return NULL;
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
		case 'd' :
				   {
					   Actor *actor=choseFromInventory(owner);
					   if ( actor ) {
						   actor->pickable->drop(actor,owner);
						   engine.gameStatus=Engine::NEW_TURN;
					   }
				   }
				   break;
		case 'g' : 
		{
			bool found=false;
			for (Actor **iterator=engine.actors.begin();
					iterator != engine.actors.end();
					iterator ++) {
				Actor *actor=*iterator;
				if (actor->pickable && 
						actor->x == owner->x && 
						actor->y == owner->y) {
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
		case 'i' :
		{
			Actor *actor = choseFromInventory(owner);
			if ( actor ) {
				actor->pickable->use(actor,owner);
				engine.gameStatus=Engine::NEW_TURN;
			}
		} 
		break;
		case '>' :
		{
			if (engine.stairs->x == owner->x && engine.stairs->y == owner->y ) {
				engine.nextLevel();
			} else {
				engine.gui->message(TCODColor::lightGrey,"There are no stairs here.");
			}
		}
		break;
	}
}

MonsterAi::MonsterAi() : moveCount(0) {
}

void MonsterAi::load(TCODZip &zip) {
	moveCount=zip.getInt();
}

void MonsterAi::save(TCODZip &zip) {
	zip.putInt(MONSTER);
	zip.putInt(moveCount);
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

ConfusedMonsterAi::ConfusedMonsterAi(int nbTurns, Ai *oldAi) : nbTurns(nbTurns),oldAi(oldAi) {
}

void ConfusedMonsterAi::load(TCODZip &zip) {
	nbTurns=zip.getInt();
	oldAi=Ai::create(zip);
}

void ConfusedMonsterAi::save(TCODZip &zip) {
	zip.putInt(CONFUSED_MONSTER);
	zip.putInt(nbTurns);
	oldAi->save(zip);
}

void ConfusedMonsterAi::update(Actor *owner) {
	TCODRandom *rng = TCODRandom::getInstance();
	int dx=rng->getInt(-1,1);
	int dy=rng->getInt(-1,1);

	if ( dx != 0 || dy != 0 ) {
		int destx=owner->x+dx;
		int desty=owner->y+dy;
		if ( engine.map->canWalk(destx, desty) ) {
			owner->x = destx;
			owner->y = desty;
		} else {
			Actor *actor=engine.getActor(destx, desty);
			if ( actor ) {
				owner->attacker->attack(owner, actor);
			}
		}
	}
	nbTurns--;
	if ( nbTurns == 0 ) {
		owner->ai = oldAi;
		delete this;
	}
}

