#ifndef DYNAMICOBJECTSMANAGER_H
#define DYNAMICOBJECTSMANAGER_H

#include <vector>
#include <irrlicht.h>
#include "App.h"

#include "DynamicObject.h"

#include "Player.h"

#include "tinyXML/tinyxml.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;



class DynamicObjectsManager
{
    public:
        static DynamicObjectsManager* getInstance();

		bool processFile(stringc filename);
        DynamicObject* createActiveObjectAt(vector3df pos);
        void removeObject(stringc uniqueName);

        DynamicObject* getActiveObject();
        void setActiveObject(stringc name);

        DynamicObject* getObjectByName(stringc name);
		DynamicObject* getPlayer();

        vector<stringc> getObjectsList();

        void showDebugData(bool show);

        void initializeCollisions();
        void clearCollisions();
		

		void updateMetaSelector(ITriangleSelector* tris, bool remove);
		IMetaTriangleSelector* getMeta();
		IMetaTriangleSelector* createMeta();

        void saveToXML(TiXmlElement* parentElement);
        bool loadFromXML(TiXmlElement* parentElement);

        void initializeAllScripts();
        void updateAll();
        void clearAllScripts();

        void clean();

        virtual ~DynamicObjectsManager();
    private:
        DynamicObjectsManager();

        DynamicObject* activeObject;
		DynamicObject* playerObject;

        vector<DynamicObject*> objectsTemplate;

        vector<DynamicObject*> objects;
        int objsCounter;

        vector<ISceneNodeAnimatorCollisionResponse*> collisionResponseAnimators;
		
		IMetaTriangleSelector* meta;
		ISceneNodeAnimatorCollisionResponse* anim;

        stringc createUniqueName();
};

#endif // DYNAMICOBJECTSMANAGER_H
