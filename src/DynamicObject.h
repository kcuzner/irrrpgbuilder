#ifndef DYNAMICOBJECT_H
#define DYNAMICOBJECT_H

#include <irrlicht.h>

#include "tinyXML/tinyxml.h"

#include "TerrainManager.h"
#include "HealthSceneNode.h"
#include "GUIManager.h"

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

enum OBJECT_ANIMATION
{
    OBJECT_ANIMATION_IDLE = 0,
    OBJECT_ANIMATION_WALK = 1,
	OBJECT_ANIMATION_RUN = 2,
    OBJECT_ANIMATION_ATTACK = 3,
	OBJECT_ANIMATION_INJURED = 4,
	OBJECT_ANIMATION_KNOCKBACK = 5,
    OBJECT_ANIMATION_DIE = 6,
	OBJECT_ANIMATION_DIE_KNOCKBACK = 7,
	OBJECT_ANIMATION_SPAWN = 7,
	OBJECT_ANIMATION_DESPAWN = 8,
	OBJECT_ANIMATION_DESPAWN_KNOCKBACK = 9,
	OBJECT_ANIMATION_OPEN = 10,
	OBJECT_ANIMATION_CLOSE = 11,
	OBJECT_ANIMATION_PRESPAWN = 12,
	OBJECT_ANIMATION_CUSTOM = 99
};
enum TYPE
{
	OBJECT_TYPE_NONE = 0,
	OBJECT_TYPE_NPC = 1,
	OBJECT_TYPE_INTERACTIVE = 2,
	OBJECT_TYPE_NON_INTERACTIVE = 3,
	OBJECT_TYPE_PLAYER	= 4,
	OBJECT_TYPE_EDITOR	= 5
};

typedef struct{
    stringc name;
	stringc meshname;
	IAnimatedMesh* mesh;
	stringc sound;
	s32 startFrame;
    s32 endFrame;
	s32 attackevent;
	s32 soundevent;
    f32 speed;
	f32 walkspeed;
	bool loop;

}DynamicObject_Animation;

typedef struct {
	int life;
	int mana;
	int maxlife;
	int maxmana;
	int regenlife;
	int regenmana;
	int money;
	int level;
	int experience;
	int mindamage;
	int maxdamage;
	int armor;
	int magic_armor;
	int hurt_resist;
	int dotduration;
	f32 hit_prob;
	f32 dodge_prop;
	int mindefense;
	int maxdefense;
}cproperty;

class DynamicObject
{
    public:
        DynamicObject(stringc name, stringc meshFile, vector<DynamicObject_Animation> animations);
		virtual ~DynamicObject();

        DynamicObject* clone();

		ISceneNode* getNode();
		ISceneNode* getShadow();
		stringc getTemplateObjectName();
        void setTemplateObjectName(stringc newName);

		void lookAt(vector3df pos);
		void setPosition(vector3df pos);
        vector3df getPosition();
		void setRotation(vector3df rot);
        vector3df getRotation();
		void moveObject(f32 speed);
		void walkTo(vector3df targetPos);
		void setWalkTarget(vector3df newTarget);
		vector3df getWalkTarget();
		f32 getDistanceFrom(vector3df pos);
 
		void setEnabled(bool enabled);
		bool isEnabled();
		void setType(stringc name);
		TYPE getType();
		
		void setName(stringc name);
        stringc getName();

		void setMaterialType(E_MATERIAL_TYPE mType);
        E_MATERIAL_TYPE getMaterialType();

		void setScale(vector3df scale);
		vector3df getScale();

		DynamicObject * getCurrentEnemy();
		void clearEnemy();
		cproperty initProperties();
		cproperty getProperties();
		void setProperties(cproperty prop);

		cproperty getProp_base();
		void setProp_base(cproperty prop);

		cproperty getProp_level();
		void setProp_level(cproperty prop);

		void setLife(int life);
        int getLife();
        void setMoney(int money);
        int getMoney();
		void setObjectLabel(stringc label);
        void objectLabelSetVisible(bool visible);
        
		 bool hasAnimation(){ return animations.size() != 0; };
        //void setAnimations( vector<DynamicObject_Animation> animations ) {this->animations = animations; };
        //vector<DynamicObject_Animation> getAnimations() {return this->animations;};
        void setFrameLoop(s32 start, s32 end);
        void setAnimationSpeed(f32 speed);
		OBJECT_ANIMATION getAnimationState(stringc animName);
		OBJECT_ANIMATION getAnimation(void);
        bool setAnimation(stringc animName);
		void checkAnimationEvent();
		void setRunningMode(bool run); // Switche the walk animation to a run animation
        
		void setAnimator(ISceneNodeAnimatorCollisionResponse* animator_node);
		ISceneNodeAnimatorCollisionResponse* getAnimator();
		ITriangleSelector* getTriangleSelector();
		
		// item management
        void addItem(stringc itemName);
        void removeItem(stringc itemName);
        vector<stringc> getItems();
        int getItemCount(stringc itemName);//returns the total of items of type "itemName"
        bool hasItem(stringc itemName);
        void removeAllItems();

		stringc getScript();
        void setScript(stringw script);
		void clearScripts();//delete lua_State
		void doScript();//called when the game starts
		void storeParams(); // Store the original position and rotation before gameplay (used when you start the game)
		void restoreParams();//restore original position and rotation after gameplay (used when you stop the game in Editor)
        void saveToXML(TiXmlElement* parentElement);    
        void update();//run "step" lua function

        void notifyClick();
		void notifyAttackRange();
		void notifyCollision();
		void notifyAnswer(bool answer);

        stringc getObjectType();

		void attackEnemy(DynamicObject* obj);
		bool isTemplate();
		void setTemplate(bool value);
		// Needed in other classes
		DynamicObject_Animation currentAnim;
      
    protected:

        bool enabled;//disabled objects aren't rendered and step() function isn't processed during gameplay

        stringc templateObjectName;//The original object name

        DynamicObject(stringc name, IMesh* mesh, vector<DynamicObject_Animation> animations = vector<DynamicObject_Animation>());

    private:
        void setupObj(stringc name, IMesh* mesh);
		void updateWalk();
		void luaRefresh();

        //lua funcs
        static int setPosition(lua_State *LS);//setPosition(x,y,z)
        static int getPosition(lua_State *LS);//x,y,z = getPosition()
        static int setRotation(lua_State *LS);//setRotation(x,y,z)
        static int getRotation(lua_State *LS);//x,y,z = getPosition()
        static int turn(lua_State *LS);//turn(degrees)
        static int move(lua_State *LS);//move(x,y,z)
		static int walkToLUA(lua_State *LS); // walkToLua("objectName")
        static int lookAt(lua_State *LS);//lookAt(x,y,z)
        static int lookToObject(lua_State *LS);//lookToObject(otherObjectName)
		static int attackObj(lua_State *LS);//attackObj(otherObjectName)
		static int setPropertie(lua_State *LS); // setPropertie("objectName","propertie",value)
		static int getPropertie(lua_State *LS); // value getPropertie("objectName","propertie")
        static int distanceFrom(lua_State *LS);
		static int getNameLUA(lua_State *LS); // value getNameLUA()

        static int setFrameLoop(lua_State *LS);//setFrameLoop(start,end);
        static int setAnimation(lua_State *LS);//setAnimation(anim_name);
        static int setAnimationSpeed(lua_State *LS);//setAnimationSpeed(speed);

        static int showObjectLabel(lua_State *LS);//showObjectLabel()
        static int hideObjectLabel(lua_State *LS);//hideObjectLabel()
        static int setObjectLabel(lua_State *LS);//setObjectLabel(newLabelText)

		//Dialog Functions
        static int showDialogMessage(lua_State *LS);//showDialogMessage(text, optional_sound_file)
		static int showDialogQuestion(lua_State *LS);//showDialogQuestion(text, optional_sound_file)

        static int setEnabled(lua_State *LS);//setEnabled(enabled?)

        stringc name;
	
        IMesh* mesh;
        ISceneNode* node;
		IAnimatedMeshSceneNode * nodeAnim;
		ISkinnedMesh* skinnedmesh;

		ITriangleSelector* selector;
		ISceneNodeAnimatorCollisionResponse* animator;
		
        ISceneNode* fakeShadow;
		scene::HealthSceneNode* Healthbar;
		stringw script;

		OBJECT_ANIMATION currentAnimation;
		OBJECT_ANIMATION oldAnimation;
		stringc oldAnimName;
		TYPE objectType;
		stringc typeText;

		DynamicObject* enemyUnderAttack;
		DynamicObject* currentObject;
		stringc namecollide;
		f32	currentSpeed;
		s32 lastframe;

		bool collided;
		bool nodeLuaCulling;
		bool deadstate;
		bool diePresent;
		bool despawnPresent;
		bool runningMode;
		bool activatedSound;
		vector3df walkTarget;
		vector3df originalPosition;
		vector3df originalRotation;
		int original_life;
		int original_maxlife;
		vector<stringc> items;
        ITextSceneNode* objLabel;

		vector<DynamicObject_Animation> animations;
		

		cproperty prop_base;
		cproperty prop_level;
		cproperty properties;

		bool templateobject;
		bool stunstate;

		// delay timers needed
		u32 timerAnimation;
		u32 timerLUA;
		// timers to unlock animation states.
		u32 timerDie;
		u32 timerDespawn;
		u32 timerHurt;
		u32 timerStun;
		// Timer to delay the sound (do it's not repeating too fast)
		u32 timerSound;


		lua_State *L;
      
};

#endif // DYNAMICOBJECT_H
