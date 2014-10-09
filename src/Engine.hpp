class Engine {
	public :
		TCODList<Actor *> actors;
		Actor *player;
		Map *map;

		Engine();
		~Engine();
		void update();
		void render();
		int fovRadius;
	
	private :
		bool computeFov;
};

extern Engine engine;
