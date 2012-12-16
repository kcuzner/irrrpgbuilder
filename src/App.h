#ifndef APP_H
#define APP_H



#include <irrlicht.h>
#include <vector>
#include <iostream>
#include <fstream>

#include "gui/CGUIFileSelector.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

static const float APP_VERSION = 1.0;

//#define APP_DEBUG

#include "LuaGlobalCaller.h"
#include "tinyXML/tinyxml.h"
#include "XML_Manager.h"

// try to force the class to be remembered for the compiler
class CIrrFrame;

//Current Application State
enum APP_STATE
{
    APP_EDIT_LOOK = 1,
    APP_EDIT_TERRAIN_TRANSFORM = 2,
    APP_EDIT_TERRAIN_SEGMENTS = 3,
	APP_EDIT_TERRAIN_EMPTY_SEGMENTS = 4,
    APP_EDIT_TERRAIN_PAINT_VEGETATION = 5,
    APP_EDIT_DYNAMIC_OBJECTS_MODE = 20,
    APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE = 21,
    APP_EDIT_CHARACTER = 30,

    APP_EDIT_DYNAMIC_OBJECTS_SCRIPT = 35,
    APP_EDIT_WAIT_GUI = 40,
    APP_EDIT_PLAYER_SCRIPT = 36,
    APP_EDIT_SCRIPT_GLOBAL = 37,
    APP_EDIT_ABOUT = 41,
	APP_EDIT_VIEWDRAG = 42,

    APP_STATE_CONTROL = 100,

    APP_GAMEPLAY_NORMAL = 101,
    APP_GAMEPLAY_VIEW_ITEMS = 102,
	APP_WAIT_DIALOG = 103,
	APP_WAIT_DIALOGQUESTION = 104,
	APP_WAIT_FILEREQUEST = 105,
	
};

typedef struct
{
	vector3df pickedPos;
	ISceneNode* pickedNode;
}MousePick;

class App
{
    public:

		#ifdef EDITOR

		void eventGuiCheckbox(s32 id);
        void eventGuiCombobox(s32 id);
		void updateEditMode();
		void hideEditGui();
		std::vector<core::stringw> getAbout();
		
		#endif

		void setScreenSize(dimension2d<u32> size);
		dimension2d<u32> getScreenSize();

        static App* getInstance();
		
		void displayGuiConsole();
        void draw2DImages();
	
        void eventGuiButton(s32 id);
        void eventKeyPressed(s32 key);

        void eventMousePressed(s32 mouse);
        void eventMouseWheel(f32 value);

        IrrlichtDevice* getDevice();
        MousePick getMousePosition3D(int id = 0);
		
		void playGame();
		void stopGame();

		void run();
		void update();
		void quickUpdate();
		void shutdown();

		stringw getLangText(stringc node);
		void setupDevice(IrrlichtDevice* IRRdevice);
	    void updateGameplay();
        void cleanWorkspace();

		void createNewProject();
        void loadProject();
        void loadProject(stringc filename);
		void loadProjectFile(bool value);
        void saveProject();
		stringc getProjectName();

        void initialize();
		


        void setAppState(APP_STATE newAppState);
        APP_STATE getAppState();

		void saveProjectToXML(stringc filename);
        bool loadProjectFromXML(stringc filename);
		bool loadConfig();

		std::vector<stringw> getConsoleText();
		std::vector<SColor> getConsoleColor();
		
		void clearConsole();

		bool wxSystemState;

		//temporary made public to determine if the config is fullscreen
		bool fullScreen;

		// used for the logger
		std::vector<core::stringw> textevent;
		std::vector<core::stringw> console_event;
		std::vector<video::SColor> console_event_color;

		// This check in the GUI manager to get the radius of the Brush in the GUI;
		irr::f32 getBrushRadius();

		scene::ISceneNode* selectedNode;
		MousePick lastMousePick;
		
    private:

        App();
        ~App();

		//ConsoleDialog * console_dialog;

        bool cursorIsInEditArea();
		void setPreviewSelection();

		vector3df oldcampos;
		vector3df oldcamtar;
	
        IrrlichtDevice *device;
        IVideoDriver* driver;
        ISceneManager* smgr;
        IGUIEnvironment* guienv;
		
		u32 timer;
		u32 timer2;
		u32 timer3;
		
		dimension2d<u32> screensize;

		//temporary made public
		//bool fullScreen;
		bool resizable;
		
		stringc language;
		
		
        APP_STATE app_state;
		APP_STATE old_state;

        stringc scriptGlobal;

        stringc currentProjectName;

		stringc lastPickedNodeName;

// Used to load the startup map when the game start (only the player app)
#ifndef EDITOR
		stringc mapname;
#endif
      
		MousePick lastScannedPick;

		CGUIFileSelector * selector;
		CGUIFileSelector * saveselector;

		bool initRotation;
		bool lockcam;
		vector2df oldmouse;
};

#endif // APP_H
