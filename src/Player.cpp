#include "Player.h"

#include "App.h"
#include "tinyXML/tinyxml.h"
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
	playerObject = DynamicObjectsManager::getInstance()->getPlayer();
	playerObject->getNode()->setVisible(true);
	playerObject->setEnabled(true);
	playerObject->getNode()->setDebugDataVisible( false ? EDS_BBOX | EDS_SKELETON : EDS_OFF );
	playerObject->setAnimation("idle");

	// Specific properties for the player (hardcoded for now)
	property playerprop = playerObject->getProperties();
	property player_base = playerObject->getProp_base();
	property player_level = playerObject->getProp_level();

	playerprop.experience = 0;
	playerprop.mindamage = 3;
	playerprop.maxdamage = 10;
	playerprop.level = 1;
	
	// Set the upgradable properties (properties that will increase automatically at each level)
	player_base.maxlife=95; // Starting at level 0 with 95 hp
	player_level.maxlife=5; // each level add 5 hp more (level 1=100hp, level 2=105hp, level 3=110hp)

	player_base.mindamage = 1; // Starting at level 0 with 1 point of damage (min)
	player_level.mindamage = 2; // each level add 2 more points to the min damage (level 1=3pts, level 2=4pts, level 3=7pts)
	
	player_base.maxdamage = 5;
	player_level.maxdamage = 5;

	player_base.experience = 50;
	player_level.experience = 20;
	
	


	playerObject->setProperties(playerprop);
	playerObject->setProp_base(player_base);
	playerObject->setProp_level(player_level);
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
	vector3df walkTarget = playerObject->getWalkTarget();
	if (timercheck-timer1>17)
	{
		// Update the combat system (mostly for damage over time management (dot))
		Combat::getInstance()->update();
		timer1 = timercheck;
		//printf("current state of animation is %i\n",playerObject->getAnimation());
		// Calculate the size of the mesh, and multiplicate it with the scale
		// Will give the real size on the map
		f32 sizePlayer = playerObject->getNode()->getBoundingBox().getExtent().X;
		f32 meshScale = playerObject->getScale().X;
		
		// Walk until in range
		if( (this->playerObject->getPosition().getDistanceFrom(walkTarget) > (meshScale*sizePlayer)) &&  (this->playerObject->getLife()!=0))
		{
			TerrainManager::getInstance()->getHeightAt(walkTarget);
			if (this->playerObject->getAnimation()!=OBJECT_ANIMATION_WALK)
			{
				this->playerObject->setAnimation("walk");
				printf("Hey the player specificalled for a walk state!\n");
			}

			this->playerObject->walkTo(walkTarget); 
			return;
		}

		// Stop the walk when in range
		if (playerObject->getAnimation()==OBJECT_ANIMATION_WALK && this->playerObject->getPosition().getDistanceFrom(walkTarget) < (meshScale*sizePlayer))
		{
			printf("Hey the player specificalled for a idle state!\n");
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
		//updateDisplay();
	}
}

void Player::updateDisplay()
{
	// Update the GUI display
	stringc playerLife = LANGManager::getInstance()->getText("txt_player_life");
	playerLife += playerObject->getProperties().life;
	playerLife += "/";
	playerLife += playerObject->getProperties().maxlife;
	playerLife += " Exp:";
	stringc playerxp = (stringc)playerObject->getProperties().experience;
	playerLife += playerxp;
	playerLife += " Level:";
	playerLife += playerObject->getProperties().level;
	//+(stringc)properties.experience;
	GUIManager::getInstance()->setStaticTextText(ST_ID_PLAYER_LIFE,playerLife);
	
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