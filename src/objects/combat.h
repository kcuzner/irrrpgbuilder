#ifndef COMBAT_H
#define COMBAT_H
//-----------------------------------------------------------------------
// Combat Class
// This class use the properties of NPCs to calculate the battle damage, XP etc.
// Actually is run as a single instance. (if using the getInstance() function)

#include <vector>
#include <irrlicht.h>

//#include "../gui/GUIManager.h"
#include "DynamicObject.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


//This class is mostly used for calculating the damage points when the player is in combat
//Will cause damage on both the attacker and the defender objects.
class Combat
{
    public:
        Combat();
		~Combat();

		static Combat* getInstance();
		int attack(DynamicObject* attacker, DynamicObject* defender);
		void updateLevel(DynamicObject* object);
		void dot(DynamicObject* victim, int duration, int damage);
		void dumpProperties (DynamicObject* object);
		void update();

	protected:
		bool percent(f32 percent);
		u32 chances(u32 min, u32 max);

	private:

      
		DynamicObject* tempObject;
		lua_State *L;

		//Dot is not fully implemented yet. (Damage over time -> poison)
		vector<DynamicObject*> dotvictim;
		vector<int> dotduration;
		vector<int> dotdamage;
		vector<u32> dottimer;
		bool dotenabled;

		u32 freepoints;
		u32 skillspoints;
      
};

#endif // DYNAMICOBJECT_H
