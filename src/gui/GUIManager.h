#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <irrlicht.h>
#include <vector>
#include "../App.h"

//Should create a preview of the object (disabled for now, need a full rework)
#include "NodePreview.h"

//Custom edit box used for the script editor
#include "CGUIEditBoxIRB.h"

#include "GUIConfigWindow.h" //Configuration screen interface. (Will need to be reworked for better event handling)

// Stretching window class
#include "CGUIExtWindow.h"

// Split functions of the GUI manager for INGAME or EDITOR only stuff
#include "../Editor/GUIEditor.h"
#include "../Game/GUIGame.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

class GUIManager
{
	public:
	enum GUI_ID
	{
		BT_ID_TERRAIN_TRANSFORM = 2048,
		BT_ID_TERRAIN_ADD_SEGMENT,
		BT_ID_TERRAIN_ADD_CUSTOM_SEGMENT,

		BT_ID_TERRAIN_ADD_EMPTY_SEGMENT,

		SC_ID_TERRAIN_BRUSH_STRENGTH,
		SC_ID_TERRAIN_BRUSH_RADIUS,
		SC_ID_TERRAIN_BRUSH_RADIUS2,
		SC_ID_TERRAIN_BRUSH_PLATEAU,
		BT_ID_TERRAIN_PAINT_VEGETATION,
		BT_ID_SAVE_PROJECT,
		BT_ID_DYNAMIC_OBJECTS_MODE,
		BT_ID_DYNAMIC_OBJECTS_PROPS,
		BT_ID_DYNAMIC_OBJECTS_LOOT,
		CB_ID_DYNAMIC_OBJECT_HAS_ACTION,
		CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER,
		CO_ID_CUSTOM_SEGMENT_CATEGORY,
		CO_ID_DYNAMIC_OBJECT_OBJ_CATEGORY,
		BT_ID_DYNAMIC_OBJECT_BT_SPAWN,
		BT_ID_DYNAMIC_OBJECT_BT_REPLACE,
		BT_ID_DYNAMIC_OBJECT_BT_REPLACE2,
		BT_ID_DYNAMIC_OBJECT_BT_EDITSCRIPTS,
		BT_ID_DYNAMIC_OBJECT_BT_MOVEROTATE,
		BT_ID_DYNAMIC_OBJECT_BT_REMOVE,
		BT_ID_DYNAMIC_OBJECT_BT_CENTER,
		BT_ID_DYNAMIC_VIEW_BT_CENTER,
		BT_ID_DYNAMIC_OBJECT_BT_CANCEL,
		CO_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE,
		BT_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE,
		EB_ID_DYNAMIC_OBJECT_SCRIPT,
		BT_ID_LOAD_PROJECT,
		BT_ID_DYNAMIC_OBJECT_VALIDATE_SCRIPT,
		EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE,
		BT_ID_DYNAMIC_OBJECT_SCRIPT_CLOSE,
		BT_ID_DYNAMIC_OBJECT_INFO,
		BT_ID_PLAY_GAME,
		BT_ID_STOP_GAME,
		BT_ID_EDIT_CHARACTER,
		ST_ID_PLAYER_LIFE,
		BT_ID_PLAYER_EDIT_SCRIPT,
		BT_ID_CLOSE_PROGRAM,
		BT_ID_VIEW_ITEMS,
		BT_ID_USE_ITEM,
		BT_ID_CLOSE_ITEMS_WINDOW,
		LB_ID_PLAYER_ITEMS,
		BT_ID_DROP_ITEM,
		BT_ID_EDIT_SCRIPT_GLOBAL,
		BT_ID_ABOUT,
		BT_ID_ABOUT_WINDOW_CLOSE,
		ST_ID_PLAYER_MONEY,
		BT_ID_NEW_PROJECT,
		BT_ID_HELP,
		BT_ID_CONFIG,
		SC_ID_VEGETATION_BRUSH_STRENGTH,
		TXT_ID_DIALOG,
		TXT_ID_LOOT_DESCRIPTION,
		BT_ID_DIALOG_YES,
		BT_ID_DIALOG_CANCEL,
		BT_ID_DIALOG_NO,
		IMG_BAR,
		IMG_LOOT,
		IMG_LIFEGAUGE,
		IMG_MANAGAUGE,
		IMG_THUMBNAIL,
		CONSOLE,
		ST_ID_CUTSCENE_TEXT,
		LB_ID_LOGGER_MESSAGE,
		LB_ID_LOGGER_CONSOLE,

		TXT_ID_STATUS_CAMERA,
		TXT_ID_STATUS_TEXT,
		TXT_ID_SELOBJECT,
		TXT_ID_SELOBJECT_TYPE,
		TXT_ID_CUR_TEMPLATE,
		TXT_ID_OBJ_SCRIPT,
		TXT_ID_LOADER,
		TXT_ID_TERRAIN_BRUSH_STRENGTH,
		TXT_ID_TERRAIN_RADIUS,
		TXT_ID_TERRAIN_RADIUS1,
		TXT_ID_TERRAIN_PLATEAU,
		TXT_ID_MDL_NAME,
		TXT_ID_MDL_DESC,
		TXT_ID_MDL_AUTH,
		TXT_ID_MDL_LIC,


		CO_ID_DYNAMIC_OBJECT_OBJLIST_CATEGORY,
		CO_ID_CUSTOM_TILES_OBJLIST_CATEGORY,
		CO_ID_CUSTOM_SEGMENT_OBJ_CHOOSER,
		CO_ID_ACTIVE_SCENE_LIST,
		CO_ID_ACTIVE_LIST_FILTER,

		BT_ID_TILE_ROT_LEFT,
		BT_ID_TILE_ROT_RIGHT,
		BT_ID_DO_ADD_MODE,
		BT_ID_DO_SEL_MODE,
		BT_ID_DO_MOV_MODE,
		BT_ID_DO_ROT_MODE,
		BT_ID_DO_SCA_MODE,

		BT_CAMERA_RTS,
		BT_CAMERA_RPG,
		BT_CAMERA_FPS,

		CB_ID_POS_X,
		CB_ID_POS_Y,
		CB_ID_POS_Z,
		CB_ID_ROT_X,
		CB_ID_ROT_Y,
		CB_ID_ROT_Z,
		CB_ID_SCA_X,
		CB_ID_SCA_Y,
		CB_ID_SCA_Z,

		TI_ID_POS_X,
		TI_ID_POS_Y,
		TI_ID_POS_Z,
		TI_ID_ROT_X,
		TI_ID_ROT_Y,
		TI_ID_ROT_Z,
		TI_ID_SCA_X,
		TI_ID_SCA_Y,
		TI_ID_SCA_Z,

		CB_SCREENCOMBO,
		CB_SNAPCOMBO,
		BT_PLAYER_CONFIG,
		BT_PLAYER_START,
		WIN_GAMEPLAY,
		WIN_LOADER,

		VEGE_CHECKBOX,
		VEGE_LISTBOX,
		VEGE_IMAGE,
		ID_FADER,
		ID_TABCONTROL_MAIN
	};

	//here are all windows of the editor (except mainWindow - toolbar)
	enum GUI_CUSTOM_WINDOW //Need to start as a hight number as the GUI id should not conflict with the other items
	{
		GCW_DYNAMIC_OBJECT_CHOOSER = 10500,
		GCW_DYNAMIC_OBJECT_CHOOSER_INNER,
		GCW_DYNAMIC_OBJECT_CHOOSER_INNER1,
		GCW_DYNAMIC_OBJECT_CHOOSER_INNER2,
		GCW_DYNAMIC_OBJECT_CHOOSER_INNER3,
		GCW_DYNAMIC_PLAYER_EDIT,
		GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,
		GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU1,
		GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT,
		GCW_TERRAIN_TOOLBAR,
		GCW_VEGE_TOOLBAR,
		GCW_GAMEPLAY_ITEMS,
		GCW_ABOUT,
		GCW_TERRAIN_PAINT_VEGETATION,
		GCW_DIALOG,
		GCW_CONSOLE,
		GCW_DYNAMIC_OBJECT_INFO,  // this is a pane not a window
		GCW_CUSTOM_SEGMENT_CHOOSER,
		GCW_VIEW_MENU,
		GCW_TOP_WINDOW,
		GCW_STATUSBAR
	};

	enum GUI_HELP_IMAGE
	{
		HELP_TERRAIN_TRANSFORM = 1,
		HELP_IRR_RPG_BUILDER_1 = 2,
		HELP_VEGETATION_PAINT = 3,
		HELP_TERRAIN_SEGMENTS = 4,
	};

	enum GUI_MSG_TYPE
	{
		GUI_MSG_TYPE_ERROR = 1,
	};

	enum FONT_NAME
	{
		FONT_ARIAL = 1,
		FONT_LARGE,
	};

	enum LIST_TYPE
	{
		LIST_NPC = 1,
		LIST_PROP,
		LIST_SEGMENT,
		LIST_LOOT,
		LIST_ALL,
	};

        static GUIManager* getInstance();

		inline IGUIEnvironment* getGuiEnv() { return guienv;}
		inline IVideoDriver* getDriver() { return driver;}

		void setupGameplayGUI();

		void showDialogMessage(stringw text, std::string sound);
		bool showDialogQuestion(stringw text, std::string sound );
		core::stringc showInputQuestion(stringw text);

     	void UpdateCollections(LIST_TYPE type = LIST_NPC);
		void UpdateGUIChooser(LIST_TYPE type = LIST_NPC);
		void updateCurrentCategory(LIST_TYPE type = LIST_NPC);
		void buildSceneObjectList(DynamicObject::TYPE objtype = DynamicObject::OBJECT_TYPE_NONE);

		core::stringw getEditCameraString(ISceneNode* node);
		void updateEditCameraString(scene::ISceneNode * node); 

		void setTextLoader(stringw text);
		IGUIFont* getFont(FONT_NAME fontName);
		void showCutsceneText(bool visible);
		void setCutsceneText(core::stringw text);

    	void drawPlayerStats();
		
		bool isGuiChildPresent(gui::IGUIElement* elem, vector2d<s32> mousepos);
		void createConsole();
		IGUIElement* getGUIElement(u32 id);

		void update();

#ifdef EDITOR
		void setupEditorGUI();
		void setEditBoxText(GUI_ID id, stringw text);
		void expandPanel(GUI_CUSTOM_WINDOW id);
		void contractPanel(GUI_ID id);
#endif
		void drawHelpImage(GUI_HELP_IMAGE img);
        bool getCheckboxState(GUI_ID id);
		void setCheckboxState(GUI_ID id, bool value);
        f32 getScrollBarValue(GUI_ID id);
        stringc getComboBoxItem(GUI_ID id);
		IGUIListBox* getListBox(GUI_ID id);

		void getInfoAboutModel(LIST_TYPE type = LIST_NPC);

		void updateGuiPositions(dimension2d<u32> screensize);

        stringc getEditBoxText(GUI_ID id);

 		void setConsoleText (stringw text, video::SColor color = video::SColor(255,0,0,0));
		void clearConsole();
		void setConsoleLogger(vector<core::stringw> &text);

        void setElementEnabled(GUI_ID id, bool enable);
        void setElementVisible(GUI_ID id, bool visible);

        void setWindowVisible(GUI_CUSTOM_WINDOW window, bool visible);
		bool isWindowVisible(GUI_CUSTOM_WINDOW window);

        void updateDynamicObjectPreview();

        void showMessage(GUI_MSG_TYPE msgType, stringc msg);

        void showBlackScreen(stringc text);
        void hideBlackScreen();

		void stopDialogSound();
        

        //returns the active item on Player Items Window
        stringc getActivePlayerItem();

		//Return the id of the selected item in the inventory
		DynamicObject* getActiveLootItem();


        //Update the list of items (player inventory)
		void updateItemsList();

		// Update the infos of the node when the user move the mouse (call coming from APP class)
		void updateNodeInfos(ISceneNode * node);

		rect<s32> myRect(s32 x, s32 y, s32 w, s32 h);

        void flush();
		void loadFonts();

        void showConfigWindow();

		inline void setStatusText(core::stringw text) {IGUIStaticText* guiStatusText=(IGUIStaticText*)getGUIElement(TXT_ID_STATUS_TEXT); if (guiStatusText) {guiStatusText->setText(text.c_str());}}
		// Accessing the loader window directly
		IGUIWindow* guiLoaderWindow;

		IGUIFont* guiFontC12;//arial 10
        IGUIFont* guiFontCourier12;
		IGUIFont* guiFontCourier11;
		IGUIFont* guiFontCourier10;
        IGUIFont* guiFontLarge28;//arial 28
        IGUIFont* guiFontDialog;
		IGUIFont* guiFont6;
		IGUIFont* guiFont8;
		IGUIFont* guiFont9;
		IGUIFont* guiFont10;
		IGUIFont* guiFont12;
		IGUIFont* guiFont14;

	    
    private:
		 GUIManager();
		 ~GUIManager();
		
		IrrlichtDevice *device;
        IGUIEnvironment* guienv;
		IVideoDriver* driver;
		dimension2d<u32> screensize;

        int mouseX;
        int mouseY;

		u32 timer3; //Timer for the Context menu disabling

		// Used to store text events in a buffer while the GUI is not displayed
		std::vector<core::stringw> textevent;
		std::vector<video::SColor> texteventcolor;

		GUIConfigWindow* configWindow;

       
		s32 displayheight;
		s32 displaywidth;


};

#endif // GUIMANAGER_H
