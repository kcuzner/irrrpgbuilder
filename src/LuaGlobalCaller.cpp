#include "LuaGlobalCaller.h"

#include "GUIManager.h"
#include "LANGManager.h"
#include <time.h>
#include "DynamicObjectsManager.h"
#include "CameraSystem.h"
#include "fx/EffectsManager.h"
#include "SoundManager.h"
#include "Player.h"

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

        DynamicObjectsManager::getInstance()->clearCollisions();

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


            //load player items
            TiXmlNode* playerItemXML = playerXML->FirstChild( "item" );

            while(playerItemXML)
            {
                Player::getInstance()->getObject()->addItem(playerItemXML->ToElement()->Attribute("name"));
                playerItemXML = playerXML->IterateChildren( "item", playerItemXML );
            }
        }

        //DynamicObjectsManager::getInstance()->initializeCollisions();

        return true;
    }
    else
        return false;
}





LuaGlobalCaller::LuaGlobalCaller()
{
    // create an Lua pointer instance
    L = lua_open();

    // load the libs
    luaL_openlibs(L);

    this->registerBasicFunctions(L);

    //globalMap = new GlobalMap();
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

    lua_register(LS,"setCameraTarget",setCameraTarget);//setCameraTarget(x,y,z)    or    setCameraTarget(objName)
    lua_register(LS,"getCameraTarget",getCameraTarget);//x,y,z = getCameraTarget()

    lua_register(LS,"getObjectPosition",getObjectPosition);//x,y,z getObjectPosition(objName)

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
    lua_register(LS,"getItemCount",getItemCount);

	// NPC functions
	lua_register(LS,"setObjectLife",setObjectLife);
    lua_register(LS,"getObjectLife",getObjectLife);

    //Dialog Functions
    lua_register(LS,"showDialogMessage",showDialogMessage);
   
    //inGame Save/Load
    lua_register(LS,"saveGame",inGameSave);
    lua_register(LS,"loadGame",inGameLoad);

	lua_register(LS,"getAnswer",getAnswer);
	lua_register(LS,"getLanguage",getLanguage);

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
    int error = luaL_dostring(L,script.c_str());
// Only available in the editor
#ifdef EDITOR
    if(error)
        GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE,lua_tostring(L, -1));
    else
	{
		GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE,
			LANGManager::getInstance()->getText("bt_dynamic_objects_script_noerror").c_str());
	}
#endif
}


void LuaGlobalCaller::storeGlobalParams()
{
    lua_getglobal(L,"IRBStoreGlobalParams");
    if(lua_isfunction(L, -1)) lua_pcall(L,0,0,0);
    lua_pop( L, -1 );
}

void LuaGlobalCaller::restoreGlobalParams()
{
    lua_getglobal(L,"IRBRestoreGlobalParams");
    if(lua_isfunction(L, -1)) lua_pcall(L,0,0,0);
    lua_pop( L, -1 );
}

void LuaGlobalCaller::usePlayerItem(stringc item)
{
    lua_getglobal(L,item.c_str());
    if(lua_isfunction(L,-1))
    {
        lua_pcall(L,0,0,0);
    }
    else
    {
        lua_pop(L,-1);
    }
}

void LuaGlobalCaller::setAnswer(bool answer)
{
	this->answer=answer;
}

int LuaGlobalCaller::getItemCount(lua_State *LS)
{
    stringc itemName = "";

    if(lua_isstring(LS,-1))
    {
        itemName = lua_tostring(LS,-1);
        lua_pop(LS,1);
    }

    lua_pushnumber(LS,Player::getInstance()->getObject()->getItemCount(itemName));

    return 1;
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

    CameraSystem::getInstance()->setPosition(otherPos);

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
    // For an unknown reason, codeblock produce a illegal instruction here.
    #if defined(_MSC_VER)
	stringc objName = "";
	int life = 100;
	int top = lua_gettop(LS);
    if (top==2)
	{
		life = (int)lua_tonumber(LS, -1);
		lua_pop(LS,1);
		objName = lua_tostring(LS, -1);
		lua_pop(LS,1);
	}

    printf ("Life for object %s is changed to %i\n",objName,life);
    stringc dynamicObjName = "";
    if( stringc( objName.subString(0,14)) == "dynamic_object" )
		dynamicObjName = objName.c_str();
    else
		dynamicObjName = GlobalMap::getInstance()->getGlobal(objName.c_str()).c_str();

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(dynamicObjName.c_str());
	printf("Here is the current life for object %s: %d\n",objName.c_str(),life);
    tempObj->setLife(life);

    #endif
	return 0;
}

int LuaGlobalCaller::getObjectLife(lua_State *LS)
{
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);
	stringc dynamicObjName = "";
	if( stringc( objName.subString(0,14)) == "dynamic_object" )
        dynamicObjName = objName.c_str();
    else
        dynamicObjName = GlobalMap::getInstance()->getGlobal(objName.c_str()).c_str();

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(dynamicObjName.c_str());

    int life = tempObj->getLife();

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


int LuaGlobalCaller::showDialogMessage(lua_State *LS)
{
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