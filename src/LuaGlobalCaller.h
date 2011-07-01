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

        void usePlayerItem(stringc item);

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

        static int setWeather(lua_State *LS);//rain and snow

        static int setCameraTarget(lua_State *LS);
        static int getCameraTarget(lua_State *LS);

        static int getObjectPosition(lua_State *LS);//x,y,z = getObjectPosition(objName)

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

        static int addPlayerItem(lua_State *LS);
        static int removePlayerItem(lua_State *LS);
        static int getItemCount(lua_State *LS);

        //Save/Load inGame
        static int inGameSave(lua_State *LS);
        static int inGameLoad(lua_State *LS);
		static int loadMap(lua_State *LS);

		static int getAnswer(lua_State *LS);
		static int getLanguage(lua_State *LS);

    protected:
    private:
        LuaGlobalCaller();
        lua_State *L;
		bool answer;
};

#endif // LUAGLOBALCALLER_H
