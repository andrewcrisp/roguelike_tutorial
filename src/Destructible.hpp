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
		void load(TCODZip &zip);
		static Destructible *creat(TCODZip &zip);

	protected :
		enum DestructibleType {
			MONSTER,PLAYER
		};
		
};

class MonsterDestructible : public Destructible {
	public :
		MonsterDestructible(float maxHp, float defense, const char *corpseName);
		void die(Actor *owner);
};

class PlayerDestructible : public Destructible {
	public :
		PlayerDestructible(float maxHp, float defense, const char *corpseName);
		void die(Actor *owner);
};

