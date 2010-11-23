#include "Player.h"

#include "App.h"
#include "tinyXML/tinyxml.h"
#include "TerrainManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

Player::Player()
{
    ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();

    TiXmlDocument doc("../media/player/player.xml");
    if (!doc.LoadFile())
    {
        #ifdef APP_DEBUG
        cout << "DEBUG : XML : ERROR LOADING PLAYER XML!" << endl;
        #endif
    }

	TiXmlElement* root = doc.FirstChildElement( "IrrRPG_Builder_Player" );

    if ( root )
    {
        if( atof(root->Attribute("version"))!=APP_VERSION )
        {
            #ifdef APP_DEBUG
            cout << "DEBUG : XML : INCORRECT PLAYER VERSION!" << endl;
            #endif
        }

        TiXmlNode* playerModelXML = root->FirstChild( "player_model" );

        node = smgr->addEmptySceneNode();

        while( playerModelXML != NULL )
        {

			stringc pathFile = "../media/player/";
            playerModel newModel;

			// Init the file check for matched animation in the XML
			newModel.idle = false;
			newModel.walk = false;
			newModel.attack = false;
			newModel.die = false;
			newModel.injured = false;
			newModel.run = false;

            newModel.name = playerModelXML->ToElement()->Attribute("name");

            
			// Animation meshes -- main mesh
            stringc meshFile = playerModelXML->ToElement()->Attribute("mesh");
			// Set the meshes for the animations.
			newModel.mesh = smgr->getMesh(pathFile+meshFile);

			stringc scriptname = playerModelXML->ToElement()->Attribute("script");
			// Loading a predefined script from the XML
			if (scriptname.size()>1)
			{
				stringc newScript = "";
				stringc filename = "../media/scripts/";
				filename += scriptname;
				std::string line;
				ifstream fileScript (filename.c_str());
				if (fileScript.is_open())
				{
					while (! fileScript.eof() )
					{
						getline (fileScript,line);
						newScript += line.c_str();
						newScript += '\n';
					}
					fileScript.close();
				}
				script = newScript;
			} else 
				script = "";

			stringc s_scale = playerModelXML->ToElement()->Attribute("scale");
			if (s_scale.size()>0) newModel.scale = (f32)atof(s_scale.c_str()); else newModel.scale = 1;

			stringc s_wspeed = playerModelXML->ToElement()->Attribute("walkspeed");
			if (s_wspeed.size()>0) walkSpeed = (f32)atof(s_wspeed.c_str()); else walkSpeed = (f32)0.02;
			// Animation meshes -- Skinned options
			//-----------------------------------------------------------------------------
			TiXmlNode* currentAnimXML = playerModelXML->FirstChild( "animation" );

            vector<PlayerObject_Animation> animations;
			
            //Iterate animations
            while( currentAnimXML != NULL )
            {
                PlayerObject_Animation currAnim;
				// Get the infos from the animation	
                currAnim.name = currentAnimXML->ToElement()->Attribute("name");
				currAnim.mesh = currentAnimXML->ToElement()->Attribute("mesh");

				// TODO: Not totally implemented
				currAnim.sound = currentAnimXML->ToElement()->Attribute("sound");

				stringc s_start = currentAnimXML->ToElement()->Attribute("start");
				if (s_start.size()>0) 
					currAnim.startFrame = atoi(s_start.c_str()); 
				else 
					currAnim.startFrame=0;
				            
				stringc s_end = currentAnimXML->ToElement()->Attribute("end");
				if (s_end.size()>0) 
					currAnim.endFrame = atoi(s_end.c_str()); 
				else 
					currAnim.endFrame=0;
                // TODO: Partially implemented
                stringc s_attack = currentAnimXML->ToElement()->Attribute("attackevent");
				if (s_attack.size()>0) 
					currAnim.attackevent = atoi(s_attack.c_str()); 
				else 
					currAnim.attackevent=currAnim.startFrame;
				
				// TODO: Not totally implemented
				stringc s_sound = currentAnimXML->ToElement()->Attribute("soundevent");
				if (s_sound.size()>0) 
					currAnim.soundevent = atoi(s_sound.c_str());

				// TODO: Not totally implemented
				stringc s_speed = currentAnimXML->ToElement()->Attribute("speed");
				if (s_speed.size()>0) 
					currAnim.speed = (f32)atof(s_speed.c_str());

				// Associate the animation infos
				if (currAnim.name=="idle")
				{	
					newModel.idle=true;
					if (currAnim.mesh!="") 
						newModel.idlemesh = smgr->getMesh(pathFile+currAnim.mesh);
					else
						newModel.idlemesh = NULL;

					newModel.idle_start = currAnim.startFrame;
					newModel.idle_end = currAnim.endFrame;
				}
				else if (currAnim.name=="walk")
				{
					newModel.walk=true;
					if (currAnim.mesh!="") 
						newModel.walkmesh = smgr->getMesh(pathFile+currAnim.mesh);
					else
						newModel.walkmesh = NULL;

					newModel.walk_start = currAnim.startFrame;
					newModel.walk_end = currAnim.endFrame;
				}
				else if (currAnim.name=="attack")
				{
					newModel.attack=true;
					if (currAnim.mesh!="") 
						newModel.attackmesh = smgr->getMesh(pathFile+currAnim.mesh);
					else
						newModel.attackmesh = NULL;

					if (currAnim.attackevent>1) 
						newModel.attackevent = currAnim.attackevent;
					newModel.attack_start = currAnim.startFrame;
					newModel.attack_end = currAnim.endFrame;
				}
				else if (currAnim.name=="die")
				{
					newModel.die=true;
					if (currAnim.mesh!="") 
						newModel.diemesh = smgr->getMesh(pathFile+currAnim.mesh);
					else
						newModel.diemesh = NULL;

					newModel.die_start = currAnim.startFrame;
					newModel.die_end = currAnim.endFrame;
				}
				else if (currAnim.name=="injured")
				{
					newModel.injured = true;
					if (currAnim.mesh!="")
						newModel.injuredmesh = smgr->getMesh(pathFile+currAnim.mesh);
					else
						newModel.injuredmesh = NULL;

					newModel.injured_start = currAnim.startFrame;
					newModel.injured_end = currAnim.endFrame;
				}
				else if (currAnim.name=="run")
				{
					newModel.run=true;
					if (currAnim.mesh!="") 
						newModel.runmesh = smgr->getMesh(pathFile+currAnim.mesh);
					else 
						newModel.runmesh = NULL;

					newModel.run_start = currAnim.startFrame;
					newModel.run_end = currAnim.endFrame;
				}

				// Iterate and store the animations infos
                currentAnimXML = playerModelXML->IterateChildren( "animation", currentAnimXML );
                animations.push_back(currAnim);
            }
			//---------------------------------------------------------------------------------
			        
			// Set the reference/main mesh
            newModel.node = smgr->addAnimatedMeshSceneNode(newModel.mesh,node);
            newModel.node->setScale(vector3df(newModel.scale,newModel.scale,newModel.scale));
            newModel.node->setMaterialFlag(EMF_FOG_ENABLE,true);
            newModel.node->setFrameLoop(0,0);
            //newModel.node->setVisible(false);
            /// TODO: uncomment and do a setVisible setPlayerModel (stringc name)
            /// View the names in the player screen edition

			playerModels.push_back(newModel);

            currentModel = newModel;

            playerModelXML = root->IterateChildren( "player_model", playerModelXML );
        }
    }

    //Fake Shadow
    fakeShadow = smgr->addMeshSceneNode(smgr->getMesh("../media/dynamic_objects/shadow.obj"),node);
    fakeShadow->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
    fakeShadow->setMaterialFlag(EMF_FOG_ENABLE,true);
    fakeShadow->setScale(vector3df(32,72,32));
    fakeShadow->setPosition(vector3df(0,(f32)0.03 ,0));

    this->setAnimation(PLAYER_ANIMATION_IDLE);

    enemyUnderAttack = NULL;

    // initialize the properties the player
	prop_base.mindamage=1;
	prop_level.mindamage=1;
	prop_base.maxdamage=3;
	prop_level.maxdamage=2;
	prop_base.experience=0;
	prop_level.experience=10;
	prop_base.maxlife=95;
	prop_level.maxlife=5;
	
	// Theses properties are fixed.
	properties.level=1;
	properties.money=0;
	properties.experience = 0;

	properties.maxlife = prop_base.maxlife+(prop_level.maxlife * properties.level);
	properties.life=properties.maxlife;
	
    
	timer = App::getInstance()->getDevice()->getTimer();
	currentime = timer->getRealTime();
	oldtime = timer->getRealTime();
	timer1 = oldtime;
	timer2 = oldtime;
	collided=false;
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

void Player::setPosition(vector3df pos)
{
    node->setPosition(pos);
}

vector3df Player::getPosition()
{
	return fakeShadow->getAbsolutePosition();
		//node->getPosition();
}

void Player::setRotation(vector3df rot)
{
    for(int i=0;i<(int)playerModels.size();i++)
    {
        ((playerModel)playerModels[i]).node->setRotation(rot);
    }
}

vector3df Player::getRotation()
{
    return currentModel.node->getRotation();
}

stringc Player::getScript()
{
    return script;
}

void Player::setScript(stringc script)
{
    this->script = script;
}

void Player::walkTo(vector3df targetPos, f32 speed)
{
	// Will have the player walk to the targetposition at the current speed.
	// Walk can be interrupted by:
	// - A collision with another object
	// - Moving into a part of the terrain that is not reachable (based on height of terrain)

	targetPos = vector3df(round32(targetPos.X),round32(targetPos.Y),round32(targetPos.Z));
    lookAt(targetPos);

    vector3df pos=this->getPosition();
    pos.Z -= cos((this->getRotation().Y)*PI/180)*speed;
    pos.X -= sin((this->getRotation().Y)*PI/180)*speed;
    //pos.Y = 0;///TODO: fixar no Y da terrain (gravidade)
	f32 height = TerrainManager::getInstance()->getHeightAt(pos);
	
	if (height>-0.09f && height<0.05f && !collided)
	{
		pos.Y = height;
		this->setPosition(pos);
	}
	else
	{
		walkTarget = this->getPosition();
		setAnimation(PLAYER_ANIMATION_IDLE);
		collided=false; // reset the collision flag
	}
}

void Player::lookAt(vector3df pos)
{
    vector3df offsetVector = pos - this->getPosition();

    vector3df rot = (-offsetVector).getHorizontalAngle();

    rot.X=0;
    rot.Z=0;

    this->setRotation(rot);
}

void Player::setWalkTarget(vector3df newTarget)
{
    walkTarget = newTarget;
}

void Player::setWalkSpeed(f32 newSpeed)
{
    walkSpeed = newSpeed;
}

void Player::setAnimation(PLAYER_ANIMATION anim)
{
	if((anim != PLAYER_ANIMATION_DIE) && (currentAnimation == PLAYER_ANIMATION_INJURED) && (currentModel.node->getFrameNr()<currentModel.injured_end)) return;
	if(anim == currentAnimation) return;
	ISkinnedMesh* skin = (ISkinnedMesh*)currentModel.mesh;
    switch(anim)
    {
        case PLAYER_ANIMATION_IDLE:
			// Use another animation mesh if it's defined
			if (currentModel.idle)
			{
				#ifdef DEBUG
				printf("Animation states: switched to idle\n");
				#endif
				if (currentModel.idlemesh!=NULL)  
				{
					ISkinnedMesh* skinidle = (ISkinnedMesh*)currentModel.idlemesh;
					skin->useAnimationFrom(skinidle);
				}
				else
				{
					skin->useAnimationFrom(skin);
				}
				currentModel.node->setFrameLoop(currentModel.idle_start,currentModel.idle_end);
				currentModel.node->setLoopMode(true);
			}
			currentAnimation = PLAYER_ANIMATION_IDLE;
            break;
        case PLAYER_ANIMATION_WALK:
			if (currentModel.walk)
			{
				#ifdef DEBUG
				printf("Animation states: switched to walk\n");
				#endif
				if (currentModel.walkmesh!=NULL)  
				{
					ISkinnedMesh* skinwalk = (ISkinnedMesh*)currentModel.walkmesh;
					skin->useAnimationFrom(skinwalk);
				}
				else
				{
					skin->useAnimationFrom(skin);
				}
				currentModel.node->setFrameLoop(currentModel.walk_start,currentModel.walk_end);
				currentModel.node->setLoopMode(true);
			}
			currentAnimation = PLAYER_ANIMATION_WALK;
            break;
        case PLAYER_ANIMATION_ATTACK:
			if (currentModel.attack)
			{
				#ifdef DEBUG
				printf("Animation states: switched to attack\n");
				#endif
				if (currentModel.attackmesh!=NULL)  
				{
					ISkinnedMesh* skinattack = (ISkinnedMesh*)currentModel.attackmesh;
					skin->useAnimationFrom(skinattack);
				}
				else
				{
					skin->useAnimationFrom(skin);	
				}
				currentModel.node->setFrameLoop(currentModel.attack_start,currentModel.attack_end);
				currentModel.node->setLoopMode(true);
			}
            currentAnimation = PLAYER_ANIMATION_ATTACK;
            break;
        case PLAYER_ANIMATION_DIE:
			if (currentModel.die)
			{	
				#ifdef DEBUG
				printf("Animation states: switched to die\n");
				#endif
				if (currentModel.diemesh!=NULL)  
				{	
					ISkinnedMesh* skindie = (ISkinnedMesh*)currentModel.diemesh;
					skin->useAnimationFrom(skindie);
				}
				else
				{
					skin->useAnimationFrom(skin);
				}
				currentModel.node->setFrameLoop(currentModel.die_start,currentModel.die_end);
			}
			currentAnimation = PLAYER_ANIMATION_DIE;
            break;
		case PLAYER_ANIMATION_INJURED:
			if (currentModel.injured)
			{
				#ifdef DEBUG
				printf("Animation states: switched to injured\n");
				#endif
				if (currentModel.injuredmesh!=NULL)  
				{
					ISkinnedMesh* skininjured = (ISkinnedMesh*)currentModel.injuredmesh;
					skin->useAnimationFrom(skininjured);
				}
				else
				{
					skin->useAnimationFrom(skin);
				}
				currentModel.node->setFrameLoop(currentModel.injured_start,currentModel.injured_end);
			}
            currentAnimation = PLAYER_ANIMATION_INJURED;
            break;
    }
}

PLAYER_ANIMATION Player::getAnimation()
{
    return currentAnimation;
}

bool Player::CheckAnimationEvent()
// Will be used to monitor animations and trigger events
// Planned events to have to monitor: ATTACK events, SOUND events.
{
	// Temporary, until the combat system is created
	// The frame "event" can't be determined, so we have to monitor the frames.
	// Current amimation checked for event is the attack 
	if((currentAnimation == PLAYER_ANIMATION_ATTACK) && (currentModel.node->getFrameNr() > currentModel.attackevent)&& (currentModel.node->getFrameNr() < currentModel.attackevent+1))
	{
		currentime = timer->getRealTime();
		u32 delay = currentime - oldtime;
		// Check that the event is not repeating for 200ms, because there could be some imprecision in determining the
		// proper frame of the event. 
		if (delay>200)
		{
			int en_life = enemyUnderAttack->getLife();
			
			// Basic rule that evaluate only the damage (and evaluation per level)
			properties.maxdamage = prop_base.maxdamage+(prop_level.maxdamage*properties.level);
			properties.mindamage = prop_base.mindamage+(prop_level.mindamage*properties.level);
			en_life -= rand() % (properties.maxdamage-properties.mindamage) + properties.mindamage; // temporary.

			if (en_life<0) 
			{
				en_life=0;
				this->properties.experience += 10;
				// This increase the level based on the experience increase. Will increase then the damage + others stuff.
				if ((this->prop_base.experience+(this->prop_level.experience*this->properties.level))<this->properties.experience)
					{
						this->properties.level+=1; // increase level
						this->prop_level.experience += (this->prop_level.experience); // increase required experience for next level
						this->properties.maxlife = this->prop_base.maxlife + (this->prop_level.maxlife * this->properties.level); // increase the hp
						this->properties.life = this->properties.maxlife; // give back the health at level change
					}
				// Give 10 points of experience per kill
				// TODO: This need to be read from the NPC experience info. Not implemented
			}
			// Set the new life value and call the update so the display is sure to be refreshed when attack.
			// I want also to have a "on_attack" event sent to the LUA
			enemyUnderAttack->setLife(en_life);
			enemyUnderAttack->update();
			oldtime = timer->getRealTime();
			return true;
		}
		
	}
	return false;
}

void Player::update()
{
	u32 timercheck = App::getInstance()->getDevice()->getTimer()->getRealTime();
	// Animations event check, done as fast as possible, the faster the more precise
	CheckAnimationEvent();
	if (anim->collisionOccurred())
	{
		printf ("Collision occured with %s\n",anim->getCollisionNode()->getName());
		collided=true;
	}
	// Standard player events
	if (currentAnimation!=PLAYER_ANIMATION_DIE && (timercheck-timer1>17))
	{
		
		timer1 = timercheck;
		// If a target position is set then move to that position, call the walk animation
		f32 sizePlayer = node->getBoundingBox().getExtent().X;
		if( (this->getPosition().getDistanceFrom(walkTarget) > sizePlayer) &&  (this->getLife()!=0))
		{
			TerrainManager::getInstance()->getHeightAt(walkTarget);

			this->setAnimation(PLAYER_ANIMATION_WALK);
		
			this->walkTo(walkTarget, walkSpeed);
		}
		// If there is a enemy selected and the attack is occuring do the animations (align to enemy, stop the attack when it's dead)
		else if( enemyUnderAttack && this->getAnimation() == PLAYER_ANIMATION_ATTACK)
		{
			this->lookAt(enemyUnderAttack->getPosition());
			walkTarget = this->getPosition();
			if(( this->getPosition().getDistanceFrom(enemyUnderAttack->getPosition()) > 72.0f ) || (enemyUnderAttack->getLife()==0))
			{
				this->setAnimation(PLAYER_ANIMATION_IDLE);
				enemyUnderAttack = NULL;
			}
		}
		// The player is being injured in combat
		else if((this->getLife()!=this->oldlife) && (this->getLife()!=0))
			{
				
				this->setAnimation(PLAYER_ANIMATION_INJURED);
				currentModel.node->setLoopMode(false);
			}
		else
		{
			this->setAnimation(PLAYER_ANIMATION_IDLE);
			walkTarget = this->getPosition();
		}

	}
	//call player step
	lua_getglobal(L,"step");
	if(lua_isfunction(L, -1)) lua_call(L,0,0);
	lua_pop( L, -1 );
	this->oldlife = this->properties.life;
	
}

void Player::attackEnemy(DynamicObject* obj)
{
    enemyUnderAttack = obj;

    if(obj)
    {
        this->lookAt(obj->getPosition());
        this->setAnimation(PLAYER_ANIMATION_ATTACK);
    }
}

void Player::doScript()
{
    // create an Lua pointer instance
    L = lua_open();

    // load the libs
    luaL_openlibs(L);

    //register basic functions
    LuaGlobalCaller::getInstance()->registerBasicFunctions(L);

    lua_register(L,"setPosition",setPosition);
    lua_register(L,"getPosition",getPosition);

    //associate the "objName" keyword to the dynamic object name
    stringc scriptTemp = "objName = 'player'; setObjectName('player')";

    luaL_dostring(L,scriptTemp.c_str());

    luaL_dostring(L,script.c_str());

    items.clear();

    lua_getglobal(L,"onLoad");
    if(lua_isfunction(L, -1)) lua_call(L,0,0);
    lua_pop( L, -1 );

    lua_getglobal(L,"IRBStorePlayerParams");
    if(lua_isfunction(L, -1)) lua_call(L,0,0);
    lua_pop( L, -1 );

    walkTarget = this->getPosition();
}

void Player::clearScripts()
{
    lua_getglobal(L,"IRBRestorePlayerParams");
    if(lua_isfunction(L, -1)) lua_call(L,0,0);
    lua_pop( L, -1 );

    lua_close(L);

    this->setAnimation(PLAYER_ANIMATION_IDLE);
}

void Player::setLife(int life)
{
    this->properties.life = life;
	
	
	// Update the GUI display
	stringc playerLife = LANGManager::getInstance()->getText("txt_player_life");
	playerLife += life;
	playerLife += "/";
	playerLife += this->properties.maxlife;
	playerLife += " Exp:";
	stringc playerxp = (stringc)this->properties.experience;
	playerLife += playerxp;
	playerLife += " Level:";
	playerLife += this->properties.level;
	//+(stringc)properties.experience;
	GUIManager::getInstance()->setStaticTextText(ST_ID_PLAYER_LIFE,playerLife);

	// Trigger the death animation immediately.
	if (life==0)
	{
		this->setAnimation(PLAYER_ANIMATION_DIE);
		currentModel.node->setLoopMode(false);
	}
}

int Player::getLife()
{
    return this->properties.life;
}

void Player::setMoney(int money)
{
    this->properties.money = money;
}

int Player::getMoney()
{
    return this->properties.money;
}

void Player::setHighLight(bool highlight)
{
    if(!highlight)
    {
        fakeShadow->setMaterialTexture(0,App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/dynamic_objects/treeFakeShadow.png"));
        fakeShadow->setMaterialFlag(EMF_LIGHTING,true);
    }
    else
    {
        fakeShadow->setMaterialTexture(0,App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/player/highlight.png"));
        fakeShadow->setMaterialFlag(EMF_LIGHTING,false);
    }
}

void Player::saveToXML(TiXmlElement* parentElement)
{
    TiXmlElement* playerXML = new TiXmlElement("player");
    playerXML->SetAttribute("x",stringc(getPosition().X).c_str());
	playerXML->SetAttribute("y",stringc(getPosition().Y).c_str());
    playerXML->SetAttribute("z",stringc(getPosition().Z).c_str());
    playerXML->SetAttribute("script",script.c_str());

    parentElement->LinkEndChild(playerXML);
}

bool Player::loadFromXML(TiXmlElement* parentElement)
{
	// Preset value, in case of failing to retrieve the attributes.
	f32 x=0;
	f32 y=0;
	f32 z=0;
	script = "";
	
	// Failsafe method to load the field, will not crash if the attribute is not there.
	stringc s_x = parentElement->ToElement()->Attribute("x");
	if (s_x.size()>0) x = (f32)atoi(s_x.c_str());
	
	stringc s_y = parentElement->ToElement()->Attribute("y");
	if (s_y.size()>0) y = (f32)atoi(s_y.c_str());

    stringc s_z = parentElement->ToElement()->Attribute("z"); 
	if (s_z.size()>0) z = (f32)atoi(s_z.c_str());

    stringc s_script = parentElement->ToElement()->Attribute("script");
	if (s_script.size()>0) script = s_script.c_str();

    this->setPosition(vector3df(x,y,z));
	return true;
}

int Player::getPosition(lua_State *LS)
{
    vector3df pos = Player::getInstance()->getPosition();
    lua_pushnumber(LS,pos.X);
    lua_pushnumber(LS,pos.Y);
    lua_pushnumber(LS,pos.Z);

    return 3;
}

int Player::setPosition(lua_State *LS)
{
    float z = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	float y = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	float x = (float)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

    Player::getInstance()->setPosition(vector3df(x,y,z));

    return 0;
}

void Player::addItem(stringc itemName)
{
    items.push_back(itemName);
}

void Player::removeItem(stringc itemName)
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

vector<stringc> Player::getItems()
{
    return items;
}

int Player::getItemCount(stringc itemName)
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

bool Player::hasItem(stringc itemName)
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

void Player::removeAllItems()
{
    items.clear();
}

void Player::setAnimator(irr::scene::ISceneNodeAnimatorCollisionResponse *coll)
{
	this->getNode()->addAnimator(coll);
	anim = coll;
}