#include "LuaGlobalCaller.h"

#include "gui/GUIManager.h"
#include "LANGManager.h"
#include <time.h>
#include "objects/DynamicObjectsManager.h"
#include "camera/CameraSystem.h"
#include "fx/EffectsManager.h"
#include "sound/SoundManager.h"
#include "objects/Player.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;


GlobalMap::GlobalMap()
{
    stringMap.clear();
}

GlobalMap* GlobalMap::getInstance()
{
    static GlobalMap *instance = 0;
    if (!instance) instance = new GlobalMap();
    return instance;
}

void GlobalMap::setGlobal(std::string var, std::string data)
{
    std::map<std::string, std::string>::iterator it;
    it = stringMap.find(var);

    if(it==stringMap.end())
    {
        #ifdef APP_DEBUG
        printf("GLOBAL %s CREATED = %s\n",var.c_str(),data.c_str());
        #endif

        stringMap.insert(StringMapPair(var.c_str(),data.c_str()));//insert a net key if var don't exist
    }
    else
    {
        #ifdef APP_DEBUG
        printf("GLOBAL %s UPDATED = %s\n",var.c_str(),data.c_str());
        #endif

        stringMap.find(var)->second = data.c_str();//replace value if var already exist
    }
}

std::string GlobalMap::getGlobal(std::string var)
{
    std::map<std::string, std::string>::iterator it;
    it = stringMap.find(var);

    if(it==stringMap.end())
    {
        #ifdef APP_DEBUG
        printf("GLOBAL %s DOES NOT EXIST\n",var.c_str());
        #endif

        return "IRR_RPG_BUILDER_GLOBAL_VAR_ERROR";
    }
    else
        return (std::string)it->second;
}


void GlobalMap::deleteGlobal(std::string var)
{
    stringMap.erase(var);
    #ifdef APP_DEBUG
    printf("GLOBAL %s DELETED\n",var.c_str());
    #endif
}

void GlobalMap::clearGlobals()
{
    stringMap.clear();
}

bool GlobalMap::saveGlobalsToXML(stringc filename)
{
    TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );

	TiXmlElement* irb_save_ingame = new TiXmlElement( "IrrRPG_Builder_Save_Game" );
	irb_save_ingame->SetAttribute("version","1.0");///TODO: load version from version.h

    //save global vars
    std::map<std::string, std::string>::iterator it = stringMap.begin();

    TiXmlElement* globalVarsXML = new TiXmlElement("global_vars");
    irb_save_ingame->LinkEndChild(globalVarsXML);

    for(;it != stringMap.end();++it)
    {
        //dynamic_objects global vars and player does not need to be saved, these values are auto generated at start of gameplay
        if( stringc((it->second).c_str()) != stringc("player") && stringc((it->second).c_str()).subString(0,14) != stringc("dynamic_object"))
        {
            TiXmlElement* globalVarXML = new TiXmlElement("global");
            globalVarXML->SetAttribute("var",  (it->first).c_str() );
            globalVarXML->SetAttribute("value",(it->second).c_str() );
            globalVarsXML->LinkEndChild(globalVarXML);
        }
    }

    //save current player seetings
    TiXmlElement* playerXML = new TiXmlElement("player_info");
    irb_save_ingame->LinkEndChild(playerXML);

    playerXML->SetAttribute("x",  stringc(Player::getInstance()->getObject()->getPosition().X).c_str() );
    playerXML->SetAttribute("y",  stringc(Player::getInstance()->getObject()->getPosition().Y).c_str() );
    playerXML->SetAttribute("z",  stringc(Player::getInstance()->getObject()->getPosition().Z).c_str() );
    playerXML->SetAttribute("life",  stringc(Player::getInstance()->getObject()->getLife()).c_str() );
    playerXML->SetAttribute("money",  stringc(Player::getInstance()->getObject()->getMoney()).c_str() );

    vector<stringc> playerItems = Player::getInstance()->getObject()->getItems();

    //save current player items
    for(int i=0;i<(int)playerItems.size();i++)
    {
        TiXmlElement* playerItemXML = new TiXmlElement("item");
        playerItemXML->SetAttribute("name",  stringc(playerItems[i]).c_str() );
        playerXML->LinkEndChild(playerItemXML);
    }

	doc.LinkEndChild( decl );
	doc.LinkEndChild( irb_save_ingame );

	return doc.SaveFile( filename.c_str() );
}

bool GlobalMap::loadGlobalsFromXML(stringc filename)
{
    TiXmlDocument doc(filename.c_str());
	if (!doc.LoadFile()) return false;

    TiXmlElement* root = doc.FirstChildElement( "IrrRPG_Builder_Save_Game" );

    if ( root )
    {
        //clear and load global vars
        this->clearGlobals();

        TiXmlNode* globalVarsXML = root->FirstChild( "global_vars" );

        TiXmlNode* globalVarXML = globalVarsXML->FirstChild( "global" );

        while( globalVarXML != NULL )
        {
            this->setGlobal( globalVarXML->ToElement()->Attribute("var"),globalVarXML->ToElement()->Attribute("value") );
            globalVarXML = root->IterateChildren( "global", globalVarXML );
        }

        //load player info
        TiXmlNode* playerXML = root->FirstChild( "player_info" );

        if(playerXML)
        {
            f32 X = (f32)atof(playerXML->ToElement()->Attribute("x"));
            f32 Y = (f32)atof(playerXML->ToElement()->Attribute("y"));
            f32 Z = (f32)atof(playerXML->ToElement()->Attribute("z"));
            s32 life = atoi(playerXML->ToElement()->Attribute("life"));
            s32 money = atoi(playerXML->ToElement()->Attribute("money"));

            Player::getInstance()->getObject()->setPosition(vector3df(X,Y,Z));
            Player::getInstance()->getObject()->setWalkTarget(vector3df(X,Y,Z));
            Player::getInstance()->getObject()->setLife(life);
            Player::getInstance()->getObject()->setMoney(money);

            Player::getInstance()->getObject()->removeAllItems();
			Player::getInstance()->getObject()->removeAllLoot();


            //load player items
            TiXmlNode* playerItemXML = playerXML->FirstChild( "item" );

            while(playerItemXML)
            {
                Player::getInstance()->getObject()->addItem(playerItemXML->ToElement()->Attribute("name"));
                playerItemXML = playerXML->IterateChildren( "item", playerItemXML );
            }
        }

        return true;
    }
    else
        return false;
}





LuaGlobalCaller::LuaGlobalCaller()
{
    // create an Lua pointer instance
    LS = lua_open();

    // load the libs
    luaL_openlibs(LS);

    this->registerBasicFunctions(LS);
}

LuaGlobalCaller::~LuaGlobalCaller()
{
	//dtor
}

void LuaGlobalCaller::registerBasicFunctions(lua_State *LS)
{
    // make C functions visible in LUA
    lua_register(LS,"setGlobal",setGlobal);//setGlobal(global_unique_name,value)
    lua_register(LS,"getGlobal",getGlobal);//value = getGlobal(global_unique_name)
    lua_register(LS,"deleteGlobal",deleteGlobal);//deleteGlobal(global_unique_name)

    lua_register(LS,"setTimeOfDay",setTimeOfDay);
    lua_register(LS,"setAmbientLight",setAmbientLight);//setAmbientLight(r,g,b)
    lua_register(LS,"getAmbientLight",getAmbientLight);//r,g,b = getAmbientLight()
    lua_register(LS,"setFogColor",setFogColor);//setFogColor(r,g,b)
    lua_register(LS,"getFogColor",getFogColor);//r,g,b = getFogColor()
    lua_register(LS,"setFogRange",setFogRange);//setFogRange(near,far)
    lua_register(LS,"getFogRange",getFogRange);//near,far = getFogRange()
	lua_register(LS,"setPostFX",setPostFX); // define the postprocess FX

    lua_register(LS,"showBlackScreen",showBlackScreen);//showBlackScreen(optional_text)
    lua_register(LS,"hideBlackScreen",hideBlackScreen);//hideBlackScreen()

	lua_register(LS,"setSkydomeTexture",setSkydomeTexture);
	lua_register(LS,"setSkydomeVisible",setSkydomeVisible);
	lua_register(LS,"setBackgroundColor",setBackgroundColor);

    lua_register(LS,"setWeatherPr",setWeather);//setWeather(maxParticles,particleSpeed)

    lua_register(LS,"setCameraTarget",setCameraTarget);//setCameraTarget(x,y,z)    or    setCameraTarget(objName)
    lua_register(LS,"getCameraTarget",getCameraTarget);//x,y,z = getCameraTarget()
	lua_register(LS,"getCameraRange",getCameraRange);//near,far = getCameraRange()
	lua_register(LS,"setCameraRange",setCameraRange);//near,far = setCameraRange()
	lua_register(LS,"getCameraRTSRotation",getCameraRTSRotation);//X,Y = getCameraRTSRotation()
	lua_register(LS,"setCameraRTSRotation",setCameraRTSRotation);//setCameraRTSRotation(X,Y)
	lua_register(LS,"getCameraAngleLimit",getCameraAngleLimit);//near,far = getCameraAngleLimit()
	lua_register(LS,"setCameraAngleLimit",setCameraAngleLimit);//setCameraAngleLimit(near, far)
	lua_register(LS,"setCameraPosition",setCameraPosition);//setCameraPosition(x,y,z)    or    setCameraTarget(objName)
    lua_register(LS,"getCameraPosition",getCameraPosition);//x,y,z = getCameraPosition()
	lua_register(LS,"getCameraZoom",getCameraZoom);//zoom = getCameraZoom()
	lua_register(LS,"setCameraZoom",setCameraZoom);// setCameraZoom( zoom )
	lua_register(LS,"cutsceneMode",cutsceneMode); // Activate cutscene mode
	lua_register(LS,"gameMode",gameMode); //Activate game mode
	lua_register(LS,"setRTSView",setRTSView); //Camera view is RTS style
	lua_register(LS,"setRPGView",setRPGView); //Camera view is RPG style
	lua_register(LS,"setFPSView",setFPSView); //Camera view is RPG style
	lua_register(LS,"setRTSFixedView",setRTSFixedView); //Camera view in RTS Style with no rotation
	lua_register(LS,"defineKeys",defineKeys); //Define the keyboard keys for ingame actions
	lua_register(LS,"setCameraAttachment",setCameraAttachment); //Define the keyboard keys for ingame actions
	lua_register(LS,"setCameraOffset",setCameraOffset); //Define the keyboard keys for ingame actions

	lua_register(LS,"showCutsceneText",showCutsceneText); // Display/Hide the cutscene text
	lua_register(LS,"setCutsceneText",setCutsceneText); // Set the text to display

    lua_register(LS,"getObjectPosition",getObjectPosition);//x,y,z getObjectPosition(objName)
	lua_register(LS,"setObjectRotation",setObjectRotation);////setObjectRotation(from,to,time);

    //register Sound Functions
    lua_register(LS,"playSound2D",playSound2D);//playSound2D("sound.ogg") or playSound("sound.ogg", true) for looped
    lua_register(LS,"playSound3D",playSound3D);//playSound3D(string sound_name, bool looped, float x, float y, float z)
    lua_register(LS,"setSoundListenerPosition",setListenerPosition);//setSoundListenerPosition(x,y,z)
    lua_register(LS,"setSoundVolume",setVolume);//setSoundVolume(float volume)
    lua_register(LS,"stopSounds",stopSounds);//stopSounds()

    //Player Functions
    lua_register(LS,"setPlayerLife",setPlayerLife);
    lua_register(LS,"getPlayerLife",getPlayerLife);
    lua_register(LS,"setPlayerMoney",setPlayerMoney);
    lua_register(LS,"getPlayerMoney",getPlayerMoney);
    lua_register(LS,"addPlayerItem",addPlayerItem);
    lua_register(LS,"removePlayerItem",removePlayerItem);
    lua_register(LS,"getObjectItemCount",getObjectItemCount);
	lua_register(LS,"addObjectLoot",addObjectLoot);
	lua_register(LS,"useGlobalFunction",useGlobalFunctionLUA);
	lua_register(LS,"setObjectProperty",setObjectProperty);
	lua_register(LS,"getObjectProperty",getObjectProperty);
	lua_register(LS,"checkObjectItem",checkObjectItem);


	// NPC functions
	lua_register(LS,"setObjectLife",setObjectLife);
    lua_register(LS,"getObjectLife",getObjectLife);
	lua_register(LS,"isObjectVisible",isObjectVisible);
	lua_register(LS,"setObjectVisible",setObjectVisible);
	lua_register(LS,"destroyObjectItem",destroyObjectItem);
	lua_register(LS,"attachObject",attachObject);

    //inGame Save/Load
    lua_register(LS,"saveGame",inGameSave);
    lua_register(LS,"loadGame",inGameLoad);

	lua_register(LS,"getAnswer",getAnswer);
	lua_register(LS,"getLanguage",getLanguage);
	lua_register(LS,"loadMap",loadMap);

	lua_register(LS,"spawn",spawn);

    //do basic functions
    luaL_dofile(LS,"../media/scripts/basicFunctions.lua");

}

LuaGlobalCaller* LuaGlobalCaller::getInstance()
{
    static LuaGlobalCaller *instance = 0;
    if (!instance) instance = new LuaGlobalCaller();
    return instance;
}

void LuaGlobalCaller::doScript(stringc script)
{
    int error = luaL_dostring(LS,script.c_str());
	core::stringc result = (core::stringc)lua_tostring(LS, -1);
	printf("Global script called: result %s\n",result.c_str());
// Only available in the editor
#ifdef EDITOR
    if(error)
		GUIManager::getInstance()->setEditBoxText(GUIManager::EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE,result.c_str());
    else
	{
		GUIManager::getInstance()->setEditBoxText(GUIManager::EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE,
			LANGManager::getInstance()->getText("bt_dynamic_objects_script_noerror").c_str());
	}
#endif
}


void LuaGlobalCaller::storeGlobalParams()
{
    lua_getglobal(LS,"IRBStoreGlobalParams");
    if(lua_isfunction(LS, -1)) lua_pcall(LS,0,0,0);
    lua_pop( LS, -1 );
}

void LuaGlobalCaller::restoreGlobalParams()
{
    lua_getglobal(LS,"IRBRestoreGlobalParams");
    if(lua_isfunction(LS, -1)) lua_pcall(LS,0,0,0);
    lua_pop( LS, -1 );
}

//Calling a custom function in a global script
//From a dynamic object script.
void LuaGlobalCaller::useGlobalFunction(stringc function)
{
    lua_getglobal(LS,function.c_str());
    if(lua_isfunction(LS,-1))
    {
        lua_pcall(LS,0,0,0);
    }
    else
    {
        lua_pop(LS,-1);
    }
}

void LuaGlobalCaller::setAnswer(bool answer)
{
	this->answer=answer;
}

int LuaGlobalCaller::useGlobalFunctionLUA(lua_State *LS)
{
	stringc function = "";

    if(lua_isstring(LS,-1))
    {
        function = lua_tostring(LS,-1);
        lua_pop(LS,1);
    }
	LuaGlobalCaller::getInstance()->useGlobalFunction(function);
	return 0;
}

int LuaGlobalCaller::getObjectItemCount(lua_State *LS)
{

	stringc itemName = "";
	stringc objName="";
	stringc dynamicObjName = "";

	int top = lua_gettop(LS);
    if (top>1)
	{
		itemName = lua_tostring(LS,-1);
        lua_pop(LS,1);
		objName = lua_tostring(LS, -1);
		lua_pop(LS,1);
	} else
	{
		itemName = lua_tostring(LS,-1);
        lua_pop(LS,1);
		lua_getglobal(LS,"objName");
		objName = lua_tostring(LS, -1);

	}

	//if( stringc( objName.subString(0,14)) == "dynamic_object" || objName=="player" )
        dynamicObjName = objName.c_str();
    //else
     //  dynamicObjName = GlobalMap::getInstance()->getGlobal(objName.c_str()).c_str();

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(dynamicObjName.c_str());

    int result = 0;

	core::stringc error="Object not found:";
	error.append(dynamicObjName);

	if (tempObj)
		result = tempObj->getItemCount(itemName);
	else
		App::getInstance()->getDevice()->getLogger()->log(error.c_str());

	lua_pushnumber(LS,result);
	return 1;
}

//Object will give loot to another object
//Ex: addObjectLoot("DynamicObject","loot template")
int LuaGlobalCaller::addObjectLoot(lua_State *LS)
{

	core::stringc tempname="";
	stringc objName="";
	stringc dynamicObjName = "";

	int top = lua_gettop(LS);
    if (top>1)
	{
		tempname = lua_tostring(LS,-1);
        lua_pop(LS,1);
		objName = lua_tostring(LS, -1);
		lua_pop(LS,1);
	} else
	{
		tempname = lua_tostring(LS,-1);
        lua_pop(LS,1);
		lua_getglobal(LS,"objName");
		objName = lua_tostring(LS, -1);

	}

	//if( stringc( objName.subString(0,14)) == "dynamic_object" || objName=="player" )
        dynamicObjName = objName.c_str();
    //else
    //    dynamicObjName = GlobalMap::getInstance()->getGlobal(objName.c_str()).c_str();

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(dynamicObjName.c_str());

	if(tempObj) // Was the object found?
	{
		if (tempObj->getType()==DynamicObject::OBJECT_TYPE_NPC || tempObj->getType()==DynamicObject::OBJECT_TYPE_PLAYER) // Was the object a loot object?
		{
			DynamicObject* daloot = DynamicObjectsManager::getInstance()->createTemplateAt(tempname,vector3df(0.0f,-2000.0f,0.0f));

			if (daloot) //If the user had entered the wrong name in the template, the object will be removed (will generate the current template)
			{
				if (daloot->getType()!=DynamicObject::OBJECT_TYPE_LOOT)
				{
					DynamicObjectsManager::getInstance()->removeObject(daloot->getName());
					daloot=NULL;
				}

			}

			if (daloot) //Name is ok and will generate the object directly in this object loot
			{
				tempObj->addLootItem(daloot); //Add this pointer object to the player loot
				daloot->getNode()->setVisible(false); //Hide the node
				daloot->getNode()->setPosition(core::vector3df(0,0,0)); // Reset the position
				daloot->getNode()->setParent(Player::getInstance()->getObject()->getNode()); // Parent it to the player
				daloot->isInBag=true;
				daloot->isGenerated=true; //Tell IRB that this object was generated ingame.
			}
		}
	}

    return 0;
}

int LuaGlobalCaller::setObjectProperty(lua_State *LS)
{

	float value = 0.0f;
	stringc propertieName="";
	stringc objName="";
	stringc dynamicObjName = "";

	int top = lua_gettop(LS);
    if (top>2)
	{
		value = (float)lua_tonumber(LS, -1);
		lua_pop(LS, 1);

		propertieName = lua_tostring(LS, -1);
		lua_pop(LS,1);

		objName = lua_tostring(LS, -1);
		lua_pop(LS,1);
	} else
	{
		value = (float)lua_tonumber(LS, -1);
		lua_pop(LS, 1);

		propertieName = lua_tostring(LS, -1);
		lua_pop(LS,1);

		lua_getglobal(LS,"objName");
		objName = lua_tostring(LS, -1);

	}

	//if( stringc( objName.subString(0,14)) == "dynamic_object" || objName=="player" )
        dynamicObjName = objName.c_str();
    //else
    //    dynamicObjName = GlobalMap::getInstance()->getGlobal(objName.c_str()).c_str();

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(dynamicObjName.c_str());

	propertieName.make_lower();

	if (tempObj)
	{
		if (propertieName=="life")
			tempObj->properties.life = (int)value;
		if (propertieName=="maxlife")
			tempObj->properties.maxlife = (int)value;
		if (propertieName=="mindamage")
			tempObj->properties.mindamage = (int)value;
		if (propertieName=="maxdamage")
			tempObj->properties.maxdamage = (int)value;
		if (propertieName=="hurt_resist")
			tempObj->properties.hurt_resist = (int)value;
		if (propertieName=="experience")
			tempObj->properties.experience = (int)value;
		if (propertieName=="dodge_prob")
			tempObj->properties.dodge_prop = (f32)value;
		if (propertieName=="hit_prob")
			tempObj->properties.hit_prob = (f32)value;
		if (propertieName=="mana")
			tempObj->properties.mana = (int)value;
		if (propertieName=="maxmana")
			tempObj->properties.maxmana = (int)value;
		if (propertieName=="money")
			tempObj->properties.money = (int)value;
		if (propertieName=="dotduration")
			tempObj->properties.dotduration = (int)value;
		if (propertieName=="armor")
			tempObj->properties.armor = (int)value;
		if (propertieName=="magic_armor")
			tempObj->properties.magic_armor = (int)value;
		if (propertieName=="regenlife")
			tempObj->properties.regenlife = (int)value;
		if (propertieName=="regenmana")
			tempObj->properties.regenmana = (int)value;
		if (propertieName=="level")
			tempObj->properties.level = (int)value;
		if (propertieName=="weight")
			tempObj->properties.weight = (int)value;
		if (propertieName=="maxweight")
			tempObj->properties.maxweight = (int)value;
		if (propertieName=="currentweight")
			tempObj->properties.currentweight = (int)value;
	} else
	{
		App::getInstance()->getDevice()->getLogger()->log("Set Property: Object is not found!");
	}

	if (objName=="player")
		Player::getInstance()->updateDisplay();

	return 0;
}

int LuaGlobalCaller::getObjectProperty(lua_State *LS)
{

	stringc propertieName="";
	stringc objName="";
	stringc dynamicObjName = "";

	int top = lua_gettop(LS);
    if (top>1)
	{
		propertieName = lua_tostring(LS, -1);
		lua_pop(LS,1);
		objName = lua_tostring(LS, -1);
		lua_pop(LS,1);
	} else
	{
		propertieName = lua_tostring(LS, -1);
		lua_pop(LS,1);
		lua_getglobal(LS,"objName");
		objName = lua_tostring(LS, -1);

	}

	//if( stringc( objName.subString(0,14)) == "dynamic_object" || objName=="player" )
        dynamicObjName = objName.c_str();
    //else
    //    dynamicObjName = GlobalMap::getInstance()->getGlobal(objName.c_str()).c_str();

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(dynamicObjName.c_str());

	propertieName.make_lower();
	float value = 0.0f;
	if (tempObj)
	{
		if (propertieName=="life")
			value = (float) tempObj->properties.life;
		if (propertieName=="maxlife")
			value = (float) tempObj->properties.maxlife;
		if (propertieName=="mindamage")
			value = (float) tempObj->properties.mindamage;
		if (propertieName=="maxdamage")
			value = (float) tempObj->properties.maxdamage;
		if (propertieName=="hurt_resist")
			value = (float) tempObj->properties.hurt_resist;
		if (propertieName=="experience")
			value = (float) tempObj->properties.experience;
		if (propertieName=="dodge_prob")
			value = (float) tempObj->properties.dodge_prop;
		if (propertieName=="hit_prob")
			value = (float) tempObj->properties.hit_prob;
		if (propertieName=="mana")
			value = (float) tempObj->properties.mana;
		if (propertieName=="maxmana")
			value = (float) tempObj->properties.maxmana;
		if (propertieName=="money")
			value = (float) tempObj->properties.money;
		if (propertieName=="dotduration")
			value = (float) tempObj->properties.dotduration;
		if (propertieName=="armor")
			value = (float) tempObj->properties.armor;
		if (propertieName=="magic_armor")
			value = (float) tempObj->properties.magic_armor;
		if (propertieName=="regenlife")
			value = (float) tempObj->properties.regenlife;
		if (propertieName=="regenmana")
			value = (float) tempObj->properties.regenmana;
		if (propertieName=="level")
			value = (float) tempObj->properties.level;
		if (propertieName=="weight")
			value = (float) tempObj->properties.weight;
		if (propertieName=="maxweight")
			value = (float) tempObj->properties.maxweight;
		if (propertieName=="currentweight")
			value = (float) tempObj->properties.currentweight;
	}

	lua_pushnumber(LS,value);

	return 1;
}

int LuaGlobalCaller::checkObjectItem(lua_State *LS)
{
	bool result = false;

	stringc itemname="";
	stringc objName="";
	stringc dynamicObjName = "";

	int top = lua_gettop(LS);
    if (top>1)
	{
		itemname = lua_tostring(LS, -1);
		lua_pop(LS,1);
		objName = lua_tostring(LS, -1);
		lua_pop(LS,1);
	} else
	{
		itemname = lua_tostring(LS, -1);
		lua_pop(LS,1);
		lua_getglobal(LS,"objName");
		objName = lua_tostring(LS, -1);

	}

	//if( stringc( objName.subString(0,14)) == "dynamic_object" || objName=="player" )
        dynamicObjName = objName.c_str();
    //else
    //    dynamicObjName = GlobalMap::getInstance()->getGlobal(objName.c_str()).c_str();

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(dynamicObjName.c_str());

	if (tempObj)
	{

		std::vector<DynamicObject*> list = tempObj->getLootItems();
		//printf("Checking if the item is the same. Checking %i items.\n",list.size());
		for (int a=0; a<list.size(); a++)
		{
			if (list[a]->getTemplateObjectName()==itemname)
				result=true;
		}
	}
	lua_pushboolean(LS,result);
	return 1;
}

// Will check and remove for the first occurence of the time.
// This will not remove all the items of the same name!
int LuaGlobalCaller::destroyObjectItem(lua_State *LS)
{

	stringc itemname="";
	stringc objName="";
	stringc dynamicObjName = "";

	int top = lua_gettop(LS);
    if (top>1)
	{
		itemname = lua_tostring(LS, -1);
		lua_pop(LS,1);
		objName = lua_tostring(LS, -1);
		lua_pop(LS,1);
	} else
	{
		itemname = lua_tostring(LS, -1);
		lua_pop(LS,1);
		lua_getglobal(LS,"objName");
		objName = lua_tostring(LS, -1);

	}


	//if( stringc( objName.subString(0,14)) == "dynamic_object" || objName=="player" )
        dynamicObjName = objName.c_str();
    //else
    //    dynamicObjName = GlobalMap::getInstance()->getGlobal(objName.c_str()).c_str();

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(dynamicObjName.c_str());

	if (tempObj)
	{
		std::vector<DynamicObject*> list = tempObj->getLootItems();
		for (int a=0; a<list.size(); a++)
		{
			if (list[a]->getTemplateObjectName()==itemname)
			{
				//Unparent the object
				list[a]->getNode()->setParent(App::getInstance()->getDevice()->getSceneManager()->getRootSceneNode());
				list[a]->setLife(0); //kill the object

				if (!list[a]->getNode()->isVisible()) //If for some reason the object is still visible, make it invisible.
					list[a]->getNode()->setVisible(false);

				list.erase(list.begin()+a); //Remove from the list
				if (tempObj->getType()==DynamicObject::OBJECT_TYPE_PLAYER) //If the object is the player then update the list display in the GUI
					GUIManager::getInstance()->updateItemsList();

			}
		}
	}
	return 0;
}

int LuaGlobalCaller::setObjectVisible(lua_State *LS)
{
	int intresult=0;
	bool result = false;

	stringc objName="";
	stringc dynamicObjName = "";

	int top = lua_gettop(LS);
    if (top>1)
	{
		intresult = lua_toboolean(LS, -1);
		lua_pop(LS, 1);
		if (intresult>0)
			result = true;

		objName = lua_tostring(LS, -1);
		lua_pop(LS,1);
	} else
	{
		intresult = lua_toboolean(LS, -1);
		lua_pop(LS, 1);
		if (intresult>0)
			result = true;

		lua_getglobal(LS,"objName");
		objName = lua_tostring(LS, -1);

	}


	//if( stringc( objName.subString(0,14)) == "dynamic_object" || objName=="player" )
        dynamicObjName = objName.c_str();
    //else
    //    dynamicObjName = GlobalMap::getInstance()->getGlobal(objName.c_str()).c_str();

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(dynamicObjName.c_str());

	if (tempObj)
	{
		tempObj->getNode()->setVisible(result);
	}

	return 0;
}

int LuaGlobalCaller::isObjectVisible(lua_State *LS)
{

	bool result = false;
	stringc objName="";
	stringc dynamicObjName = "";

	int top = lua_gettop(LS);
    if (top>0)
	{
		objName = lua_tostring(LS, -1);
		lua_pop(LS,1);
	} else
	{
		lua_getglobal(LS,"objName");
		objName = lua_tostring(LS, -1);

	}

	//if( stringc( objName.subString(0,14)) == "dynamic_object" || objName=="player" )
        dynamicObjName = objName.c_str();
    //else
    //    dynamicObjName = GlobalMap::getInstance()->getGlobal(objName.c_str()).c_str();

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(dynamicObjName.c_str());


	if (tempObj)
	{
		result=tempObj->getNode()->isVisible();
	}


	lua_pushboolean(LS,result);

	return 1;

}

// Attach the current object to another object (reparent)
// attachObject(object name, bone name)
int LuaGlobalCaller::attachObject(lua_State *LS)
{
	bool result = false;
	stringc objName="";
	stringc calledObject="";
	stringc attachName="";
	stringc dynamicObjName = "";

	printf("Attach object was called");

	int top = lua_gettop(LS);
    if (top>1)
	{

		attachName = lua_tostring(LS, -1);
		lua_pop(LS,1);

		objName = lua_tostring(LS, -1);
		lua_pop(LS,1);

		lua_getglobal(LS,"objName");
		calledObject = lua_tostring(LS, -1);
	} else
	{
		GUIManager::getInstance()->setConsoleText(L"Attaching object: Wrong parameters!",SColor(255,220,0,0));
		return 0;
	}

	printf(" with 2 names: %s, %s\n",objName.c_str(),calledObject.c_str());

	DynamicObject* tempObj = NULL;
	//Check for the presence of the named object (player,etc)
	if (objName=="player")
		tempObj=Player::getInstance()->getObject();
	else
		tempObj=DynamicObjectsManager::getInstance()->getObjectByName(objName.c_str());

    
	
	DynamicObject* currObj = DynamicObjectsManager::getInstance()->getObjectByName(calledObject.c_str());

	if (tempObj && currObj) //Need to find both object so that attachment work.
	{
		//Will get the attachments from this object
		vector<DynamicObject::DynamicObject_attachment> attachments=tempObj->getAttachments(); 
		int value = attachments.size();

		for (int a=0; a<attachments.size(); a++)
		{

			if (attachments[a].name==(core::stringw)attachName)
			{
				//There already something attached to this attachment point!
				if (attachments[a].currentlyAttached)
				{
					attachments[a].currentlyAttached->getNode()->setVisible(false); //Hide it
					attachments[a].currentlyAttached->getNode()->setParent(tempObj->getNode());
					attachments[a].currentlyAttached->getNode()->setRotation(vector3df(0,0,0));
					attachments[a].currentlyAttached->getNode()->setPosition(vector3df(0,0,0));
					currObj->getNode()->setID(100); //Be clickable again 
					DynamicObjectsManager::getInstance()->removeProperties(attachments[a].currentlyAttached,tempObj); //Remove the added properties when not in use
					attachments[a].currentlyAttached=NULL; //is no longer attached
				}
				IBoneSceneNode* bone = ((IAnimatedMeshSceneNode*)tempObj->getNode())->getJointNode(core::stringc(attachments[a].bonename).c_str());
				if (bone)
				{
					currObj->getNode()->setParent(bone);
					currObj->getNode()->setPosition(attachments[a].attachpos);
					currObj->getNode()->setRotation(attachments[a].attachrot);
					currObj->getNode()->setVisible(true);
					currObj->getNode()->setID(0x0010); //Disable the ray cast so it's not selectable from the mouse pointer or collision
					DynamicObjectsManager::getInstance()->addProperties(currObj,tempObj); //Add the added properties when in use
					attachments[a].currentlyAttached = currObj;
				} else
				{
					GUIManager::getInstance()->setConsoleText(L"No attachment bone found!",SColor(255,220,0,0));
				}
			} else
			{
				GUIManager::getInstance()->setConsoleText(L"No attachment name found!",SColor(255,220,0,0));
			}
		}
	} 
	else
	{
		GUIManager::getInstance()->setConsoleText(L"Attaching object: One of the object was not found!",SColor(255,220,0,0));
	}
	return 0;


}


int LuaGlobalCaller::setGlobal(lua_State *LS)
{
    stringc data = lua_tostring(LS, -1);
	lua_pop(LS, 1);

    stringc varId = lua_tostring(LS, -1);
	lua_pop(LS, 1);

    GlobalMap::getInstance()->setGlobal(varId.c_str(),data.c_str());

    return 0;
}

int LuaGlobalCaller::getGlobal(lua_State *LS)
{
    char* text = (char*)lua_tostring(LS, -1);
	lua_pop(LS, 1);

    std::string value = GlobalMap::getInstance()->getGlobal(text);

	if(value != "IRR_RPG_BUILDER_GLOBAL_VAR_ERROR")
        lua_pushstring(LS,value.c_str());
    else
        lua_pushnil(LS);

    return 1;
}

int LuaGlobalCaller::deleteGlobal(lua_State *LS)
{
    char* text = (char*)lua_tostring(LS, -1);
	lua_pop(LS, 1);

    GlobalMap::getInstance()->deleteGlobal(text);

    return 0;
}

int LuaGlobalCaller::setTimeOfDay(lua_State *LS)
{
    int newTime = (int)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	EffectsManager::getInstance()->setTimeOfDay(newTime);

	return 0;
}

int LuaGlobalCaller::setAmbientLight(lua_State *LS)
{
    int r = 0;
    int g = 0;
    int b = 0;

    if(lua_isnumber(LS,-1))//read RGB
    {
        b = (int)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

        g = (int)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

        r = (int)lua_tonumber(LS, -1);
        lua_pop(LS, 1);
    }
    else if(lua_istable(LS,-1))
    {
        lua_pushstring(LS, "b");
        lua_gettable(LS,1);
        b = (int)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

        lua_pushstring(LS, "g");
        lua_gettable(LS,1);
        g = (int)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

        lua_pushstring(LS, "r");
        lua_gettable(LS,1);
        r = (int)lua_tonumber(LS, -1);
        lua_pop(LS, 1);
    }

    #ifdef APP_DEBUG
    printf("AMBIENT LIGHT (%d,%d,%d)\n",r,g,b);
    #endif
    App::getInstance()->getDevice()->getSceneManager()->setAmbientLight(SColorf(r/255.0f,g/255.0f,b/255.0f,1));

    return 0;
}

int LuaGlobalCaller::getAmbientLight(lua_State *LS)
{
    SColorf ambientColor = App::getInstance()->getDevice()->getSceneManager()->getAmbientLight();

    lua_pushnumber(LS,(int)(ambientColor.r*255) );
    lua_pushnumber(LS,(int)(ambientColor.g*255) );
    lua_pushnumber(LS,(int)(ambientColor.b*255) );

    return 3;
}

int LuaGlobalCaller::setFogColor(lua_State *LS)
{
    int r = 0;
    int g = 0;
    int b = 0;

    if(lua_isnumber(LS,-1))//read RGB
    {
        b = (int)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

        g = (int)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

        r = (int)lua_tonumber(LS, -1);
        lua_pop(LS, 1);
    }
    else if(lua_istable(LS,-1))
    {
        lua_pushstring(LS, "b");
        lua_gettable(LS,1);
        b = (int)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

        lua_pushstring(LS, "g");
        lua_gettable(LS,1);
        g = (int)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

        lua_pushstring(LS, "r");
        lua_gettable(LS,1);
        r = (int)lua_tonumber(LS, -1);
        lua_pop(LS, 1);
    }

    #ifdef APP_DEBUG
    printf("FOG COLOR (%d,%d,%d)\n",r,g,b);
    #endif

    SColor color;
    E_FOG_TYPE fogType;
    f32 start;
    f32 end;
    f32 density;
    bool pixelFog;
    bool rangeFog;

    //gets the current values
    App::getInstance()->getDevice()->getVideoDriver()->getFog(color,fogType,start,end,density,pixelFog,rangeFog);

    SColor newColor = SColor(255,r,g,b);

    //set new Color
    App::getInstance()->getDevice()->getVideoDriver()->setFog(newColor,fogType,start,end,density,pixelFog,rangeFog);

    return 0;
}

int LuaGlobalCaller::getFogColor(lua_State *LS)
{
    SColor color;
    E_FOG_TYPE fogType;
    f32 start;
    f32 end;
    f32 density;
    bool pixelFog;
    bool rangeFog;

    //gets the current values
    App::getInstance()->getDevice()->getVideoDriver()->getFog(color,fogType,start,end,density,pixelFog,rangeFog);

    lua_pushnumber(LS,color.getRed());
    lua_pushnumber(LS,color.getGreen());
    lua_pushnumber(LS,color.getBlue());

    return 3;
}

int LuaGlobalCaller::setFogRange(lua_State *LS)
{
    SColor color;
    E_FOG_TYPE fogType;
    int start;
    int end;
    f32 density;
    bool pixelFog;
    bool rangeFog;

    f32 oldStart;
    f32 oldEnd;

    //gets the current values
    App::getInstance()->getDevice()->getVideoDriver()->getFog(color,fogType,oldStart,oldEnd,density,pixelFog,rangeFog);

    end = (int)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	start = (int)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

    #ifdef APP_DEBUG
    printf("FOG RANGE (%d,%d)\n",start,end);
    #endif
    //set new Color
    App::getInstance()->getDevice()->getVideoDriver()->setFog(color,fogType,(f32)start,(f32)end,density,pixelFog,rangeFog);

    return 0;
}

int LuaGlobalCaller::getFogRange(lua_State *LS)
{
    SColor color;
    E_FOG_TYPE fogType;
    f32 start;
    f32 end;
    f32 density;
    bool pixelFog;
    bool rangeFog;

    //gets the current values
    App::getInstance()->getDevice()->getVideoDriver()->getFog(color,fogType,start,end,density,pixelFog,rangeFog);

    lua_pushnumber(LS,start);
    lua_pushnumber(LS,end);

    return 2;
}

int LuaGlobalCaller::setPostFX(lua_State *LS)
{
	stringc stringFX = lua_tostring(LS, -1);
	lua_pop(LS, 1);

	EffectsManager::getInstance()->SetPostFX(stringFX);

	return 0;
}

int LuaGlobalCaller::showBlackScreen(lua_State *LS)
{
    char* text = (char*)lua_tostring(LS, -1);
	lua_pop(LS, 1);

    #ifdef APP_DEBUG
    printf("BLACK SCREEN (%s)\n",text);
    #endif

    GUIManager::getInstance()->showBlackScreen(text);

    return 0;
}

int LuaGlobalCaller::hideBlackScreen(lua_State *LS)
{
    GUIManager::getInstance()->hideBlackScreen();
	return 0;
}

int LuaGlobalCaller::setSkydomeTexture(lua_State *LS)
{

	core::stringc textureFile = "";

    if(lua_isstring(LS, -1))
    {
        textureFile = lua_tostring(LS, -1);
        lua_pop(LS, 1);
    }

	EffectsManager::getInstance()->skydomeTexture(textureFile);

	return 0;
}

int LuaGlobalCaller::setSkydomeVisible(lua_State *LS)
{
	int result=0;
	if(lua_isboolean(LS, -1))
    {
        result = lua_toboolean(LS, -1);
        lua_pop(LS, 1);
    }

	bool bresult=false;

	if (result!=0)
		bresult=true;

	EffectsManager::getInstance()->skydomeVisible(bresult);
	//EffectsManager::getInstance()->turnOffSkydome();

	return 0;

}

int LuaGlobalCaller::setBackgroundColor(lua_State *LS)
{
	video::SColor color = SColor(0,0,0,0);

    if(lua_isnumber(LS, -1))//read (r,g,b)
    {
        u32 b = (u32)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

        u32 g = (u32)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

        u32 r = (u32)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

		color = video::SColor(0,r,g,b);
		App::getInstance()->setIngameBackgroundColor(color);

    }

	return 0;

}

int LuaGlobalCaller::setWeather(lua_State *LS)
{
    int maxParticles = 0;
    float particleSpeed = 1;
    stringc textureFile = "";

    if(lua_isstring(LS, -1))
    {
        textureFile = lua_tostring(LS, -1);
        lua_pop(LS, 1);
    }

    if(lua_isnumber(LS, -1))
    {
        particleSpeed = (float)lua_tonumber(LS, -1);
        lua_pop(LS, 1);
    }

    if(lua_isnumber(LS, -1))
    {
        maxParticles = (int)lua_tonumber(LS, -1);
        lua_pop(LS, 1);
    }

    EffectsManager::getInstance()->setWeather(maxParticles,particleSpeed*0.01f,textureFile);

	return 0;
}

int LuaGlobalCaller::setCameraTarget(lua_State *LS)
{
    vector3df otherPos = vector3df(0,0,0);

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
    else if(lua_isstring(LS,-1))//read Object position
    {
        std::string otherName = lua_tostring(LS, -1);
        lua_pop(LS, 1);

        DynamicObject* otherObj = DynamicObjectsManager::getInstance()->getObjectByName(GlobalMap::getInstance()->getGlobal(otherName.c_str()).c_str());

        if(otherObj)
        {
            otherPos = otherObj->getPosition();
        }
    }

	CameraSystem::getInstance()->getNode()->setTarget(otherPos);

    return 0;
}

int LuaGlobalCaller::getCameraTarget(lua_State *LS)
{
    vector3df pos = CameraSystem::getInstance()->getTarget();

    lua_pushnumber(LS,pos.X);
    lua_pushnumber(LS,pos.Y);
    lua_pushnumber(LS,pos.Z);

    return 3;
}

int LuaGlobalCaller::setCameraRange(lua_State *LS)
{
    f32 start;
    f32 end;

    end = (f32)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	start = (f32)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	CameraSystem::getInstance()->setGameCameraRange(start,end);
	printf("LUA Command setCameraRange() was called!\n near:%f, far:%f\n",start,end);
    return 0;
}

int LuaGlobalCaller::getCameraRange(lua_State *LS)
{

    f32 start;
    f32 end;

	end=CameraSystem::getInstance()->getGameCameraRange().Y;
	start=CameraSystem::getInstance()->getGameCameraRange().X;

    lua_pushnumber(LS,start);
    lua_pushnumber(LS,end);
	printf("LUA Command getCameraRange() was called!, near: %f, far: %f\n",start,end);

    return 2;
}

int LuaGlobalCaller::setCameraRTSRotation(lua_State *LS)
{
    f32 X;
    f32 Y;

    Y = (f32)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	X = (f32)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	CameraSystem::getInstance()->setPointNClickAngle(vector2df(X,Y));
	printf("LUA Command setCameraRTSRotation() was called!\n near:%f, far:%f\n",X,Y);
    return 0;
}

int LuaGlobalCaller::getCameraRTSRotation(lua_State *LS)
{

    f32 X;
    f32 Y;

	X=CameraSystem::getInstance()->getPointNClickAngle().X;
	Y=CameraSystem::getInstance()->getPointNClickAngle().Y;

    lua_pushnumber(LS,X);
    lua_pushnumber(LS,Y);
	printf("LUA Command getCameraRange() was called!, near: %f, far: %f\n",X,Y);

    return 2;
}


int LuaGlobalCaller::setCameraZoom(lua_State *LS)
{
    f32 zoom;

    zoom = (f32)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	CameraSystem::getInstance()->setCameraZoom(zoom);
    return 0;
}

int LuaGlobalCaller::getCameraZoom(lua_State *LS)
{

    f32 zoom;
	zoom=CameraSystem::getInstance()->getCameraZoom();
    lua_pushnumber(LS,zoom);

    return 1;
}

int LuaGlobalCaller::setCameraAngleLimit(lua_State *LS)
{
    f32 start;
    f32 end;

    end = (f32)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	start = (f32)lua_tonumber(LS, -1);
	lua_pop(LS, 1);

	CameraSystem::getInstance()->setGameCameraAngleLimit(vector2df(start,end));
	printf("LUA Command setCameraAngleLimit() was called!\n minimum:%f, maximum:%f\n",start,end);
    return 0;
}

int LuaGlobalCaller::getCameraAngleLimit(lua_State *LS)
{

    f32 start;
    f32 end;

	end=CameraSystem::getInstance()->getGameCameraAngleLimit().Y;
	start=CameraSystem::getInstance()->getGameCameraAngleLimit().X;

    lua_pushnumber(LS,start);
    lua_pushnumber(LS,end);
	printf("LUA Command getCameraAngleLimit() was called!, minimum: %f, maximum: %f\n",start,end);

    return 2;
}

int LuaGlobalCaller::setCameraPosition(lua_State *LS)
{
    vector3df otherPos = vector3df(0,0,0);

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
    else if(lua_isstring(LS,-1))//read Object position
    {
        std::string otherName = lua_tostring(LS, -1);
        lua_pop(LS, 1);

        DynamicObject* otherObj = DynamicObjectsManager::getInstance()->getObjectByName(GlobalMap::getInstance()->getGlobal(otherName.c_str()).c_str());

        if(otherObj)
        {
            otherPos = otherObj->getPosition();
        }
    }

    CameraSystem::getInstance()->setPosition(otherPos);

    return 0;
}

int LuaGlobalCaller::getCameraPosition(lua_State *LS)
{
	vector3df pos = CameraSystem::getInstance()->getPosition();

    lua_pushnumber(LS,pos.X);
    lua_pushnumber(LS,pos.Y);
    lua_pushnumber(LS,pos.Z);

    return 3;
}

int LuaGlobalCaller::cutsceneMode(lua_State *LS)
{
	//Hide all the GUI to present the cutscene
	CameraSystem::getInstance()->setCamera(CameraSystem::CAMERA_CUTSCENE);
	GUIManager::getInstance()->setElementVisible(GUIManager::IMG_BAR,false);
	GUIManager::getInstance()->setElementVisible(GUIManager::BT_ID_VIEW_ITEMS,false);
    return 0;
}

int LuaGlobalCaller::gameMode(lua_State *LS)
{
	//Display the game gui and select the game camera
	CameraSystem::getInstance()->setCamera(CameraSystem::CAMERA_GAME);
	GUIManager::getInstance()->setElementVisible(GUIManager::IMG_BAR,true);
	GUIManager::getInstance()->setElementVisible(GUIManager::BT_ID_VIEW_ITEMS,true);

    return 0;
}

int LuaGlobalCaller::setRTSView(lua_State *LS)
{
	CameraSystem::getInstance()->setViewType(CameraSystem::VIEW_RTS);
	return 0;
}

int LuaGlobalCaller::setRTSFixedView(lua_State *LS)
{
	CameraSystem::getInstance()->setViewType(CameraSystem::VIEW_RTS_FIXED);
	return 0;
}

int LuaGlobalCaller::setRPGView(lua_State *LS)
{
	CameraSystem::getInstance()->setViewType(CameraSystem::VIEW_RPG);
	return 0;
}

int LuaGlobalCaller::setFPSView(lua_State *LS)
{
	CameraSystem::getInstance()->setViewType(CameraSystem::VIEW_FPS);
	return 0;
}

int LuaGlobalCaller::defineKeys(lua_State *LS)
{
	core::stringc action="";
	core::stringc key="";

    if(lua_isstring(LS, -1))
    {
		action = (core::stringc)lua_tostring(LS, -1);
        lua_pop(LS, 1);

        key = (core::stringc)lua_tostring(LS, -1);
        lua_pop(LS, 1);
    }

	CameraSystem::getInstance()->defineKeys(key.make_upper(),action.make_upper());
	return 0;
}

int LuaGlobalCaller::setCameraAttachment(lua_State *LS)
{
	core::stringc bone="";
	 if(lua_isstring(LS, -1))
    {
		bone = (core::stringc)lua_tostring(LS, -1);
        lua_pop(LS, 1);
    }

	CameraSystem::getInstance()->setBoneName(bone);
	return 0;
}

int LuaGlobalCaller::setCameraOffset(lua_State *LS)
{
	vector3df position=vector3df(0,0,0);
	 if(lua_isnumber(LS, -1))
    {
		position.Z=(f32)lua_tonumber(LS, -1);
        lua_pop(LS, 1);
		position.Y=(f32)lua_tonumber(LS, -1);
        lua_pop(LS, 1);
		position.X=(f32)lua_tonumber(LS, -1);
        lua_pop(LS, 1);
    }
	 CameraSystem::getInstance()->setBoneOffset(position);
	return 0;
}

int LuaGlobalCaller::showCutsceneText(lua_State *LS)
{

	int result=0;
	if(lua_isboolean(LS, -1))
    {
        result = lua_toboolean(LS, -1);
        lua_pop(LS, 1);
    }

	bool bresult=false;
	if (result==1)
		bresult=true;

	GUIManager::getInstance()->showCutsceneText(bresult);

	return 0;
}

int LuaGlobalCaller::setCutsceneText(lua_State *LS)
{
	core::stringw text = "";

    if(lua_isstring(LS, -1))
    {
        text = lua_tostring(LS, -1);
        lua_pop(LS, 1);
    }

	GUIManager::getInstance()->setCutsceneText(text);
	return 0;
}

int LuaGlobalCaller::getObjectPosition(lua_State *LS)
{
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

	if(objName == "player")
	{
        vector3df pos = Player::getInstance()->getObject()->getPosition();

        lua_pushnumber(LS,pos.X);
        lua_pushnumber(LS,pos.Y);
        lua_pushnumber(LS,pos.Z);

        return 3;
	}


    stringc dynamicObjName = "";

    if( stringc( objName.subString(0,14)) == "dynamic_object" )
        dynamicObjName = objName.c_str();
    else
        dynamicObjName = GlobalMap::getInstance()->getGlobal(objName.c_str()).c_str();

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(dynamicObjName.c_str());

    if(tempObj)
    {
        vector3df pos = tempObj->getPosition();

        lua_pushnumber(LS,pos.X);
        lua_pushnumber(LS,pos.Y);
        lua_pushnumber(LS,pos.Z);

        return 3;
    }

    return 0;
}

int LuaGlobalCaller::setObjectRotation(lua_State *LS)
{

	float time = (float)lua_tonumber(LS, -1);
    lua_pop(LS, 1);

	float z2 = (float)lua_tonumber(LS, -1);
    lua_pop(LS, 1);
	float y2 = (float)lua_tonumber(LS, -1);
    lua_pop(LS, 1);
	float x2 = (float)lua_tonumber(LS, -1);
    lua_pop(LS, 1);

	float z1 = (float)lua_tonumber(LS, -1);
    lua_pop(LS, 1);
	float y1 = (float)lua_tonumber(LS, -1);
    lua_pop(LS, 1);
	float x1 = (float)lua_tonumber(LS, -1);
    lua_pop(LS, 1);

	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);


    stringc dynamicObjName = "";

    if( stringc( objName.subString(0,14)) == "dynamic_object" || "player")
        dynamicObjName = objName.c_str();
    else
        dynamicObjName = GlobalMap::getInstance()->getGlobal(objName.c_str()).c_str();

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(dynamicObjName.c_str());

    if(tempObj)
    {
		tempObj->rotateObject(vector3df((f32)x1,(f32)y1,(f32)z1),vector3df((f32)x2,(f32)y2,(f32)z2),(u32)time);

		return 0;
    }

    return 0;
}


int LuaGlobalCaller::playSound2D(lua_State *LS)
{
    stringc soundName = "../media/sound/";

    int LUAlooped = lua_toboolean(LS, -1);

	bool looped = false;
	if (LUAlooped==1)
		looped = true;

	lua_pop(LS, 1);

    soundName += (char*)lua_tostring(LS, -1);
	lua_pop(LS, 1);

    SoundManager::getInstance()->playSound2D(soundName.c_str(),looped);

    return 0;
}

int LuaGlobalCaller::playSound3D(lua_State *LS)
{
    stringc soundName = "../media/sound/";

    float z = (float)lua_tonumber(LS, -1);
    lua_pop(LS, 1);

    float y = (float)lua_tonumber(LS, -1);
    lua_pop(LS, 1);

    float x = (float)lua_tonumber(LS, -1);
    lua_pop(LS, 1);

    int LUAlooped = lua_toboolean(LS, -1);
	bool looped = false;
	if (LUAlooped==1)
		looped=true;
    lua_pop(LS, 1);

    soundName += (char*)lua_tostring(LS, -1);
	lua_pop(LS, 1);
    ///TODO: arrumar Range
    SoundManager::getInstance()->playSound3D(soundName.c_str(), vec3df(x,y,z), looped);

    return 0;
}

int LuaGlobalCaller::setListenerPosition(lua_State *LS)
{
    float z = (float)lua_tonumber(LS, -1);
    lua_pop(LS, 1);

    float y = (float)lua_tonumber(LS, -1);
    lua_pop(LS, 1);

    float x = (float)lua_tonumber(LS, -1);
    lua_pop(LS, 1);

    SoundManager::getInstance()->setListenerPosition(vec3df(x,y,z),vec3df(x,y,z));
	return 0;
}

int LuaGlobalCaller::setVolume(lua_State *LS)
{
    float volume = (float)lua_tonumber(LS, -1);
    lua_pop(LS, 1);

    SoundManager::getInstance()->setVolume(volume);
	return 0;
}

int LuaGlobalCaller::stopSounds(lua_State *LS)
{
    SoundManager::getInstance()->stopSounds();
	return 0;
}

int LuaGlobalCaller::setPlayerLife(lua_State *LS)
{
    int life = (int)lua_tonumber(LS, -1);
    lua_pop(LS, 1);

    Player::getInstance()->getObject()->setLife(life);

	return 0;
}

int LuaGlobalCaller::getPlayerLife(lua_State *LS)
{
    int life = Player::getInstance()->getObject()->getLife();

    lua_pushnumber(LS,life);

    return 1;
}

int LuaGlobalCaller::setObjectLife(lua_State *LS)
{
	//NEW This updated code can work with 1 or 2 parameter, not giving the object take the object it was called.
	//So would not need setLife() or setPlayerLife() anymore with this new way.
	//Will have to see why codeblock doesnt like this.
    // For an unknown reason, codeblock produce a illegal instruction here.
    #if defined(_MSC_VER)
	stringc objName = "";
	stringc dynamicObjName = "";

	int life = 100;
	int top = lua_gettop(LS);
    if (top>1)
	{
		life = (int)lua_tonumber(LS, -1);
		lua_pop(LS,1);
		objName = lua_tostring(LS, -1);
		lua_pop(LS,1);
	} else
	{
		life = (int)lua_tonumber(LS, -1);
		lua_pop(LS,1);
		lua_getglobal(LS,"objName");
		objName = lua_tostring(LS, -1);
	}


  //  if( stringc( objName.subString(0,14)) == "dynamic_object" || objName=="player" )
		dynamicObjName = objName.c_str();
//    else
//		dynamicObjName = GlobalMap::getInstance()->getGlobal(objName.c_str()).c_str();

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(dynamicObjName.c_str());

	core::stringc error="Object not found:";
	error.append(dynamicObjName);
	if (tempObj)
		tempObj->setLife(life);
	else
		App::getInstance()->getDevice()->getLogger()->log(error.c_str());

    #endif
	return 0;
}

int LuaGlobalCaller::getObjectLife(lua_State *LS)
{

	stringc objName="";
	stringc dynamicObjName = "";

	int top = lua_gettop(LS);
    if (top>0)
	{
		objName = lua_tostring(LS, -1);
		lua_pop(LS,1);
	} else
	{
		lua_getglobal(LS,"objName");
		objName = lua_tostring(LS, -1);

	}


	//if( stringc( objName.subString(0,14)) == "dynamic_object" || objName=="player" )
        dynamicObjName = objName.c_str();
    //else
    //    dynamicObjName = GlobalMap::getInstance()->getGlobal(objName.c_str()).c_str();

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(dynamicObjName.c_str());

    int life = 0;

	core::stringc error="Object not found:";
	error.append(dynamicObjName);

	if (tempObj)
		life = tempObj->getLife();
	else
		App::getInstance()->getDevice()->getLogger()->log(error.c_str());


    lua_pushnumber(LS,life);

    return 1;
}

int LuaGlobalCaller::setPlayerMoney(lua_State *LS)
{
    int money = (int)lua_tonumber(LS, -1);
    lua_pop(LS, 1);

    Player::getInstance()->getObject()->setMoney(money);
	return 0;
}

int LuaGlobalCaller::getPlayerMoney(lua_State *LS)
{
    int money = Player::getInstance()->getObject()->getMoney();

    lua_pushnumber(LS,money);

    return 1;
}

int LuaGlobalCaller::addPlayerItem(lua_State *LS)
{
    if(lua_isstring(LS,-1))
    {
        stringc item = lua_tostring(LS, -1);
        lua_pop(LS, 1);

        Player::getInstance()->getObject()->addItem(item);
    }
    else
    {
        #ifdef APP_DEBUG
        cout << "ERROR : LUA : ADD PLAYER ITEM @PARAM IS NULL!" << endl;
        #endif
    }
	return 0;
}

int LuaGlobalCaller::removePlayerItem(lua_State *LS)
{
    if(lua_isstring(LS,-1))
    {
        stringc item = lua_tostring(LS, -1);
        lua_pop(LS, 1);

        Player::getInstance()->getObject()->removeItem(item);
    }
    else
    {
        #ifdef APP_DEBUG
        cout << "ERROR : LUA : REMOVE PLAYER ITEM @PARAM IS NULL!" << endl;
        #endif
    }
	return 0;
}

int LuaGlobalCaller::inGameSave(lua_State *LS)
{
    bool result = false;

    if(lua_isstring(LS, -1))
    {
        stringc filename = lua_tostring(LS, -1);
        lua_pop(LS, 1);

        result = GlobalMap::getInstance()->saveGlobalsToXML(filename);
    }
    else
    {
        #ifdef APP_DEBUG
        cout << "ERROR : LUA : UNABLE TO SAVE GLOBALS @FILENAME IS NULL!" << endl;
        #endif
    }

    lua_pushboolean(LS,result);

    return 1;
}

int LuaGlobalCaller::inGameLoad(lua_State *LS)
{
    bool result = false;

    if(lua_isstring(LS, -1))
    {
        stringc filename = lua_tostring(LS, -1);
        lua_pop(LS, 1);

        result = GlobalMap::getInstance()->loadGlobalsFromXML(filename);
    }
    else
    {
        #ifdef APP_DEBUG
        cout << "ERROR : LUA : UNABLE TO LOAD GLOBALS @FILENAME IS NULL!" << endl;
        #endif
    }

    lua_pushboolean(LS,result);

    return 1;
}

int LuaGlobalCaller::loadMap(lua_State *LS)
{
bool result = false;

    if(lua_isstring(LS, -1))
    {
        stringc name = lua_tostring(LS, -1);
        lua_pop(LS, 1);

		stringc filename = "../projects/";
		filename += name;

		App::getInstance()->loadProjectGame(filename);
		/*//App::getInstance()->setAppState(APP_WAIT_DIALOG);
		App::getInstance()->stopGame();
		App::getInstance()->cleanWorkspace();
		result = App::getInstance()->loadProjectFromXML(filename);
		Player::getInstance()->getObject()->setAnimation("idle");
		if (result)
			App::getInstance()->playGame();*/
    }
    else
    {
        #ifdef APP_DEBUG
        cout << "ERROR : LUA : UNABLE TO LOAD MAP @FILENAME IS NULL!" << endl;
        #endif
    }

    //lua_pushboolean(LS,result);

    return 1;
}

int LuaGlobalCaller::getAnswer(lua_State *LS)
{
	lua_pop(LS, 1);
	lua_pushboolean(LS,LuaGlobalCaller::getInstance()->answer);
	return 1;
}

int LuaGlobalCaller::getLanguage(lua_State *LS)
{
	lua_pop(LS,1);
	lua_pushstring(LS,(char *)LANGManager::getInstance()->getLanguage().c_str());
	return 1;
}

int LuaGlobalCaller::spawn(lua_State *LS)
{

	core::vector3df pos = vector3df(0,0,0);
	core::vector3df angle = vector3df(0,0,0);

	if(lua_isnumber(LS, -1))//read (x,y,z)
    {
        float ry = (float)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

       	angle.Y=ry;

        float z = (float)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

        float y = (float)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

        float x = (float)lua_tonumber(LS, -1);
        lua_pop(LS, 1);

		pos.X=x; pos.Y=y; pos.Z=z;
    }

	core::stringc tempname = (core::stringc)lua_tostring(LS, -1);
    lua_pop(LS, 1);

	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

	irr::f32 maxRayHeight = 5000.0f;
	scene::ISceneCollisionManager* collMan = App::getInstance()->getDevice()->getSceneManager()->getSceneCollisionManager();
	core::line3d<f32> ray;

	// Start the ray 500 unit from the character, ray lenght is 1000 unit.
	ray.start = pos+vector3df(0,+(maxRayHeight/2.0f),0);
	ray.end = pos+vector3df(0,-(maxRayHeight/2),0);

	// Tracks the current intersection point with the level or a mesh
	core::vector3df intersection;
	// Used to show with triangle has been hit
	core::triangle3df hitTriangle;
	scene::ISceneNode * selectedSceneNode =
	collMan->getSceneNodeAndCollisionPointFromRay(
		ray,
		intersection,
		hitTriangle,
		100, //100 is the default ID for walkable (ground obj + props)
		0); // Check the entire scene (this is actually the implicit default)

	if (selectedSceneNode)
	{
		// return the height found.
		pos.Y=intersection.Y;
	}
	else
			// if not return -1000 (Impossible value, so it failed)
		pos.Y=0;


	DynamicObject* daloot = DynamicObjectsManager::getInstance()->createTemplateAt(tempname,pos);

	if (daloot) //Name is ok and will generate the object directly in this object loot
	{
		daloot->getNode()->setRotation(angle); // Reset the position
		daloot->isGenerated=true; //Tell IRB that this object was generated ingame.
	}

	if(daloot) //return a name if the object is created
    {
		core::stringc name = daloot->getName();

		lua_pushstring(LS,name.c_str());
    }

    return 1;
}

