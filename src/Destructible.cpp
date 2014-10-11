#include <stdio.h>
#include "main.hpp"

Destructible::Destructibe(float maxHp, float defense, const char *corpseName) :
	maxHp(maxHp),hp(maxhp),defense(defense),corpseName(corpseName) {
}

float Destructible::takeDamage(Actor *owner, float damage) {
	damage -= defense;
	if ( damage > 0 ) {
		hp -= damage;
		if ( hp <= 0 ) {
			die(owner);
		}
	} else {
		damage = 0;
	}
	return damage;
}

void Destructible::die(Actor *owner) {
	//make into corpse
	owner->ch='%';
	owner->col=TCODColor::darkRed;
	owner->name=corpseName;
	owner->blocks=false;
	engine.sendToBack(owner);
}

MonsterDestructible::MonsterDestructible(float maxHp, float defense, const char *corpseName) : 
	Destructible(maxHp,defense,corpseName) {
}

void MonsterDestructible::die(Actor *owner) {
	printf ("%s is dead.\n",owner->name);
	Destructible::die(owner);
}


PlayerDestructible::PlayerDestructible(float maxHp, float defense, const char *corpseName) :
	Destruble(maxHp,defense,corpseName) {
}

void PlayerDestructible::die(Actor *owner) {
	printf ("You died!\n");
	engine.gameStatus=Engine::DEFEAT;
}

