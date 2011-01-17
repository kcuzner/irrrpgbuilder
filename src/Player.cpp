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
	}
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