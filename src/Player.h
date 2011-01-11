#ifndef MAINCHARACTER_H
#define MAINCHARACTER_H

#include <irrlicht.h>
#include <vector>
#include "LuaGlobalCaller.h"
#include "LANGManager.h"
#include "DynamicObject.h"

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
        static Player* getInstance();
        virtual ~Player();

		// Return the dynamic object for the player
		// Player must be defined in the XML file to work properly.
		DynamicObject* getObject();

		void update();
		
		
		// Other player specific functions
		
		
		//set shadow green to highlight player
        void setHighLight(bool highlight);
		
		ISceneNode* getNode() { return playerObject->getNode(); };

    protected:
    private:
        Player();
		
		DynamicObject* playerObject;
		
        ITimer* timer;
		u32 currentime;
		u32 oldtime;
		u32 timer1;
		u32 timer2;

		bool collided;

		vector<stringc> items;
};

#endif // MAINCHARACTER_H
