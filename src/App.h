#ifndef APP_H
#define APP_H

#include <irrlicht.h>

#include <iostream>
#include <fstream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

static const float APP_VERSION = 1.0;

#define APP_DEBUG

//Current Application State
enum APP_STATE
{
    APP_EDIT_LOOK = 1,
    APP_EDIT_TERRAIN_TRANSFORM = 2,
    APP_EDIT_TERRAIN_SEGMENTS = 3,
    APP_EDIT_TERRAIN_PAINT_VEGETATION = 4,
    APP_EDIT_DYNAMIC_OBJECTS_MODE = 5,
    APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE = 6,
    APP_EDIT_CHARACTER = 7,

    APP_EDIT_DYNAMIC_OBJECTS_SCRIPT = 17,
    APP_EDIT_WAIT_GUI = 18,
    APP_EDIT_PLAYER_SCRIPT = 19,
    APP_EDIT_SCRIPT_GLOBAL = 20,
    APP_EDIT_ABOUT = 21,


    APP_STATE_CONTROL = 100,


    APP_GAMEPLAY_NORMAL = 101,
    APP_GAMEPLAY_VIEW_ITEMS = 102,
};

typedef struct
{
    vector3df pickedPos;
    ISceneNode* pickedNode;
}MousePick;

class App
{
    public:
        static App* getInstance();

        void draw2DImages();

        void eventGuiButton(s32 id);
        void eventGuiCheckbox(s32 id);
        void eventGuiCombobox(s32 id);

        void eventKeyPressed(s32 key);

        void eventMousePressed(s32 mouse);
        void eventMouseWheel(f32 value);

        IrrlichtDevice* getDevice();

        MousePick getMousePosition3D(int id = 0);

        void run();

		void setupDevice();

        void updateEditMode();
        void updateGameplay();

        void cleanWorkspace();
        void createNewProject();

        void loadProject();
        void loadProject(stringc filename);
        void saveProject();

        void initialize();

        void setAppState(APP_STATE newAppState);
        APP_STATE getAppState();

    private:

        App();
        ~App();

        bool cursorIsInEditArea();
		

        void saveProjectToXML();
        bool loadProjectFromXML(stringc filename);
		
        IrrlichtDevice *device;
        IVideoDriver* driver;
        ISceneManager* smgr;
        IGUIEnvironment* guienv;
		
		u32 timer;
		u32 timer2;
		int screenW,screenH,fullScreen;
		bool loadConfig();
		
        APP_STATE app_state;

        stringc scriptGlobal;

        stringc currentProjectName;

        MousePick lastMousePick;
};

#endif // APP_H
