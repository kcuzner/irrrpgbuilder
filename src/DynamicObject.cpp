#include "DynamicObject.h"

#include "App.h"
#include "DynamicObjectsManager.h"
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

	this->properties.life = 100;
	this->properties.experience = 10;
    this->properties.money = 0;
	timer = App::getInstance()->getDevice()->getTimer()->getRealTime();
}

DynamicObject::DynamicObject(stringc name, IMesh* mesh, vector<DynamicObject_Animation> animations)
{
    this->animations = animations;

    setupObj(name, mesh);

	this->properties.life = 100;
	this->properties.experience = 10;
    this->properties.money = 0;
	timer = App::getInstance()->getDevice()->getTimer()->getRealTime();
	timer2 = App::getInstance()->getDevice()->getTimer()->getRealTime();
}

void DynamicObject::setupObj(stringc name, IMesh* mesh)
{
    ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();
	IVideoDriver* driver = App::getInstance()->getDevice()->getVideoDriver();

    this->mesh = mesh;
    this->name = name;

	if(hasAnimation())
	{
		this->mesh->setHardwareMappingHint(EHM_DYNAMIC);
		
        IAnimatedMeshSceneNode * NodeAnim = smgr->addAnimatedMeshSceneNode((IAnimatedMesh*)mesh,0,0x0010);
		this->node = NodeAnim;
			
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

		this->collisionAnimator = NULL;

		node->setDebugDataVisible(EDS_BBOX | EDS_SKELETON);

		script = "";

		//Fake Shadow
		fakeShadow = smgr->addMeshSceneNode(smgr->getMesh("../media/dynamic_objects/shadow.obj"),node);
		fakeShadow->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
		fakeShadow->setPosition(vector3df(0,0.03f + (rand()%5)*0.01f ,0));

		node->setMaterialFlag(EMF_FOG_ENABLE,true);
		fakeShadow->setMaterialFlag(EMF_FOG_ENABLE,true);

		objLabel = smgr->addTextSceneNode(GUIManager::getInstance()->getFont(FONT_ARIAL),L"",SColor(255,255,255,0),node,vector3df(0,1,0));
		objLabel->setVisible(false);

		this->setEnabled(true);
		if(hasAnimation()) this->setFrameLoop(0,0);
		
		
	}
}

DynamicObject::~DynamicObject()
{
    selector->drop();
    node->remove();
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
void DynamicObject::setType(stringc name)
{
	if (name=="npc")
		this->objectType=OBJECT_TYPE_NPC;
	if (name=="interactive")
		this->objectType=OBJECT_TYPE_INTERACTIVE;
	if (name=="non-interactive")
		this->objectType=OBJECT_TYPE_NON_INTERACTIVE;
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

ISceneNode* DynamicObject::getNode()
{
    return node;
}

void DynamicObject::setPosition(vector3df pos)
{
	
	node->setPosition(pos);
	vector3df pos2 = node->getAbsolutePosition();
	node->updateAbsolutePosition();
	// Debug, for fixing the problem with positionning.
	//printf ("Position requested is: %f,%f,%f\nPosition final is %f,%f,%f\n",pos.X,pos.Y,pos.Z,pos2.X,pos2.Y,pos2.Z);

}

vector3df DynamicObject::getPosition()
{
	return fakeShadow->getAbsolutePosition();
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

ITriangleSelector* DynamicObject::getTriangleSelector()
{
    return selector;
}

stringc DynamicObject::getScript()
{
    return script;
}

void DynamicObject::setScript(stringc script)
{
    this->script = script;
}

void DynamicObject::saveToXML(TiXmlElement* parentElement)
{
    TiXmlElement* dynamicObjectXML = new TiXmlElement("obj");
    //dynamicObjectXML->SetAttribute("name",name.c_str());

    dynamicObjectXML->SetAttribute("x",stringc(this->getPosition().X).c_str());
    dynamicObjectXML->SetAttribute("y",stringc(this->getPosition().Y).c_str());
    dynamicObjectXML->SetAttribute("z",stringc(this->getPosition().Z).c_str());

    //dynamicObjectXML->SetAttribute("s",stringc(this->getScale().X).c_str());

    dynamicObjectXML->SetAttribute("r",stringc(this->getRotation().Y).c_str());

    dynamicObjectXML->SetAttribute("template",templateObjectName.c_str());
    dynamicObjectXML->SetAttribute("script",getScript().c_str());

    parentElement->LinkEndChild(dynamicObjectXML);
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

stringc DynamicObject::getTemplateObjectName()
{
    return templateObjectName;
}

void DynamicObject::setTemplateObjectName(stringc newName)
{
    templateObjectName = newName;
}

//Initialize Dynamic Object for gameplay
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
    lua_register(L,"move",move);
    lua_register(L,"distanceFrom",distanceFrom);

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


    luaL_dostring(L,script.c_str());

    //set default object type
    luaL_dostring(L,"objType = 'OBJECT'");
    //set enemy (when you click an enemy you attack it)
    luaL_dostring(L,"function setEnemy() objType = 'ENEMY' end");
    //set object (when you click an object you interact with it)
    luaL_dostring(L,"function setObject() objType = 'OBJECT' end");


    //store the original position and rotation before start the gameplay (restore it with "IRBRestoreParams")
    lua_getglobal(L,"IRBSaveParams");
    if(lua_isfunction(L, -1)) lua_call(L,0,0);
    lua_pop( L, -1 );

    //run onLoad() function if it exists
    lua_getglobal(L,"onLoad");
    //if top of stack is not a function then onLoad does not exist
    if(lua_isfunction(L, -1)) lua_call(L,0,0);
    lua_pop( L, -1 );
}

void DynamicObject::update()
{
	// New optimisations (november 2010): Adding timed interface an culling check.
	// Added a timed call to the lua but only a 1/4 sec intervals. (Should be used for decision making)
	// Check if the node is in walk state, so update the walk at 1/60 intervals (animations need 1/60 check)
	// Check for culling on a node and don't update it if it's culled.
	
	u32 timerobject = App::getInstance()->getDevice()->getTimer()->getRealTime();
	bool culled = false;
	if((timerobject-timer>250) && enabled) // 1/4 second
	{
		culled = App::getInstance()->getDevice()->getSceneManager()->isCulled(this->getNode());
		if (!culled)
		{
			// Perhaps this is not needed is the node is really culled
			if (!this->getNode()->isVisible())
				this->getNode()->setVisible(true);
			if (App::getInstance()->getAppState() > 100) 
			{//app_state < APP_STATE_CONTROL
				lua_getglobal(L,"step");
				if(lua_isfunction(L, -1)) lua_call(L,0,0);
				lua_pop( L, -1 );

				//custom update function (updates walkTo for example..)
				lua_getglobal(L,"CustomDynamicObjectUpdate");
				if(lua_isfunction(L, -1)) lua_call(L,0,0);
				lua_pop( L, -1 );
			}
			lua_getglobal(L,"CustomDynamicObjectUpdateProgrammedAction");
			if(lua_isfunction(L, -1)) lua_call(L,0,0);
			lua_pop( L, -1 );
			timer = timerobject;
		}
		// Perhaps this is not needed if the node is really culled
		else if (this->getNode()->isVisible())
			this->getNode()->setVisible(false);
			
	}
	
	if (currentAnimation==OBJECT_ANIMATION_WALK && !culled && (timerobject-timer2>17)) // 1/60 second
	{
		currentObject->moveObject(currentSpeed);
		timer2=timerobject;
	}
}

void DynamicObject::clearScripts()
{
    if(hasAnimation())
	{
		this->setFrameLoop(0,0);
		this->setAnimation("idle");
	}

    lua_close(L);
}

void DynamicObject::lookAt(vector3df pos)
{
    vector3df offsetVector = pos - node->getPosition();

    vector3df rot = (-offsetVector).getHorizontalAngle();

    rot.X=0;
    rot.Z=0;

    node->setRotation(rot);
}

void DynamicObject::setFrameLoop(s32 start, s32 end)
{
    if(hasAnimation()) ((IAnimatedMeshSceneNode*)node)->setFrameLoop(start,end);
}

void DynamicObject::setAnimationSpeed(f32 speed)
{
    if(hasAnimation()) ((IAnimatedMeshSceneNode*)node)->setAnimationSpeed(speed);
}

f32 DynamicObject::getDistanceFrom(vector3df pos)
{
    return node->getPosition().getDistanceFrom(pos);
}

void DynamicObject::restoreParams()
{
    lua_getglobal(L,"IRBRestoreParams");
    if(lua_isfunction(L, -1)) lua_call(L,0,0);
    lua_pop( L, -1 );
}

void DynamicObject::setLife(int life)
{
    this->properties.life = life;
}

int DynamicObject::getLife()
{
	return this->properties.life;
}

void DynamicObject::setMoney(int money)
{
	this->properties.money = money;
}

int DynamicObject::getMoney()
{
	return this->properties.money;
}

void DynamicObject::setObjectLabel(stringc label)
{
    objLabel->setText(stringw(label).c_str());
}

void DynamicObject::objectLabelSetVisible(bool visible)
{
    objLabel->setVisible(visible);
}

void DynamicObject::setEnabled(bool enabled)
{
    this->enabled = enabled;
/*
    if(enabled && collisionAnimator)
    {
        Player::getInstance()->getNode()->addAnimator(collisionAnimator);
    }
    else
    {
        Player::getInstance()->getNode()->removeAnimator(collisionAnimator);
        Player::getInstance()->attackEnemy(NULL);///TODO: find a better way to broke the link between player and dead DO
    }
*/
    this->node->setVisible(enabled);
	if (!enabled)
		DynamicObjectsManager::getInstance()->updateMetaSelector(this->getNode()->getTriangleSelector(),true);
}

OBJECT_ANIMATION DynamicObject::getAnimationState(stringc animName)
{
	if (animName==NULL)
		return OBJECT_ANIMATION_IDLE;
	//printf("Asked animation name is: %s\n",animName);
	OBJECT_ANIMATION Animation;
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

void DynamicObject::setAnimation(stringc animName)
{
    for(int i=0;i < (int)animations.size();i++)
    {
        DynamicObject_Animation tempAnim = (DynamicObject_Animation)animations[i];
		OBJECT_ANIMATION Animation = this->getAnimationState(animName);
        if( tempAnim.name == animName )
        {
			if (Animation!=this->currentAnimation)
			{
				this->currentAnimation=Animation;	
				this->setFrameLoop(tempAnim.startFrame,tempAnim.endFrame);
				this->setAnimationSpeed(tempAnim.speed);
			}
            return;
        }
    }

	#ifdef APP_DEBUG
    cout << "ERROR : DYNAMIC_OBJECT : ANIMATION " << animName.c_str() <<  " NOT FOUND!" << endl;
    #endif
}

void DynamicObject::moveObject(f32 speed)
{
	//DynamicObject* tempObj
	//tempObj->setPosition(tempObj->getPosition() + vector3df(x,y,z));
	
	vector3df pos=this->getPosition();
	pos.Z -= cos((this->getRotation().Y)*PI/180)*speed;
    pos.X -= sin((this->getRotation().Y)*PI/180)*speed;
    pos.Y = 0;///TODO: fixar no Y da terrain (gravidade)

	this->setPosition(pos);
	currentObject=this;
	currentSpeed=speed;
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
            vector3df pos = Player::getInstance()->getPosition();
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
		tempObj->setAnimation("walk");
		
        tempObj->moveObject(speed);
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
        otherObjPosition = Player::getInstance()->getPosition();
    }
    else
    {
        DynamicObject* otherObj = DynamicObjectsManager::getInstance()->getObjectByName(GlobalMap::getInstance()->getGlobal(otherObjName.c_str()).c_str());

        otherObjPosition = otherObj->getPosition();
    }


    tempObj->lookAt(otherObjPosition);

    return 0;
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
            otherPos = Player::getInstance()->getPosition();
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

stringc DynamicObject::getObjectType()
{
    lua_getglobal(L,"objType");
    stringc objType = lua_tostring(L, -1);
	lua_pop(L, 1);

	return objType;
}

void DynamicObject::setCollisionAnimator(ISceneNodeAnimatorCollisionResponse* collisionAnimator)
{
    this->collisionAnimator = collisionAnimator;
}
