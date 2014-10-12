class Engine {
	public :
		TCODList<Actor *> actors;
		Actor *player;
		Map *map;
		enum GameStatus {
			STARTUP,
			IDLE,
			NEW_TURN,
			VICTORY,
			DEFEAT
		} gameStatus;
		int screenWidth;
		int screenHeight;
		TCOD_key_t lastKey;

		Engine(int screenWidth, int screenHeight);
		~Engine();
		void update();
		void render();
		int fovRadius;
		void sendToBack(Actor *actor);
	
	private :
//bool computeFov;
};

extern Engine engine;
