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

#include "raytests.h"
#include "objects/DynamicObject.h"

//Current Application State

class App
{
    public:

		typedef struct
		{
			vector3df pickedPos;
			ISceneNode* pickedNode;
		}MousePick;


		enum APP_STATE
		{
			APP_EDIT_LOOK = 1,
			APP_EDIT_TERRAIN_TRANSFORM = 2,
			APP_EDIT_TERRAIN_SEGMENTS = 3,
			APP_EDIT_TERRAIN_EMPTY_SEGMENTS = 4,
			APP_EDIT_TERRAIN_CUSTOM_SEGMENTS = 5,

			APP_EDIT_TERRAIN_PAINT_VEGETATION = 6,

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
			APP_WAIT_FILEREQUEST = 99,
	
		};

		// States for IRB TOOLS
		enum APP_TOOLSTATE
		{
			TOOL_NONE = 1024,
			TOOL_TILEROTATE_LEFT,
			TOOL_TILEROTATE_RIGHT,
			TOOL_DO_ADD,
			TOOL_DO_SEL,
			TOOL_DO_MOV,
			TOOL_DO_ROT,
			TOOL_DO_SCA

		};

		enum DIALOG_FUNCTION
		{
			DF_PROJECT = 1,
			DF_MODEL = 2,
		};

		#ifdef EDITOR

		void eventGuiCheckbox(s32 id);
        void eventGuiCombobox(s32 id);
		void eventGuiEditBox(s32 id);
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
		bool isKeyPressed(int key);

		void eventGuiSpinbox(s32 id);

        void eventMousePressed(s32 mouse);
        void eventMouseWheel(f32 value);

		void eventMessagebox(gui::EGUI_EVENT_TYPE type);
		void openItemsPanel();

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
        void loadProject(DIALOG_FUNCTION function = DF_PROJECT);
        void loadProject(stringc filename);
		void loadProjectFile(bool value);

		void loadProjectGame(stringc filename); //This is called from a Lua SCRIPT and reset and load a new game.
        void saveProjectDialog();
		stringc getProjectName();

        void initialize();
		bool cursorIsInEditArea();
		


        void setAppState(APP_STATE newAppState);
        APP_STATE getAppState();

		void saveProjectToXML(stringc filename);
        bool loadProjectFromXML(stringc filename);
		bool loadConfig();

		std::vector<stringw> getConsoleText();
		std::vector<SColor> getConsoleColor();
		
		void clearConsole();

		//temporary made public to determine if the config is fullscreen
		bool fullScreen;

		//For the background color ingame
		inline void setIngameBackgroundColor(const video::SColor color){ingamebackground=color;}

		// used for the logger
		std::vector<core::stringw> textevent;
		std::vector<core::stringw> console_event;
		std::vector<video::SColor> console_event_color;

		// This check in the GUI manager to get the radius of the Brush in the GUI;
		irr::f32 getBrushRadius(int number = 0);

		// Snapping function
		core::vector3df calculateSnap(vector3df input, f32 snapvalue);

		scene::ISceneNode* selectedNode;
		scene::ISceneNode* scriptNode;
		MousePick lastMousePick;

		//Multiple selections
		std::vector<DynamicObject*> selectedSet;

		inline void setComboBoxUsed(bool set) { combobox_used = set;}
		
    private:

        App();
        ~App();

		raytest* raytester;

		//ConsoleDialog * console_dialog;

        
		void setPreviewSelection();

		vector3df oldcampos;
		vector3df oldcamtar;
	
        IrrlichtDevice *device;
        IVideoDriver* driver;
        ISceneManager* smgr;
        IGUIEnvironment* guienv;

		video::SColor background;
		video::SColor ingamebackground;
		
		u32 timer;
		u32 timer1;
		u32 timer2;
		u32 timer3;
		u32 timer4;
		u32 timer_lua; //Used when a script ask for a level change
		bool levelchange; //used to check if a new level change is needed (Loading a new project or level)
		core::stringc levelfilename;
		
		dimension2d<u32> screensize;

		//temporary made public
		//bool fullScreen;
		bool resizable;
		bool vsync;
		bool antialias;
		bool overdraw; // This will tell if we need to overdraw the player
		bool silouette; // This will determine if we need to activate the player silouette rendering when occluded

		bool keytoggled; // Toggle check for a combo key press
		
		stringc language;
		
		
        APP_STATE app_state;
		APP_STATE old_state;

		APP_TOOLSTATE toolstate;
		APP_TOOLSTATE old_do_state;

		DynamicObject::TYPE current_listfilter; //Last filter for the object list

		bool toolactivated; //This determine if the tool is activated.

        stringc scriptGlobal;

        stringc currentProjectName;

		stringc lastPickedNodeName;

		stringc lastFilename;

// Used to load the startup map when the game start (only the player app)
#ifndef EDITOR
		stringc mapname;
#endif
      
		MousePick lastScannedPick;

		CGUIFileSelector * selector;
		CGUIFileSelector * saveselector;

		position2d<s32> mousepos;
		vector3df initialposition;
		vector3df initialrotation;
		vector3df initialscale;

		// Tools substates
		bool moveupdown; // Moving and object up/down
		bool snapfunction; // Snapping activated
		
		video::ITexture * tex_occluded;
		video::ITexture * tex_normal;

		DIALOG_FUNCTION df;

		bool combobox_used;
		f32 currentsnapping;

};

#endif // APP_H
