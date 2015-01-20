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

		bool findTemplate(stringc filename, DynamicObject::TYPE type);
        TemplateObject* getActiveObject();
        bool setActiveObject(stringc name);
		
		DynamicObject* createTemplateAt(core::stringc name, vector3df position);
        DynamicObject* getObjectByName(stringc name);
		DynamicObject* getPlayer();
		DynamicObject* getTarget();

		scene::ISceneNode* findActiveObject(void);
		void resetObjectsHeight(DynamicObject::TYPE objtype);

		vector<stringw> getObjectsList(GUIManager::LIST_TYPE type, core::stringw collection, core::stringw category, DynamicObject::SPECIAL special = DynamicObject::SPECIAL_NONE);
		vector<stringw> getObjectsListCategories(GUIManager::LIST_TYPE type,stringw collection, DynamicObject::SPECIAL special = DynamicObject::SPECIAL_NONE);
		vector<stringw> getObjectsCollections(GUIManager::LIST_TYPE type);
		vector<stringw> getObjectsSceneList(DynamicObject::TYPE objectType);
		vector<stringw> getObjectsSceneListAlias(DynamicObject::TYPE objectType);


		void setObjectsID(DynamicObject::TYPE objectType, s32 ID);
		void setObjectsVisible(DynamicObject::TYPE objectType, bool visible);
		void resetObjectsWalkTarget(DynamicObject::TYPE objectType);

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
		void removeGenerated();

		//Character operation, cumulation of experience when objects add over another
		void addProperties(DynamicObject* source, DynamicObject* destination);
		void removeProperties(DynamicObject* source, DynamicObject* destination);

		void setPlayer();
		vector<DynamicObject*> buildInteractiveList();

		stringc createUniqueName(DynamicObject::TYPE objtype);

        void clean(bool full);
		int getEnemyCount();

		vector<DynamicObject*> getObjectNearPosition(vector3df pos, f32 radius, DynamicObject::TYPE type); //get all object around position and range
		vector<DynamicObject*> getObjectsOfType(DynamicObject::TYPE objectType); //Code from Devjitjit, return list of objects from specific types

		// Data object for the current template
		 TemplateObject* activeObject;
		 vector<core::stringw> meshtypename;



        virtual ~DynamicObjectsManager();

    private:
        DynamicObjectsManager();
		
		IrrlichtDevice *device;
		DynamicObject* playerObject;
		DynamicObject* targetObject;

		DynamicObject* dialogCaller;
		

        vector<DynamicObject*> objectsTemplate; // Temporary -> Contain all the templates as dynamic objects

        vector<DynamicObject*> objects; // List of all the dynamic objects
		vector<DynamicObject*> interactiveobjects; //List of interactive objects from the objects list.

		TemplateObject* newObj;
		vector<TemplateObject*> objTemplate; // List of all the dynamic object template data 
        int objsCounter_npc;
		int objsCounter_regular;
		int objsCounter_walkable;
		int objsCounter_others;

		vector<ISceneNodeAnimatorCollisionResponse*> collisionResponseAnimators;
		
		IMetaTriangleSelector* meta;
		ISceneNodeAnimatorCollisionResponse* anim;

		int objectCounter;
		core::stringw setname;

		u32 objectcounter;
};

#endif // DYNAMICOBJECTSMANAGER_H
