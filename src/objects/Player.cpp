#include "Player.h"

#include "../App.h"
#include "../tinyXML/tinyxml.h"
#include "DynamicObjectsManager.h"


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
	
	player_ref=playerObject->getNode()->clone();
	player_ref->setVisible(false);

	// Specific properties for the player (hardcoded for now)
	cproperty playerprop = playerObject->initProperties();
	cproperty player_base = playerObject->initProperties();
	cproperty player_level = playerObject->initProperties();

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


void Player::update()
{
	u32 timercheck = App::getInstance()->getDevice()->getTimer()->getRealTime();

	
	// Standard checks updated by timer (update the tagged object, the range etc.
	if (timercheck-timer3>17)
	{
		timer3=timercheck;
		//printf("current state of animation is %i\n",playerObject->getAnimation());
		// Calculate the size of the mesh, and multiplicate it with the scale
		// Will give the real size on the map
		f32 sizePlayer = playerObject->getNode()->getBoundingBox().getExtent().X;
		f32 meshScale = playerObject->getScale().X;

		vector3df walkTarget = playerObject->getWalkTarget();

		// With this the target reticle will follow the target that has been selected (app.cpp)
		if (taggedObject)
		{
			DynamicObjectsManager::getInstance()->getTarget()->setPosition(taggedObject->getPosition()+vector3df(0,0.1f,0));
		}


		// New code to test (should only define run and walk mode)
		if (this->playerObject->getPosition().getDistanceFrom(walkTarget) < 121)
			this->playerObject->setRunningMode(false);
		else
			this->playerObject->setRunningMode(true);

		/*
		// Walk until in range
		if( (this->playerObject->getPosition().getDistanceFrom(walkTarget) > (meshScale*sizePlayer)) &&  (this->playerObject->getLife()!=0))
		{
			SoundManager::getInstance()->setListenerPosition(this->getObject()->getPosition(),this->getObject()->getRotation());
			TerrainManager::getInstance()->getHeightAt(walkTarget);
			if (this->playerObject->getPosition().getDistanceFrom(walkTarget) < 121)
			{
				if (this->playerObject->getAnimation()!=OBJECT_ANIMATION_WALK)
				{
					this->playerObject->setRunningMode(false);
					this->playerObject->setAnimation("walk");
					//printf("Hey the player specifically asked for a walk state!\n");
				}
			}
			if (this->playerObject->getPosition().getDistanceFrom(walkTarget) > 120)
			{
				if (this->playerObject->getAnimation()!=OBJECT_ANIMATION_RUN)
				{
					this->playerObject->setRunningMode(true);
					bool result=this->playerObject->setAnimation("run");
					// in case the run animation is not present
					if (!result)
					{
						this->playerObject->setRunningMode(false);
						this->playerObject->setAnimation("walk");
					}

					//printf("Hey the player specifically asked for a run state!\n");
				}
			}
			return;
		}

		// Stop the walk when in range
		if (playerObject->getAnimation()==OBJECT_ANIMATION_WALK && this->playerObject->getPosition().getDistanceFrom(walkTarget) < (meshScale*sizePlayer)+20)
		{
			//printf("Hey the player specifically asked for a idle state!\n");
			this->playerObject->setWalkTarget(playerObject->getPosition());
			this->playerObject->setAnimation("idle");
			DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(false);
			return;
		}

		// Cancel the move if another animation is triggered
		if (playerObject->getAnimation()!=OBJECT_ANIMATION_WALK || playerObject->getAnimation()==OBJECT_ANIMATION_IDLE)
		{
			this->playerObject->setWalkTarget(playerObject->getPosition());
		}

		*/
		// This should trigger the player attack if the enemy is in range.
		if (timercheck-timer1>300) // 1 attack per 1/4 sec
		{
			timer1 = timercheck;
			if (playerObject->getCurrentEnemy() && playerObject->getCurrentEnemy()->getDistanceFrom(getObject()->getPosition())<72.0f)
			{
				//printf("The is an enemy here named: %s\n",playerObject->getCurrentEnemy()->getName());
				if (playerObject->getAnimation()!=OBJECT_ANIMATION_ATTACK)
				{
					playerObject->lookAt(playerObject->getCurrentEnemy()->getPosition());
					if (playerObject->getLife()!=0)
						playerObject->setAnimation("attack");
					else
						playerObject->setAnimation("die");
				}
			}
		}
		//updateDisplay();
	}
}

void Player::updateDisplay()
{
	GUIManager::getInstance()->drawPlayerStats();
}

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

void Player::setTaggedTarget(DynamicObject* object)
{
	taggedObject = object;
}

DynamicObject* Player::getTaggedTarget()
{
	return taggedObject;
}


void Player::displayTarget(bool visible)
{
	DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(visible);
}