#include "main.hpp"

static const int PANEL_HEIGHT=7;
static const int BAR_WIDTH=20;

Gui::Gui() {
	con = new TCODConsole(engine.screenWidth,PANEL_HEIGHT);
}

Gui::~Gui() {
	delete con;
}

void Gui::render() {
	con->setDefaultBackground(TCODColor::black);
	con->clear();
	renderBar(1,1,BAR_WIDTH,"HP",engine.player->destructible->hp,engine.player->destructible->maxHp,TCODColor::lightRed,TCODColor::darkerRed);
	TCODConsole::blit(con,0,0,engine.screenWidth,PANEL_HEIGHT,TCODConsole::root,0,engine.screenHeight-PANEL_HEIGHT);

}

void Gui::renderBar(int x, int y, int width, const char *name, float value, float maxValue, const TCODColor &barColor, const TCODColor &backColor){
	con->setDefaultBackground(backColor);
	con->rect(x,y,width,1,false,TCOD_BKGND_SET);
