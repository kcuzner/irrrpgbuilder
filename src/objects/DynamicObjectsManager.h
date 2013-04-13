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

		DynamicObject* createCustomObjectAt(vector3df pos, core::stringc meshfile);
        DynamicObject* createActiveObjectAt(vector3df pos);

		core::stringc getScript(stringc uniqueName);
		void setScript(stringc uniqueName);
        void removeObject(stringc uniqueName);

		bool findTemplate(stringc filename);
        TemplateObject* getActiveObject();
        bool setActiveObject(stringc name);

        DynamicObject* getObjectByName(stringc name);
		DynamicObject* getPlayer();
		DynamicObject* getTarget();

		scene::ISceneNode* findActiveObject(void);

		vector<stringw> getObjectsList(core::stringw objectType, core::stringw category);
		vector<stringw> getObjectsListCategories(stringw objectType);

		void setObjectsID(TYPE objectType, s32 ID);
		void setObjectsVisible(TYPE objectType, bool visible);
		void resetObjectsWalkTarget(TYPE objectType);

        void showDebugData(bool show);

		IMetaTriangleSelector* getMeta();
		IMetaTriangleSelector* createMeta();
	
        void saveToXML(TiXmlElement* parentElement);
        bool loadFromXML(TiXmlElement* parentElement);

		void checkTemplateNames();

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

		void setPlayer();

        void clean(bool full);

		// Data object for the current template
		 TemplateObject* activeObject;
		 vector<core::stringw> meshtypename;



        virtual ~DynamicObjectsManager();

    private:
        DynamicObjectsManager();
		
		IrrlichtDevice *device;

       
		DynamicObject* activeObject2;
		DynamicObject* playerObject;
		DynamicObject* targetObject;

		DynamicObject* dialogCaller;
		

        vector<DynamicObject*> objectsTemplate; // Temporary -> Contain all the templates as dynamic objects

        vector<DynamicObject*> objects; // List of all the dynamic objects

		TemplateObject* newObj;
		vector<TemplateObject*> objTemplate; // List of all the dynamic object template data 
        int objsCounter;

		vector<ISceneNodeAnimatorCollisionResponse*> collisionResponseAnimators;
		
		IMetaTriangleSelector* meta;
		ISceneNodeAnimatorCollisionResponse* anim;

        stringc createUniqueName();
		int objectCounter;
		core::stringw setname;

		u32 objectcounter;
};

#endif // DYNAMICOBJECTSMANAGER_H
