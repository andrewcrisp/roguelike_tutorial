class Engine {
	public :
		enum GameStatus {
			STARTUP,
			IDLE,
			NEW_TURN,
			VICTORY,
			DEFEAT
		} gameStatus;
		TCODList<Actor *> actors;
		int fovRadius;
		Actor *player;
		Actor *stairs;
		Map *map;
		int screenWidth;
		int screenHeight;
		int level;
		Gui *gui;
		TCOD_key_t lastKey;
		TCOD_mouse_t mouse;

		Engine(int screenWidth, int screenHeight);
		~Engine();
		void update();
		void render();
		void sendToBack(Actor *actor);
		Actor *getClosestMonster(int x, int y, float range) const;
		bool pickATile(int *x, int *y, float maxRange = 0.0f);
		void init();
		void term();
		void nextLevel();
		void load();
		void save();
		Actor *getActor(int x, int y) const;

	private :
};

extern Engine engine;
