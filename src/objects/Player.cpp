#include "Player.h"

#include "../App.h"
#include "../tinyXML/tinyxml.h"
#include "DynamicObjectsManager.h"
#include "../camera/CameraSystem.h"


using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

Player::Player()
{

	timer1=App::getInstance()->getDevice()->getTimer()->getRealTime();;
	timer2=timer1;
	timer3=timer1;
	playerObject = DynamicObjectsManager::getInstance()->getPlayer();
	playerObject->getNode()->setVisible(true);

	playerObject->setEnabled(true);
	playerObject->getNode()->setDebugDataVisible( false ? EDS_BBOX | EDS_SKELETON : EDS_OFF );
	//playerObject->setAnimation("idle");
	playerObject->setName("player");
	
	// Specific properties for the player (hardcoded for now)
	DynamicObject::cproperty playerprop = playerObject->initProperties();
	DynamicObject::cproperty player_base = playerObject->initProperties();
	DynamicObject::cproperty player_level = playerObject->initProperties();

	playerprop.experience = 0;
	playerprop.mindamage = 3;
	playerprop.maxdamage = 10;
	playerprop.level = 1;
	playerprop.mana=100;
	playerprop.maxmana=100;
	playerprop.regenlife=1;
	playerprop.regenmana=1;
	playerprop.maxlife = 100;
	playerprop.dodge_prop = 16;
	playerprop.hit_prob = 60;
	playerprop.money=100;
	playerprop.attackdelay=800;
	playerObject->setProperties(playerprop);
	

	// Set the upgradable properties (properties that will increase automatically at each level)
	player_base.maxlife=95; // Starting at level 0 with 95 hp
	player_level.maxlife=5; // each level add 5 hp more (level 1=100hp, level 2=105hp, level 3=110hp)

	player_base.mindamage = 3; // Starting at level 0 with 1 point of damage (min)
	player_level.mindamage = 2; // each level add 2 more points to the min damage (level 1=3pts, level 2=4pts, level 3=7pts)

	player_base.maxdamage = 10;
	player_level.maxdamage = 5;

	player_base.experience = 50;
	player_level.experience = 20;

	player_base.hit_prob = 60;
	player_level.hit_prob = 0.5f;

	player_base.dodge_prop = 16;
	player_level.dodge_prop = 0.5f;

	
	playerObject->setProp_base(player_base);
	playerObject->setProp_level(player_level);
	
	playerObject->getNode()->setID(0);

	taggedObject=NULL;
}

Player::~Player()
{
    //dtor
}

Player* Player::getInstance()
{
    static Player* instance = 0;
    if(!instance) instance = new Player();
    return instance;
}

DynamicObject* Player::getObject()
{
	return playerObject;
}

//! Main player update loop
// Define the distance needed to walk or run (when clicking on the map)
// Might have to place some of the code there into the dynamic object class (so all characters have this ability)
// The player have a "walktarget" that is defined by the coordinate given by the mouse pointer when clicked on the map

// Determine also the attack rate(animation) and the distance the player can attack an object that was found.
void Player::update()
{
	if (CameraSystem::getInstance()->getViewType()==CameraSystem::VIEW_RTS || CameraSystem::getInstance()->getViewType()==CameraSystem::VIEW_RTS_FIXED)
	{
		updateRTSTargetting();
	}
	else
	{
		updateTargetting();
	}

	//u32 timercheck = App::getInstance()->getDevice()->getTimer()->getRealTime();
	u32 timercheck = App::getInstance()->getTimer();
	
	// Standard checks updated by timer (update the tagged object, the range etc.
	if (timercheck-timer3>17)
	{

		timer3=timercheck;
		//printf("current state of animation is %i\n",playerObject->getAnimation());
	
		vector3df walkTarget = playerObject->getWalkTarget();

		// With this the target reticle will follow the target that has been selected (app.cpp)
		if (taggedObject)
		{
			DynamicObjectsManager::getInstance()->getTarget()->setPosition(taggedObject->getPosition()+vector3df(0,0.1f,0));
		}


		// New code to test (should only define run and walk mode)
		// Would be more useful if determining the distance based on the zoom distance.
		if (this->playerObject->getPosition().getDistanceFrom(walkTarget) < 80) 
			this->playerObject->setRunningMode(false);
		
		if (this->playerObject->getPosition().getDistanceFrom(walkTarget) > 120)
			this->playerObject->setRunningMode(true);

		
		// This should trigger the player attack if the enemy is in range.
		if (timercheck-timer1>300 && controltype==CONTROL_POINTNCLICK) // 1 attack per 1/4 sec			
		{
			timer1 = timercheck;
			
			if (playerObject->getCurrentEnemy() && playerObject->getCurrentEnemy()->getDistanceFrom(getObject()->getPosition())<(playerObject->getObjectSize()*1.10f))
			{
				//printf("The is an enemy here named: %s\n",playerObject->getCurrentEnemy()->getName());
				if (playerObject->getAnimation()!=playerObject->OBJECT_ANIMATION_ATTACK)
				{
					playerObject->lookAt(playerObject->getCurrentEnemy()->getPosition());
					if (playerObject->getLife()!=0)
					{
						if (playerObject->getCurrentEnemy()->getLife()!=0) //Why hit a corpse?! :)
							playerObject->setAnimation("attack");
					}

					else
						playerObject->setAnimation("die");
				}
			}
		}
		playerObject->getNode()->updateAbsolutePosition();
		//updateDisplay();
	}
}

// Update the player stats (from the GUI Manager)
void Player::updateDisplay()
{
	GUIManager::getInstance()->drawPlayerStats();
}

// Display the shadow object of another color for highlighting the player. Used mostly for moving the player in editor mode
void Player::setHighLight(bool highlight)
{
    if(!highlight)
    {
		this->playerObject->getShadow()->setMaterialTexture(0,App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/dynamic_objects/treeFakeShadow.png"));
        this->playerObject->getShadow()->setMaterialFlag(EMF_LIGHTING,true);
    }
    else
    {
        this->playerObject->getShadow()->setMaterialTexture(0,App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/player/highlight.png"));
        this->playerObject->getShadow()->setMaterialFlag(EMF_LIGHTING,false);
    }
}

// Define a "tagged" object, selected by the player in play mode when the cursor found a dynamic object
void Player::setTaggedTarget(DynamicObject* object)
{
	taggedObject = object;
}

// Will return the last object that was "tagged"
DynamicObject* Player::getTaggedTarget()
{
	return taggedObject;
}

ISceneNode* Player::getTarget()
{
	return DynamicObjectsManager::getInstance()->getTarget()->getNode();
}

//! Show/Hide the target object
void Player::displayTarget(bool visible)
{
	DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(visible);
}

void Player::updateRTSTargetting()
{
	if(App::getInstance()->isMousePressed(0) && 
		App::getInstance()->getAppState() == App::APP_GAMEPLAY_NORMAL)
		
	{
				
		// Try a new trick to pick up only the NPC and the ground (AS object can walk on other objects)
		//DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_NON_INTERACTIVE,0x0010);
		DynamicObjectsManager::getInstance()->setObjectsID(DynamicObject::OBJECT_TYPE_NPC,100);
		DynamicObjectsManager::getInstance()->setObjectsID(DynamicObject::OBJECT_TYPE_INTERACTIVE,100);
		DynamicObjectsManager::getInstance()->setObjectsID(DynamicObject::OBJECT_TYPE_WALKABLE,0x0010);
		// Filter only object with the ID=100 to get the resulting node
		App::MousePick mousePick = App::getInstance()->getMousePosition3D(100);
				

		// Set back to the defaults
		//DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_NON_INTERACTIVE,100);
		DynamicObjectsManager::getInstance()->setObjectsID(DynamicObject::OBJECT_TYPE_NPC,0x0010);
		DynamicObjectsManager::getInstance()->setObjectsID(DynamicObject::OBJECT_TYPE_INTERACTIVE,0x0010);
		DynamicObjectsManager::getInstance()->setObjectsID(DynamicObject::OBJECT_TYPE_WALKABLE,100);
		// Failed to pick something, try to select "walkable"
		if (!mousePick.pickedNode)
		{
			mousePick = App::getInstance()->getMousePosition3D(100);
			//mousePick.pickedNode = NULL; //Forget about the node since we only need the collision point
		}

		stringc nodeName = "";
		// Check for a node(need to get the name of the node)
		if (mousePick.pickedNode != NULL)
		{
			// Get the name of the object that has been clicked on
			stringc nodeName = mousePick.pickedNode->getName();
			//if you click on a Dynamic Object...
			if( stringc( nodeName.subString(0,14)) == "dynamic_object" )
			{
				DynamicObject* obj = DynamicObjectsManager::getInstance()->getObjectByName(nodeName);
				// TODO: Need to get more accuracy for the distance hardcoded value is not ideal
				//Since an object as been clicked the walktarget of the player is changed
				if (obj)
				{
					vector3df pos = obj->getPosition();
					vector3df pos2 = getObject()->getPosition();
					f32 desiredDistance=50.0f;
					f32 distance = getObject()->getDistanceFrom(pos);
					f32 final = (distance-desiredDistance)/distance;
					vector3df walkTarget = pos.getInterpolated(pos2,final);
					getObject()->setWalkTarget(walkTarget);
					DynamicObjectsManager::getInstance()->getTarget()->setPosition(obj->getPosition()+vector3df(0,0.1f,0));
					DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(true);
					getObject()->lookAt(obj->getPosition());
					setTaggedTarget(obj);
				}
				// If the distance is ok notify the object lua script that it's being clicked
				// Currently set at 100 unit
				if (obj && (obj->getDistanceFrom(getObject()->getPosition()) < 100.0f))
					obj->notifyClick();
				
				if(obj->getObjectType() == stringc("ENEMY"))
				{
					getObject()->attackEnemy(obj);
				}
				else
				{
					getObject()->clearEnemy();
				}
				return;
			}
			else
			{ // Did not clicked on a NPC or object but on a walkable area
				//mousePick = getMousePosition3D(100);
				if (mousePick.pickedPos!=vector3df(0,0,0))
				{
					getObject()->setWalkTarget(mousePick.pickedPos);
					DynamicObjectsManager::getInstance()->getTarget()->setPosition(mousePick.pickedPos+vector3df(0,0.1f,0));
					DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(true);
					setTaggedTarget(NULL);
					getObject()->clearEnemy();
				}
				//getObject()->clearEnemy();
				return;
			}
		}
	}
}

// Update the RPG and FPS targeting. Check for NPC, Interactive and loot objects
void Player::updateTargetting()
{	
	//Front of the player 30 units in front
		vector3df tas = vector3df (0,0,-40.0f);
		tas.rotateXZBy(-getNode()->getRotation().Y);
		vector3df pos = getNode()->getPosition()+ tas;

	bool found=false;

	//Activated for debugging purpose
	//getTarget()->setVisible(true);
	//getTarget()->setPosition(pos);
	vector<DynamicObject*> list = DynamicObjectsManager::getInstance()->getObjectNearPosition(pos, 40.0f, DynamicObject::OBJECT_TYPE_INTERACTIVE);
	if (list.size()>0)
	{		
			DynamicObject* object = getNearest(pos,list);
			if (object)
			{
				//printf("Object was found!:%s\n",object->displayName.c_str());
				setTaggedTarget(object);
				getTarget()->setVisible(true);
				getTarget()->setPosition(object->getPosition());
				found=true;
				printf("Interactive object found %s\n",object->getName().c_str());
			}
		
	}
	list = DynamicObjectsManager::getInstance()->getObjectNearPosition(pos, 40.0f, DynamicObject::OBJECT_TYPE_LOOT); //DynamicObject::OBJECT_TYPE_NPC
	if (list.size()>0)
	{
		DynamicObject* object = getNearest(pos,list);
		if (object)
		{
			//printf("Object was found!:%s\n",object->displayName.c_str());
			setTaggedTarget(object);
			getTarget()->setVisible(true);
			getTarget()->setPosition(object->getPosition());
			found=true;	
			printf("Loot object found %s\n",object->getName().c_str());
		}
	} 
	
	list = DynamicObjectsManager::getInstance()->getObjectNearPosition(pos, 40.0f, DynamicObject::OBJECT_TYPE_NPC);
	if (list.size()>0)
	{
		DynamicObject* object = getNearest(pos,list);
		if (object)
		{
			//printf("Object was found!:%s\n",object->displayName.c_str());
			setTaggedTarget(object);
			getTarget()->setVisible(true);
			getTarget()->setPosition(object->getPosition());
			printf("NPC object found %s\n",object->getName().c_str());
			found=true;
		}
	}
	
	if (!found)
	{
		setTaggedTarget(NULL);
		getTarget()->setVisible(false);
	}

}

DynamicObject* Player::getNearest(vector3df pos, vector<DynamicObject*> list)
{

	DynamicObject* nearobject = NULL;
	f32 near1 = 256.0f;

	if (list.size()>0)
	{
		for(int i=0;i<(int)list.size();i++)
		{
			DynamicObject* d = list[i];
			if (d)
			{
				f32 distance = pos.getDistanceFrom(d->getPosition());
				if (distance<near1 && distance>10.0f)
				{
					nearobject = d;
				}
			}
		}
		return nearobject;
	}
	else
		return NULL;
}