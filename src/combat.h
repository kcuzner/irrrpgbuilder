#ifndef COMBAT_H
#define COMBAT_H

#include <irrlicht.h>

#include "GUIManager.h"
#include "DynamicObject.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}


class Combat
{
    public:
        Combat();
		~Combat();

	void attack(DynamicObject* attacker, DynamicObject* defender);
	void dumpProperties (DynamicObject* object);

	private:
      
		DynamicObject* tempObject;
		lua_State *L;
      
};

#endif // DYNAMICOBJECT_H
