#ifndef DYNAMICOBJECTSMANAGER_H
#define DYNAMICOBJECTSMANAGER_H

#include <vector>
#include <irrlicht.h>
#include "../App.h"

#include "DynamicObject.h"
#include "TemplateObject.h"

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

		bool loadTemplates();
		TemplateObject* searchTemplate(stringc name);

		bool loadBlock(IrrlichtDevice * device, core::stringc file);
		bool loadSet();
        DynamicObject* createActiveObjectAt(vector3df pos);
        void removeObject(stringc uniqueName);

        TemplateObject* getActiveObject();
        void setActiveObject(stringc name);

        DynamicObject* getObjectByName(stringc name);
		DynamicObject* getPlayer();
		DynamicObject* getTarget();

		scene::ISceneNode* findActiveObject(void);

		vector<stringw> getObjectsList(TYPE objectType, core::stringw category);
		vector<stringw> getObjectsListCategories(TYPE objectType);

		void setObjectsID(TYPE objectType, s32 ID);

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

		void updateAnimationBlend();
		void objectsToIdle();
        void initializeAllScripts();
		void displayShadow(bool visible);
        void updateAll();
        void clearAllScripts();

        void clean(bool full);

        virtual ~DynamicObjectsManager();

	protected:
		void initializeCollisions();

    private:
        DynamicObjectsManager();
		
		IrrlichtDevice *device;

        TemplateObject* activeObject;
		DynamicObject* activeObject2;
		DynamicObject* playerObject;
		DynamicObject* targetObject;

		DynamicObject* dialogCaller;
		

        vector<DynamicObject*> objectsTemplate; // Temporary -> Contain all the templates as dynamic objects

        vector<DynamicObject*> objects; // List of all the dynamic objects

		TemplateObject* newObj;
		vector<TemplateObject*> objTemplate; // List of all the dynamic object template data 
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
