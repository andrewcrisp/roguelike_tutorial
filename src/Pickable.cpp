#include "main.hpp"

bool Pickable::pick(Actor *owner, Actor *wearer) {
	if (wearer->container && wearer->container->add(owner) ) {
		engine.actors.remove(owner);
		return true;
	}
	return false;
}

bool Pickable::use(Actor *owner, Actor *wearer) {
	if ( wearer->container ) {
		wearer->container->remove(owner);
		delete owner;
		return true;
	}
	return false;
}

Healer::Healer(float amount) : amount(amount) {
}

bool Healer::use(Actor *owner, Actor *wearer) {
	if ( wearer->destructible ) {
		float amountHealed = wearer->destructible->heal(amount);
		if ( amountHealed > 0 ) {
			return Pickable::use(owner,wearer);
		}
	}
	return false;
}

LightningBolt::LightningBolt(float range, float damage) : range(range),damage(damage) {
}

bool LightningBolt::use(Actor *owner, Actor *wearer) {
	Actor *closestMonster=engine.getClosestMonster(wearer->x,wearer->y,range);
	if (! closestMonster ) {
		engine.gui->message(TCODColor::lightGrey, "No enemy is close enough to strike.");
		return false;
	}
	engine.gui->message(TCODColor::lightBlue,
			"A lightning bolt strikes the %s with a loud thunder!\n"
			"The damage is %g hit points.",
			closestMonster->name,damage);
	closestMonster->destructible->takeDamage(closestMonster,damage);
	return Pickable::use(owner,wearer);
}

Fireball::Fireball(float range, float damage) : LightningBolt(range,damage) {
}

bool Fireball::use(Actor *owner, Actor *wearer) {
	engine.gui->message(TCODColor::cyan, "Left-click a target tile for the fireball,\nor right-click to cancel.");
	int x,y;
	if (! engine.pickATile(&x,&y)) {
		return false;
	}
	engine.gui->message(TCODColor::orange,"The fireball explodes, burning everything within %g tiles.",range);
	for (Actor **iterator=engine.actors.begin();
			iterator != engine.actors.end(); iterator++) {
		Actor *actor=*iterator;
		if ( actor->destructible && !actor->destructible->isDead() && actor->getDistance(x,y) <= range) {
			engine.gui->message(TCODColor::orange,"The %s gets burned for %g hit points.",actor->name,damage);
			actor->destructible->takeDamage(actor,damage);
		}
	}
	return Pickable::use(owner,wearer);
}



