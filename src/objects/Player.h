#ifndef MAINCHARACTER_H
#define MAINCHARACTER_H

#include <irrlicht.h>
#include <vector>
#include "../LuaGlobalCaller.h"
#include "../LANGManager.h"
#include "DynamicObject.h"
#include "combat.h"
#include "../sound/SoundManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

class Player
{
    public:
		enum CONTROL_TYPE
		{
			CONTROL_POINTNCLICK = 0,
			CONTROL_WASD = 1,
			CONTROL_COUNT = 2
		};

        static Player* getInstance();
        virtual ~Player();

		// Return the dynamic object for the player
		// Player must be defined in the XML file to work properly.
		DynamicObject* getObject();

		void update();
		void updateDisplay();
		
		
		void setTaggedTarget(DynamicObject* object);
		DynamicObject* getTaggedTarget();
		
		// Other player specific functions
		
		
		//set shadow green to highlight player
        void setHighLight(bool highlight);

		void displayTarget(bool visible);
		ISceneNode* getTarget(); 
		
		ISceneNode* getNode() { return playerObject->getNode(); };

		//Store the control type
		CONTROL_TYPE controltype;

    protected:
    private:
        Player();
		void updateRTSTargetting();
		void updateTargetting();
		DynamicObject* getNearest(vector3df pos, vector<DynamicObject*> list);
		
		DynamicObject* playerObject;
		DynamicObject* taggedObject;

		ISceneNode* player_ref;
		
        ITimer* timer;
		u32 currentime;
		u32 oldtime;
		u32 timer1;
		u32 timer2;
		u32 timer3;

		bool collided;

		vector<stringc> items;
};

#endif // MAINCHARACTER_H
