#ifndef LANGMANAGER_H
#define LANGMANAGER_H

#include <irrlicht.h>
#include "GUIManager.h"
#include "App.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class LANGManager
{
    public:
        static LANGManager* getInstance();

        stringc getText(stringc node);
		void setDefaultLanguage(stringc language);

        virtual ~LANGManager();
    protected:
    private:
        LANGManager();

        stringc defaultLanguage;
		stringc description;
};

#endif // LANGMANAGER_H
