class Destructible : public Persistent {
	public :
		float maxHp;
		float hp;
		float defense;
		const char *corpseName;

		Destructible(float maxHp, float defense, const char *corpseName);
		inline bool isDead() { return hp <= 0; }
		float takeDamage(Actor *owner, float damage);
		float heal(float amount);
		virtual void die(Actor *owner);
		void load(TCODZip &zip);
		void save(TCODZip &zip);
		static Destructible *create(TCODZip &zip);

	protected :
		enum DestructibleType {
			MONSTER,PLAYER
		};
		
};

class MonsterDestructible : public Destructible {
	public :
		MonsterDestructible(float maxHp, float defense, const char *corpseName);
		void die(Actor *owner);
		void save(TCODZip &zip);
};

class PlayerDestructible : public Destructible {
	public :
		PlayerDestructible(float maxHp, float defense, const char *corpseName);
		void die(Actor *owner);
		void save(TCODZip &zip);
};

