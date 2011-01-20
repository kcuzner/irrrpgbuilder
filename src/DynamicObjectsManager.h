#ifndef DYNAMICOBJECTSMANAGER_H
#define DYNAMICOBJECTSMANAGER_H

#include <vector>
#include <irrlicht.h>
#include "App.h"

#include "DynamicObject.h"
#include "combat.h"

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
		DynamicObject* getTarget();
		Combat* getCombat();

        vector<stringc> getObjectsList();

        void showDebugData(bool show);

		void startCollisions();
        void clearCollisions();

		

		void updateMetaSelector();
		IMetaTriangleSelector* getMeta();
		IMetaTriangleSelector* createMeta();
	
        void saveToXML(TiXmlElement* parentElement);
        bool loadFromXML(TiXmlElement* parentElement);

        void initializeAllScripts();
        void updateAll();
        void clearAllScripts();

        void clean();

        virtual ~DynamicObjectsManager();

	protected:
		void initializeCollisions();

    private:
        DynamicObjectsManager();
		

        DynamicObject* activeObject;
		DynamicObject* playerObject;
		DynamicObject* targetObject;
		Combat*	combat;

        vector<DynamicObject*> objectsTemplate;

        vector<DynamicObject*> objects;
        int objsCounter;
		int collisionCounter;
		bool createcollisions;

        vector<ISceneNodeAnimatorCollisionResponse*> collisionResponseAnimators;
		
		IMetaTriangleSelector* meta;
		ISceneNodeAnimatorCollisionResponse* anim;

        stringc createUniqueName();
};

#endif // DYNAMICOBJECTSMANAGER_H
