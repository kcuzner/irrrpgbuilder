#ifndef MAINCHARACTER_H
#define MAINCHARACTER_H

#include <irrlicht.h>
#include <vector>
#include "LuaGlobalCaller.h"
#include "LANGManager.h"
#include "DynamicObject.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

enum PLAYER_ANIMATION
{
    PLAYER_ANIMATION_IDLE = 0,
    PLAYER_ANIMATION_WALK = 1,
    PLAYER_ANIMATION_ATTACK = 2,
    PLAYER_ANIMATION_DIE = 3,
	PLAYER_ANIMATION_INJURED = 4,
	PLAYER_ANIMATION_RUN = 5
};

typedef struct{
    stringc name;
	stringc mesh;
	stringc sound;
    s32 startFrame;
    s32 endFrame;
	s32 attackevent;
	s32 soundevent;
    f32 speed;
}PlayerObject_Animation;

class Player
{
    public:
        static Player* getInstance();
        virtual ~Player();

        void setPosition(vector3df pos);
        vector3df getPosition();

        void setRotation(vector3df rot);
        vector3df getRotation();

        void walkTo(vector3df targetPos, f32 speed);
        void lookAt(vector3df pos);
        void setWalkTarget(vector3df newTarget);
        void setWalkSpeed(f32 newSpeed);///TODO: deixar disponivel tambem no lua

        void setAnimation(PLAYER_ANIMATION anim);
        PLAYER_ANIMATION getAnimation();//returns Current animation

		bool CheckAnimationEvent();

        void attackEnemy(DynamicObject* obj);

        void update();

        void doScript();
        void clearScripts();

        void setLife(int life);
        int getLife();

        void setMoney(int money);
        int getMoney();

        void saveToXML(TiXmlElement* parentElement);
        bool loadFromXML(TiXmlElement* parentElement);

        //set shadow green to highlight player
        void setHighLight(bool highlight);

        stringc getScript();
        void setScript(stringc script);

        void addItem(stringc itemName);
        void removeItem(stringc itemName);
        vector<stringc> getItems();
        int getItemCount(stringc itemName);//returns the total of items of type "itemName"
        bool hasItem(stringc itemName);
        void removeAllItems();
		
		void setAnimator(ISceneNodeAnimatorCollisionResponse* coll);
		

        ISceneNode* getNode() { return node; };

    protected:
    private:
        Player();

        ISceneNode* fakeShadow;

        struct playerModel{
            stringc name;

            f32 scale;
			
			bool idle;
			bool walk;
			bool run;
			bool attack;
			bool injured;
			bool die;

			IAnimatedMesh* idlemesh;
			s32 idle_start;
			s32 idle_end;

			IAnimatedMesh* walkmesh;
            s32 walk_start;
            s32 walk_end;

			IAnimatedMesh* runmesh;
			s32 run_start;
			s32 run_end;

			IAnimatedMesh* attackmesh;
			s32 attackevent;
            s32 attack_start;
            s32 attack_end;

			IAnimatedMesh* injuredmesh;
			s32 injured_start;
			s32 injured_end;
			
			IAnimatedMesh* diemesh;
            s32 die_start;
            s32 die_end;

            IAnimatedMesh* mesh;
            IAnimatedMeshSceneNode* node;
        };

		ITimer* timer;
		u32 currentime;
		u32 oldtime;

        ISceneNode* node;

        playerModel currentModel;

        vector<playerModel> playerModels;

        vector3df walkTarget;
        f32 walkSpeed;

        DynamicObject* enemyUnderAttack;

        PLAYER_ANIMATION currentAnimation;
		vector<PlayerObject_Animation> animations;
		ISceneNodeAnimatorCollisionResponse* anim;

        int life;
        int money;

		int oldlife;

        lua_State *L;

        vector<stringc> items;

        stringc script;

        static int getPosition(lua_State *LS);
        static int setPosition(lua_State *LS);
};

#endif // MAINCHARACTER_H
