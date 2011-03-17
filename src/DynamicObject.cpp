#include "DynamicObject.h"

#include "App.h"
#include "DynamicObjectsManager.h"
#include "HealthSceneNode.h"
#include "combat.h"
#include "LuaGlobalCaller.h"
#include "Player.h"


using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


DynamicObject::DynamicObject(irr::core::stringc name, irr::core::stringc meshFile, vector<DynamicObject_Animation> animations)
{
    ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();

    stringc realFile = "../media/dynamic_objects/";
    realFile += meshFile;
	//printf("Here is the object: %s \n",realFile.c_str());
	smgr = App::getInstance()->getDevice()->getSceneManager();
	mesh = smgr->getMesh(realFile);
	//meshName = meshFile;
    this->animations = animations;

	setupObj(name, mesh);
	// Setup default properties for all dynamic objects
	initProperties();


	// When enabled, the LUA will update even if the node is culled.
	this->nodeLuaCulling = false;
	lastframe=0;
	enabled=true;
	enemyUnderAttack=NULL;
	setAnimation("idle");

	// Tries out animation blending
	//scene::IAnimatedMeshSceneNode* nodeBlend = (IAnimatedMeshSceneNode*)node;
	//nodeBlend->setJointMode(irr::scene::EJUOR_CONTROL);
	//nodeBlend->setTransitionTime(0.5);


	timer = App::getInstance()->getDevice()->getTimer()->getRealTime();
}

DynamicObject::DynamicObject(stringc name, IMesh* mesh, vector<DynamicObject_Animation> animations)
{
    this->animations = animations;

    setupObj(name, mesh);

	// Tries out animation blending
	//scene::IAnimatedMeshSceneNode* nodeBlend = (IAnimatedMeshSceneNode*)node;
	//nodeBlend->setJointMode(irr::scene::EJUOR_CONTROL);
	//nodeBlend->setTransitionTime(0.5f);

	initProperties();

	timer = App::getInstance()->getDevice()->getTimer()->getRealTime();
	timer2 = App::getInstance()->getDevice()->getTimer()->getRealTime();
}

DynamicObject::~DynamicObject()
{
    selector->drop();
    node->remove();
	Healthbar->remove();
}

void DynamicObject::initProperties()
{
	// Initialize the properties
	this->properties.armor=0;
	this->prop_base.armor=0;
	this->prop_level.armor=0;

	this->properties.dotduration=0;
	this->prop_base.dotduration=0;
	this->prop_level.dotduration=0;

	this->properties.experience=0;
	this->prop_base.experience=0;
	this->prop_level.experience=0;

	this->properties.hurt_resist=0;
	this->prop_base.hurt_resist=0;
	this->prop_level.hurt_resist=0;

	this->properties.level=0;
	this->prop_base.level=0;
	this->prop_level.level=0;

	this->properties.life=0;
	this->prop_base.life=0;
	this->prop_level.life=0;

	this->properties.magic_armor=0;
	this->prop_base.magic_armor=0;
	this->prop_level.magic_armor=0;

	this->properties.mana=0;
	this->prop_base.mana=0;
	this->prop_level.mana=0;

	this->properties.maxdamage=0;
	this->prop_base.maxdamage=0;
	this->prop_level.maxdamage=0;

	this->properties.maxlife=0;
	this->prop_base.maxlife=0;
	this->prop_level.maxlife=0;

	this->properties.maxmana=0;
	this->prop_base.maxmana=0;
	this->prop_level.maxmana=0;

	this->properties.mindamage=0;
	this->prop_base.mindamage=0;
	this->prop_level.mindamage=0;

	this->properties.money=0;
	this->prop_base.money=0;
	this->prop_level.money=0;

	this->properties.regenlife=0;
	this->prop_base.regenlife=0;
	this->prop_level.regenlife=0;

	this->properties.regenmana=0;
	this->prop_base.regenmana=0;
	this->prop_level.regenmana=0;

	this->properties.dodge_prop=0;
	this->prop_base.dodge_prop=0;
	this->prop_level.dodge_prop=0;

	this->properties.hit_prob=0;
	this->prop_base.hit_prob=0;
	this->prop_level.hit_prob=0;
	// end

	//Default values
	this->properties.life = 100;
	this->properties.experience = 10; // for a NPC this will give 10 XP to the attacker if he win
    this->properties.money = 0;
	this->properties.mindamage=1;
	this->properties.maxdamage=3;
	this->properties.maxlife=100;
	this->properties.maxmana=100;
	this->properties.dodge_prop=12;
	this->properties.hit_prob=70;

}

void DynamicObject::setupObj(stringc name, IMesh* mesh)
{
    ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();
//	IVideoDriver* driver = App::getInstance()->getDevice()->getVideoDriver();

    this->mesh = mesh;
    this->name = name;

	if(hasAnimation())
	{
		this->mesh->setHardwareMappingHint(EHM_DYNAMIC);

        nodeAnim = smgr->addAnimatedMeshSceneNode((IAnimatedMesh*)mesh,0,0x0010);
		//nodeAnim->setJointMode(irr::scene::EJUOR_CONTROL);
	    //nodeAnim->setTransitionTime(0.5f);
		this->node = nodeAnim;

	}
    else
	{
		this->mesh->setHardwareMappingHint(EHM_STATIC);
        this->node = smgr->addMeshSceneNode((IAnimatedMesh*)mesh,0,0x0010);
	}
	if (node)
	{//this->selector = smgr->createTriangleSelector(mesh,node);
		this->selector = smgr->createTriangleSelectorFromBoundingBox(node);
		this->node->setTriangleSelector(selector);

		this->animator = NULL;
		if (objectType != OBJECT_TYPE_EDITOR)
		{
			node->setDebugDataVisible(EDS_BBOX | EDS_SKELETON);
			//Fake Shadow
			fakeShadow = smgr->addMeshSceneNode(smgr->getMesh("../media/dynamic_objects/shadow.obj"),node);
			fakeShadow->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
			fakeShadow->setPosition(vector3df(0,0.03f + (rand()%5)*0.01f ,0));
			// Temporary fix. Need to have a shadow scaled to the size of the object.

			if (name=="player_normal")
				fakeShadow->setScale(vector3df(32,32,32));
			fakeShadow->setMaterialFlag(EMF_FOG_ENABLE,true);

			if(hasAnimation()) this->setFrameLoop(0,0);
		}
		else
			setAnimation("idle");

		// Setup the animations
		f32 meshSize = this->getNode()->getBoundingBox().getExtent().Y;
	    f32 meshScale = this->getNode()->getScale().Y;
		printf ("Scaling for node: %s, is meshSize %f, meshScale: %f, final scale: %f\n",this->getName().c_str(),meshSize,meshScale,meshSize*meshScale);
		script = "";
		this->setEnabled(true);
		node->setMaterialFlag(EMF_FOG_ENABLE,true);
		objLabel = smgr->addTextSceneNode(GUIManager::getInstance()->getFont(FONT_ARIAL),L"",SColor(255,255,255,0),node,vector3df(0,meshSize*meshScale*1.1f,0));
		objLabel->setVisible(false);
		scene::ISceneCollisionManager* coll = smgr->getSceneCollisionManager();
		Healthbar = new scene::HealthSceneNode(this->node,smgr,-1,coll,50,5,vector3df(0,meshSize*meshScale*1.05f,0),video::SColor(255,192,0,0),video::SColor(255,0,0,0),video::SColor(255,128,128,128));
		Healthbar->setVisible(false);

	}
}

DynamicObject* DynamicObject::clone()
{
    DynamicObject* newObj = new DynamicObject(name,mesh,animations);

    newObj->setScale(this->getScale());
    newObj->setMaterialType(this->getMaterialType());
	newObj->setType(typeText);
    newObj->templateObjectName = this->templateObjectName;///TODO: scale and material can be protected too, then we does not need get and set for them.

    return newObj;
}
//-----------------------------------------------------------------------
// Return node info
//-----------------------------------------------------------------------
ISceneNode* DynamicObject::getNode()
{
    return node;
}

ISceneNode* DynamicObject::getShadow()
{
	return fakeShadow;
}

stringc DynamicObject::getTemplateObjectName()
{
    return templateObjectName;
}

void DynamicObject::setTemplateObjectName(stringc newName)
{
    templateObjectName = newName;
}

//-----------------------------------------------------------------------
// Character Movement
//-----------------------------------------------------------------------
void DynamicObject::lookAt(vector3df pos)
{
    vector3df offsetVector = pos - node->getPosition();

    vector3df rot = (-offsetVector).getHorizontalAngle();

    rot.X=0;
    rot.Z=0;

    node->setRotation(rot);
}

void DynamicObject::setPosition(vector3df pos)
{

	node->setPosition(pos);
	vector3df pos2 = node->getAbsolutePosition();
	node->updateAbsolutePosition();
}

vector3df DynamicObject::getPosition()
{
	vector3df pos = fakeShadow->getAbsolutePosition();
	return pos;
	//return node->getPosition();
}

void DynamicObject::setRotation(vector3df rot)
{
    node->setRotation(rot);
}

vector3df DynamicObject::getRotation()
{
    return node->getRotation();
}

void DynamicObject::moveObject(f32 speed)
{

	vector3df pos=this->getPosition();
	pos.Z -= cos((this->getRotation().Y)*PI/180)*speed;
    pos.X -= sin((this->getRotation().Y)*PI/180)*speed;
    pos.Y = 0;///TODO: fixar no Y da terrain (gravidade)

	this->setPosition(pos);
	currentObject=this;
	currentSpeed=speed;
}

void DynamicObject::walkTo(vector3df targetPos)
{
	// Will have the object walk to the targetposition at the current speed.
	// Walk can be interrupted by:
	// - A collision with another object
	// - Moving into a part of the terrain that is not reachable (based on height of terrain)

	targetPos = vector3df((f32)round32(targetPos.X),(f32)round32(targetPos.Y),(f32)round32(targetPos.Z));
	this->lookAt(targetPos);

	f32 speed = currentAnim.walkspeed;

    vector3df pos=this->getPosition();
    pos.Z -= cos((this->getRotation().Y)*PI/180)*speed;
    pos.X -= sin((this->getRotation().Y)*PI/180)*speed;
    pos.Y = 0;///TODO: fixar no Y da terrain (gravidade)
	f32 height = TerrainManager::getInstance()->getHeightAt(pos);

	//TODO: Fix the problem with custom scaling of the objects
	if (height>-(0.09f*72) && height<(0.05f*72) && !collided)
	{
		pos.Y = height;
		this->setPosition(pos);

	}
	else
	{
		if (objectType==OBJECT_TYPE_PLAYER)
			DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(false);
		walkTarget = this->getPosition();
		this->setAnimation("idle");
		printf("Stop because of a collision...\n");
		collided=false; // reset the collision flag
	}
}

void DynamicObject::setWalkTarget(vector3df newTarget)
{
    walkTarget = newTarget;
}

vector3df DynamicObject::getWalkTarget()
{
	return walkTarget;
}


f32 DynamicObject::getDistanceFrom(vector3df pos)
{
    return node->getPosition().getDistanceFrom(pos);
}

DynamicObject * DynamicObject::getCurrentEnemy()
{
	return enemyUnderAttack;
}

void DynamicObject::clearEnemy()
{
	enemyUnderAttack=NULL;
}

//-----------------------------------------------------------------------
// Properties
//-----------------------------------------------------------------------
property DynamicObject::getProperties()
{
	return this->properties;
}

void DynamicObject::setProperties(property prop)
{
	properties = prop;
}


property DynamicObject::getProp_base()
{
	return this->prop_base;
}

void DynamicObject::setProp_base(property prop)
{
	prop_base=prop;
}

property DynamicObject::getProp_level()
{
	return this->prop_level;
}

void DynamicObject::setProp_level(property prop)
{
	prop_level=prop;
}

void DynamicObject::setEnabled(bool enabled)
{
    this->enabled = enabled;

    this->node->setVisible(enabled);
	if (!enabled)
		DynamicObjectsManager::getInstance()->updateMetaSelector();
	if (this->getNode()->isVisible()==false && this==Player::getInstance()->getObject()->getCurrentEnemy())
	{
		Player::getInstance()->getObject()->clearEnemy();
		Player::getInstance()->getObject()->setAnimation("idle");
	}
}

bool DynamicObject::isEnabled()
{
	return enabled;
}
void DynamicObject::setType(stringc name)
{
	if (name=="npc")
		this->objectType=OBJECT_TYPE_NPC;
	if (name=="interactive")
		this->objectType=OBJECT_TYPE_INTERACTIVE;
	if (name=="non-interactive")
		this->objectType=OBJECT_TYPE_NON_INTERACTIVE;
	if (name=="player")
		this->objectType=OBJECT_TYPE_PLAYER;
	if (name=="editor")
		this->objectType=OBJECT_TYPE_EDITOR;
	this->typeText = name;
}

TYPE DynamicObject::getType()
{
	return objectType;
}

void DynamicObject::setName(stringc name)
{
    this->name = name;
    this->getNode()->setName(name);
}

stringc DynamicObject::getName()
{
    return name;
}

void DynamicObject::setMaterialType(E_MATERIAL_TYPE mType)
{
    node->setMaterialType(mType);
}

E_MATERIAL_TYPE DynamicObject::getMaterialType()
{
    return node->getMaterial(0).MaterialType;
}

void DynamicObject::setScale(vector3df scale)
{
    node->setScale(scale);
}

vector3df DynamicObject::getScale()
{
    return node->getScale();
}

void DynamicObject::setLife(int life)
{
    this->properties.life = life;
	if (objectType==OBJECT_TYPE_PLAYER)
		Player::getInstance()->updateDisplay();
	// Trigger the death animation immediately.
	if (life==0)
	{
		this->setAnimation("die");
	}
}

int DynamicObject::getLife()
{
	if (properties.life)
		return this->properties.life;
	else
		return 0;
}

void DynamicObject::setMoney(int money)
{
	this->properties.money = money;
}

int DynamicObject::getMoney()
{
	
	return this->properties.money;

}

// Label
void DynamicObject::setObjectLabel(stringc label)
{
    objLabel->setText(stringw(label).c_str());
	s32 percent = (properties.life * 100);
	s32 p2 = percent / 100;
	if (properties.maxlife>0)
		p2 = percent / properties.maxlife;

	Healthbar->setProgress(p2);
}

void DynamicObject::objectLabelSetVisible(bool visible)
{
    objLabel->setVisible(visible);
	Healthbar->setVisible(visible);
}
//-----------------------------------------------------------------------
// Animation system functions
//-----------------------------------------------------------------------
void DynamicObject::setFrameLoop(s32 start, s32 end)
{
    if(hasAnimation()) ((IAnimatedMeshSceneNode*)node)->setFrameLoop(start,end);
}

void DynamicObject::setAnimationSpeed(f32 speed)
{
    if(hasAnimation()) ((IAnimatedMeshSceneNode*)node)->setAnimationSpeed(speed);
}

OBJECT_ANIMATION DynamicObject::getAnimationState(stringc animName)
{
	if (animName==NULL)
		return OBJECT_ANIMATION_CUSTOM;
	//printf("Asked animation name is: %s\n",animName.c_str());
	OBJECT_ANIMATION Animation;
	// Preset the animation type as "custom", but can be overwritten if standard types are discovered.
	Animation=OBJECT_ANIMATION_CUSTOM;
	if (animName=="idle")
		Animation=OBJECT_ANIMATION_IDLE;
	if (animName=="walk")
		Animation=OBJECT_ANIMATION_WALK;
	if (animName=="run")
		Animation=OBJECT_ANIMATION_RUN;
	if (animName=="attack")
		Animation=OBJECT_ANIMATION_ATTACK;
	if (animName=="injured")
		Animation=OBJECT_ANIMATION_INJURED;
	if (animName=="knockback")
		Animation=OBJECT_ANIMATION_KNOCKBACK;
	if (animName=="die")
		Animation=OBJECT_ANIMATION_DIE;
	if (animName=="die_knockback")
		Animation=OBJECT_ANIMATION_DIE_KNOCKBACK;
	if (animName=="spawn")
		Animation=OBJECT_ANIMATION_SPAWN;
	if (animName=="despawn")
		Animation=OBJECT_ANIMATION_DESPAWN;
	if (animName=="despawn_knockback")
		Animation=OBJECT_ANIMATION_DESPAWN_KNOCKBACK;
	return Animation;
}

OBJECT_ANIMATION DynamicObject::getAnimation(void)
{
	return currentAnimation;
}

void DynamicObject::setAnimation(stringc animName)
{
	// Setup the animation skinning of the meshes (Allow external animation to be used)
	ISkinnedMesh* skin = NULL;
	ISkinnedMesh* defaultskin = NULL;
	if (this->mesh)
		defaultskin = (ISkinnedMesh*)this->mesh;

	// Search for the proper animation name and set it.
    for(int i=0;i < (int)animations.size();i++)
    {
		DynamicObject_Animation tempAnim = (DynamicObject_Animation)animations[i];
		OBJECT_ANIMATION Animation = this->getAnimationState(animName);
		if( tempAnim.name == animName )
        {
			if (Animation!=this->currentAnimation)
			{
				// Setup the skinned mesh animation. Check if the meshname is present
				if (tempAnim.meshname!=L"undefined" && defaultskin)
				{
					skin = (ISkinnedMesh*)tempAnim.mesh;
					defaultskin->useAnimationFrom(skin);
				}
				else if (defaultskin)
					defaultskin->useAnimationFrom(defaultskin);

				// Set the frameloop, the current animation and the speed
				this->currentAnimation=Animation;
				this->currentAnim=tempAnim;

				this->setFrameLoop(tempAnim.startFrame,tempAnim.endFrame);
				this->setAnimationSpeed(tempAnim.speed);
				this->nodeAnim->setLoopMode(tempAnim.loop);
			}
            return;
        }
    }

	#ifdef APP_DEBUG
    cout << "ERROR : DYNAMIC_OBJECT : ANIMATION " << animName.c_str() <<  " NOT FOUND!" << endl;
    #endif
}

// To do, this will trigger the combat damage and the sound when it reach the proper frame
// It is checking the current animation (does care what animation)
// Called at each refresh (1/60 th sec)
void DynamicObject::checkAnimationEvent()
{
	// Check if the current animation have an attack event
	if (enemyUnderAttack && (s32)nodeAnim->getFrameNr()!=lastframe)
	{
		if (((s32)nodeAnim->getFrameNr() == currentAnim.attackevent))

		{
			printf("Should trigger the attack now...\n");
			// Init the combat
			Combat::getInstance()->attack(this,enemyUnderAttack);

		}
		//printf("Current Frame of animation is: %i, and lastframe is %i\n",(s32)nodeAnim->getFrameNr(),lastframe);
	}

	// Check if the current animation have an sound event
	if ((currentAnim.sound.size() > 0) &&
		(nodeAnim->getFrameNr() > currentAnim.soundevent) &&
		(nodeAnim->getFrameNr() < currentAnim.soundevent+1))
	{
		printf("Should trigger the sound now...\n");
	}
	lastframe=(s32)nodeAnim->getFrameNr();

}
//-----------------------------------------------------------------------
// Collision response
//-----------------------------------------------------------------------
void DynamicObject::setAnimator(ISceneNodeAnimatorCollisionResponse* animator_node)
{
	this->animator=animator_node;
}

ISceneNodeAnimatorCollisionResponse* DynamicObject::getAnimator()
{
	return animator;
}

ITriangleSelector* DynamicObject::getTriangleSelector()
{
    return selector;
}



// Main attack feature (Player + NPC)
void DynamicObject::attackEnemy(DynamicObject* obj)
{
	if (enemyUnderAttack==obj)
		return;

    enemyUnderAttack = obj;

    if(obj)
    {
		printf("Attack for this enemy asked %s\n",obj->getName().c_str());
        this->lookAt(obj->getPosition());
        this->setAnimation("attack");
		obj->notifyClick();
    }

}

//-----------------------------------------------------------------------
// INVENTORY features
//-----------------------------------------------------------------------
void DynamicObject::addItem(stringc itemName)
{
    items.push_back(itemName);
}

void DynamicObject::removeItem(stringc itemName)
{
    for(int i=0;i<(int)items.size();i++)
    {
        if(items[i] == itemName)
        {
            items.erase(items.begin() + i);
            return;//remove only one item
        }
    }
}

vector<stringc> DynamicObject::getItems()
{
    return items;
}

int DynamicObject::getItemCount(stringc itemName)
{
    int total = 0;

    for(int i=0;i<(int)items.size();i++)
    {
        if(items[i] == itemName)
        {
            total++;
        }
    }

    return total;
}

bool DynamicObject::hasItem(stringc itemName)
{
    for(int i=0;i<(int)items.size();i++)
    {
        if(items[i] == itemName)
        {
            return true;
        }
    }

    return false;
}

void DynamicObject::removeAllItems()
{
    items.clear();
}
//-----------------------------------------------------------------------
//Script management
//-----------------------------------------------------------------------
stringc DynamicObject::getScript()
{
    return script;
}

void DynamicObject::setScript(stringw script)
{
    this->script = script;
}

void DynamicObject::clearScripts()
{
    if(hasAnimation())
	{
		this->setFrameLoop(0,0);
		this->setAnimation("idle");
		printf("Script had been cleared... idle.\n");
	}
	/*if (objectType == OBJECT_TYPE_PLAYER)
	{
		printf("Player restore the original position \n");
		lua_getglobal(L,"IRBRestorePlayerParams");
		if(lua_isfunction(L, -1)) lua_call(L,0,0);
		lua_pop( L, -1 );
		lua_close(L);
	}*/
	if (objectType != OBJECT_TYPE_PLAYER)
	{
		lua_close(L);
	}
}

void DynamicObject::doScript()
{
    // create an Lua pointer instance
    L = lua_open();

    // load the libs
    luaL_openlibs(L);

    // register dynamic object functions
    lua_register(L,"setPosition",setPosition);
    lua_register(L,"getPosition",getPosition);
    lua_register(L,"setRotation",setRotation);
    lua_register(L,"getRotation",getRotation);
    lua_register(L,"lookAt",lookAt);
    lua_register(L,"lookToObject",lookToObject);

	lua_register(L,"attack",attackObj);
	lua_register(L,"setPropertie",setPropertie);
	lua_register(L,"getPropertie",getPropertie);
    lua_register(L,"move",move);
	lua_register(L,"walkTo",walkToLUA);
    lua_register(L,"distanceFrom",distanceFrom);
	lua_register(L,"getName",getNameLUA);

    lua_register(L,"setFrameLoop",setFrameLoop);
    lua_register(L,"setAnimationSpeed",setAnimationSpeed);
    lua_register(L,"setAnimation",setAnimation);

    lua_register(L,"showObjectLabel",showObjectLabel);
    lua_register(L,"hideObjectLabel",hideObjectLabel);
    lua_register(L,"setObjectLabel",setObjectLabel);

    lua_register(L,"setEnabled",setEnabled);

    //register basic functions
    LuaGlobalCaller::getInstance()->registerBasicFunctions(L);

    //associate the "objName" keyword to the dynamic object name
    stringc scriptTemp = "objName = '";
    scriptTemp += this->getNode()->getName();
    scriptTemp += "'";
    luaL_dostring(L,scriptTemp.c_str());


    luaL_dostring(L,stringc(script).c_str());

    //set default object type
    luaL_dostring(L,"objType = 'OBJECT'");
    //set enemy (when you click an enemy you attack it)
    luaL_dostring(L,"function setEnemy() objType = 'ENEMY' end");
    //set object (when you click an object you interact with it)
    luaL_dostring(L,"function setObject() objType = 'OBJECT' end");


	//run onLoad() function if it exists
    lua_getglobal(L,"onLoad");
    //if top of stack is not a function then onLoad does not exist
    if(lua_isfunction(L, -1)) lua_call(L,0,0);
    lua_pop( L, -1 );
	storeParams();

}

void DynamicObject::storeParams()
{
	// store in memory the current position and rotation of the object for later retrieval.
	this->originalPosition=this->getPosition();
	this->originalRotation=this->getRotation();
}

void DynamicObject::restoreParams()
{
    // Restore the initial parameters of the dynamic object.
	this->setPosition(this->originalPosition);
	this->setRotation(this->originalRotation);
	this->setEnabled(true);
	this->objLabel->setVisible(false);
	this->deadstate=false;
}

void DynamicObject::saveToXML(TiXmlElement* parentElement)
{
	if (this->getType()!=OBJECT_TYPE_PLAYER)
	{
		TiXmlElement* dynamicObjectXML = new TiXmlElement("obj");
		//dynamicObjectXML->SetAttribute("name",name.c_str());

		dynamicObjectXML->SetAttribute("x",stringc(this->getPosition().X).c_str());
		dynamicObjectXML->SetAttribute("y",stringc(this->getPosition().Y).c_str());
		dynamicObjectXML->SetAttribute("z",stringc(this->getPosition().Z).c_str());

		dynamicObjectXML->SetAttribute("s",stringc(this->getScale().X).c_str());

		dynamicObjectXML->SetAttribute("r",stringc(this->getRotation().Y).c_str());

		dynamicObjectXML->SetAttribute("template",templateObjectName.c_str());
		dynamicObjectXML->SetAttribute("script",getScript().c_str());
		dynamicObjectXML->SetAttribute("life",this->properties.life);
		dynamicObjectXML->SetAttribute("maxlife",this->properties.maxlife);
		dynamicObjectXML->SetAttribute("mana",this->properties.mana);
		dynamicObjectXML->SetAttribute("maxmana",this->properties.maxmana);
		dynamicObjectXML->SetAttribute("level",this->properties.level);
		dynamicObjectXML->SetAttribute("XP",this->properties.experience);
		dynamicObjectXML->SetAttribute("mindamage",this->properties.mindamage);
		dynamicObjectXML->SetAttribute("maxdamage",this->properties.maxdamage);
		dynamicObjectXML->SetAttribute("hurtprob",this->properties.hurt_resist);
		dynamicObjectXML->SetAttribute("dodgechance",stringc(this->properties.dodge_prop).c_str());
		dynamicObjectXML->SetAttribute("hitchance",stringc(this->properties.hit_prob).c_str());

		parentElement->LinkEndChild(dynamicObjectXML);
	}
}

// Update the node, for animation event, collision check, lua refresh, etc.
void DynamicObject::update()
{

	// Check for an event in the current animation.
	checkAnimationEvent();

	// Check for collision with another node
	if (animator && animator->collisionOccurred())
	{
		//printf ("Collision occured with %s\n",anim->getCollisionNode()->getName());
		collided=true;
		notifyCollision();
		stringc namecollide = animator->getCollisionNode()->getName();
		printf ("Collision occured with this: %s\n",namecollide.c_str());
	}

	// timed interface an culling check.
	// Added a timed call to the lua but only a 1/4 sec intervals. (Should be used for decision making)
	// Check if the node is in walk state, so update the walk at 1/60 intervals (animations need 1/60 check)
	// Check for culling on a node and don't update it if it's culled.

	u32 timerobject = App::getInstance()->getDevice()->getTimer()->getRealTime();
	bool culled = false;
	//check if the node is culled
	culled = App::getInstance()->getDevice()->getSceneManager()->isCulled(this->getNode());

	// This is for the LUA move command. Refresh and update the position of the mesh (Now refresh of this is 1/60th sec)
	if (currentAnimation==OBJECT_ANIMATION_WALK && !culled && (timerobject-timer2>17) && (objectType!=OBJECT_TYPE_PLAYER)) // 1/60 second
	{
		updateWalk();
		if (currentSpeed!=0)
			//currentObject->moveObject(currentSpeed);
		timer2=timerobject;
	}
	// Tries out animation blending.
	//nodeAnim->animateJoints();
	if((timerobject-timer>250) && enabled) // Lua UPdate to 1/4 second
	{

		if (!nodeLuaCulling)
		{// Special abilitie of the object. this will overide the culling refresh
			if (!culled)
			{
				luaRefresh();
				timer = timerobject;
			}
		} else
		{// if not then check if the node is culled to refresh

			luaRefresh();

			timer = timerobject;
		}
	}
}

void DynamicObject::updateWalk()
{
	f32 meshSize = this->getNode()->getBoundingBox().getExtent().X;
	f32 meshScale = this->getScale().X;

		// Walk until in range
		if( (this->getPosition().getDistanceFrom(walkTarget) > (meshScale*meshSize)) &&  (this->getLife()!=0))
		{
			TerrainManager::getInstance()->getHeightAt(walkTarget);
			if (this->getAnimation()!=OBJECT_ANIMATION_WALK)
			{
				this->setAnimation("walk");
				printf("Hey the object specifically asked for a walk state!\n");
			}

			this->walkTo(walkTarget);
			return;
		}

		// Stop the walk when in range
		if (this->getAnimation()==OBJECT_ANIMATION_WALK && this->getPosition().getDistanceFrom(walkTarget)==0)
			//this->getPosition().getDistanceFrom(walkTarget) < (meshScale*meshSize))
		{
			printf("Hey the object specifically asked  for a idle state!\n");
			this->setWalkTarget(this->getPosition());
			this->setAnimation("idle");
			if (objectType==OBJECT_TYPE_PLAYER)
				DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(false);
			return;
		}

		// Cancel the move if another animation is triggered
		if (this->getAnimation()!=OBJECT_ANIMATION_WALK || this->getAnimation()==OBJECT_ANIMATION_IDLE)
		{
			this->setWalkTarget(this->getPosition());
		}

}

void DynamicObject::luaRefresh()
{
	if (App::getInstance()->getAppState() > 100)
	{//app_state < APP_STATE_CONTROL
		lua_getglobal(L,"onUpdate");
		if(lua_isfunction(L, -1))
			lua_call(L,0,0);
		lua_pop( L, -1 );

		lua_getglobal(L,"step");

		if(lua_isfunction(L, -1))
			lua_call(L,0,0);
		lua_pop( L, -1 );

		//custom update function (updates walkTo for example..)
		lua_getglobal(L,"CustomDynamicObjectUpdate");

		if(lua_isfunction(L, -1))
			lua_call(L,0,0);
		lua_pop( L, -1 );
	}
	lua_getglobal(L,"CustomDynamicObjectUpdateProgrammedAction");
	if(lua_isfunction(L, -1))
		lua_call(L,0,0);
	lua_pop( L, -1 );
}


//LUA FUNCTIONS

int DynamicObject::setEnabled(lua_State *LS)
{
    int LUAenabled = lua_toboolean(LS, -1);

	bool enabled = false;
	if (LUAenabled==1)
		enabled=true;

	lua_pop(LS, 1);

	///TODO: create getObjectByName() as static to avoid code duplication!
	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);
	///================================

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    tempObj->setEnabled(enabled);

	return 0;
}

int DynamicObject::setPosition(lua_State *LS)
{
    float z = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	float y = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	float x = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);


	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);
	DynamicObjectsManager::getInstance()->getObjectByName(objName)->getNode()->updateAbsolutePosition();
    DynamicObjectsManager::getInstance()->getObjectByName(objName)->setPosition(vector3df(x,y,z));

    return 0;
}

int DynamicObject::setRotation(lua_State *LS)
{
	float z = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	float y = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

    float x = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);
    if(tempObj) tempObj->setRotation(vector3df(x,y,z));

    return 0;
}

int DynamicObject::getPosition(lua_State* LS)
{
    lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);
	if(objName.c_str() == "player")
        {
            vector3df pos = Player::getInstance()->getObject()->getPosition();
			lua_pushnumber(LS,pos.X);
			lua_pushnumber(LS,pos.Y);
			lua_pushnumber(LS,pos.Z);
        }
	else
		{
			DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);
		    if(tempObj)
			{
				 vector3df pos = tempObj->getPosition();

				lua_pushnumber(LS,pos.X);
				lua_pushnumber(LS,pos.Y);
				lua_pushnumber(LS,pos.Z);
			}
		}

    return 3;
}

int DynamicObject::getRotation(lua_State* LS)
{
    lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    if(tempObj)
    {
        vector3df rot = tempObj->getRotation();

        lua_pushnumber(LS,rot.X);
        lua_pushnumber(LS,rot.Y);
        lua_pushnumber(LS,rot.Z);
    }

    return 3;
}

int DynamicObject::turn(lua_State *LS)//turn(degrees)
{
    lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

	float angle = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    if(tempObj)
    {
        tempObj->setRotation(tempObj->getRotation() + vector3df(0,angle,0));
    }

    return 0;
}

int DynamicObject::move(lua_State *LS)//move(speed)
{
    lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

	float speed = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);


    if(tempObj)
    {
		if (tempObj->getAnimation()!=OBJECT_ANIMATION_WALK)
			tempObj->setAnimation("walk");
		printf ("Lua call the walk animation.\n");
		tempObj->moveObject(speed);
    }

    return 0;
}

int DynamicObject::walkToLUA(lua_State *LS)
{
	float z = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	float y = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

    float x = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

    lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

	DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);


    if(tempObj)
    {
		if (tempObj->getAnimation()!=OBJECT_ANIMATION_WALK)
			tempObj->setAnimation("walk");
		tempObj->setWalkTarget(vector3df(x,y,z));
    }

    return 0;
}

int DynamicObject::lookAt(lua_State *LS)
{
	float z = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	float y = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

    float x = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    if(tempObj)
    {
        tempObj->lookAt(vector3df(x,y,z));
    }

    return 0;
}

int DynamicObject::lookToObject(lua_State *LS)
{
    stringc otherObjName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);


    vector3df otherObjPosition;

    if(otherObjName == "player")
    {
		otherObjPosition = Player::getInstance()->getObject()->getPosition();
    }
    else
    {
        DynamicObject* otherObj = DynamicObjectsManager::getInstance()->getObjectByName(GlobalMap::getInstance()->getGlobal(otherObjName.c_str()).c_str());

        otherObjPosition = otherObj->getPosition();
    }


    tempObj->lookAt(otherObjPosition);


    return 0;
}

int DynamicObject::attackObj(lua_State *LS)
{
	stringc otherObjName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

	DynamicObject* tempObj = NULL;
	if(otherObjName != "player")
    {
		tempObj = DynamicObjectsManager::getInstance()->getObjectByName(otherObjName);
	}
	else
	{
		tempObj = Player::getInstance()->getObject();
	}
	if (tempObj)
	{
		printf("The LUA use attack with that target: %s\n",tempObj->getName().c_str());
		Combat::getInstance()->attack(DynamicObjectsManager::getInstance()->getObjectByName(objName),tempObj);
	}

	return 0;
}

int DynamicObject::setPropertie(lua_State *LS)
{
	float value = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	stringc propertieName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

	// I would like to specify the object name so lua could set the properies of another object (another command?)
	//stringc otherObjName = lua_tostring(LS, -1);
	//lua_pop(LS, 1);

	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);
	DynamicObject* Obj = DynamicObjectsManager::getInstance()->getObjectByName(objName);
	if (propertieName=="life")
		Obj->properties.life = (int)value;
	if (propertieName=="maxlife")
		Obj->properties.maxlife = (int)value;
	if (propertieName=="mindamage")
		Obj->properties.mindamage = (int)value;
	if (propertieName=="maxdamage")
		Obj->properties.maxdamage = (int)value;
	if (propertieName=="hurt_resist")
		Obj->properties.hurt_resist = (int)value;
	if (propertieName=="experience")
		Obj->properties.experience = (int)value;
	if (propertieName=="dodge_prob")
		Obj->properties.dodge_prop = (f32)value;
	if (propertieName=="hit_prob")
		Obj->properties.hit_prob = (f32)value;
	if (propertieName=="mana")
		Obj->properties.mana = (int)value;
	if (propertieName=="maxmana")
		Obj->properties.maxmana = (int)value;
	if (propertieName=="money")
		Obj->properties.money = (int)value;
	if (propertieName=="dotduration")
		Obj->properties.dotduration = (int)value;
	if (propertieName=="armor")
		Obj->properties.armor = (int)value;
	if (propertieName=="magic_armor")
		Obj->properties.magic_armor = (int)value;
	if (propertieName=="regenlife")
		Obj->properties.regenlife = (int)value;
	if (propertieName=="regenmana")
		Obj->properties.regenmana = (int)value;

	//if (otherObjName=="me")

	return 0;
}

int DynamicObject::getPropertie(lua_State *LS)
{
	stringc propertieName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

	//stringc otherObjName = lua_tostring(LS, -1);
	//lua_pop(LS, 1);

	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);
	int value = 0;
	DynamicObject* Obj = DynamicObjectsManager::getInstance()->getObjectByName(objName);
	if (propertieName=="life")
		value = Obj->properties.life;
	if (propertieName=="maxlife")
		value = Obj->properties.maxlife;
	if (propertieName=="mindamage")
		value = Obj->properties.mindamage;
	if (propertieName=="maxdamage")
		value = Obj->properties.maxdamage;
	if (propertieName=="hurt_resist")
		value = Obj->properties.hurt_resist;
	if (propertieName=="experience")
		value = Obj->properties.experience;

	lua_pushnumber(LS,value);

	return 1;
}
int DynamicObject::getNameLUA(lua_State *LS)
{
	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

	DynamicObject* Obj = DynamicObjectsManager::getInstance()->getObjectByName(objName);
	stringc value = Obj->getTemplateObjectName();
	lua_pushstring(LS,value.c_str());

	return 1;
}
int DynamicObject::setFrameLoop(lua_State *LS)
{
    int start = (int)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	int end = (int)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

    DynamicObjectsManager::getInstance()->getObjectByName(objName)->setFrameLoop(start,end);

    return 0;
}

int DynamicObject::setAnimation(lua_State *LS)
{
    stringc animName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

    DynamicObjectsManager::getInstance()->getObjectByName(objName)->setAnimation(animName);
	return true;
}

int DynamicObject::setAnimationSpeed(lua_State *LS)
{
    f32 speed = (f32)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

    DynamicObjectsManager::getInstance()->getObjectByName(objName)->setAnimationSpeed(speed);
	return 0;
}

int DynamicObject::distanceFrom(lua_State *LS)
{
    vector3df otherPos = vector3df(-1000,-1000,-1000);

    if(lua_isnumber(LS, -1))//read (x,y,z)
    {
        float z = (float)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

        float y = (float)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

        float x = (float)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

        otherPos = vector3df(x,y,z);
    }
    else if(lua_isstring(LS,-1))//get distance from an object
    {
        std::string otherName = lua_tostring(LS, -1);
        lua_pop(LS, 1);

		if(otherName.c_str() == "player")
        {
            otherPos = Player::getInstance()->getObject()->getPosition();
			printf("Asked the distance from the player: %f,%f,%f\n",otherPos.X,otherPos.Y,otherPos.Z);
        }
        else
        {
            DynamicObject* otherObj = DynamicObjectsManager::getInstance()->getObjectByName(GlobalMap::getInstance()->getGlobal(otherName.c_str()).c_str());

            if(otherObj)
            {
                otherPos = otherObj->getPosition();
            }
        }
    }

    lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    if(tempObj)
    {
        lua_pushnumber(LS,otherPos.getDistanceFrom(tempObj->getPosition()));
        return 1;
    }

    return 0;
}

int DynamicObject::showObjectLabel(lua_State *LS)
{
    lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    if(tempObj) tempObj->objectLabelSetVisible(true);

    return 0;
}

int DynamicObject::hideObjectLabel(lua_State *LS)
{
    lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    if(tempObj) tempObj->objectLabelSetVisible(false);

    return 0;
}

int DynamicObject::setObjectLabel(lua_State *LS)
{
    std::string newLabel = lua_tostring(LS, -1);
    lua_pop(LS, 1);

    lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    if(tempObj) tempObj->setObjectLabel(newLabel.c_str());

    return 0;
}


void DynamicObject::notifyClick()
{
    lua_getglobal(L,"onClicked");
    if(lua_isfunction(L, -1)) lua_call(L,0,0);
    lua_pop( L, -1 );
}

void DynamicObject::notifyAttackRange()
{
    lua_getglobal(L,"onAttackRange");
    if(lua_isfunction(L, -1)) lua_call(L,0,0);
    lua_pop( L, -1 );
}

void DynamicObject::notifyCollision()
{
    lua_getglobal(L,"onCollision");
    if(lua_isfunction(L, -1)) lua_call(L,0,0);
    lua_pop( L, -1 );
}


stringc DynamicObject::getObjectType()
{
    lua_getglobal(L,"objType");
    stringc objType = lua_tostring(L, -1);
	lua_pop(L, 1);

	return objType;
}
