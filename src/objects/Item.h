#ifndef ITEM_H
#define ITEM_H

#include <irrlicht.h>
#include "../LuaGlobalCaller.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class Item
{
    public:
        Item(stringc name, stringc script);

        Item* clone();

        void runScript();

        void setScript(stringc script);
        stringc getScript();

		irr::core::stringc getName() { return name; };

        virtual ~Item();
    protected:
    private:
        stringc name;
        stringc script;
};

#endif // ITEM_H
