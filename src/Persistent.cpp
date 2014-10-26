#include "main.hpp"

void Engine::save(){
	if ( player->destructible->isDead() ) {
		TCODSystem::deleteFile("game.sav");
	} else {
		TCODZip zip;
		zip.putInt(map->width);
		zip.putInt(map->height);
		map->save(zip);
		player->save(zip);
		zip.putInt(actors.size()-1);
		for (Actor **it=actors.begin(); it!=actors.end(); it++) {
			if ( *it!=player ) {
				(*it)->save(zip);
			}
		}
		gui->save(zip);
		zip.saveToFile("game.sav");
	}
}

void Engine::load() {
	engine.gui->menu.clear();
	engine.gui->menu.addItem(Menu::NEW_GAME,"New game");
	if ( TCODSystem::fileExists("game.sav")) {
		engine.gui->menu.addItem(Menu::CONTINUE,"Continue");
	}
	engine.gui->menu.addItem(Menu::EXIT,"Exit");
	Menu::MenuItemCode menuItem=engine.gui->menu.pick();
	if ( menuItem == Menu::EXIT || menuItem == Menu::NONE ) {
		exit(0);
	} else if ( menuItem == Menu::NEW_GAME ) {
		engine.term();
		engine.init();
	} else {
		TCODZip zip;
		engine.term();
		zip.loadFromFile("game.sav");
		int width = zip.getInt();
		int height = zip.getInt();
		map = new Map(width,height);
		map->load(zip);
		player = new Actor(0,0,0,NULL,TCODColor::white);
		player->load(zip);
		actors.push(player);
		int nbActors=zip.getInt();
		while ( nbActors > 0 ) {
			Actor *actor = new Actor(0,0,0,NULL,TCODColor::white);
			actor->load(zip);
			actors.push(actor);
			nbActors--;
		}
		gui->load(zip);
		gameStatus=STARTUP;
	}
//	if ( TCODSystem::fileExists("game.sav")) {
//		TCODZip zip;
//		zip.loadFromFile("game.sav");
//		int width = zip.getInt();
//		int height = zip.getInt();
//		map = new Map(width,height);
//		map->load(zip);
//		player = new Actor(0,0,0,NULL,TCODColor::white);
//		player->load(zip);
//		actors.push(player);
//		int nbActors=zip.getInt();
//		while ( nbActors > 0 ) {
//			Actor *actor = new Actor(0,0,0,NULL,TCODColor::white);
//			actor->load(zip);
//			actors.push(actor);
//			nbActors--;
//		}
//		gui->load(zip);
//	} else {
//		engine.init();
//	}
}
