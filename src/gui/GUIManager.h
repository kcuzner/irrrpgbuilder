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

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

enum GUI_ID
{
    BT_ID_TERRAIN_TRANSFORM = 1,
    BT_ID_TERRAIN_ADD_SEGMENT = 2,
    CB_ID_TERRAIN_SHOW_PLAYABLE_AREA = 3,
    SC_ID_TERRAIN_BRUSH_STRENGTH = 5,
	SC_ID_TERRAIN_BRUSH_RADIUS = 6,
	SC_ID_TERRAIN_BRUSH_PLATEAU = 7,
    BT_ID_TERRAIN_PAINT_VEGETATION = 10,
    BT_ID_SAVE_PROJECT = 15,
    BT_ID_DYNAMIC_OBJECTS_MODE = 16,
    CB_ID_DYNAMIC_OBJECT_HAS_ACTION = 17,
    CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER = 18,
    BT_ID_DYNAMIC_OBJECT_BT_EDITSCRIPTS = 19,
    BT_ID_DYNAMIC_OBJECT_BT_MOVEROTATE= 20,
    BT_ID_DYNAMIC_OBJECT_BT_REMOVE= 21,
    BT_ID_DYNAMIC_OBJECT_BT_CANCEL= 22,
    CO_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE = 23,
    BT_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE = 24,
    EB_ID_DYNAMIC_OBJECT_SCRIPT = 25,
    BT_ID_LOAD_PROJECT = 26,
    BT_ID_DYNAMIC_OBJECT_VALIDATE_SCRIPT = 27,
    EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE = 28,
    BT_ID_DYNAMIC_OBJECT_SCRIPT_CLOSE = 29,
    BT_ID_PLAY_GAME = 30,
    BT_ID_STOP_GAME = 31,
    BT_ID_EDIT_CHARACTER = 32,
    ST_ID_PLAYER_LIFE = 33,
    BT_ID_PLAYER_EDIT_SCRIPT = 34,
    BT_ID_CLOSE_PROGRAM = 35,
    BT_ID_VIEW_ITEMS = 36,
    BT_ID_USE_ITEM = 37,
    BT_ID_CLOSE_ITEMS_WINDOW = 38,
    LB_ID_PLAYER_ITEMS = 39,
    BT_ID_DROP_ITEM = 40,
    BT_ID_EDIT_SCRIPT_GLOBAL = 41,
    BT_ID_ABOUT = 42,
    BT_ID_ABOUT_WINDOW_CLOSE = 43,
    ST_ID_PLAYER_MONEY = 44,
    BT_ID_NEW_PROJECT = 45,
    BT_ID_HELP = 46,
    BT_ID_CONFIG = 47,
    SC_ID_VEGETATION_BRUSH_STRENGTH = 48,
	TXT_ID_DIALOG = 49,
	BT_ID_DIALOG_YES = 50,
	BT_ID_DIALOG_CANCEL = 51,
	BT_ID_DIALOG_NO = 52,
	IMG_BAR = 53,
	CONSOLE = 54,
	CO_ID_DYNAMIC_OBJECT_OBJ_CATEGORY
};

//here are all windows of the editor (except mainWindow - toolbar)
enum GUI_CUSTOM_WINDOW
{
    GCW_DYNAMIC_OBJECT_CHOOSER = 1,
    GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU = 2,
    GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT = 3,
    GCW_TERRAIN_TOOLBAR = 4,
    GCW_GAMEPLAY_ITEMS = 5,
    GCW_ABOUT = 6,
    GCW_TERRAIN_PAINT_VEGETATION = 7,
	GCW_DIALOG = 8,
	GCW_CONSOLE = 9
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
};

class GUIManager
{
    public:
        static GUIManager* getInstance();


        void setupGameplayGUI();
		void UpdateGUIChooser(int objType);
		void setTextLoader(stringw text);
		IGUIFont* getFont(FONT_NAME fontName);

    	void drawPlayerStats();
		bool isGuiPresent(vector2d<s32> mousepos);

		#ifdef EDITOR
		void setupEditorGUI();
		void drawHelpImage(GUI_HELP_IMAGE img);
        bool getCheckboxState(GUI_ID id);
        f32 getScrollBarValue(GUI_ID id);
        stringc getComboBoxItem(GUI_ID id);
		void setEditBoxText(GUI_ID id, stringw text);

		#endif


		void updateGuiPositions(dimension2d<u32> screensize);

        stringc getEditBoxText(GUI_ID id);

        void setStaticTextText(GUI_ID id, stringc text);
		void setConsoleText (stringw text, video::SColor color);
		void clearConsole();
		void setConsoleLogger(vector<core::stringw> &text);

        void setElementEnabled(GUI_ID id, bool enable);
        void setElementVisible(GUI_ID id, bool visible);

        void setWindowVisible(GUI_CUSTOM_WINDOW window, bool visible);

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

        //Shows a input message, input the text and hit RETURN
        stringc showInputQuestion(stringw text);

        void updateItemsList();

        void flush();

        void showConfigWindow();
		// Accessing the loader window directly
		IGUIWindow* guiLoaderWindow;

	    virtual ~GUIManager();
    private:
        IGUIEnvironment* guienv;
		dimension2d<u32> screensize;

        int mouseX;
        int mouseY;

		u32 timer;
		u32 timer2;
		
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

		IGUIListBox* console;
		IGUIWindow * consolewin;

		// Loader window

		IGUIStaticText* guiLoaderDescription;


        ///Main window (like a toolbar on the top...)
        IGUIWindow* guiMainWindow;
		IGUIWindow* guiMainToolWindow;

        ///TerrainEditor
        IGUIButton* guiTerrainTransform;
        IGUIButton* guiTerrainAddSegment;
        ITexture* helpTerrainTransform;
        ITexture* helpTerrainSegments;


        ///Terrain Toolbar
        IGUIWindow* guiTerrainToolbar;
        IGUICheckBox* guiTerrainShowPlayableArea;
        IGUIScrollBar* guiTerrainBrushStrength;
        IGUIStaticText* guiTerrainBrushStrengthLabel;
		IGUIScrollBar* guiTerrainBrushPlateau;
		IGUIStaticText* guiTerrainBrushPlateauValue;
		IGUIScrollBar* guiTerrainBrushRadius;
        IGUIStaticText* guiTerrainBrushRadiusLabel;
		IGUIStaticText* guiTerrainBrushStrengthValue;
        IGUIStaticText* guiTerrainBrushRadiusValue;

        IGUIButton* guiTerrainPaintVegetation;
        ITexture* helpVegetationPaint;

        ///Vegetation Toolbar
        IGUIWindow* guiVegetationToolbar;
        IGUIScrollBar* guiVegetationBrushStrength;
        IGUIStaticText* guiVegetationBrushStrengthLabel;

        ///Dynamic Objects
        IGUIButton* guiDynamicObjectsMode;
        IGUIWindow* guiDynamicObjectsWindowChooser;
        IGUIWindow* guiDynamicObjectsWindowEditAction;

        IGUIComboBox* guiDynamicObjects_OBJChooser;
		IGUIComboBox* guiDynamicObjects_Category;
        NodePreview* guiDynamicObjects_NodePreview;
		NodePreview* guiPlayerNodePreview;

        IGUIComboBox* guiDynamicObjects_LoadScriptTemplateCB;
        IGUIButton* guiDynamicObjects_LoadScriptTemplateBT;

        IGUIWindow* guiDynamicObjects_Context_Menu_Window;
        IGUIButton* guiDynamicObjects_Context_btEditScript;
        IGUIButton* guiDynamicObjects_Context_btMoveRotate;
        IGUIButton* guiDynamicObjects_Context_btRemove;
        IGUIButton* guiDynamicObjects_Context_btCancel;

        CGUIEditBoxIRB* guiDynamicObjects_Script;
        IGUIEditBox* guiDynamicObjects_Script_Console;
        IGUIButton* guiDynamicObjects_Script_Close;

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

        IGUIButton* guiBtUseItem;
        IGUIButton* guiBtDropItem;
        IGUIButton* guiBtCloseItemsWindow;

		IGUIWindow* guidialog;
		IGUIStaticText* txt_dialog;
		IGUIButton* guiBtDialogYes;
		IGUIButton* guiBtDialogCancel;


        GUIManager();
        rect<s32> myRect(s32 x, s32 y, s32 w, s32 h);
		s32 displayheight;
		s32 displaywidth;

        void loadFonts();

        void loadScriptTemplates();

		
};

#endif // GUIMANAGER_H
