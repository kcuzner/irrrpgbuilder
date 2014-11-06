#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <irrlicht.h>
#include "../App.h"
#include <vector>

#include "NodePreview.h"
#include "CGUIEditBoxIRB.h"
#include "CGUIGfxStatus.h"
#include "GUIConfigWindow.h"
#include "../sound/SoundManager.h"

// Stretching window class
#include "CGUIExtWindow.h"

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
		CONSOLE,

		TXT_ID_SELOBJECT,
		TXT_ID_SELOBJECT_TYPE,
		TXT_ID_CUR_TEMPLATE,
		TXT_ID_OBJ_SCRIPT,

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
		WIN_LOADER
	};

	//here are all windows of the editor (except mainWindow - toolbar)
	enum GUI_CUSTOM_WINDOW
	{
		GCW_DYNAMIC_OBJECT_CHOOSER = 1,
		GCW_DYNAMIC_PLAYER_EDIT,
		GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,
		GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU1,
		GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT,
		GCW_TERRAIN_TOOLBAR,
		GCW_GAMEPLAY_ITEMS,
		GCW_ABOUT,
		GCW_TERRAIN_PAINT_VEGETATION,
		GCW_DIALOG,
		GCW_CONSOLE,
		GCW_DYNAMIC_OBJECT_INFO,  // this is a pane not a window
		GCW_CUSTOM_SEGMENT_CHOOSER,
		GCW_VIEW_MENU
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


        void setupGameplayGUI();
		void UpdateCollections(LIST_TYPE type = LIST_NPC);
		void UpdateGUIChooser(LIST_TYPE type = LIST_NPC);
		void updateCurrentCategory(LIST_TYPE type = LIST_NPC);
		void buildSceneObjectList(DynamicObject::TYPE objtype = DynamicObject::OBJECT_TYPE_NONE);

		void setTextLoader(stringw text);
		IGUIFont* getFont(FONT_NAME fontName);

    	void drawPlayerStats();
		bool isGuiPresent(vector2d<s32> mousepos);
		bool isGuiChildPresent(gui::IGUIElement* elem, vector2d<s32> mousepos);
		void createConsole();

		void update();

#ifdef EDITOR
		void setupEditorGUI();
		void createProjectTab();
		void createPlayTab();
		void createEnvironmentTab();
		void createObjectTab();
		void createMainToolbar();
		void createMainTabs();

		void createDisplayOptionsGUI();
		void createAboutWindowGUI();
		void createTerrainToolbar();
		void createDynamicObjectChooserGUI();
		void createCustomSegmentChooserGUI();
		void createContextMenuGUI();
		void createCodeEditorGUI();
		void setEditBoxText(GUI_ID id, stringw text);
#endif
		void drawHelpImage(GUI_HELP_IMAGE img);
        bool getCheckboxState(GUI_ID id);
		void setCheckboxState(GUI_ID id, bool value);
        f32 getScrollBarValue(GUI_ID id);
        stringc getComboBoxItem(GUI_ID id);
		IGUIListBox* getListBox(GUI_ID id);

		bool getVisibleStatus(s32 ID);
		void getInfoAboutModel(LIST_TYPE type = LIST_NPC);

		void updateGuiPositions(dimension2d<u32> screensize);

        stringc getEditBoxText(GUI_ID id);

        void setStaticTextText(GUI_ID id, stringc text);
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
        //Shows a simple Text Dialog message. Hit RETURN to close-it
        void showDialogMessage(stringw text, std::string sound = "" );

        //Shows a question message, use LEFT and RIGHT arrows to choose YES or NO and hit RETURN
        bool showDialogQuestion(stringw text, std::string sound = "");

        //returns the active item on Player Items Window
        stringc getActivePlayerItem();

		//Return the id of the selected item in the inventory
		DynamicObject* getActiveLootItem();

        //Shows a input message, input the text and hit RETURN
        stringc showInputQuestion(stringw text);

        //Update the list of items (player inventory)
		void updateItemsList();

		// Update the infos of the node when the user move the mouse (call coming from APP class)
		void updateNodeInfos(ISceneNode * node);

		core::stringw getEditCameraString(scene::ISceneNode *node);
		inline void updateEditCameraString(scene::ISceneNode * node) {if (guiStatusCameraText){guiStatusCameraText->setText(getEditCameraString(node).c_str());}}

		inline void setCutsceneText(core::stringw text) { guiCutsceneText->setText(text.c_str());}
		inline void showCutsceneText(bool visible) { guiCutsceneText->setVisible(visible);}
        void flush();

        void showConfigWindow();

		inline void setStatusText(core::stringw text) {if (guiStatusText) {guiStatusText->setText(text.c_str());}}
		// Accessing the loader window directly
		IGUIWindow* guiLoaderWindow;

	    virtual ~GUIManager();
    private:
		IrrlichtDevice *device;
        IGUIEnvironment* guienv;
		IVideoDriver* driver;
		dimension2d<u32> screensize;

        int mouseX;
        int mouseY;

		u32 timer;
		u32 timer2;
		u32 timer3; //Timer for the Context menu disabling

		// Used to store text events in a buffer while the GUI is not displayed
		std::vector<core::stringw> textevent;
		std::vector<video::SColor> texteventcolor;

		// used for sounds in dialogs;
		ISound* dialogSound;


        position2di mainToolbarPos;
		IGUITabControl * mainTabCtrl;
		IGUITabControl * mainToolCtrl;
		IGUITabControl * prjTabCtrl;
		IGUITabControl * gameTabCtrl;

		IGUITab * tabProject;
		IGUITab * tabPlayTool;
		IGUITab * tabEnv;
		IGUITab * tabObject;
		IGUITab * tabTools;
		IGUITab * tabConfig;

        IGUIFont* guiFontC12;//arial 10
        IGUIFont* guiFontCourier12;
        IGUIFont* guiFontLarge28;//arial 28
        IGUIFont* guiFontDialog;
		IGUIFont* guiFont6;
		IGUIFont* guiFont8;
		IGUIFont* guiFont9;
		IGUIFont* guiFont10;
		IGUIFont* guiFont12;
		IGUIFont* guiFont14;

        ///Main Functions
		IGUIImage*  guiBackImage;
		IGUIImage*  guiBackImage2;
        IGUIButton* guiMainNewProject;
        IGUIButton* guiMainSaveProject;
        IGUIButton* guiMainLoadProject;
        IGUIButton* guiPlayGame;
        IGUIButton* guiStopGame;
        IGUIButton* guiEditCharacter;
        IGUIButton* guiEditScriptGlobal;

        IGUIButton* guiAbout;
        IGUIWindow* guiAboutWindow;
        IGUIButton* guiAboutClose;
        IGUIButton* guiCloseProgram;
		IGUIListBox* guiAboutText;

        IGUIButton* guiHelpButton;
        IGUIButton* guiConfigButton;
        GUIConfigWindow* configWindow;
		IGUIWindow* guiStatus;
		IGUIStaticText* guiStatusText;
		IGUIStaticText* guiStatusCameraText;
		IGUIStaticText* guiCutsceneText;

		IGUIListBox* console;
		IGUIListBox* consolelog;
		CGUIExtWindow* consolewin;//IGUIWindow * consolewin;

		// Loader window
		IGUIStaticText* guiLoaderDescription;

        ///Main window (like a toolbar on the top...)
        IGUIWindow* guiMainWindow;
		IGUIWindow* guiMainToolWindow;

        ///TerrainEditor
        IGUIButton* guiTerrainTransform;
		IGUIButton* guiTerrainAddCustomSegment;
        IGUIButton* guiTerrainAddSegment;
		IGUIButton* guiTerrainAddEmptySegment;
        ITexture* helpTerrainTransform;
        ITexture* helpTerrainSegments;


        ///Terrain Toolbar
        IGUIWindow* guiTerrainToolbar;
        IGUIScrollBar* guiTerrainBrushStrength;
        IGUIStaticText* guiTerrainBrushStrengthLabel;
		IGUIScrollBar* guiTerrainBrushPlateau;
		IGUIStaticText* guiTerrainBrushPlateauValue;
		IGUIScrollBar* guiTerrainBrushRadius;
		IGUIScrollBar* guiTerrainBrushRadius2;
        IGUIStaticText* guiTerrainBrushRadiusLabel;
		IGUIStaticText* guiTerrainBrushStrengthValue;
        IGUIStaticText* guiTerrainBrushRadiusValue;
		IGUIStaticText* guiTerrainBrushRadiusValue2;

		CGUIExtWindow* guiCustomSegmentWindowChooser;

        IGUIButton* guiTerrainPaintVegetation;
        ITexture* helpVegetationPaint;

        ///Vegetation Toolbar
        IGUIWindow* guiVegetationToolbar;
        IGUIScrollBar* guiVegetationBrushStrength;
        IGUIStaticText* guiVegetationBrushStrengthLabel;

        ///Dynamic Objects
        IGUIButton* guiDynamicObjectsMode;
		IGUIButton* guiDynamicObjectsProps;
		IGUIButton* guiDynamicObjectsLoot;

        //IGUIWindow* guiDynamicObjectsWindowChooser;
		CGUIExtWindow* guiDynamicObjectsWindowChooser;
		CGUIExtWindow* guiDynamicPlayerWindowChooser;
		gui::IGUIWindow* InnerChooser; // For the inner part of the chooser (add)
		gui::IGUIWindow* InnerChooser1; // For the inner part of the chooser (select)
		gui::IGUIWindow* InnerChooser2; // For the inner part of the chooser (move/rotate/scale)
		gui::IGUIWindow* InnerChooser3; // For the inner part of the chooser (select->Object lists)


		IGUIWindow* guiDynamicObjectsWindowInfo;
        CGUIExtWindow* guiDynamicObjectsWindowEditAction; //IGUIWindow* guiDynamicObjectsWindowEditAction;

		IGUIComboBox * screencombo; //Screen settings combo box
		IGUIComboBox * snappingcombo; //snapping distance combo box

		IGUIToolBar * guiDynamicObjectEditModesPanel;
		IGUIButton* guiDOAddMode;
		IGUIButton* guiDOSelMode;
		IGUIButton* guiDOMovMode;
		IGUIButton* guiDORotMode;
		IGUIButton* guiDOScaMode;

        IGUIListBox* guiDynamicObjects_OBJChooser;
		IGUIListBox* guiDynamicObjects_OBJCategory;
		IGUIComboBox* guiDynamicObjects_Category;
        NodePreview* guiDynamicObjects_NodePreview;
		NodePreview* guiPlayerNodePreview;
		IGUIButton* guiDynamicObjectsInfo;

		// For the Custom tiles selection
		IGUIListBox* guiCustom_Segment_OBJChooser;
		IGUIListBox* guiCustom_Segment_OBJCategory;
		IGUIListBox* guiSceneObjectList;
		IGUIComboBox* guiCustom_Segment_Category;
		IGUIButton* guiSegmentRotateLeft;
		IGUIButton* guiSegmentRotateRight;

        IGUIComboBox* guiDynamicObjects_LoadScriptTemplateCB;
        IGUIButton* guiDynamicObjects_LoadScriptTemplateBT;

        IGUIWindow* guiDynamicObjects_Context_Menu_Window;
		IGUIWindow* guiDynamicObjects_Context_Menu_Window1;
        IGUIButton* guiDynamicObjects_Context_btEditScript;
        IGUIButton* guiDynamicObjects_Context_btMoveRotate;
        IGUIButton* guiDynamicObjects_Context_btRemove;
        IGUIButton* guiDynamicObjects_Context_btCancel;
		IGUIButton* guiDynamicObjects_Context_btSpawn;
		IGUIButton* guiDynamicObjects_Context_btReplace;
		IGUIButton* guiDynamicObjects_Context_btReplace2;

        CGUIEditBoxIRB* guiDynamicObjects_Script;
        IGUIEditBox* guiDynamicObjects_Script_Console;
        IGUIButton* guiDynamicObjects_Script_Close;


		// Information bar
		IGUIStaticText * mdl_name;
		IGUIStaticText * mdl_desc;
		IGUIStaticText * mdl_auth;
		IGUIStaticText * mdl_lic;

		// Information bar custom tiles
		IGUIStaticText * mdl_name1;
		IGUIStaticText * mdl_desc1;
		IGUIStaticText * mdl_auth1;
		IGUIStaticText * mdl_lic1;

		//Move, Rotate, Scale text input + lock
		IGUISpinBox * pos_x_text;
		IGUISpinBox * pos_y_text;
		IGUISpinBox * pos_z_text;
		IGUICheckBox * pos_x_lock;
		IGUICheckBox * pos_y_lock;
		IGUICheckBox * pos_z_lock;

		IGUISpinBox * rot_x_text;
		IGUISpinBox * rot_y_text;
		IGUISpinBox * rot_z_text;
		IGUICheckBox * rot_x_lock;
		IGUICheckBox * rot_y_lock;
		IGUICheckBox * rot_z_lock;

		IGUISpinBox * sca_x_text;
		IGUISpinBox * sca_y_text;
		IGUISpinBox * sca_z_text;
		IGUICheckBox * sca_x_lock;
		IGUICheckBox * sca_y_lock;
		IGUICheckBox * sca_z_lock;

		// Items list
		IGUIComboBox* guiDynamicObjects_listfilter;

        ///IrrRPG Builder LOGO
        ITexture* logo1;

        ///Player
        IGUIButton* guiPlayerEditScript;

		// Gameplay bar
		IGUIImage* gameplay_bar_image;
		CGUIGfxStatus * lifegauge;
		CGUIGfxStatus * managauge;


        ///GAMEPLAY
		stringc playerLifeText;

        IGUIInOutFader* fader;
        IGUIStaticText* guiPlayerLife;
        IGUIStaticText* guiPlayerLife_Shadow;
        IGUIStaticText* guiPlayerMoney;

		ITexture* guiDialogImgYes;
        ITexture* guiDialogImgYes_s;
        ITexture* guiDialogImgNo;
        ITexture* guiDialogImgNo_s;

        IGUIButton* guiBtViewItems;
        IGUIWindow* guiWindowItems;

        IGUIListBox* guiPlayerItems;
		IGUIImage*  guiPlayerLootImage;

        IGUIButton* guiBtUseItem;
        IGUIButton* guiBtDropItem;
        IGUIButton* guiBtCloseItemsWindow;

		IGUIWindow* guidialog;
		IGUIStaticText* txt_dialog;
		IGUIButton* guiBtDialogYes;
		IGUIButton* guiBtDialogCancel;

		IGUIButton* guiBtGamePlay;
		IGUIButton* guiBtGameConfig;

		// Textures
		ITexture* backtexture;
		ITexture* imgNewProject;
		ITexture* imgNewProject1;
		ITexture* imgLoadProject;
		ITexture* imgLoadProject1;
		ITexture* imgSaveProject;
		ITexture* imgSaveProject1;
		ITexture* imgCloseProgram;
		ITexture* imgAbout;
		ITexture* imgAbout1;
		ITexture* imgHelp;
		ITexture* imgHelp1;
		ITexture* imgConfig;
		ITexture* imgConfig1;

		// Default textures for the info panel
		ITexture* info_none; // no texture in the template
		ITexture* info_current; // current assigned texture
		gui::IGUIImage * thumbnail; // GUI item that will have the content

		ITexture* info_current1; // current assigned texture inside the Custom Segments
		gui::IGUIImage * thumbnail1; // GUI item that will have the content inside Custom Segments


        GUIManager();
        rect<s32> myRect(s32 x, s32 y, s32 w, s32 h);
		s32 displayheight;
		s32 displaywidth;

		int currentObjType;

        void loadFonts();

        void loadScriptTemplates();




};

#endif // GUIMANAGER_H
