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
		Map *map;
		int screenWidth;
		int screenHeight;
		Gui *gui;
		TCOD_key_t lastKey;

		Engine(int screenWidth, int screenHeight);
		~Engine();
		void update();
		void render();
		void sendToBack(Actor *actor);
	
	private :
//bool computeFov;
};

extern Engine engine;
