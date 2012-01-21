#ifndef DYNAMICOBJECTSMANAGER_H
#define DYNAMICOBJECTSMANAGER_H

#include <vector>
#include <irrlicht.h>
#include "../App.h"

#include "DynamicObject.h"

#include "Player.h"

#include "../tinyXML/tinyxml.h"

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

		vector<stringc> getObjectsList(TYPE objectType);

        void showDebugData(bool show);

		void startCollisions();
        void clearCollisions();

		void updateMetaSelector();
		IMetaTriangleSelector* getMeta();
		IMetaTriangleSelector* createMeta();
	
        void saveToXML(TiXmlElement* parentElement);
        bool loadFromXML(TiXmlElement* parentElement);

		void freezeAll();
		void unFreezeAll();

		void setDialogCaller(DynamicObject* object);
		DynamicObject* getDialogCaller();

        void initializeAllScripts();
        void updateAll();
        void clearAllScripts();

        void clean(bool full);

        virtual ~DynamicObjectsManager();

	protected:
		void initializeCollisions();

    private:
        DynamicObjectsManager();
		
		IrrlichtDevice *device;

        DynamicObject* activeObject;
		DynamicObject* playerObject;
		DynamicObject* targetObject;

		DynamicObject* dialogCaller;
		

        vector<DynamicObject*> objectsTemplate;

        vector<DynamicObject*> objects;
        int objsCounter;
		int collisionCounter;
		bool createcollisions;

        vector<ISceneNodeAnimatorCollisionResponse*> collisionResponseAnimators;
		
		IMetaTriangleSelector* meta;
		ISceneNodeAnimatorCollisionResponse* anim;

        stringc createUniqueName();
		int objectCounter;
};

#endif // DYNAMICOBJECTSMANAGER_H
