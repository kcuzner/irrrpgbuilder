#ifndef LUAGLOBALCALLER_H
#define LUAGLOBALCALLER_H
#include <irrlicht.h>
#include <iostream>
#include <map>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

class GlobalMap
{
    public:
        void setGlobal(std::string var, std::string data);
        std::string getGlobal(std::string var);
        void deleteGlobal(std::string var);

        void clearGlobals();

        bool saveGlobalsToXML(stringc filename);
        bool loadGlobalsFromXML(stringc filename);

        static GlobalMap* getInstance();

    private:
        GlobalMap();

        typedef std::map<std::string, std::string> StringMap;
        typedef std::pair<std::string, std::string> StringMapPair;

        StringMap stringMap;
};

class LuaGlobalCaller
{
    public:
        static LuaGlobalCaller* getInstance();
        virtual ~LuaGlobalCaller();

        void registerBasicFunctions(lua_State *LS);

        void doScript(stringc script);

        void storeGlobalParams();
        void restoreGlobalParams();

        void useGlobalFunction(stringc function);

		void setAnswer(bool answer);

        static int setGlobal(lua_State *LS);//setGlobal(varName,value)
        static int getGlobal(lua_State *LS);//getGlobal(varName)
        static int deleteGlobal(lua_State *LS);//getGlobal(varName)

        static int setTimeOfDay(lua_State *LS);//setTimeOfDay(time) -> (0-23h)
        static int setAmbientLight(lua_State *LS);//setAmbientLight(r,g,b) -> (0-255) or setAmbientLight(COLOR_NAME)
        static int getAmbientLight(lua_State *LS);// r,g,b = getAmbientLight() -> returns 0-255 values
        static int setFogRange(lua_State *LS);//setFogRange(start,end)
        static int getFogRange(lua_State *LS);//start,end = getFogRange()
        static int setFogColor(lua_State *LS);//setFogColor(r,g,b)   or  or setFogColor(COLOR_NAME)
        static int getFogColor(lua_State *LS);//r,g,b = getFogColor() -> return 0-255 values
		static int setPostFX(lua_State *LS); // Define a PostProcess FX
        static int showBlackScreen(lua_State *LS);//showBlackScreen(optional_text)
        static int hideBlackScreen(lua_State *LS);//showBlackScreen()

		static int setSkydomeTexture(lua_State *LS); // Change the texture of the skydome
		static int setSkydomeVisible(lua_State *LS); // Set the visibility of the skydome
		static int setBackgroundColor(lua_State *LS); // Set the background rendering color

        static int setWeather(lua_State *LS);//rain and snow

		static int setCameraTarget(lua_State *LS);
        static int getCameraTarget(lua_State *LS);
		static int setCameraRange(lua_State *LS);
        static int getCameraRange(lua_State *LS);
		static int setCameraRTSRotation(lua_State *LS);
        static int getCameraRTSRotation(lua_State *LS);
		static int setCameraZoom(lua_State *LS);
        static int getCameraZoom(lua_State *LS);
		static int setCameraAngleLimit(lua_State *LS);
        static int getCameraAngleLimit(lua_State *LS);
        static int setCameraPosition(lua_State *LS);
        static int getCameraPosition(lua_State *LS);
		static int cutsceneMode(lua_State *LS);
		static int gameMode(lua_State *LS);
		static int setRTSView(lua_State *LS);
		static int setRTSFixedView(lua_State *LS);
		static int setRPGView(lua_State *LS);
		static int setFPSView(lua_State *LS);
		static int defineKeys(lua_State *LS);
		static int setCameraAttachment(lua_State *LS);
		static int setCameraOffset(lua_State *LS);

		static int showCutsceneText(lua_State *LS);
		static int setCutsceneText(lua_State *LS);


        static int getObjectPosition(lua_State *LS);//x,y,z = getObjectPosition(objName)
		static int setObjectRotation(lua_State *LS);//setObjectRotation(objectname,from,to,time);

        //Sound Functions
        static int playSound2D(lua_State *LS);
        static int playSound3D(lua_State *LS);
        static int setListenerPosition(lua_State *LS);
        static int setVolume(lua_State *LS);
        static int stopSounds(lua_State *LS);

        //Player Functions
        static int setPlayerLife(lua_State *LS);
        static int getPlayerLife(lua_State *LS);

		static int setObjectLife(lua_State *LS);
        static int getObjectLife(lua_State *LS);

        static int setPlayerMoney(lua_State *LS);
        static int getPlayerMoney(lua_State *LS);

        static int addPlayerItem(lua_State *LS); //Will need to be removed. Old commands
        static int removePlayerItem(lua_State *LS);
        static int getItemCount(lua_State *LS);
		static int useGlobalFunctionLUA(lua_State *LS);

		static int addObjectLoot(lua_State *LS);
		static int setObjectProperty(lua_State *LS);
		static int getObjectProperty(lua_State *LS);

		static int checkObjectItem(lua_State *LS);
		static int destroyObjectItem(lua_State *LS);
		static int setObjectVisible(lua_State *LS);
		static int isObjectVisible(lua_State *LS);

        //Save/Load inGame
        static int inGameSave(lua_State *LS);
        static int inGameLoad(lua_State *LS);
		static int loadMap(lua_State *LS);

		static int getAnswer(lua_State *LS);
		static int getLanguage(lua_State *LS);

		// Spawn a template on the ground
		// Parameters are Name, X,Y,Z, Rotation (Y)
		static int spawn(lua_State *LS); // Spawn a template somewhere


    protected:
    private:
        LuaGlobalCaller();
        lua_State *LS;
		bool answer;

		
};

#endif // LUAGLOBALCALLER_H
