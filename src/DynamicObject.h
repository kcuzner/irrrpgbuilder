#ifndef DYNAMICOBJECT_H
#define DYNAMICOBJECT_H

#include <irrlicht.h>

//#include "HardwareSkinCallback.h"

#include "tinyXML/tinyxml.h"

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
	OBJECT_ANIMATION_CLOSE = 11
};
enum TYPE
{
	OBJECT_TYPE_NPC = 0,
	OBJECT_TYPE_INTERACTIVE = 1,
	OBJECT_TYPE_NON_INTERACTIVE = 2,
	OBJECT_TYPE_PLAYER	= 3,
	OBJECT_TYPE_EDITOR_OBJ	= 4
};

typedef struct{
    stringc name;
    s32 startFrame;
    s32 endFrame;
    f32 speed;
}DynamicObject_Animation;

class DynamicObject
{
    public:
        DynamicObject(stringc name, stringc meshFile, vector<DynamicObject_Animation> animations);

        DynamicObject* clone();

		void setType(stringc name);
		TYPE getType();
		stringc typeText;

        void setName(stringc name);
        stringc getName();
        ISceneNode* getNode();

        void setPosition(vector3df pos);
        vector3df getPosition();
        void lookAt(vector3df pos);

        f32 getDistanceFrom(vector3df pos);

        void setFrameLoop(s32 start, s32 end);
        void setAnimationSpeed(f32 speed);

        void setRotation(vector3df rot);
        vector3df getRotation();

        void setMaterialType(E_MATERIAL_TYPE mType);
        E_MATERIAL_TYPE getMaterialType();

        void setScale(vector3df scale);
        vector3df getScale();

        ITriangleSelector* getTriangleSelector();

        stringc getScript();
        void setScript(stringc script);

        stringc getTemplateObjectName();
        void setTemplateObjectName(stringc newName);

        void saveToXML(TiXmlElement* parentElement);

        void doScript();//called when the game starts
        void update();//run "step" lua function
        void clearScripts();//delete lua_State
        void restoreParams();//restore original position and rotation after gameplay (used when you stop the game in Editor)

		void setLife(int life);
        int getLife();

        void setMoney(int money);
        int getMoney();

        void notifyClick();

        void setObjectLabel(stringc label);
        void objectLabelSetVisible(bool visible);

        void setEnabled(bool enabled);

        stringc getObjectType();

        bool hasAnimation(){ return animations.size() != 0; };
        //void setAnimations( vector<DynamicObject_Animation> animations ) {this->animations = animations; };
        //vector<DynamicObject_Animation> getAnimations() {return this->animations;};

		OBJECT_ANIMATION getAnimationState(stringc animName);
        void setAnimation(stringc animName);

        void setCollisionAnimator(ISceneNodeAnimatorCollisionResponse* collisionAnimator);
		void moveObject(f32 speed);

        virtual ~DynamicObject();
    protected:

        bool enabled;//disabled objects aren't rendered and step() function isn't processed during gameplay

        stringc templateObjectName;//The original object name

        DynamicObject(stringc name, IMesh* mesh, vector<DynamicObject_Animation> animations = vector<DynamicObject_Animation>());

    private:
        void setupObj(stringc name, IMesh* mesh);

        //lua funcs
        static int setPosition(lua_State *LS);//setPosition(x,y,z)
        static int getPosition(lua_State *LS);//x,y,z = getPosition()
        static int setRotation(lua_State *LS);//setRotation(x,y,z)
        static int getRotation(lua_State *LS);//x,y,z = getPosition()
        static int turn(lua_State *LS);//turn(degrees)
        static int move(lua_State *LS);//move(x,y,z)
        static int lookAt(lua_State *LS);//lookAt(x,y,z)
        static int lookToObject(lua_State *LS);//lookToObject(otherObjectName)
        static int distanceFrom(lua_State *LS);

        static int setFrameLoop(lua_State *LS);//setFrameLoop(start,end);
        static int setAnimation(lua_State *LS);//setAnimation(anim_name);
        static int setAnimationSpeed(lua_State *LS);//setAnimationSpeed(speed);

        static int showObjectLabel(lua_State *LS);//showObjectLabel()
        static int hideObjectLabel(lua_State *LS);//hideObjectLabel()
        static int setObjectLabel(lua_State *LS);//setObjectLabel(newLabelText)

        static int setEnabled(lua_State *LS);//setEnabled(enabled?)

        stringc name;

        IMesh* mesh;
        ISceneNode* node;
		ISkinnedMesh* skinnedmesh;

        ISceneNode* fakeShadow;

        ITriangleSelector* selector;

        stringc script;

		OBJECT_ANIMATION currentAnimation;
		TYPE objectType;

		struct property{
			int life;
			int mana;
			int maxlife;
			int maxmana;
			int regenlife;
			int regenmana;
			int money;
			int level;
			int skill_level;
			int experience;
			int mindamage;
			int maxdamage;
			int armor;
			int magic_armor;
			int hurt_resist;
			int dotduration;
		};
		property prop_base;
		property prop_level;
		property properties;

		int oldlife;
		u32 timer;
		u32 timer2;
		DynamicObject* currentObject;
		f32	currentSpeed;

        lua_State *L;

        ITextSceneNode* objLabel;

        vector<DynamicObject_Animation> animations;

        ISceneNodeAnimatorCollisionResponse* collisionAnimator;
};

#endif // DYNAMICOBJECT_H
