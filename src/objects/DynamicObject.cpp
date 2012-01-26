#include "DynamicObject.h"

#include "../App.h"
#include "DynamicObjectsManager.h"
#include "HealthSceneNode.h"
#include "combat.h"
#include "../LuaGlobalCaller.h"
#include "Player.h"


using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


DynamicObject::DynamicObject(irr::core::stringc name, irr::core::stringc meshFile, vector<DynamicObject_Animation> animations)
{
	// This is done when an dynamic object is initialised (template && player)
    ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();

	properties=initProperties();
	prop_base=initProperties();
	prop_level=initProperties();

    stringc realFile = "../media/dynamic_objects/";
    realFile += meshFile;
	//printf("Here is the object: %s \n",realFile.c_str());
	
		mesh = smgr->getMesh(realFile);
	
	//meshName = meshFile;
    this->animations = animations;

	setupObj(name, mesh);

	// When enabled, the LUA will update even if the node is culled.
	this->nodeLuaCulling = false;
	this->templateobject = false;
	lastframe=0;
	enabled=true;
	enemyUnderAttack=NULL;
	namecollide="";
	
	diePresent=true;
	despawnPresent = true;
	runningMode = false;
	soundActivated = false;
	attackActivated = false;
	stunstate=false;
	lastTime=App::getInstance()->getDevice()->getTimer()->getRealTime();

	timerAnimation = App::getInstance()->getDevice()->getTimer()->getRealTime();
	timerLUA = App::getInstance()->getDevice()->getTimer()->getRealTime();
	// Set the animation and AI state to idle (default)
	this->setAnimation("idle");
	this->AI_State=AI_STATE_IDLE;
	// Init the timedelay taken for a loop
	lastTime=0;
}

DynamicObject::DynamicObject(stringc name, IMesh* mesh, vector<DynamicObject_Animation> animations)
{
	// This is done when a new character is created from the template
	properties=initProperties();
	prop_base=initProperties();
	prop_level=initProperties();

    this->animations = animations;

    setupObj(name, mesh);

	enemyUnderAttack=NULL;

	timerAnimation = App::getInstance()->getDevice()->getTimer()->getRealTime();
	timerLUA = App::getInstance()->getDevice()->getTimer()->getRealTime();

	diePresent=true;
	despawnPresent = true;
	runningMode = false;
	soundActivated = false;
	attackActivated = false;

	stunstate=false;
	currentAnimation=OBJECT_ANIMATION_CUSTOM;
	oldAnimation=OBJECT_ANIMATION_CUSTOM;
	this->setAnimation("prespawn");
	lastTime=App::getInstance()->getDevice()->getTimer()->getRealTime();
}

DynamicObject::~DynamicObject()
{
    selector->drop();
	Healthbar->remove();
    node->remove();
	
}

cproperty DynamicObject::initProperties()
{
	cproperty prop;
	// Initialize the property
	prop.armor=0;
	prop.dodge_prop=0;
	prop.dotduration=0;
	prop.experience=0;
	prop.hurt_resist=0;
	prop.hit_prob=0;
	prop.level=0;
	prop.life=0;
	prop.magic_armor=0;
	prop.mana=0;
	prop.maxdamage=0;
	prop.maxdefense=0;
	prop.maxlife=0;
	prop.maxmana=0;
	prop.mindamage=0;
	prop.mindefense=0;
	prop.money=0;
	prop.regenlife=0;
	prop.regenmana=0;
	return prop;

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
		// bone transition test
		//nodeAnim->setJointMode(irr::scene::EJUOR_CONTROL);
	    //nodeAnim->setTransitionTime(0.5f);
		this->node = nodeAnim;
		if (node)
		{
			this->selector = smgr->createOctreeTriangleSelector((IAnimatedMesh*)mesh, node);
			//this->selector = smgr->createTriangleSelectorFromBoundingBox(node);
			this->node->setTriangleSelector(selector);
		}

	}
    else
	{
		this->mesh->setHardwareMappingHint(EHM_STATIC);
        //this->node = smgr->addMeshSceneNode((IAnimatedMesh*)mesh,0,0x0010);
		// Would like to load non-animated meshes as occtrees, but something is crashing.

		// Check the size of the mesh and create it as an octree if it's big. (200 unit min)
		if (mesh->getBoundingBox().getExtent().X>200 && mesh->getBoundingBox().getExtent().Y>200 && mesh->getBoundingBox().getExtent().Z>200)
			this->node = smgr->addOctreeSceneNode ((IMesh*)mesh,0,0x0010);
		else
			this->node = smgr->addMeshSceneNode((IAnimatedMesh*)mesh,0,0x0010);
		if (node)
		{
			//this->selector = smgr->createTriangleSelector((IAnimatedMesh*)mesh,node);
			this->selector = smgr->createOctreeTriangleSelector((IMesh*)mesh, node);
			//this->selector = smgr->createTriangleSelectorFromBoundingBox(node);
			this->node->setTriangleSelector(selector);
		}
	}
	if (node)
	{	
		// Setup the animations
		this->animator = NULL;

		node->setName(name);
	
		f32 meshSize = this->getNode()->getBoundingBox().getExtent().Y;
	    f32 meshScale = this->getNode()->getScale().Y;

		if (objectType != OBJECT_TYPE_EDITOR)
		{
			// Editor objects don't have the fake shadow.
			//node->setDebugDataVisible(EDS_BBOX | EDS_SKELETON);
			//Fake Shadow
			fakeShadow = smgr->addMeshSceneNode(smgr->getMesh("../media/dynamic_objects/shadow.obj"),node);
			fakeShadow->setScale(vector3df(0.75f,0.75f,0.75f));
			fakeShadow->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
			fakeShadow->setPosition(vector3df(0,0.03f + (rand()%5)*0.01f ,0));
			
			// Temporary fix. Need to have a shadow scaled to the size of the object.
			// This need to be calculated proportionnaly to the object.
		
			//(may 13 2011, the player was scaled to 1 unit, so commented that "hack" to have a proper shadow.
			//if (name=="player_normal")
			//	fakeShadow->setScale(vector3df(32,32,32));

			fakeShadow->setMaterialFlag(EMF_FOG_ENABLE,true);

			// This set the frameloop to the static pose, we could use a flag if the user decided this
			//if(hasAnimation()) this->setFrameLoop(0,0);
		}
		
		//printf ("Scaling for node: %s, is meshSize %f, meshScale: %f, final scale: %f\n",this->getName().c_str(),meshSize,meshScale,meshSize*meshScale);
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
	newObj->setTemplate(false);
	// use a temporary state to define animation, will set the idle animation, but with a random initial frame.

	fakeShadow->setVisible(false);
	// Preset telling the die animation is present (will be tested for this)
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
	node->updateAbsolutePosition();
}

vector3df DynamicObject::getPosition()
{
	//Fakeshadow seem to cause problems... Let put this off while we investigate
	/*if (fakeShadow)
	{
		vector3df pos = fakeShadow->getAbsolutePosition();
		return pos;
	}
	else
	*/
	{
		return node->getAbsolutePosition();
		//return vector3df(0,0,0);
	}
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
	
	
	if (getType()==OBJECT_TYPE_PLAYER && Player::getInstance()->getTaggedTarget())
		targetPos = Player::getInstance()->getTaggedTarget()->getPosition();
	else
		targetPos = vector3df((f32)round32(targetPos.X),(f32)round32(targetPos.Y),(f32)round32(targetPos.Z));
	
	this->lookAt(targetPos);

	//Attemp to calculate the proper time/distance interval
	u32 delay=App::getInstance()->getDevice()->getTimer()->getRealTime()-lastTime;
	lastTime=App::getInstance()->getDevice()->getTimer()->getRealTime();
	
	// Temporary removed the distance interval as numbers are
	f32 speed = currentAnim.walkspeed/10;
	//f32 speed = (currentAnim.walkspeed*(f32)delay)/170; //(170 value seem ok for the setting done)
	if (speed == 0)
		speed=1.0f;
		

	 vector3df pos=this->getPosition();
    pos.Z -= cos((this->getRotation().Y)*PI/180)*speed;
    pos.X -= sin((this->getRotation().Y)*PI/180)*speed;
    pos.Y = 0;///TODO: fixar no Y da terrain (gravidade)
	
	// Sampling points on the ground
	vector3df posfront1 = pos+(targetPos.normalize()*20);
	vector3df posfront = pos+(targetPos.normalize()*10);
	vector3df posback1 = pos-(targetPos.normalize()*20);
	vector3df posback = pos-(targetPos.normalize()*10);
	
	// Samples position where the ground is 
	f32 height = TerrainManager::getInstance()->getHeightAt(pos);
	f32 height2 = TerrainManager::getInstance()->getHeightAt(posfront);
	f32 height3 = TerrainManager::getInstance()->getHeightAt(posfront1);
	f32 height4 = TerrainManager::getInstance()->getHeightAt(posback);
	f32 height5 = TerrainManager::getInstance()->getHeightAt(posback1);
	f32 cliff =  height3 - height5; 
	if (cliff<0) 
		cliff = -cliff;

	//printf ("Here are the height: front: %f, middle: %f, back: %f, cliff: %f\n",height2,height,height3,cliff);
	

	//TODO: Fix the problem with custom scaling of the objects
	// old terrain values
	// (height>-(0.09f*72) && height<(0.05f*72)

	// The "cliff" is the number of unit of difference from one point to another
	// The limit in the water is to get to -80 (legs into water)
	if (height>-80 && (cliff < 55) && !collided)
	{
		//pos.Y = height;
		pos.Y=((height+height2+height3+height4+height5)/5)+2;
		this->setPosition(pos);
	
	}
	else
	{
		if (objectType==OBJECT_TYPE_PLAYER)
			DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(false);
		
		walkTarget = this->getPosition();
		if (enemyUnderAttack)
		{
			stringc currentenemy = enemyUnderAttack->getNode()->getName();
			if (namecollide==currentenemy)
			{
				if (objectType==OBJECT_TYPE_PLAYER)
					DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(true);
				lookAt(enemyUnderAttack->getPosition());
				setAnimation("attack");
			} else
				this->setAnimation("idle");

		} else
			this->setAnimation("idle");

		printf("Collision detected..\n");
		
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

void DynamicObject::setTemplate(bool value)
{
	this->templateobject=value;
}

bool DynamicObject::isTemplate()
{
	return this->templateobject;
}

cproperty DynamicObject::getProperties()
{
	return this->properties;
}

void DynamicObject::setProperties(cproperty prop)
{
	properties = prop;
}


cproperty DynamicObject::getProp_base()
{
	return this->prop_base;
}

void DynamicObject::setProp_base(cproperty prop)
{
	prop_base=prop;
}

cproperty DynamicObject::getProp_level()
{
	return this->prop_level;
}

void DynamicObject::setProp_level(cproperty prop)
{
	prop_level=prop;
}

void DynamicObject::setEnabled(bool enabled)
{
    this->enabled = enabled;

    this->node->setVisible(enabled);
	//if (!enabled)
	//	DynamicObjectsManager::getInstance()->updateMetaSelector();
	if (this->getNode()->isVisible()==false && this==Player::getInstance()->getObject()->getCurrentEnemy())
	{
		Player::getInstance()->getObject()->clearEnemy();
		Player::getInstance()->getObject()->setAnimation("prespawn");
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
	if (animName=="hurt")
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
	if (animName=="prespawn")
		Animation=OBJECT_ANIMATION_PRESPAWN;
	return Animation;
}

OBJECT_ANIMATION DynamicObject::getAnimation(void)
{
	return currentAnimation;
}

bool DynamicObject::setAnimation(stringc animName)
{
	//define if we use a random frame in the idle animation
	bool randomize=false;

	// Setup the animation skinning of the meshes (Allow external animation to be used)
	ISkinnedMesh* skin = NULL;
	ISkinnedMesh* defaultskin = NULL;
	

	if (animName=="die")
	{
		// debug output to the console
		stringw text = L"Die animation for character: ";
		text.append(getNode()->getName());
		text.append(L" encountered.");
		GUIManager::getInstance()->setConsoleText(text.c_str(),SColor(255,0,128,0));
		
		// init the DieState timer. (the update() loop will wait 5 sec to initiate the despawn animation)
		timerDie = App::getInstance()->getDevice()->getTimer()->getRealTime();
		// If the current animation is DIE, then remove the collision on the object
		if (this->currentAnimation!=OBJECT_ANIMATION_DIE && this->getType()!=OBJECT_TYPE_PLAYER)
			DynamicObjectsManager::getInstance()->updateMetaSelector();
		// disable the stun state if present. Dying takes over
		stunstate=false;
	}

	// Return if the character is stunned
	if (stunstate)
	{
		printf("The stun state is active no animation is permitted!\n");
		return false;
	}

	// This will activate the "hurt" stun state
	if (animName=="hurt" && !stunstate)
	{
		stunstate=true;
		timerStun = App::getInstance()->getDevice()->getTimer()->getRealTime();
		
	}

	// When a character is dead, don't allow anything exept prespawn or despawn
	if (currentAnimation==OBJECT_ANIMATION_DIE)
	{
		if  (animName!="prespawn" && animName!="despawn")
			return false;
	}

	// Search for the proper animation name and set it.
    for(int i=0;i < (int)animations.size();i++)
    {
		// temporary (until a real prespawn is defined inside the game)
		if (animName=="prespawn")
		{
			stunstate=false;
			animName="idle";
			randomize=true;
			printf("Prespawn is called here.\n");
		}
		DynamicObject_Animation tempAnim = (DynamicObject_Animation)animations[i];
		OBJECT_ANIMATION Animation = this->getAnimationState(animName);

		if (animName=="despawn")
		printf("The despawn animation was called!\n");
		
		if( tempAnim.name == animName )
        {
			if ((Animation!=this->currentAnimation)) //|| Animation==OBJECT_ANIMATION_DIE
			{
				
				// To redo... This is a real mess. 
				// It will need to be implemented in another method
				// All the animation MUST be in a single archive!!!
				// So we can use the anim from another file, the character will need all the animations.
				/*if (this->mesh)
					defaultskin = (ISkinnedMesh*)this->mesh;
				// Setup the skinned mesh animation. Check if the meshname is present
				if (tempAnim.meshname!=L"undefined" && defaultskin)
				{
					skin = (ISkinnedMesh*)tempAnim.mesh;
					defaultskin->useAnimationFrom(skin);
				}
				else if (defaultskin)
					defaultskin->useAnimationFrom(defaultskin);

				*/
				// Store the old animations
				this->oldAnimation=currentAnimation;
				this->oldAnimName = animName;

				// Set the state of the current one
				this->currentAnimation=Animation;
				this->currentAnim=tempAnim;
				
				// Set the frameloop, the current animation and the speed
				this->setFrameLoop(tempAnim.startFrame,tempAnim.endFrame);
				this->setAnimationSpeed(tempAnim.speed);
				this->nodeAnim->setLoopMode(tempAnim.loop);
				
				// Special case for the idle animation (randomisation)
				if (animName=="idle" && randomize)
				{
					printf ("if you drop a character on the map this should display!");
					// Fix a random frame so the idle for different character are not the same.
					if (tempAnim.endFrame>0)
					{
						f32 random = (f32)(rand() % tempAnim.endFrame+1);
						this->nodeAnim->setCurrentFrame(random+1);
					}

					stringw text2 = L"idle animation for character: ";
					text2.append(getNode()->getName());
					text2.append(L" encountered.");
					GUIManager::getInstance()->setConsoleText(text2.c_str(),SColor(255,0,128,0));
				}
			}
            return true;
        }
    }

	#ifdef APP_DEBUG
    cout << "ERROR : DYNAMIC_OBJECT : ANIMATION " << animName.c_str() <<  " NOT FOUND!" << endl;
    #endif

	// If the die animation is not there, the flag become active (will start the die timer anyway)
	// As always this does not apply to the player (event if it misse it's die animation)
	if (animName=="die" && this->getType()!=OBJECT_TYPE_PLAYER) 
		diePresent=false;
	if (animName=="despawn" && this->getType()!=OBJECT_TYPE_PLAYER) 
		despawnPresent=false;

	return false;
}

// To do, this will trigger the combat damage and the sound when it reach the proper frame
// It is checking the current animation (does care what animation)
// Called at each refresh (1/60 th sec)
void DynamicObject::checkAnimationEvent()
{

	// Temporary.. .Print the player animation frames because of a problem
	if (getType()==OBJECT_TYPE_PLAYER && currentAnimation==OBJECT_ANIMATION_DIE)
	{
		stringw temptext="";
		s32 frm = 0;
		frm = (s32)nodeAnim->getFrameNr();
		// Names
		
		temptext="This is the current frame:";
		temptext+=stringw(frm);
		
		GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));
	
	}



	// Check if the character is hurt and tell the combat manager to stop the attack while the character play all the animation
	// Need to update this to support more specific animation that MUST not be stopped
	if ((s32)nodeAnim->getFrameNr()!=lastframe && this->currentAnimation==OBJECT_ANIMATION_INJURED)
	{
		if (((s32)nodeAnim->getFrameNr() == currentAnim.startFrame))
		{
			// Set the AI State to busy, so the combat manager won't call animations
			AI_State=AI_STATE_BUSY;
		}

		// should do something when the animation can reach the end
		// This is required
		if (((s32)nodeAnim->getFrameNr() == currentAnim.endFrame))
		{
			AI_State=AI_STATE_IDLE;
		}

	}
	// Check if the current animation have an attack event
	if ((s32)nodeAnim->getFrameNr()!=lastframe && this->currentAnimation==OBJECT_ANIMATION_ATTACK)
	{
		// Set a default attack event if there is none defined.
		if (currentAnim.attackevent==-1)
			currentAnim.attackevent = currentAnim.startFrame; 
		
		// Should do something if the animation can start
		// This is required
		if (((s32)nodeAnim->getFrameNr() == currentAnim.startFrame))
		{
			// Set the AI State to busy, so the combat manager won't call animations
			AI_State=AI_STATE_BUSY;
		}

		// should do something when the animation can reach the end
		// This is required
		if (((s32)nodeAnim->getFrameNr() == currentAnim.endFrame))
		{
			AI_State=AI_STATE_IDLE;
		}

		// Set up a "default" attack frame in case the user forgot to define one
		if (currentAnim.attackevent=-1)
			currentAnim.attackevent=currentAnim.startFrame+1;

		if (nodeAnim->getFrameNr()<currentAnim.attackevent) 
			attackActivated=true;
		
		if ((nodeAnim->getFrameNr() > currentAnim.attackevent) && attackActivated)
		{

		 // Init the combat for the player, check also that there is a enemy defined
			if (getType()==OBJECT_TYPE_PLAYER)
			{
				if (enemyUnderAttack)
				{
					Combat::getInstance()->attack(this,enemyUnderAttack);
					attackActivated=false;
				}
			} // Init the combat for the NPC (enemy at the moment), will attack the player. Anim is called from lua
			
			if (getType()!=OBJECT_TYPE_PLAYER)
			{
				Combat::getInstance()->attack(this,Player::getInstance()->getObject());
				attackActivated=false;
				GUIManager::getInstance()->setConsoleText(L"Ennemy try to attack",SColor(255,128,0,128));

				
			}
		}
	}

	// Check if the current animation have an sound event
	if (nodeAnim->getFrameNr()<currentAnim.soundevent) 
			soundActivated=true;

	if ((currentAnim.sound.size() > 0) &&
		(nodeAnim->getFrameNr() > currentAnim.soundevent) && soundActivated)
	{
		stringc sound = currentAnim.sound;
		//Play dialog sound (yes you can record voices!)
		ISound * soundfx = NULL;
		u32 timerobject = App::getInstance()->getDevice()->getTimer()->getRealTime();
		// After the sound as been called for this duration, permit to trigger other sounds

		if (sound.size()>0)
		//if((sound.c_str() != "") | (sound.c_str() != NULL))
		{
			soundActivated=false;
			stringc soundName = "../media/sound/";
			soundName += sound.c_str();
			irrklang::vec3df pos = this->getNode()->getPosition();
			if (currentAnim.name=="walk" || currentAnim.name=="run")
				soundfx = SoundManager::getInstance()->playSound3D(soundName.c_str(),pos,false);
			else
				soundfx = SoundManager::getInstance()->playSound2D(soundName.c_str(),false);
		}
	}
	lastframe=(s32)nodeAnim->getFrameNr();

}

void DynamicObject::setRunningMode(bool run)
{
	runningMode=run;
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
		//printf("Attack for this enemy asked %s\n",obj->getName().c_str());
        this->lookAt(obj->getPosition());
		if(obj->getDistanceFrom(Player::getInstance()->getObject()->getPosition()) < 72.0f)
		{
			this->setAnimation("attack");
			obj->notifyClick();
		}
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
		//this->setFrameLoop(0,0);
		this->setAnimation("prespawn");
		//printf("Script had been cleared... idle.\n");
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

	//Dialog Functions
    lua_register(L,"showDialogMessage",showDialogMessage);
	lua_register(L,"showDialogQuestion",showDialogQuestion);

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
    if(lua_isfunction(L, -1)) lua_pcall(L,0,0,0);
    lua_pop( L, -1 );
	storeParams();

}

void DynamicObject::storeParams()
{
	// store in memory the current position and rotation of the object for later retrieval.
	this->originalPosition=this->getPosition();
	this->originalRotation=this->getRotation();
	this->original_life=this->getLife();
	this->original_maxlife=this->getProperties().maxlife;
}

void DynamicObject::restoreParams()
{
    // Restore the initial parameters of the dynamic object.
	this->setPosition(this->originalPosition);
	this->setRotation(this->originalRotation);
	this->setEnabled(true);
	this->objLabel->setVisible(false);
	this->deadstate=false;
	this->setLife(original_life);
	this->properties.maxlife=original_maxlife;
	this->setAnimation("prespawn");
}

void DynamicObject::saveToXML(TiXmlElement* parentElement)
{
	//if (this->getType()!=OBJECT_TYPE_PLAYER)
	//{
		TiXmlElement* dynamicObjectXML = new TiXmlElement("obj");
		//dynamicObjectXML->SetAttribute("name",name.c_str());
		dynamicObjectXML->SetAttribute("type",(int)getType());
		dynamicObjectXML->SetAttribute("x",stringc(this->getPosition().X).c_str());
		dynamicObjectXML->SetAttribute("y",stringc(this->getPosition().Y).c_str());
		dynamicObjectXML->SetAttribute("z",stringc(this->getPosition().Z).c_str());

		dynamicObjectXML->SetAttribute("s",stringc(this->getScale().X).c_str());

		dynamicObjectXML->SetAttribute("r",stringc(this->getRotation().Y).c_str());

		dynamicObjectXML->SetAttribute("template",templateObjectName.c_str());

		if (script.size()>0)
			dynamicObjectXML->SetAttribute("script",getScript().c_str());
		
		if (properties.life>0)
			dynamicObjectXML->SetAttribute("life",this->properties.life);
		
		if (properties.maxlife>0)
			dynamicObjectXML->SetAttribute("maxlife",this->properties.maxlife);
		
		if (properties.mana>0 && properties.mana<101)
			dynamicObjectXML->SetAttribute("mana",this->properties.mana);
		
		if (properties.maxmana>0 && properties.maxmana<101)
			dynamicObjectXML->SetAttribute("maxmana",this->properties.maxmana);
		
		if (properties.level>0 && properties.level<101)
			dynamicObjectXML->SetAttribute("level",this->properties.level);
		
		if (properties.experience>0)
			dynamicObjectXML->SetAttribute("XP",this->properties.experience);
		
		if (properties.mindamage>0)
			dynamicObjectXML->SetAttribute("mindamage",this->properties.mindamage);
		
		if (properties.maxdamage>0)
			dynamicObjectXML->SetAttribute("maxdamage",this->properties.maxdamage);
		
		if (properties.hurt_resist>0 && properties.hurt_resist<101)
			dynamicObjectXML->SetAttribute("hurtresist",this->properties.hurt_resist);
		
		if (properties.dodge_prop>0 && properties.dodge_prop<101)
			dynamicObjectXML->SetAttribute("dodgechance",stringc(this->properties.dodge_prop).c_str());
		
		if (properties.hit_prob>0 && properties.hit_prob<101)
			dynamicObjectXML->SetAttribute("hitchance",stringc(this->properties.hit_prob).c_str());

		if (properties.regenlife>0 && properties.regenlife<101)
			dynamicObjectXML->SetAttribute("regenlife",stringc(this->properties.regenlife).c_str());

		if (properties.regenmana>0 && properties.regenmana<101)
			dynamicObjectXML->SetAttribute("regenmana",stringc(this->properties.regenmana).c_str());

		parentElement->LinkEndChild(dynamicObjectXML);
	//}
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
		namecollide = animator->getCollisionNode()->getName();
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
	//old code: if (currentAnimation==OBJECT_ANIMATION_WALK && !culled && (timerobject-timerLUA>17) && (objectType!=OBJECT_TYPE_PLAYER)) // 1/60 second
	if (currentAnimation==OBJECT_ANIMATION_WALK && !culled && (objectType!=OBJECT_TYPE_PLAYER)) 
	{ // timerLUA=17
		updateWalk();
		if (currentSpeed!=0)
			//currentObject->moveObject(currentSpeed);
		timerLUA=timerobject;
	}
	// Tries out animation blending.
	//nodeAnim->animateJoints();
	if((timerobject-timerAnimation>250) && enabled) // Lua UPdate to 1/4 second
	{

		if (!nodeLuaCulling)
		{// Special abilitie of the object. this will overide the culling refresh
			if (!culled)
			{
				luaRefresh();
				timerAnimation = timerobject;
			}
		} else
		{// if not then check if the node is culled to refresh

			luaRefresh();
			timerAnimation = timerobject;
		}
	}
	// Special timer check for animation states, will trigger after a time has passed
	// Special timer to init when the character is dead for 5 seconds
	if (this->enabled)
	{
		if((this->currentAnimation==OBJECT_ANIMATION_DIE) && (timerobject-timerDie>5000) && (this->getType()!=OBJECT_TYPE_PLAYER) || (!diePresent))
		{
			// Init the despawn timer
			this->setAnimation("despawn");
			timerDespawn = timerobject;	
		}
	}
	// Special timer to init when the character is being despawned (5 seconds)
	// This can be overided if the character don't have a die or despawn animation
	if (!despawnPresent && isEnabled()) 
	{
		printf("No despawn Anim, we should see the disabling now!\n");
		setAnimation("prespawn");
		this->setEnabled(false);
		return;
	}

	if ((this->currentAnimation==OBJECT_ANIMATION_DESPAWN && this->isEnabled()))
	{	
		if (timerobject-timerDespawn>5000)
		{
			printf("Done despawn, disabling the character now!\n");
			// will disable the character after 5 seconds
			setAnimation("prespawn");
			this->setEnabled(false);
			
		}
	}

	if (stunstate)
	{
		// 400 ms default delay for hurt
		if (timerobject-timerStun>400)
		{
			// Disable the stun state and restore the previous animation
			stunstate=false;
			//setAnimation(this->oldAnimName);
			setAnimation("idle");
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
			if (runningMode)
			{
				if (this->getAnimation()!=OBJECT_ANIMATION_RUN)
				{
					this->setAnimation("run");
					//printf("Hey the object specifically asked for a run state!\n");
				}
			}
			else
			{
				if (this->getAnimation()!=OBJECT_ANIMATION_WALK)
				{
					this->setAnimation("walk");
					//printf("Hey the object specifically asked for a walk state!\n");
				}
			}

			this->walkTo(walkTarget);
			return;
		}

		// Stop the walk when in range
		if (this->getAnimation()==OBJECT_ANIMATION_WALK && this->getPosition().getDistanceFrom(walkTarget)==0)
			//this->getPosition().getDistanceFrom(walkTarget) < (meshScale*meshSize))
		{
			//printf("Hey the object specifically asked  for a idle state!\n");
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
		{
			if (lua_pcall(L,0,0,0)!=0)
			{
				printf("error running function `onUpdate'\n");
				GUIManager::getInstance()->setConsoleText("LUA error running funtion <<onUpdate>>",SColor(255,255,0,0));
			}

		}
		lua_pop( L, -1 );

		lua_getglobal(L,"step");

		if(lua_isfunction(L, -1))
		{
			if (lua_pcall(L,0,0,0)!=0)
			{
				printf("error running function `step': \n");
				GUIManager::getInstance()->setConsoleText("LUA error running funtion <<step>>",SColor(255,255,0,0));
			}

		}
		lua_pop( L, -1 );

		//custom update function (updates walkTo for example..)
		lua_getglobal(L,"CustomDynamicObjectUpdate");

		if(lua_isfunction(L, -1))
		{
			if (lua_pcall(L,0,0,0)!=0)
			{
				printf("error running function `CustomDynamicObjectUpdate': \n");
				GUIManager::getInstance()->setConsoleText("LUA error running funtion <<CustomDynamicObjectUpdate>>",SColor(255,255,0,0));
			}
		}
		lua_pop( L, -1 );
	}
	lua_getglobal(L,"CustomDynamicObjectUpdateProgrammedAction");
	if(lua_isfunction(L, -1))
		lua_pcall(L,0,0,0);
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
		if (tempObj->runningMode)
		{
			if (tempObj->getAnimation()!=OBJECT_ANIMATION_RUN)
				tempObj->setAnimation("run");
		}
		else
		{
			if (tempObj->getAnimation()!=OBJECT_ANIMATION_WALK)
				tempObj->setAnimation("walk");
		}
		
		//printf ("Lua call the walk animation.\n");

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
		/*if (tempObj->runningMode)
		{
			if (tempObj->getAnimation()!=OBJECT_ANIMATION_RUN)
				tempObj->setAnimation("run");
		}
		else*/
		{
			if (tempObj->getAnimation()!=OBJECT_ANIMATION_WALK)
				tempObj->setAnimation("walk");
		}
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
		//printf("The LUA use attack with that target: %s\n",tempObj->getName().c_str());
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

	//printf("Set propertie %s to %f from object named: %s\n",propertieName,value,objName.c_str());

	DynamicObject* Obj = NULL;
	if (objName!="")
		Obj = DynamicObjectsManager::getInstance()->getObjectByName(objName.c_str());
	else
		Obj = DynamicObjectsManager::getInstance()->getPlayer();

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
	if (propertieName=="level")
		Obj->properties.level = (int)value;

	if (objName=="player")
		Player::getInstance()->updateDisplay();

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
	DynamicObject* Obj = DynamicObjectsManager::getInstance()->getObjectByName(objName.c_str());
	if (propertieName=="life")
		value = Obj->properties.life;
	if (propertieName=="money")
		value = Obj->properties.money;
	if (propertieName=="maxlife")
		value = Obj->properties.maxlife;
	if (propertieName=="mana")
		value = Obj->properties.mana;
	if (propertieName=="maxmana")
		value = Obj->properties.maxmana;
	if (propertieName=="mindamage")
		value = Obj->properties.mindamage;
	if (propertieName=="maxdamage")
		value = Obj->properties.maxdamage;
	if (propertieName=="hurt_resist")
		value = Obj->properties.hurt_resist;
	if (propertieName=="experience")
		value = Obj->properties.experience;
	if (propertieName=="level")
		value = Obj->properties.level;
	if (propertieName=="regenlife")
		value = Obj->properties.regenlife;
	if (propertieName=="regenmana")
		value = Obj->properties.regenmana;

	if (propertieName=="dodge_prob")
	{
		float value2 = Obj->properties.dodge_prop;
		lua_pushnumber(LS,value2);
		return 1;
	}
	if (propertieName=="hit_prob")
	{
		float value2 = Obj->properties.hit_prob;
		lua_pushnumber(LS,value2);
		return 1;
	}

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
			//printf("Asked the distance from the player: %f,%f,%f\n",otherPos.X,otherPos.Y,otherPos.Z);
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

int DynamicObject::showDialogMessage(lua_State *LS)
{
	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

	DynamicObjectsManager::getInstance()->setDialogCaller(tempObj);

	std::string param1 = lua_tostring(LS, -1);
    lua_pop(LS, 1);

    std::string param2 = "";

    if(lua_isstring(LS, -1))
    {
        param2 = lua_tostring(LS, -1);
        lua_pop(LS, 1);
    }

    if(param2!="")
		GUIManager::getInstance()->showDialogMessage((stringw)param2.c_str(), param1);
    else
		GUIManager::getInstance()->showDialogMessage((stringw)param1.c_str(), "");

    return 1;
}

int DynamicObject::showDialogQuestion(lua_State *LS)
{
	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

	DynamicObjectsManager::getInstance()->setDialogCaller(tempObj);

	std::string param1 = lua_tostring(LS, -1);
    lua_pop(LS, 1);

    std::string param2 = "";

    if(lua_isstring(LS, -1))
    {
        param2 = lua_tostring(LS, -1);
        lua_pop(LS, 1);
    }

    if(param2!="")
		GUIManager::getInstance()->showDialogQuestion((stringw)param2.c_str(), param1);
    else
		GUIManager::getInstance()->showDialogQuestion((stringw)param1.c_str(), "");

    return 1;
}

void DynamicObject::notifyClick()
{
    lua_getglobal(L,"onClicked");
    if(lua_isfunction(L, -1)) lua_pcall(L,0,0,0);
    lua_pop( L, -1 );
}

void DynamicObject::notifyAttackRange()
{
    lua_getglobal(L,"onAttackRange");
    if(lua_isfunction(L, -1)) lua_pcall(L,0,0,0);
    lua_pop( L, -1 );
}

void DynamicObject::notifyCollision()
{
    lua_getglobal(L,"onCollision");
    if(lua_isfunction(L, -1)) lua_pcall(L,0,0,0);
    lua_pop( L, -1 );
}

void DynamicObject::notifyAnswer(bool answer)
{
	LuaGlobalCaller::getInstance()->setAnswer(answer);
	lua_getglobal(L,"onAnswer");
    if(lua_isfunction(L, -1)) lua_pcall(L,0,0,0);
    lua_pop( L, -1 );
}


stringc DynamicObject::getObjectType()
{
    lua_getglobal(L,"objType");
    stringc objType = lua_tostring(L, -1);
	lua_pop(L, 1);

	return objType;
}