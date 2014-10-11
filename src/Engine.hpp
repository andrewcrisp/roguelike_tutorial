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

		Engine();
		~Engine();
		void update();
		void render();
		int fovRadius;
		void sendToBack(Actor *actor);
	
	private :
//bool computeFov;
};

extern Engine engine;
