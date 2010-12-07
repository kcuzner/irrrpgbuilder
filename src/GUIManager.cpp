#include "GUIManager.h"
#include "LANGManager.h"
#include "DynamicObjectsManager.h"
#include "EventReceiver.h"
#include "SoundManager.h"
#include "Player.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace irrklang;

GUIManager::GUIManager()
{
    guienv = App::getInstance()->getDevice()->getGUIEnvironment();
    loadFonts();
    setupGameplayGUI();
	timer = App::getInstance()->getDevice()->getTimer()->getRealTime();
	timer2 = timer;
}

GUIManager::~GUIManager()
{
    //dtor
}

void GUIManager::drawHelpImage(GUI_HELP_IMAGE img)
{
    IVideoDriver* driver = App::getInstance()->getDevice()->getVideoDriver();

    switch(img)
    {
        case HELP_TERRAIN_TRANSFORM:
            driver->draw2DImage(helpTerrainTransform, position2di(0,driver->getScreenSize().Height - helpTerrainTransform->getSize().Height),
				myRect(0,0,helpTerrainTransform->getSize().Width,helpTerrainTransform->getSize().Height), 0,
				video::SColor(255,255,255,255), true);
            break;
        case HELP_TERRAIN_SEGMENTS:
            driver->draw2DImage(helpTerrainSegments, position2di(0,driver->getScreenSize().Height - helpTerrainSegments->getSize().Height),
				myRect(0,0,helpTerrainSegments->getSize().Width,helpTerrainSegments->getSize().Height), 0,
				video::SColor(255,255,255,255), true);
            break;
        case HELP_VEGETATION_PAINT:
            driver->draw2DImage(helpVegetationPaint, position2di(0,driver->getScreenSize().Height - helpVegetationPaint->getSize().Height),
				myRect(0,0,helpVegetationPaint->getSize().Width,helpVegetationPaint->getSize().Height), 0,
				video::SColor(255,255,255,255), true);
            break;
        case HELP_IRR_RPG_BUILDER_1:
            driver->draw2DImage(logo1, position2di(driver->getScreenSize().Width - logo1->getSize().Width,driver->getScreenSize().Height - logo1->getSize().Height),
				myRect(0,0,logo1->getSize().Width,logo1->getSize().Height), 0,
				video::SColor(255,255,255,255), true);
            break;
    }
}

bool GUIManager::getCheckboxState(GUI_ID id)
{
    switch(id)
    {
        case CB_ID_TERRAIN_SHOW_PLAYABLE_AREA :
            return guiTerrainShowPlayableArea->isChecked();
            break;
    }
    return false;
}

f32 GUIManager::getScrollBarValue(GUI_ID id)
{
    switch(id)
    {
        case SC_ID_TERRAIN_BRUSH_STRENGTH :
            return (f32)guiTerrainBrushStrength->getPos();
            break;
		case SC_ID_TERRAIN_BRUSH_RADIUS :
			return (f32)guiTerrainBrushRadius->getPos();
			break;
    }
    return 0;
}

stringc GUIManager::getComboBoxItem(GUI_ID id)
{
    switch(id)
    {
        case CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER:
            return stringc(guiDynamicObjects_OBJChooser->getItem(guiDynamicObjects_OBJChooser->getSelected()));
            break;
        case CO_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE:
            return stringc(guiDynamicObjects_LoadScriptTemplateCB->getItem(guiDynamicObjects_LoadScriptTemplateCB->getSelected()));
            break;
    }
    return "";
}

IGUIFont* GUIManager::getFont(FONT_NAME fontName)
{
    switch(fontName)
    {
        case FONT_ARIAL:
            return guiFontC12;
            break;
    }

    return NULL;
}

GUIManager* GUIManager::getInstance()
{
    static GUIManager *instance = 0;
    if (!instance) instance = new GUIManager();
    return instance;
}

rect<s32> GUIManager::myRect(s32 x, s32 y, s32 w, s32 h)
{
    return rect<s32>(x,y,x+w,y+h);
}

void GUIManager::setupEditorGUI()
{
    IVideoDriver* driver = App::getInstance()->getDevice()->getVideoDriver();
    ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();

    //guienv->getSkin()->setFont(guiFontC12);
	guienv->getSkin()->setFont(guiFontCourier12);


    mainToolbarPos = position2di(2,2);

    guiMainWindow = guienv->addWindow(myRect(0,0,driver->getScreenSize().Width,36),false);
    guiMainWindow->setDraggable(false);
    guiMainWindow->setDrawTitlebar(false);
	
    guiMainWindow->getCloseButton()->setVisible(false);
	
	ITexture* backtexture = driver->getTexture("../media/art/back.png");
	
	guiBackImage = guienv->addImage(backtexture,vector2d<s32>(0,0),false,guiMainWindow);
	guiBackImage->setScaleImage(true);
	guiBackImage->setMaxSize(dimension2du(driver->getScreenSize().Width,36)); 
	guiBackImage->setMinSize(dimension2du(driver->getScreenSize().Width,36));
	
    //this var is used to set X position to the buttons in mainWindow (at each button this value is incresed,
    //so the next button will be positioned at the right side of the previous button)
    s32 x = 0;

    ///MAIN FUNCTIONS
    //New Project
    guiMainNewProject = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     guiMainWindow,
                                     BT_ID_NEW_PROJECT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_new_project")).c_str() );

    guiMainNewProject->setImage(driver->getTexture("../media/art/bt_new_project.png"));
	guiMainNewProject->setPressedImage(driver->getTexture("../media/art/bt_new_project_ghost.png"));

    x+=42;

    //Load Project
    guiMainLoadProject = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     guiMainWindow,
                                     BT_ID_LOAD_PROJECT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_load_project")).c_str() );

    guiMainLoadProject->setImage(driver->getTexture("../media/art/bt_load_project.png"));
	guiMainLoadProject->setPressedImage(driver->getTexture("../media/art/bt_load_project_ghost.png"));

    x+=42;

    guiCloseProgram = guienv->addButton(myRect(driver->getScreenSize().Width - 36,mainToolbarPos.Y,32,32),
                                     guiMainWindow,
                                     BT_ID_CLOSE_PROGRAM,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_close_program")).c_str() );

    guiCloseProgram->setImage(driver->getTexture("../media/art/bt_close_program.png"));

    //ABOUT BUTTON
    guiAbout = guienv->addButton(myRect(driver->getScreenSize().Width - 36 - 42,mainToolbarPos.Y,32,32),
                                     guiMainWindow,
                                     BT_ID_ABOUT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_about")).c_str() );

    guiAbout->setImage(driver->getTexture("../media/art/bt_about.png"));
	guiAbout->setPressedImage(driver->getTexture("../media/art/bt_about_ghost.png"));

    guiHelpButton = guienv->addButton(myRect(driver->getScreenSize().Width - 36 - 84,mainToolbarPos.Y,32,32),
                                     guiMainWindow,
                                     BT_ID_HELP,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_help")).c_str() );

    guiHelpButton->setImage(driver->getTexture("../media/art/bt_help.png"));

    //ABOUT WINDOW
    guiAboutWindow = guienv->addWindow(myRect(driver->getScreenSize().Width/2 - 300,driver->getScreenSize().Height/2 - 200,600,400),false);
    guiAboutWindow->setDraggable(false);
    guiAboutWindow->setDrawTitlebar(false);
    guiAboutWindow->getCloseButton()->setVisible(false);
    guiAboutWindow->setVisible(false);

    guienv->addImage(driver->getTexture("../media/art/logo1.png"),position2di(guiAboutWindow->getAbsoluteClippingRect().getWidth()/2-100,10),true,guiAboutWindow);

    guiAboutClose = guienv->addButton(myRect(guiAboutWindow->getAbsoluteClippingRect().getWidth() - 37,guiAboutWindow->getAbsoluteClippingRect().getHeight() - 37,32,32),guiAboutWindow,BT_ID_ABOUT_WINDOW_CLOSE);

    guiAboutClose->setImage(driver->getTexture("../media/art/bt_yes_32.png"));

	guiAboutText = guienv ->addListBox(myRect(guiAboutWindow->getAbsoluteClippingRect().getWidth()/2-250,160,500,200),guiAboutWindow);
    //IGUIStaticText* aboutText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_about")).c_str(),myRect(guiAboutWindow->getAbsoluteClippingRect().getWidth()/2-250,140,500,400),true,true,guiAboutWindow);
	//aboutText->setOverrideFont(guiFontCourier12);
	
    //aboutText->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);
	guiAboutText->setSubElement(true);
	
	LANGManager::getInstance()->getText("txt_about");
	guiAboutText->setEnabled(false);
	
	//aboutText->addItem(stringw(LANGManager::getInstance()->getText("txt_about")).c_str());

    //Save Project
    guiMainSaveProject = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     guiMainWindow,
                                     BT_ID_SAVE_PROJECT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_save_project")).c_str() );

    guiMainSaveProject->setImage(driver->getTexture("../media/art/bt_save_project.png"));
	guiMainSaveProject->setPressedImage(driver->getTexture("../media/art/bt_save_project_ghost.png"));

    x+=42;


    //Transform Terrain
    guiTerrainTransform = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     guiMainWindow,
                                     BT_ID_TERRAIN_TRANSFORM,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_terrain_transform")).c_str());

    guiTerrainTransform->setImage(driver->getTexture("../media/art/bt_terrain_up.png"));
	guiTerrainTransform->setPressedImage(driver->getTexture("../media/art/bt_terrain_up_ghost.png"));


    x+= 42;

    //Terrain Add Segment
    guiTerrainAddSegment = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     guiMainWindow,
                                     BT_ID_TERRAIN_ADD_SEGMENT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_terrain_segments")).c_str());

    guiTerrainAddSegment->setImage(driver->getTexture("../media/art/bt_terrain_add_segment.png"));
	guiTerrainAddSegment->setPressedImage(driver->getTexture("../media/art/bt_terrain_add_segment_ghost.png"));

    x+= 42;

    //Terrain Add Segment
    guiTerrainPaintVegetation = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     guiMainWindow,
                                     BT_ID_TERRAIN_PAINT_VEGETATION,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_paint_vegetation")).c_str());

    guiTerrainPaintVegetation->setImage(driver->getTexture("../media/art/bt_terrain_paint_vegetation.png"));
	guiTerrainPaintVegetation->setPressedImage(driver->getTexture("../media/art/bt_terrain_paint_vegetation_ghost.png"));


    x += 42;

    //Dynamic Objects
    guiDynamicObjectsMode= guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     guiMainWindow,
                                     BT_ID_DYNAMIC_OBJECTS_MODE,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_mode")).c_str());

    guiDynamicObjectsMode->setImage(driver->getTexture("../media/art/bt_dynamic_objects_mode.png"));
	guiDynamicObjectsMode->setPressedImage(driver->getTexture("../media/art/bt_dynamic_objects_mode_ghost.png"));


    x += 42;

    //Edit Character
    guiEditCharacter = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     guiMainWindow,
                                     BT_ID_EDIT_CHARACTER,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_edit_character")).c_str());

    guiEditCharacter->setImage(driver->getTexture("../media/art/bt_edit_character.png"));
	guiEditCharacter->setPressedImage(driver->getTexture("../media/art/bt_edit_character_ghost.png"));


    x += 42;

    //Edit Items Script
    guiEditScriptGlobal = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     guiMainWindow,
                                     BT_ID_EDIT_SCRIPT_GLOBAL,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_edit_script_global")).c_str());

    guiEditScriptGlobal->setImage(driver->getTexture("../media/art/bt_edit_script_global.png"));
	guiEditScriptGlobal->setPressedImage(driver->getTexture("../media/art/bt_edit_script_global_ghost.png"));

    x += 42;

    //Play Game
    guiPlayGame= guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     guiMainWindow,
                                     BT_ID_PLAY_GAME,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_play_game")).c_str());

    guiPlayGame->setImage(driver->getTexture("../media/art/bt_play_game.png"));


    //Stop Game
    guiStopGame= guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     guiMainWindow,
                                     BT_ID_STOP_GAME,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_stop_game")).c_str());

    guiStopGame->setImage(driver->getTexture("../media/art/bt_stop_game.png"));
    guiStopGame->setVisible(false);



    ///TERRAIN TOOLBAR
    guiTerrainToolbar = guienv->addWindow(myRect(2,34,200,54));
    guiTerrainToolbar->getCloseButton()->setVisible(false);
    guiTerrainToolbar->setDrawTitlebar(false);
    guiTerrainToolbar->setDraggable(false);
    guiTerrainToolbar->setVisible(false);

	guiTerrainBrushRadiusLabel = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_transform_brush_radius_label")).c_str(),
                                                         myRect(2,mainToolbarPos.Y+32,98,16),
                                                         false,true, guiTerrainToolbar);

    guiTerrainBrushRadius = guienv->addScrollBar(true,myRect(100,mainToolbarPos.Y+32,100,16),guiTerrainToolbar,SC_ID_TERRAIN_BRUSH_STRENGTH );
    guiTerrainBrushRadius->setMin(50);
    guiTerrainBrushRadius->setMax(300);
    guiTerrainBrushRadius->setPos(175);

    guiTerrainBrushStrengthLabel = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_transform_brush_strength_label")).c_str(),
                                                         myRect(2,mainToolbarPos.Y+16,98,16),
                                                         false,true, guiTerrainToolbar);

    guiTerrainBrushStrength = guienv->addScrollBar(true,myRect(100,mainToolbarPos.Y+16,100,16),guiTerrainToolbar,SC_ID_TERRAIN_BRUSH_STRENGTH );
    guiTerrainBrushStrength->setMin(0);
    guiTerrainBrushStrength->setMax(300);
    guiTerrainBrushStrength->setPos(175);

    //Show Playable Area (areas with no Y == 0 will be red)
    guiTerrainShowPlayableArea = guienv->addCheckBox(true,myRect(0,mainToolbarPos.Y,200,16),
                                                     guiTerrainToolbar,
                                                     CB_ID_TERRAIN_SHOW_PLAYABLE_AREA,
                                                     stringw(LANGManager::getInstance()->getText("bt_show_playable_area")).c_str());


    ///Dynamic Objects Chooser (to choose and place dynamic objects on the scenery)
    rect<s32> windowRect = myRect(driver->getScreenSize().Width - 160,guiMainWindow->getAbsoluteClippingRect().getHeight(),160,driver->getScreenSize().Height);
    guiDynamicObjectsWindowChooser = guienv->addWindow(windowRect,false,L"",0,GCW_DYNAMIC_OBJECT_CHOOSER);
    guiDynamicObjectsWindowChooser->setDraggable(false);
    guiDynamicObjectsWindowChooser->getCloseButton()->setVisible(false);
    guiDynamicObjectsWindowChooser->setDrawTitlebar(false);

    s32 guiDynamicObjectsWindowChooser_Y = 5;


    rect<s32> nodePreviewPos = myRect(5 + guiDynamicObjectsWindowChooser->getAbsolutePosition().UpperLeftCorner.X,
                                      guiDynamicObjectsWindowChooser_Y + guiDynamicObjectsWindowChooser->getAbsolutePosition().UpperLeftCorner.Y,
                                      150,150);

    guiDynamicObjects_NodePreview = new NodePreview(nodePreviewPos);
    guiDynamicObjects_NodePreview->setNode(DynamicObjectsManager::getInstance()->getActiveObject()->getNode());

    guiDynamicObjectsWindowChooser_Y += 155;


    guiDynamicObjects_OBJChooser = guienv->addComboBox(myRect(5,guiDynamicObjectsWindowChooser_Y,150,20),guiDynamicObjectsWindowChooser,CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER);

    vector<stringc> listDynamicObjs = DynamicObjectsManager::getInstance()->getObjectsList();

    for (int i=0 ; i<(int)listDynamicObjs.size() ; i++)
    {
    	guiDynamicObjects_OBJChooser->addItem( stringw( listDynamicObjs[i] ).c_str() );
    }

    guiDynamicObjectsWindowChooser_Y += 25;

    guiDynamicObjects_Context_Menu_Window = guienv->addWindow(myRect(100,100,200,105),false,L"",0,GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU);
    guiDynamicObjects_Context_Menu_Window->getCloseButton()->setVisible(false);
    guiDynamicObjects_Context_Menu_Window->setDraggable(false);
    guiDynamicObjects_Context_Menu_Window->setDrawTitlebar(false);
    guiDynamicObjects_Context_Menu_Window->setVisible(false);

    guiDynamicObjects_Context_btEditScript = guienv->addButton(myRect(5,5,190,20),
                                                           guiDynamicObjects_Context_Menu_Window,
                                                           BT_ID_DYNAMIC_OBJECT_BT_EDITSCRIPTS,
                                                           stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_edit_script")).c_str() );
	guiDynamicObjects_Context_btEditScript->setOverrideFont(guiFontC12);

    guiDynamicObjects_Context_btMoveRotate= guienv->addButton(myRect(5,30,190,20),
                                                           guiDynamicObjects_Context_Menu_Window,
                                                           BT_ID_DYNAMIC_OBJECT_BT_MOVEROTATE,
                                                           stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_move_rotate")).c_str() );
	guiDynamicObjects_Context_btMoveRotate->setOverrideFont(guiFontC12);

    guiDynamicObjects_Context_btRemove= guienv->addButton(myRect(5,55,190,20),
                                                           guiDynamicObjects_Context_Menu_Window,
                                                           BT_ID_DYNAMIC_OBJECT_BT_REMOVE,
                                                           stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_remove")).c_str() );
	guiDynamicObjects_Context_btRemove->setOverrideFont(guiFontC12);

    guiDynamicObjects_Context_btCancel= guienv->addButton(myRect(5,80,190,20),
                                                           guiDynamicObjects_Context_Menu_Window,
                                                           BT_ID_DYNAMIC_OBJECT_BT_CANCEL,
                                                           stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_cancel")).c_str() );
	guiDynamicObjects_Context_btCancel->setOverrideFont(guiFontC12);

    ///Edit scripts window
    guiDynamicObjectsWindowEditAction = guienv->addWindow(myRect(100,100,driver->getScreenSize().Width-200,driver->getScreenSize().Height-150),false,L"",0,GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT);
    guiDynamicObjectsWindowEditAction->getCloseButton()->setVisible(false);
    guiDynamicObjectsWindowEditAction->setDrawTitlebar(false);
    guiDynamicObjectsWindowEditAction->setDraggable(false);

    s32 X_ScriptToolbar = 10;

    guiDynamicObjects_LoadScriptTemplateCB = guienv->addComboBox(myRect(X_ScriptToolbar,10,400,20),guiDynamicObjectsWindowEditAction,CO_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE);

    this->loadScriptTemplates();

    X_ScriptToolbar+=400;

    guiDynamicObjects_LoadScriptTemplateBT = guienv->addButton(myRect(X_ScriptToolbar,10,150,20),
                      guiDynamicObjectsWindowEditAction,
                      BT_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE,
                      stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_load_script_template")).c_str() );
	guiDynamicObjects_LoadScriptTemplateBT->setOverrideFont(guiFontC12);

    X_ScriptToolbar+=160;

    IGUIButton* validate = guienv->addButton(myRect(X_ScriptToolbar,10,150,20),
                      guiDynamicObjectsWindowEditAction,
                      BT_ID_DYNAMIC_OBJECT_VALIDATE_SCRIPT,
                      stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_validate_script")).c_str() );
	validate->setOverrideFont(guiFontC12);

    X_ScriptToolbar+=160;

    IGUIButton* close = guiDynamicObjects_Script_Close = guienv->addButton(myRect(X_ScriptToolbar,10,82,20),
                      guiDynamicObjectsWindowEditAction,
                      BT_ID_DYNAMIC_OBJECT_SCRIPT_CLOSE,
                      stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_close_script")).c_str() );
	close->setOverrideFont(guiFontC12);

    //scripts editor
    guiDynamicObjects_Script = new CGUIEditBoxIRB(L"",
                       true,
                       guienv,
                       guiDynamicObjectsWindowEditAction,
                       EB_ID_DYNAMIC_OBJECT_SCRIPT,
                       myRect(10,40,driver->getScreenSize().Width-220,driver->getScreenSize().Height-260),
					   App::getInstance()->getDevice());

    guiDynamicObjects_Script->setMultiLine(true);
    guiDynamicObjects_Script->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

    guienv->getSkin()->setColor( gui::EGDC_WINDOW, video::SColor(255, 255, 255, 255) );

    guiDynamicObjects_Script->setOverrideFont(guiFontCourier12);


    guiDynamicObjects_Script_Console = guienv->addEditBox(L"",
                                                          myRect(10,driver->getScreenSize().Height-220,driver->getScreenSize().Width-220,60),
                                                          true,
                                                          guiDynamicObjectsWindowEditAction,
                                                          EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE);

    guiDynamicObjects_Script_Console->setOverrideColor(SColor(255,255,0,0));
    guiDynamicObjects_Script_Console->setEnabled(false);

    guiDynamicObjectsWindowEditAction->setVisible(false);

    ///EDIT CHARACTER
    guiPlayerEditScript = guienv->addButton(myRect(guiEditCharacter->getAbsoluteClippingRect().UpperLeftCorner.X,38,32,32),
                                                           0,
                                                           BT_ID_PLAYER_EDIT_SCRIPT,
                                                           L"",
                                                           stringw(LANGManager::getInstance()->getText("bt_player_edit_script")).c_str() );

	guiPlayerEditScript->setOverrideFont(guiFontC12);
    guiPlayerEditScript->setImage(driver->getTexture("../media/art/bt_player_edit_script.png"));

    guiPlayerEditScript->setVisible(false);



    ///LOAD HELP IMAGES
    helpTerrainTransform = App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/art/help_terrain_transform.png");

    helpVegetationPaint = App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/art/help_vegetation_paint.png");

    helpTerrainSegments = App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/art/help_terrain_segments.png");

    logo1 = App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/art/logo1.png");

}

void GUIManager::setupGameplayGUI()
{
    IVideoDriver* driver = App::getInstance()->getDevice()->getVideoDriver();

    fader=guienv->addInOutFader();
    fader->setVisible(false);

    guiPlayerLife_Shadow=guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_player_life")).c_str(),myRect(20,50,600,50),false,false,0,-1,false);
    guiPlayerLife_Shadow->setOverrideColor(SColor(255,30,30,30));
    guiPlayerLife_Shadow->setOverrideFont(guiFontLarge28);

    guiPlayerLife=guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_player_life")).c_str(),myRect(21,51,600,50),false,false,0,-1,false);
    guiPlayerLife->setOverrideColor(SColor(255,255,255,100));
    guiPlayerLife->setOverrideFont(guiFontLarge28);

    this->setElementVisible(ST_ID_PLAYER_LIFE, false);


    ///DIALOG
    guiDialogImgYes = driver->getTexture("../media/art/img_yes.png");
    guiDialogImgYes_s = driver->getTexture("../media/art/img_yes_s.png");
    guiDialogImgNo = driver->getTexture("../media/art/img_no.png");
    guiDialogImgNo_s = driver->getTexture("../media/art/img_no_s.png");


    //view items
    guiBtViewItems = guienv->addButton(myRect(driver->getScreenSize().Width - 50,driver->getScreenSize().Height - 50,48,48),
                                     0,
                                     BT_ID_VIEW_ITEMS,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_view_items")).c_str() );

    guiBtViewItems->setImage(driver->getTexture("../media/art/bt_view_items.png"));
    guiBtViewItems->setVisible(false);

    //Items window
    guiWindowItems = guienv->addWindow(myRect(100,100,driver->getScreenSize().Width-200,driver->getScreenSize().Height-150),false,L"",0,GCW_GAMEPLAY_ITEMS);
    guiWindowItems->getCloseButton()->setVisible(false);
    guiWindowItems->setDrawTitlebar(false);
    guiWindowItems->setDraggable(false);
    guiWindowItems->setVisible(false);

    guiPlayerItems = guienv->addListBox(myRect(10,10,driver->getScreenSize().Width-220,driver->getScreenSize().Height-170 - 32),guiWindowItems,LB_ID_PLAYER_ITEMS);



    guiBtUseItem = guienv->addButton(myRect(10,driver->getScreenSize().Height-160 - 32,32,32),
                                         guiWindowItems,
                                         BT_ID_USE_ITEM,
                                         L"",
                                         stringw(LANGManager::getInstance()->getText("bt_use_item")).c_str());
    guiBtUseItem->setImage(driver->getTexture("../media/art/bt_yes_32.png"));

    guiBtDropItem = guienv->addButton(myRect(52,driver->getScreenSize().Height-192,32,32),
                                         guiWindowItems,
                                         BT_ID_DROP_ITEM,
                                         L"",
                                         stringw(LANGManager::getInstance()->getText("bt_drop_item")).c_str());
    guiBtDropItem->setImage(driver->getTexture("../media/art/bt_no_32.png"));


    guiBtCloseItemsWindow = guienv->addButton(myRect(driver->getScreenSize().Width-210-32,driver->getScreenSize().Height-160 - 32,32,32),
                                         guiWindowItems,
                                         BT_ID_CLOSE_ITEMS_WINDOW,
                                         L"",
                                         stringw(LANGManager::getInstance()->getText("bt_close_items_window")).c_str());
    guiBtCloseItemsWindow->setImage(driver->getTexture("../media/art/bt_arrow_32.png"));


    guiPlayerMoney = guienv->addStaticText(L"GOLD:129",myRect(52+42,driver->getScreenSize().Height-160 - 32,300,32),false,false,guiWindowItems);
    guiPlayerMoney->setOverrideFont(guiFontLarge28);
    guiPlayerMoney->setOverrideColor(SColor(255,255,255,255));
}

void GUIManager::setWindowVisible(GUI_CUSTOM_WINDOW window, bool visible)
{
    switch(window)
    {
        case GCW_DYNAMIC_OBJECT_CHOOSER:
            guiDynamicObjectsWindowChooser->setVisible(visible);
            break;
        case GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU:
            mouseX = App::getInstance()->getDevice()->getCursorControl()->getPosition().X;
            mouseY = App::getInstance()->getDevice()->getCursorControl()->getPosition().Y;
            guiDynamicObjects_Context_Menu_Window->setRelativePosition(myRect(mouseX,mouseY,200,105));
            guiDynamicObjects_Context_Menu_Window->setVisible(visible);
            break;
        case GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT:
            guiDynamicObjectsWindowEditAction->setVisible(visible);
            break;
        case GCW_TERRAIN_TOOLBAR:
            guiTerrainToolbar->setVisible(visible);
            break;
        case GCW_GAMEPLAY_ITEMS:
            this->updateItemsList();
            guiWindowItems->setVisible(visible);
            break;
        case GCW_ABOUT:
            guiAboutWindow->setVisible(visible);
            break;
    }
}

void GUIManager::loadScriptTemplates()
{
    TiXmlDocument doc("../media/scripts/template_scripts.xml");

	if (!doc.LoadFile())
	{
	    #ifdef APP_DEBUG
        cout << "ERROR : XML : LOADING TEMPLATE SCRIPTS!" << endl;
        #endif
        return;
	}

	#ifdef APP_DEBUG
    cout << "DEBUG : XML : LOADING TEMPLATE SCRIPTS" << endl;
    #endif

    TiXmlElement* root = doc.FirstChildElement( "IrrRPG_Builder_TemplateScripts" );

    if ( root )
    {
        TiXmlNode* scriptXML = root->FirstChild( "script" );

        while( scriptXML != NULL )
        {
            guiDynamicObjects_LoadScriptTemplateCB->addItem( stringw(scriptXML->ToElement()->Attribute("file")).c_str() );

            scriptXML = root->IterateChildren( "script", scriptXML );
        }
    }
}

void GUIManager::drawNodePreview()
{
    guiDynamicObjects_NodePreview->draw(App::getInstance()->getDevice()->getVideoDriver());
}

void GUIManager::updateDynamicObjectPreview()
{
    guiDynamicObjects_NodePreview->setNode(DynamicObjectsManager::getInstance()->getActiveObject()->getNode());
}

stringc GUIManager::getEditBoxText(GUI_ID id)
{
    switch(id)
    {
        case EB_ID_DYNAMIC_OBJECT_SCRIPT:
            return stringc(guiDynamicObjects_Script->getText());
            break;
    }
	return "";
}

void GUIManager::setEditBoxText(GUI_ID id, stringc text)
{
    switch(id)
    {
        case EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE:
            guiDynamicObjects_Script_Console->setText(stringw(text).c_str());
            break;
        case EB_ID_DYNAMIC_OBJECT_SCRIPT:
            guiDynamicObjects_Script->setText(stringw(text).c_str());
            break;
    }
}

void GUIManager::setElementEnabled(GUI_ID id, bool enable)
{
    switch(id)///TODO: fazer metodo getElement by ID!!!
    {
        case BT_ID_DYNAMIC_OBJECT_BT_EDITSCRIPTS:
            guiDynamicObjects_Context_btEditScript->setEnabled(enable);
			guiDynamicObjects_Context_btEditScript->setPressed(!enable);
            break;
        case BT_ID_DYNAMIC_OBJECTS_MODE:
            guiDynamicObjectsMode->setEnabled(enable);
			guiDynamicObjectsMode->setPressed(!enable);
            break;
        case BT_ID_TERRAIN_ADD_SEGMENT:
            guiTerrainAddSegment->setEnabled(enable);
			guiTerrainAddSegment->setPressed(!enable);
            break;
        case BT_ID_TERRAIN_PAINT_VEGETATION:
            guiTerrainPaintVegetation->setEnabled(enable);
			guiTerrainPaintVegetation->setPressed(!enable);
            break;
        case BT_ID_TERRAIN_TRANSFORM:
            guiTerrainTransform->setEnabled(enable);
			guiTerrainTransform->setPressed(!enable);
            break;
        case BT_ID_SAVE_PROJECT:
            guiMainSaveProject->setEnabled(enable);
			guiMainSaveProject->setPressed(!enable);
            break;
        case BT_ID_LOAD_PROJECT:
            guiMainLoadProject->setEnabled(enable);
			guiMainLoadProject->setPressed(!enable);
            break;
        case BT_ID_EDIT_CHARACTER:
            guiEditCharacter->setEnabled(enable);
			guiEditCharacter->setPressed(!enable);
            break;
        case BT_ID_EDIT_SCRIPT_GLOBAL:
            guiEditScriptGlobal->setEnabled(enable);
			guiEditScriptGlobal->setPressed(!enable);
            break;
        case BT_ID_ABOUT:
            guiAbout->setEnabled(enable);
			guiAbout->setPressed(!enable);
            break;
    }
}

void GUIManager::setElementVisible(GUI_ID id, bool visible)
{
    switch(id)
    {
        case BT_ID_PLAY_GAME:
            guiPlayGame->setVisible(visible);
            break;
        case BT_ID_STOP_GAME:
            guiStopGame->setVisible(visible);
            break;
        case ST_ID_PLAYER_LIFE:
            guiPlayerLife->setVisible(visible);
            guiPlayerLife_Shadow->setVisible(visible);
            break;
        case BT_ID_PLAYER_EDIT_SCRIPT:
            guiPlayerEditScript->setVisible(visible);
            break;
        case BT_ID_VIEW_ITEMS:
            guiBtViewItems->setVisible(visible);
            break;
    }
}

void GUIManager::showMessage(GUI_MSG_TYPE msgType, stringc msg)
{
    stringc msg_type = "";

    switch(msgType)
    {
        case GUI_MSG_TYPE_ERROR:
            msg_type = LANGManager::getInstance()->getText("msg_error");
            break;
    }

    printf("MESSAGE:%s\n",msg.c_str());

    ///TODO:messageBox and Confirm
}

void GUIManager::showBlackScreen(stringc text)
{
    fader->setVisible(true);
    fader->fadeOut(1000);

    while(!fader->isReady() && App::getInstance()->getDevice()->run())
    {
        App::getInstance()->getDevice()->getVideoDriver()->beginScene(true, true, SColor(0,200,200,200));
        App::getInstance()->getDevice()->getSceneManager()->drawAll();

        guienv->drawAll();

        guiFontLarge28->draw(stringw(text),myRect(0,0,
                                             App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width,
                                             App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height ),
                     SColor(255,255,255,255),true,true);


        App::getInstance()->getDevice()->getVideoDriver()->endScene();
    }
}

void GUIManager::hideBlackScreen()
{
    fader->fadeIn(1000);

    while(!fader->isReady() && App::getInstance()->getDevice()->run())
    {
        App::getInstance()->getDevice()->getVideoDriver()->beginScene(true, true, SColor(0,200,200,200));
        App::getInstance()->getDevice()->getSceneManager()->drawAll();
        guienv->drawAll();
        App::getInstance()->getDevice()->getVideoDriver()->endScene();
    }

    fader->setVisible(false);
}

void GUIManager::loadFonts()
{
    ///Load Fonts
    guiFontCourier12 = guienv->getFont("../media/fonts/courier12.xml");
    guiFontC12 = guienv->getFont("../media/fonts/char12.xml");
    guiFontLarge28 = guienv->getFont("../media/fonts/large28.xml");
    guiFontDialog = guienv->getFont("../media/fonts/dialog.xml");
}

void GUIManager::setStaticTextText(GUI_ID id, stringc text)
{
    switch(id)
    {
        case ST_ID_PLAYER_LIFE:
            guiPlayerLife->setText(stringw(text).c_str());
            guiPlayerLife_Shadow->setText(stringw(text).c_str());
            break;
        case ST_ID_PLAYER_MONEY:
            guiPlayerMoney->setText(stringw(text).c_str());
            break;
    }
}

void GUIManager::showDialogMessage(std::string text, std::string sound)
{
    //insert '\n' to enable multiline (user can add \n from lua call too)
    for(int i = 0; i<(int)text.size(); i+=80) text.insert(text.begin() + i, '\n');

    bool mouseExit = false;

    //Play dialog sound (yes you can record voices!)
    ISound* dialogSound = NULL;

    if((sound.c_str() != "") | (sound.c_str() != NULL))
    {
        stringc soundName = "../media/sound/";
        soundName += sound.c_str();
        dialogSound = SoundManager::getInstance()->playSound2D(soundName.c_str());
    }

    while(!EventReceiver::getInstance()->isKeyPressed(KEY_RETURN) && mouseExit==false && App::getInstance()->getDevice()->run())
    {
        App::getInstance()->getDevice()->getVideoDriver()->beginScene(true, true, SColor(0,200,200,200));
        App::getInstance()->getDevice()->getSceneManager()->drawAll();
        //guienv->drawAll();

        App::getInstance()->getDevice()->getVideoDriver()->draw2DRectangle(SColor(150,0,0,0), rect<s32>(10,
                                                                                                        App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 200,
                                                                                                        App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width - 10,
                                                                                                        App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 10));

        rect<s32> textRect = rect<s32>(10,  App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 200,
                                            App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width - 10,
                                            App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 10);

        guiFontDialog->draw(stringw(text.c_str()),textRect,SColor(255,255,255,255),true,false,&textRect);

        //draw YES GREEN button
        position2di buttonYesPosition = position2di(App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width - 58,
                                                    App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 58);

        App::getInstance()->getDevice()->getVideoDriver()->draw2DImage(guiDialogImgYes,buttonYesPosition,rect<s32>(0,0,48,48),0,SColor(255,255,255,255),true);


        //check mouse click on OK button
        position2di mousePos = App::getInstance()->getDevice()->getCursorControl()->getPosition();
        if(mousePos.getDistanceFrom(buttonYesPosition+position2di(16,16)) < 16 && EventReceiver::getInstance()->isMousePressed(0)) mouseExit = true;

        App::getInstance()->getDevice()->getVideoDriver()->endScene();
    }

    //stop sound when player cancel the dialog
    if(dialogSound)
    {
        dialogSound->stop();
    }

    EventReceiver::getInstance()->flushKeys();
    EventReceiver::getInstance()->flushMouse();
    this->flush();
}

bool GUIManager::showDialogQuestion(std::string text, std::string sound )
{
    //insert '\n' to enable multiline (user can add \n from lua call too)
    for(int i = 0; i<(int)text.size(); i+=80) text.insert(text.begin() + i, '\n');

    bool result = true;
    int changeTime = 0;

    bool mouseExit = false;

    //Play question sound (optional voice)
    ISound* dialogSound = NULL;
	
	//int len = sound.length();
	printf ("Here is the dialog for the sound %s, \n",sound.c_str());
	if(sound.c_str() != "")
    {
        stringc soundName = "../media/sound/";
        soundName += sound.c_str();
        dialogSound = SoundManager::getInstance()->playSound2D(soundName.c_str());
    }
	else
		printf("The sound was null");

	while(!EventReceiver::getInstance()->isKeyPressed(KEY_RETURN) && mouseExit==false && App::getInstance()->getDevice()->run())
    {
        App::getInstance()->getDevice()->getVideoDriver()->beginScene(true, true, SColor(0,200,200,200));
        App::getInstance()->getDevice()->getSceneManager()->drawAll();
        //guienv->drawAll();

        App::getInstance()->getDevice()->getVideoDriver()->draw2DRectangle(SColor(150,0,0,0), rect<s32>(10,
                                                                                                        App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 200,
                                                                                                        App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width - 10,
                                                                                                        App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 10));

        rect<s32> textRect = rect<s32>(10,  App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 200,
                                            App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width - 10,
                                            App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 10);

        guiFontDialog->draw(stringw(text.c_str()),textRect,SColor(255,255,255,255),true,false,&textRect);

        position2di buttonYesPosition = position2di(App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width - 58,
                                                    App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 58);

        position2di buttonNoPosition = position2di(App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width - 58 - 58,
                                                    App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 58);

        //draw shadowed YES image
        App::getInstance()->getDevice()->getVideoDriver()->draw2DImage(guiDialogImgYes_s,buttonYesPosition,
                                                                       rect<s32>(0,0,48,48),0,SColor(150,255,255,255),true);

        //draw shadowed NO image
        App::getInstance()->getDevice()->getVideoDriver()->draw2DImage(guiDialogImgNo_s,buttonNoPosition,
                                                                       rect<s32>(0,0,48,48),0,SColor(150,255,255,255),true);

        if(result)
        {
            //draw YES GREEN button
            App::getInstance()->getDevice()->getVideoDriver()->draw2DImage(guiDialogImgYes,buttonYesPosition,
                                                                       rect<s32>(0,0,48,48),0,SColor(255,255,255,255),true);
        }
        else
        {
            //draw NO GREEN button
            App::getInstance()->getDevice()->getVideoDriver()->draw2DImage(guiDialogImgNo,buttonNoPosition,
                                                                       rect<s32>(0,0,48,48),0,SColor(150,255,255,255),true);
        }

        if((EventReceiver::getInstance()->isKeyPressed(KEY_RIGHT) || EventReceiver::getInstance()->isKeyPressed(KEY_LEFT)) && changeTime == 20)
        {
            result = !result;
            changeTime = 0;
        }


        //check mouse click on OK button
        position2di mousePos = App::getInstance()->getDevice()->getCursorControl()->getPosition();
        if(mousePos.getDistanceFrom(buttonYesPosition+position2di(16,16)) < 16 && EventReceiver::getInstance()->isMousePressed(0))
        {
            result = 1;
            mouseExit = true;
        }
        if(mousePos.getDistanceFrom(buttonNoPosition+position2di(16,16)) < 16 && EventReceiver::getInstance()->isMousePressed(0))
        {
            result = 0;
            mouseExit = true;
        }


        if(changeTime < 20) changeTime++;

        App::getInstance()->getDevice()->getVideoDriver()->endScene();
    }

    //stop sound when player cancel the question
    if(dialogSound)
    {
        dialogSound->stop();
    }

    EventReceiver::getInstance()->flushKeys();
    EventReceiver::getInstance()->flushMouse();
    this->flush();

    return result;
}

stringc GUIManager::showInputQuestion(std::string text)
{
    std::string newtxt = "";

    bool mouseExit = false;

    while(!EventReceiver::getInstance()->isKeyPressed(KEY_RETURN) && mouseExit==false && App::getInstance()->getDevice()->run())
    {
		u32 timercheck = App::getInstance()->getDevice()->getTimer()->getRealTime(); 
        App::getInstance()->getDevice()->getVideoDriver()->beginScene(true, true, SColor(0,200,200,200));
        App::getInstance()->getDevice()->getSceneManager()->drawAll();
        //guienv->drawAll();

        App::getInstance()->getDevice()->getVideoDriver()->draw2DRectangle(SColor(150,0,0,0), rect<s32>(10,
                                                                                                        App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 200,
                                                                                                        App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width - 10,
                                                                                                        App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 10));

        rect<s32> textRect = rect<s32>(10,  App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 180,
                                            App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width - 10,
                                            App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 10);

        stringw realTxt = stringw(text.c_str());
        realTxt += stringw(newtxt.c_str());
		// Flashing cursor, flash at 1/4 second interval (based on realtime)
	    if((timercheck-timer2>250)) 
		{
			realTxt += L'_';
			if (timercheck-timer2>500)
				timer2=timercheck;
		}

        guiFontDialog->draw(realTxt.c_str(),textRect,SColor(255,255,255,255),false,false,&textRect);


        //draw YES GREEN button
        position2di buttonYesPosition = position2di(App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width - 58,
                    App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 58);

        App::getInstance()->getDevice()->getVideoDriver()->draw2DImage(guiDialogImgYes,buttonYesPosition,
                                                                   rect<s32>(0,0,48,48),0,SColor(255,255,255,255),true);

        //check mouse click on OK button
        position2di mousePos = App::getInstance()->getDevice()->getCursorControl()->getPosition();
        if(mousePos.getDistanceFrom(buttonYesPosition+position2di(16,16)) < 16 && EventReceiver::getInstance()->isMousePressed(0)) mouseExit = true;


        //verify pressed chars and add it to the string
		
        if(timercheck-timer > 160)
        {
            //process all keycodes [0-9] and [A-Z]
            for(int i=0x30;i<0x5B;i++)
            {
                if(EventReceiver::getInstance()->isKeyPressed(i))
                {
                    newtxt += i;
                    timer = timercheck;
                }
            }

            //process delete and backspace (same behavior for both of them -> remove the last char)
            if(EventReceiver::getInstance()->isKeyPressed(KEY_BACK) || EventReceiver::getInstance()->isKeyPressed(KEY_DELETE))
            {
                newtxt = newtxt.substr(0,newtxt.size()-1);
				timer = timercheck;
            }
        }
        App::getInstance()->getDevice()->getVideoDriver()->endScene();
    }

    EventReceiver::getInstance()->flushKeys();
    EventReceiver::getInstance()->flushMouse();
    this->flush();

    return stringc(newtxt.c_str());
}

stringc GUIManager::getActivePlayerItem()
{
    return stringc(guiPlayerItems->getListItem(guiPlayerItems->getSelected()));
}

void GUIManager::updateItemsList()
{
    guiPlayerItems->clear();
    vector<stringc> items = Player::getInstance()->getItems();

    for(int i = 0; i<(int)items.size(); i++) guiPlayerItems->addItem( stringw(items[i]).c_str() );
}

void GUIManager::addAboutTextItem(stringc text)
{
	if (guiAboutText)
	{
		this->guiAboutText->addItem(stringw(text).c_str());
	}
	
}

void GUIManager::flush()
{
    guiMainLoadProject->setPressed(false);
    guiMainSaveProject->setPressed(false);
    guiMainNewProject->setPressed(false);
    guiDynamicObjects_LoadScriptTemplateBT->setPressed(false);
    guiBtViewItems->setPressed(false);
}
