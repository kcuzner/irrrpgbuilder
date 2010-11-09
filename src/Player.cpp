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


            playerModel newModel;

            newModel.name = playerModelXML->ToElement()->Attribute("name");

            stringc pathFile = "../media/player/";
			// Animation meshes -- main mesh
            stringc meshFile = playerModelXML->ToElement()->Attribute("mesh");
			// Animation meshes -- Skinned options
			stringc idleFile = playerModelXML->ToElement()->Attribute("idlemesh");
			stringc walkFile = playerModelXML->ToElement()->Attribute("walkmesh");
			stringc runFile = playerModelXML->ToElement()->Attribute("walkmesh");
			stringc attackFile = playerModelXML->ToElement()->Attribute("attackmesh");
			stringc injuredFile = playerModelXML->ToElement()->Attribute("injuredmesh");
			stringc dieFile = playerModelXML->ToElement()->Attribute("diemesh");

            newModel.scale = (f32)atof(playerModelXML->ToElement()->Attribute("scale"));
					
			newModel.idle_start = atoi(playerModelXML->ToElement()->Attribute("idle_start"));
			newModel.idle_end = atoi(playerModelXML->ToElement()->Attribute("idle_end"));

            newModel.walk_start = atoi(playerModelXML->ToElement()->Attribute("walk_start"));
            newModel.walk_end = atoi(playerModelXML->ToElement()->Attribute("walk_end"));
			printf("Loading the walk end info: frame is %d\n",newModel.walk_end);
			
			stringc s_run_start = playerModelXML->ToElement()->Attribute("run_start");
			if (s_run_start.size()>1) newModel.run_start = atoi(s_run_start.c_str());
            
			stringc s_run_end = playerModelXML->ToElement()->Attribute("run_end");
			if (s_run_end.size()>1) newModel.run_end = atoi(s_run_end.c_str());
			
            newModel.attack_start = atoi(playerModelXML->ToElement()->Attribute("attack_start"));
            newModel.attack_end = atoi(playerModelXML->ToElement()->Attribute("attack_end"));

			newModel.injured_start = atoi(playerModelXML->ToElement()->Attribute("injured_start"));
            newModel.injured_end = atoi(playerModelXML->ToElement()->Attribute("injured_end"));

            newModel.die_start = atoi(playerModelXML->ToElement()->Attribute("die_start"));
            newModel.die_end = atoi(playerModelXML->ToElement()->Attribute("die_end"));
			
			// Set the meshes for the animations. Will not set the other meshes if not present.
            newModel.mesh = smgr->getMesh(pathFile+meshFile);
			// Check for the presence of another animation file 
			if (idleFile!="") 
			{
				newModel.idlemesh = smgr->getMesh(pathFile+idleFile);
				newModel.idle=true;
			}
			else
			{
				printf("The mesh is not present for the idle animations");
				newModel.idle=false;
			}
			// Check for the presence of another animation file
			if (walkFile!="")
			{
				newModel.walkmesh = smgr->getMesh(pathFile+walkFile);
				newModel.walk = true;
			}
			else
			{
				newModel.walk = false;
			}
			// Check for the presence of another animation file
			if (runFile!="") 
			{
				newModel.runmesh = smgr->getMesh(pathFile+runFile);
				newModel.run = true;
			}
			else
			{
				newModel.run = false;
			}
			// Check for the presence of another animation file
			if (attackFile!="") 
			{
				newModel.attackmesh = smgr->getMesh(pathFile+attackFile);
				newModel.attack = true;
			}
			else
			{	
				newModel.attack = false;
			}
			// Check for the presence of another animation file
			if (injuredFile!="") 
			{
				newModel.injuredmesh = smgr->getMesh(pathFile+injuredFile);
				newModel.injured = true;
			}
			else
			{
				newModel.injured = false;
			}
			// Check for the presence of another animation file
			if (dieFile!="") 
			{
				newModel.diemesh = smgr->getMesh(pathFile+dieFile);
				newModel.die = true;
			}
			else
			{
				newModel.die = false;
			}
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
    fakeShadow->setScale(vector3df(0.5,1,0.5));
    fakeShadow->setPosition(vector3df(0,(f32)0.03 ,0));

	walkSpeed = (f32)0.02;

    this->setAnimation(PLAYER_ANIMATION_IDLE);

    enemyUnderAttack = NULL;

    life = 100;
    money = 0;

    script = "";
	timer = App::getInstance()->getDevice()->getTimer();
	currentime = timer->getRealTime();
	oldtime = timer->getRealTime();
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
    lookAt(targetPos);

    vector3df pos=this->getPosition();
    pos.Z -= cos((this->getRotation().Y)*PI/180)*speed;
    pos.X -= sin((this->getRotation().Y)*PI/180)*speed;
    pos.Y = 0;///TODO: fixar no Y da terrain (gravidade)

    if(TerrainManager::getInstance()->getHeightAt(pos) == 0)
        this->setPosition(pos);
    else
        walkTarget = this->getPosition();
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
	if (anim==PLAYER_ANIMATION_DIE)
		printf("The player was asked to die!");
	// When injured, don't do anything until the animation is completed.
	if((anim != PLAYER_ANIMATION_DIE) && (currentAnimation == PLAYER_ANIMATION_INJURED) && (currentModel.node->getFrameNr()<currentModel.injured_end)) return;
	if(anim == currentAnimation) return;
	ISkinnedMesh* skin = (ISkinnedMesh*)currentModel.mesh;
    switch(anim)
    {
        case PLAYER_ANIMATION_IDLE:
			// Use another animation mesh if it's defined
			if (currentModel.idle)  
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
            currentAnimation = PLAYER_ANIMATION_IDLE;
            break;
        case PLAYER_ANIMATION_WALK:
			if (currentModel.walk)  
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
            currentAnimation = PLAYER_ANIMATION_WALK;
            break;
        case PLAYER_ANIMATION_ATTACK:
			if (currentModel.attack)  
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
            currentAnimation = PLAYER_ANIMATION_ATTACK;
            break;
        case PLAYER_ANIMATION_DIE:
			if (currentModel.die)  
			{	
				ISkinnedMesh* skindie = (ISkinnedMesh*)currentModel.diemesh;
				skin->useAnimationFrom(skindie);
			}
			else
			{
				skin->useAnimationFrom(skin);
				
			}
			currentModel.node->setFrameLoop(currentModel.die_start,currentModel.die_end);
            currentAnimation = PLAYER_ANIMATION_DIE;
			printf("Doing the death animation");
            break;
		case PLAYER_ANIMATION_INJURED:
			if (currentModel.injured)  
			{
				
				ISkinnedMesh* skininjured = (ISkinnedMesh*)currentModel.injuredmesh;
				skin->useAnimationFrom(skininjured);
			}
			else
			{
				skin->useAnimationFrom(skin);
				
			}
			currentModel.node->setFrameLoop(currentModel.injured_start,currentModel.injured_end);
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
	if((currentAnimation == PLAYER_ANIMATION_ATTACK) && (currentModel.node->getFrameNr() > currentModel.attack_start+10) && (currentModel.node->getFrameNr() < currentModel.attack_start+11))
	{
		currentime = timer->getRealTime();
		u32 delay = currentime - oldtime;
		// Check that the event is not repeating for 200ms, because there could be some imprecision in determining the
		// proper frame of the event. 
		if (delay>200)
		{
			int en_life = enemyUnderAttack->getLife();
			en_life -= 1;
			if (en_life<0) en_life=0;
			// Set the new life value and call the update so the display is sure to be refreshed when attack.
			// I want also to have a "on_attack" event sent to the LUA
			enemyUnderAttack->setLife(en_life);
			enemyUnderAttack->update();
			oldtime = timer->getRealTime();
			return true;
		}
	}
	else return false;
}

void Player::update()
{
	if (currentAnimation!=PLAYER_ANIMATION_DIE)
	{
		CheckAnimationEvent();
		if( (this->getPosition().getDistanceFrom(walkTarget) > 0.2) &&  (this->getLife()!=0))
		{
			TerrainManager::getInstance()->getHeightAt(walkTarget);

			this->setAnimation(PLAYER_ANIMATION_WALK);
		
			this->walkTo(walkTarget, walkSpeed);
		}
		else if( enemyUnderAttack && this->getAnimation() == PLAYER_ANIMATION_ATTACK)
		{
			this->lookAt(enemyUnderAttack->getPosition());

			if(( this->getPosition().getDistanceFrom(enemyUnderAttack->getPosition()) > 1 ) || (enemyUnderAttack->getLife()==0))
			{
				this->setAnimation(PLAYER_ANIMATION_IDLE);
				enemyUnderAttack = NULL;
			}
		}
		else if((this->getLife()!=this->getInstance()->oldlife) && (this->getLife()!=0))
			{
				//printf("The player is being injured in combat!\n");
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
	this->oldlife = this->life;
	
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

    life = 100;
	oldlife = 100;
    money = 0;

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
    this->life = life;
	
	// Update the GUI display
	stringc playerLife = LANGManager::getInstance()->getText("txt_player_life");
	playerLife += life;
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
    return this->life;
}

void Player::setMoney(int money)
{
    this->money = money;
}

int Player::getMoney()
{
    return this->money;
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
	if (s_x.size()>1) x = (f32)atoi(s_x.c_str());
	
	stringc s_y = parentElement->ToElement()->Attribute("y");
	if (s_y.size()>1) y = (f32)atoi(s_y.c_str());

    stringc s_z = parentElement->ToElement()->Attribute("z"); 
	if (s_z.size()>1) z = (f32)atoi(s_z.c_str());

    stringc s_script = parentElement->ToElement()->Attribute("script");
	if (s_script.size()>1) script = s_script.c_str();

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
