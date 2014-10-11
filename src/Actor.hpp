class Actor {
public :
	int x,y;
	int ch;
	TCODColor col;
	const char *name;
	bool blocks; //can we walk over this actor?
	Attacker *attacker;
	Destructible *destructible;
	Ai *ai;

	Actor(int x, int y, int ch, const char *name, const TCODColor &col);
	void update();
	void render() const;
};
