#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <irrlicht.h>
#include "App.h"
#include <vector>

#include "NodePreview.h"

#include "CGUIEditBoxIRB.h"

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
    BT_ID_TERRAIN_PAINT_VEGETATION = 6,
    BT_ID_SAVE_PROJECT = 7,
    BT_ID_DYNAMIC_OBJECTS_MODE = 8,
    CB_ID_DYNAMIC_OBJECT_HAS_ACTION = 9,
    CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER = 10,
    BT_ID_DYNAMIC_OBJECT_BT_EDITSCRIPTS = 12,
    BT_ID_DYNAMIC_OBJECT_BT_MOVEROTATE= 13,
    BT_ID_DYNAMIC_OBJECT_BT_REMOVE= 14,
    BT_ID_DYNAMIC_OBJECT_BT_CANCEL= 15,
    CO_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE = 16,
    BT_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE = 17,
    EB_ID_DYNAMIC_OBJECT_SCRIPT = 18,
    BT_ID_LOAD_PROJECT = 19,
    BT_ID_DYNAMIC_OBJECT_VALIDATE_SCRIPT = 20,
    EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE = 21,
    BT_ID_DYNAMIC_OBJECT_SCRIPT_CLOSE = 22,
    BT_ID_PLAY_GAME = 23,
    BT_ID_STOP_GAME = 24,
    BT_ID_EDIT_CHARACTER = 25,
    ST_ID_PLAYER_LIFE = 26,
    BT_ID_PLAYER_EDIT_SCRIPT = 27,
    BT_ID_CLOSE_PROGRAM = 28,
    BT_ID_VIEW_ITEMS = 29,
    BT_ID_USE_ITEM = 30,
    BT_ID_CLOSE_ITEMS_WINDOW = 31,
    LB_ID_PLAYER_ITEMS = 32,
    BT_ID_DROP_ITEM = 33,
    BT_ID_EDIT_SCRIPT_GLOBAL = 34,
    BT_ID_ABOUT = 35,
    BT_ID_ABOUT_WINDOW_CLOSE = 36,
    ST_ID_PLAYER_MONEY = 37,
    BT_ID_NEW_PROJECT = 38,
    BT_ID_HELP = 39,
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

        void setupEditorGUI();
        void setupGameplayGUI();

        void drawHelpImage(GUI_HELP_IMAGE img);
        void drawNodePreview();

        bool getCheckboxState(GUI_ID id);
        f32 getScrollBarValue(GUI_ID id);
        stringc getComboBoxItem(GUI_ID id);

        IGUIFont* getFont(FONT_NAME fontName);

        void setEditBoxText(GUI_ID id, stringc text);
        stringc getEditBoxText(GUI_ID id);

        void setStaticTextText(GUI_ID id, stringc text);

        void setElementEnabled(GUI_ID id, bool enable);
        void setElementVisible(GUI_ID id, bool visible);

        void setWindowVisible(GUI_CUSTOM_WINDOW window, bool visible);

        void updateDynamicObjectPreview();

        void showMessage(GUI_MSG_TYPE msgType, stringc msg);

        void showBlackScreen(stringc text);
        void hideBlackScreen();

        //Shows a simple Text Dialog message. Hit RETURN to close-it
        void showDialogMessage(std::string text, std::string sound = "" );

        //Shows a question message, use LEFT and RIGHT arrows to choose YES or NO and hit RETURN
        bool showDialogQuestion(std::string text, std::string sound = "");

        //returns the active item on Player Items Window
        stringc getActivePlayerItem();

        //Shows a input message, input the text and hit RETURN
        stringc showInputQuestion(std::string text);

        void updateItemsList();

        void flush();

        virtual ~GUIManager();
    private:
        IGUIEnvironment* guienv;

        int mouseX;
        int mouseY;

		u32 timer;
		u32 timer2;

        position2di mainToolbarPos;

        IGUIFont* guiFontC12;//arial 10
        IGUIFont* guiFontCourier12;
        IGUIFont* guiFontLarge28;//arial 28
        IGUIFont* guiFontDialog;

        ///Main Functions
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

        IGUIButton* guiHelpButton;

        ///Main window (like a toolbar on the top...)
        IGUIWindow* guiMainWindow;

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

        IGUIButton* guiTerrainPaintVegetation;
        ITexture* helpVegetationPaint;


        ///Dynamic Objects
        IGUIButton* guiDynamicObjectsMode;
        IGUIWindow* guiDynamicObjectsWindowChooser;
        IGUIWindow* guiDynamicObjectsWindowEditAction;

        IGUIComboBox* guiDynamicObjects_OBJChooser;
        NodePreview* guiDynamicObjects_NodePreview;

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


        ///GAMEPLAY
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


        GUIManager();
        rect<s32> myRect(s32 x, s32 y, s32 w, s32 h);

        void loadFonts();

        void loadScriptTemplates();
};

#endif // GUIMANAGER_H
