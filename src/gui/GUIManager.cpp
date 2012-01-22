#include "GUIManager.h"
#include "../LANGManager.h"
#include "../objects/DynamicObjectsManager.h"
#include "../events/EventReceiver.h"
#include "../sound/SoundManager.h"
#include "../objects/Player.h"

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
	screensize = App::getInstance()->getScreenSize();

    loadFonts();

	// init those because they will move on the display.
	guiDynamicObjectsWindowEditAction=NULL;
	guiDynamicObjectsWindowChooser=NULL;
	guiDynamicObjects_NodePreview=NULL;
	guiTerrainToolbar=NULL;
	guiWindowItems=NULL;
	consolewin=NULL;
	guiLoaderDescription = NULL;

	timer = App::getInstance()->getDevice()->getTimer()->getRealTime();
	timer2 = timer;

	for (s32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
    {
            video::SColor col = guienv->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
            col.setAlpha(230);
            guienv->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
    }

	// Fake office style skin colors
	guienv->getSkin()->setColor(EGDC_3D_SHADOW,video::SColor(200,140,178,226));
	guienv->getSkin()->setColor(EGDC_3D_FACE,video::SColor(200,204,227,248));
	guienv->getSkin()->setColor(EGDC_WINDOW,video::SColor(255,220,220,220));

	
}

GUIManager::~GUIManager()
{

	delete managauge;
	delete lifegauge;
	delete guiDynamicObjects_NodePreview;
	delete guiPlayerNodePreview;
	delete guiDynamicObjects_Script;
	delete configWindow;

    //dtor
}

void GUIManager::drawPlayerStats()
{
//	IVideoDriver * driver = App::getInstance()->getDevice()->getVideoDriver();
	// Text display
	// Update the GUI display
	DynamicObject* playerObject = Player::getInstance()->getObject();

	stringc playerLife=playerLifeText;
	playerLife += playerObject->getProperties().life;
	playerLife += "/";
	playerLife += playerObject->getProperties().maxlife;
	playerLife += " Experience:";
	stringc playerxp = (stringc)playerObject->getProperties().experience;
	playerLife += playerxp;
	playerLife += " Level:";
	playerLife += playerObject->getProperties().level;
	//+(stringc)properties.experience;
	setStaticTextText(ST_ID_PLAYER_LIFE,playerLife);

	s32 hp = Player::getInstance()->getObject()->getProperties().life;
	s32 max = Player::getInstance()->getObject()->getProperties().maxlife;
	lifegauge->setCurrentValue(hp);
	lifegauge->setMaxValue(max);

	s32 mana = Player::getInstance()->getObject()->getProperties().mana;
	s32 maxmana = Player::getInstance()->getObject()->getProperties().maxmana;
	managauge->setCurrentValue(mana);
	managauge->setMaxValue(maxmana);
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

#ifdef EDITOR
// Specific stuff related to the editor

void GUIManager::drawHelpImage(GUI_HELP_IMAGE img)
{
    IVideoDriver* driver = App::getInstance()->getDevice()->getVideoDriver();

    switch(img)
    {
        case HELP_TERRAIN_TRANSFORM:
            driver->draw2DImage(helpTerrainTransform, position2di(0,screensize.Height - helpTerrainTransform->getSize().Height),
				myRect(0,0,helpTerrainTransform->getSize().Width,helpTerrainTransform->getSize().Height), 0,
				video::SColor(255,255,255,255), true);
            break;
        case HELP_TERRAIN_SEGMENTS:
            driver->draw2DImage(helpTerrainSegments, position2di(0,screensize.Height - helpTerrainSegments->getSize().Height),
				myRect(0,0,helpTerrainSegments->getSize().Width,helpTerrainSegments->getSize().Height), 0,
				video::SColor(255,255,255,255), true);
            break;
        case HELP_VEGETATION_PAINT:
            driver->draw2DImage(helpVegetationPaint, position2di(0,screensize.Height - helpVegetationPaint->getSize().Height),
				myRect(0,0,helpVegetationPaint->getSize().Width,helpVegetationPaint->getSize().Height), 0,
				video::SColor(255,255,255,255), true);
            break;
        case HELP_IRR_RPG_BUILDER_1:
			driver->draw2DImage(logo1, position2di(screensize.Width - logo1->getSize().Width,screensize.Height - logo1->getSize().Height),
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
			{
				stringw text = stringw(guiTerrainBrushStrength->getPos()).c_str();
				text=text+L"\"";
				guiTerrainBrushStrengthValue->setText(text.c_str());
				return (f32)guiTerrainBrushStrength->getPos();
			}
            break;
		case SC_ID_TERRAIN_BRUSH_RADIUS :
			{
				stringw text = stringw(guiTerrainBrushRadius->getPos()).c_str();
				text=text+L"\"";
				guiTerrainBrushRadiusValue->setText(text.c_str());
				return (f32)guiTerrainBrushRadius->getPos();
			}
			break;
		case SC_ID_TERRAIN_BRUSH_PLATEAU :
			{
				stringw text = stringw(guiTerrainBrushPlateau->getPos()).c_str();
				text=text+L"\"";
				guiTerrainBrushPlateauValue->setText(text.c_str());
				return (f32)guiTerrainBrushPlateau->getPos();

			}
			break;
        case SC_ID_VEGETATION_BRUSH_STRENGTH :
            {
                return (f32)guiVegetationBrushStrength->getPos();
            }
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
		case CO_ID_DYNAMIC_OBJECT_OBJ_CATEGORY:
			return stringc(guiDynamicObjects_Category->getItem(guiDynamicObjects_Category->getSelected()));
			break;
        case CO_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE:
            return stringc(guiDynamicObjects_LoadScriptTemplateCB->getItem(guiDynamicObjects_LoadScriptTemplateCB->getSelected()));
            break;
    }
    return "";
}

void GUIManager::setupEditorGUI()
{
	IVideoDriver* driver = App::getInstance()->getDevice()->getVideoDriver();
//    ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();

    //guienv->getSkin()->setFont(guiFontC12);
	guienv->getSkin()->setFont(guiFont10);
	// Load textures
	ITexture* imgLogo = driver->getTexture("../media/art/logo1.png");

// NEW Create display size since IRRlicht return wrong values

	// Check the current screen size (normally reported by wxWidget or the configuration)

	displayheight=screensize.Height;
	displaywidth=screensize.Width;

	//LOADER WINDOW
	guiLoaderWindow = guienv->addWindow(myRect(displaywidth/2-300,displayheight/2-200,600,400),false,L"Loading...");
	guiLoaderWindow->setDrawTitlebar(false);
	guiLoaderWindow->getCloseButton()->setVisible(false);
	guiLoaderWindow->setAlignment(EGUIA_CENTER,EGUIA_CENTER,EGUIA_CENTER,EGUIA_CENTER);

	guienv->addImage(imgLogo,vector2d<s32>(200,50),true,guiLoaderWindow);
	guiLoaderDescription = guienv->addStaticText(L"Loading interface graphics...",myRect(10,350,580,40),true,true,guiLoaderWindow,-1,false);

	// quick update of the Irrlicht display while loading.
	App::getInstance()->quickUpdate();

	// loading others
	ITexture* backtexture = driver->getTexture("../media/art/back.png");
	ITexture* imgNewProject = driver->getTexture("../media/art/bt_new_project.png");
	ITexture* imgNewProject1 = driver->getTexture("../media/art/bt_new_project_ghost.png");
	ITexture* imgLoadProject = driver->getTexture("../media/art/bt_load_project.png");
	ITexture* imgLoadProject1 = driver->getTexture("../media/art/bt_load_project_ghost.png");
	ITexture* imgSaveProject = driver->getTexture("../media/art/bt_save_project.png");
	ITexture* imgSaveProject1 = driver->getTexture("../media/art/bt_save_project_ghost.png");
	ITexture* imgCloseProgram = driver->getTexture("../media/art/bt_close_program.png");
	ITexture* imgAbout = driver->getTexture("../media/art/bt_about.png");
	ITexture* imgAbout1 = driver->getTexture("../media/art/bt_about_ghost.png");
	ITexture* imgHelp = driver->getTexture("../media/art/bt_help.png");
	ITexture* imgHelp1 = driver->getTexture("../media/art/bt_help_ghost.png");
	ITexture* imgConfig = driver->getTexture("../media/art/bt_config.png");
	ITexture* imgConfig1 = driver->getTexture("../media/art/bt_config_ghost.png");


	// Standard toolbar
    mainToolbarPos = position2di(2,2);

    //guiMainWindow = guienv->addWindow(myRect(0,0,driver->getScreenSize().Width-170,92),false);
	guiMainWindow = guienv->addWindow(myRect(0,0,displaywidth-170,122),false);
    guiMainWindow->setDraggable(false);
    guiMainWindow->setDrawTitlebar(false);
	guiMainWindow->getCloseButton()->setVisible(false);
	guiMainWindow->setDrawBackground(false);
	guiMainWindow->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

	guiLoaderWindow->bringToFront(guiMainWindow);
	

	//guiMainToolWindow = guienv->addWindow(myRect(driver->getScreenSize().Width-170,0,170,46),false);
	guiMainToolWindow = guienv->addWindow(myRect(displaywidth-170,0,170,120),false);
	guiMainToolWindow->setDraggable(false);
	guiMainToolWindow->setDrawTitlebar(false);
	guiMainToolWindow->getCloseButton()->setVisible(false);
	guiMainToolWindow->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

	

	guiBackImage2=guienv->addImage(backtexture,vector2d<s32>(0,0),true,guiMainToolWindow);
	guiBackImage2->setScaleImage(true);
	guiBackImage2->setMaxSize(dimension2du(2048,120));
	guiBackImage2->setMinSize(dimension2du(2048,120));
	guiBackImage2->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

	guiBackImage = guienv->addImage(backtexture,vector2d<s32>(0,0),true,guiMainWindow);
	guiBackImage->setScaleImage(true);
	guiBackImage->setMaxSize(dimension2du(2048,120));
	guiBackImage->setMinSize(dimension2du(2048,120));
	guiBackImage->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

    //this var is used to set X position to the buttons in mainWindow (at each button this value is incresed,
    //so the next button will be positioned at the right side of the previous button)
    s32 x = 0;

    ///MAIN FUNCTIONS
	//mainTabCtrl = guienv->addTabControl(myRect(0,0,driver->getScreenSize().Width-160,92),guiMainWindow,false,false);

	// project TAB
	prjTabCtrl = guienv->addTabControl(myRect(5,2,250,112),guiMainWindow,true,true);
	IGUITab * tabProject = prjTabCtrl->addTab(LANGManager::getInstance()->getText("tab_project").c_str());

	// Tool tab
	mainToolCtrl = guienv->addTabControl(myRect(2,2,164,112),guiMainToolWindow,true,true);
	IGUITab * tabPlayTool = mainToolCtrl->addTab(LANGManager::getInstance()->getText("txt_tool_des4").c_str());
	
	// Tools TAB
	mainTabCtrl = guienv->addTabControl(myRect(260,2,displaywidth-435,112),guiMainWindow,true,true);
	mainTabCtrl->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);
	
	IGUITab * tabEnv = mainTabCtrl->addTab(LANGManager::getInstance()->getText("tab_environment").c_str());
	IGUITab * tabObject = mainTabCtrl->addTab(LANGManager::getInstance()->getText("tab_objects").c_str());
	IGUITab * tabTools = mainTabCtrl->addTab(LANGManager::getInstance()->getText("tab_tools").c_str());
	IGUITab * tabConfig = mainTabCtrl->addTab(LANGManager::getInstance()->getText("tab_setup").c_str());
	//mainTabCtrl->setTabExtraWidth(25);
	mainTabCtrl->setActiveTab(1);

	// Tab description box text
	IGUIStaticText * projectTabText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des0")).c_str(),
		core::rect<s32>(0,64,250,80),false,true,tabProject,-1);
	projectTabText->setBackgroundColor(video::SColor(128,237,242,248));
	projectTabText->setOverrideColor(video::SColor(255,65,66,174));
	projectTabText->setOverrideFont(guiFont10);
	projectTabText->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	// Tab description box text
	IGUIStaticText * environmentTabText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des5")).c_str(),
		core::rect<s32>(0,64,120,80),false,true,tabEnv,-1);
	environmentTabText->setBackgroundColor(video::SColor(128,237,242,248));
	environmentTabText->setOverrideColor(video::SColor(255,65,66,174));
	environmentTabText->setOverrideFont(guiFont10);
	environmentTabText->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	IGUIStaticText * vegetationTabText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des6")).c_str(),
		core::rect<s32>(130,64,250,80),false,true,tabEnv,-1);
	vegetationTabText->setBackgroundColor(video::SColor(128,237,242,248));
	vegetationTabText->setOverrideColor(video::SColor(255,65,66,174));
	vegetationTabText->setOverrideFont(guiFont10);
	vegetationTabText->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	// Tab description box text
	IGUIStaticText * objectTabText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des1")).c_str(),
		core::rect<s32>(0,64,120,80),false,true,tabObject,-1);
	objectTabText->setBackgroundColor(video::SColor(128,237,242,248));
	objectTabText->setOverrideColor(video::SColor(255,65,66,174));
	objectTabText->setOverrideFont(guiFont10);
	objectTabText->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	// Tab description box text
	IGUIStaticText * objectTabText2 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des2")).c_str(),
		core::rect<s32>(130,64,250,80),false,true,tabObject,-1);
	objectTabText2->setBackgroundColor(video::SColor(128,237,242,248));
	objectTabText2->setOverrideColor(video::SColor(255,65,66,174));
	objectTabText2->setOverrideFont(guiFont10);
	objectTabText2->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	mainToolbarPos.Y=5;
	// Close program
	x += 12;
	guiCloseProgram = guienv->addButton(myRect(x,mainToolbarPos.Y,32,32),
                                     tabProject,
                                     BT_ID_CLOSE_PROGRAM,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_close_program")).c_str() );

    guiCloseProgram->setImage(imgCloseProgram);

	IGUIStaticText * closeText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_close_program")).c_str(),
		core::rect<s32>(x-5,36,x+40,65),false,true,tabProject,-1);
	closeText->setOverrideColor(video::SColor(255,65,66,174));
	closeText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	closeText->setOverrideFont(guiFont9);

    //New Project
	x+=60;
    guiMainNewProject = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabProject,
                                     BT_ID_NEW_PROJECT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_new_project")).c_str() );

    guiMainNewProject->setImage(imgNewProject);
	guiMainNewProject->setPressedImage(imgNewProject1);

	IGUIStaticText * newPText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_new_project")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabProject,-1);
	newPText->setOverrideColor(video::SColor(255,65,66,174));
	newPText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	newPText->setOverrideFont(guiFont9);


    //Load Project
	x+=60;
    guiMainLoadProject = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabProject,
                                     BT_ID_LOAD_PROJECT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_load_project")).c_str() );

    guiMainLoadProject->setImage(imgLoadProject);
	guiMainLoadProject->setPressedImage(imgLoadProject1);

	IGUIStaticText * loadPText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_load_project")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabProject,-1);
	loadPText->setOverrideColor(video::SColor(255,65,66,174));
	loadPText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	loadPText->setOverrideFont(guiFont9);


	//Save Project
	x+=60;
    guiMainSaveProject = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabProject,
                                     BT_ID_SAVE_PROJECT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_save_project")).c_str() );

    guiMainSaveProject->setImage(imgSaveProject);
	guiMainSaveProject->setPressedImage(imgSaveProject1);

	IGUIStaticText * savePText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_save_project")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabProject,-1);
	savePText->setOverrideColor(video::SColor(255,65,66,174));
	savePText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	savePText->setOverrideFont(guiFont9);


    //Transform Terrain
	x=12;
    //Terrain Add Segment
    guiTerrainAddSegment = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabEnv,
                                     BT_ID_TERRAIN_ADD_SEGMENT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_terrain_segments")).c_str());

    guiTerrainAddSegment->setImage(driver->getTexture("../media/art/bt_terrain_add_segment.png"));
	guiTerrainAddSegment->setPressedImage(driver->getTexture("../media/art/bt_terrain_add_segment_ghost.png"));

	IGUIStaticText * terrainSText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_segments")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabEnv,-1);
	terrainSText->setOverrideColor(video::SColor(255,65,66,174));
	terrainSText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	terrainSText->setOverrideFont(guiFont9);

	 x+= 60;

	guiTerrainTransform = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabEnv,
                                     BT_ID_TERRAIN_TRANSFORM,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_terrain_transform")).c_str());

    guiTerrainTransform->setImage(driver->getTexture("../media/art/bt_terrain_up.png"));
	guiTerrainTransform->setPressedImage(driver->getTexture("../media/art/bt_terrain_up_ghost.png"));

	IGUIStaticText * terrainTText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_transform")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabEnv,-1);
	terrainTText->setOverrideColor(video::SColor(255,65,66,174));
	terrainTText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	terrainTText->setOverrideFont(guiFont9);
    
     x+= 70;

    //Terrain Add Segment
    guiTerrainPaintVegetation = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabEnv,
                                     BT_ID_TERRAIN_PAINT_VEGETATION,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_paint_vegetation")).c_str());

    guiTerrainPaintVegetation->setImage(driver->getTexture("../media/art/bt_terrain_paint_vegetation.png"));
	guiTerrainPaintVegetation->setPressedImage(driver->getTexture("../media/art/bt_terrain_paint_vegetation_ghost.png"));

	IGUIStaticText * paintVText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_paint_vegetation")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabEnv,-1);
	paintVText->setOverrideColor(video::SColor(255,65,66,174));
	paintVText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	paintVText->setOverrideFont(guiFont9);


    //Dynamic Objects
	x = 12;
    guiDynamicObjectsMode= guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabObject,
                                     BT_ID_DYNAMIC_OBJECTS_MODE,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_mode")).c_str());

    guiDynamicObjectsMode->setImage(driver->getTexture("../media/art/bt_dynamic_objects_mode.png"));
	guiDynamicObjectsMode->setPressedImage(driver->getTexture("../media/art/bt_dynamic_objects_mode_ghost.png"));

	IGUIStaticText * dynObjText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_mode")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabObject,-1);
	dynObjText->setOverrideColor(video::SColor(255,65,66,174));
	dynObjText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	dynObjText->setOverrideFont(guiFont9);


    x += 60;

    //Edit Character
    guiEditCharacter = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabObject,
                                     BT_ID_EDIT_CHARACTER,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_edit_character")).c_str());

    guiEditCharacter->setImage(driver->getTexture("../media/art/bt_edit_character.png"));
	guiEditCharacter->setPressedImage(driver->getTexture("../media/art/bt_edit_character_ghost.png"));

	IGUIStaticText * editCharText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_edit_character")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabObject,-1);
	editCharText->setOverrideColor(video::SColor(255,65,66,174));
	editCharText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	editCharText->setOverrideFont(guiFont9);


	 x+=70;
	///EDIT CHARACTER
    guiPlayerEditScript = guienv->addButton(myRect(mainToolbarPos.X+x,mainToolbarPos.Y,32,32),
                                                           tabObject,
                                                           BT_ID_PLAYER_EDIT_SCRIPT,
                                                           L"",
                                                           stringw(LANGManager::getInstance()->getText("bt_player_edit_script")).c_str() );

	guiPlayerEditScript->setOverrideFont(guiFontC12);
    guiPlayerEditScript->setImage(driver->getTexture("../media/art/bt_player_edit_script.png"));

	IGUIStaticText * editCharSText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_player_edit_script")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabObject,-1);
	editCharSText->setOverrideColor(video::SColor(255,65,66,174));
	editCharSText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	editCharSText->setOverrideFont(guiFont9);

	//guiPlayerEditScript->setNotClipped(true);

    //guiPlayerEditScript->setVisible(false);


    x += 60;

    //Edit Items Script
    guiEditScriptGlobal = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabObject,
                                     BT_ID_EDIT_SCRIPT_GLOBAL,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_edit_script_global")).c_str());

    guiEditScriptGlobal->setImage(driver->getTexture("../media/art/bt_edit_script_global.png"));
	guiEditScriptGlobal->setPressedImage(driver->getTexture("../media/art/bt_edit_script_global_ghost.png"));

	IGUIStaticText * editGlobSText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_edit_script_global")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabObject,-1);
	editGlobSText->setOverrideColor(video::SColor(255,65,66,174));
	editGlobSText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	editGlobSText->setOverrideFont(guiFont9);

     //CONFIG BUTTON
	x=12;
    guiConfigButton = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabConfig,
                                     BT_ID_CONFIG,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_config")).c_str() );

    guiConfigButton->setImage(imgConfig);
	guiConfigButton->setPressedImage(imgConfig1);

    //Play Game
	x = 12;
	mainToolbarPos.Y=5;
    guiPlayGame= guienv->addButton(myRect(x,mainToolbarPos.Y,32,32),
                                     tabPlayTool,
                                     BT_ID_PLAY_GAME,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_play_game")).c_str());

    guiPlayGame->setImage(driver->getTexture("../media/art/bt_play_game.png"));

	IGUIStaticText * playGText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_play_game")).c_str(),
		core::rect<s32>(x-5,36,x+40,65),false,true,tabPlayTool,-1);
	playGText->setOverrideColor(video::SColor(255,65,66,174));
	playGText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	playGText->setOverrideFont(guiFont9);


    //Stop Game
    guiStopGame= guienv->addButton(myRect(+x,mainToolbarPos.Y,32,32),
                                     tabPlayTool,
                                     BT_ID_STOP_GAME,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_stop_game")).c_str());

    guiStopGame->setImage(driver->getTexture("../media/art/bt_stop_game.png"));
    guiStopGame->setVisible(false);



    //ABOUT BUTTON
	x += 50;
    guiAbout = guienv->addButton(myRect(x,mainToolbarPos.Y,32,32),
                                     tabPlayTool,
                                     BT_ID_ABOUT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_about")).c_str() );

    guiAbout->setImage(imgAbout);
	guiAbout->setPressedImage(imgAbout1);

	IGUIStaticText * aboutBText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_about")).c_str(),
		core::rect<s32>(x-5,36,x+40,65),false,true,tabPlayTool,-1);
	aboutBText->setOverrideColor(video::SColor(255,65,66,174));
	aboutBText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	aboutBText->setOverrideFont(guiFont9);

	// Help Button
	x += 50;
    guiHelpButton = guienv->addButton(myRect(x,mainToolbarPos.Y,32,32),
                                     tabPlayTool,
                                     BT_ID_HELP,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_help")).c_str() );

    guiHelpButton->setImage(imgHelp);
    guiHelpButton->setPressedImage(imgHelp1);

	IGUIStaticText * helpBText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_help")).c_str(),
		core::rect<s32>(x-5,36,x+40,65),false,true,tabPlayTool,-1);
	helpBText->setOverrideColor(video::SColor(255,65,66,174));
	helpBText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);

		// Update the display
	App::getInstance()->quickUpdate();

	//ABOUT WINDOW
    //guiAboutWindow = guienv->addWindow(myRect(driver->getScreenSize().Width/2 - 300,driver->getScreenSize().Height/2 - 200,600,400),false);
	guiAboutWindow = guienv->addWindow(myRect(displaywidth/2 - 300,displayheight/2 - 200,600,400),false);
    guiAboutWindow->setDraggable(false);
    guiAboutWindow->setDrawTitlebar(false);
    guiAboutWindow->getCloseButton()->setVisible(false);
    guiAboutWindow->setVisible(false);
	guiAboutWindow->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

    //guienv->addImage(driver->getTexture("../media/art/logo1.png"),position2di(guiAboutWindow->getAbsoluteClippingRect().getWidth()/2-100,10),true,guiAboutWindow);
	IGUIImage * logo = guienv->addImage(driver->getTexture("../media/art/logo1.png"),position2di(guiAboutWindow->getClientRect().getWidth()/2-100,10),true,guiAboutWindow);
	logo->setAlignment(EGUIA_CENTER,EGUIA_CENTER,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
    //guiAboutClose = guienv->addButton(myRect(guiAboutWindow->getAbsoluteClippingRect().getWidth() - 37,guiAboutWindow->getAbsoluteClippingRect().getHeight() - 37,32,32),guiAboutWindow,BT_ID_ABOUT_WINDOW_CLOSE);
	guiAboutClose = guienv->addButton(myRect(guiAboutWindow->getClientRect().getWidth() - 37,guiAboutWindow->getClientRect().getHeight() - 37,32,32),guiAboutWindow,BT_ID_ABOUT_WINDOW_CLOSE);
    guiAboutClose->setImage(driver->getTexture("../media/art/bt_yes_32.png"));
	guiAboutClose->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);

	//guiAboutText = guienv ->addListBox(myRect(guiAboutWindow->getAbsoluteClippingRect().getWidth()/2-250,160,500,200),guiAboutWindow);
	guiAboutText = guienv ->addListBox(myRect(guiAboutWindow->getClientRect().getWidth()/2-250,160,500,200),guiAboutWindow);
	guiAboutText->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

	// Ask the LANGManager to fill the box with the proper Language of the about text.
	LANGManager::getInstance()->setAboutText(guiAboutText);

    ///TERRAIN TOOLBAR
    guiTerrainToolbar = guienv->addWindow(
		//myRect(driver->getScreenSize().Width - 170,
		myRect(displaywidth - 170,
		//guiMainToolWindow->getAbsoluteClippingRect().getHeight(),
		guiMainToolWindow->getClientRect().getHeight(),
		170,
		//driver->getScreenSize().Height-guiMainToolWindow->getAbsoluteClippingRect().getHeight()),
		displayheight-guiMainToolWindow->getClientRect().getHeight()),
		false,stringw(LANGManager::getInstance()->getText("bt_terrain_brush")).c_str());


    guiTerrainToolbar->getCloseButton()->setVisible(false);

    guiTerrainToolbar->setDraggable(false);
    guiTerrainToolbar->setVisible(false);
	guiTerrainToolbar->setNotClipped(true);
	guiTerrainToolbar->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

	//Show Playable Area (areas with no Y == 0 will be red)
	mainToolbarPos.Y=20;
    guiTerrainShowPlayableArea = guienv->addCheckBox(true,myRect(10,mainToolbarPos.Y,160,20),
                                                     guiTerrainToolbar,
                                                     CB_ID_TERRAIN_SHOW_PLAYABLE_AREA,
                                                     stringw(LANGManager::getInstance()->getText("bt_show_playable_area")).c_str());

	// Display the brush strength
	guiTerrainBrushStrengthLabel = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_transform_brush_strength_label")).c_str(),
                                                         myRect(10,mainToolbarPos.Y+30,150,20),
                                                         false,true, guiTerrainToolbar);
	guiTerrainBrushStrengthValue = guienv->addStaticText(L"100",
                                                         myRect(10,mainToolbarPos.Y+70,150,20),
                                                         false,true, guiTerrainToolbar);

    guiTerrainBrushStrength = guienv->addScrollBar(true,myRect(10,mainToolbarPos.Y+50,150,20),guiTerrainToolbar,SC_ID_TERRAIN_BRUSH_STRENGTH );
    guiTerrainBrushStrength->setMin(0);
    guiTerrainBrushStrength->setMax(200);
    guiTerrainBrushStrength->setPos(100);

	// Display the brush radius
	guiTerrainBrushRadiusLabel = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_transform_brush_radius_label")).c_str(),
                                                         myRect(10,mainToolbarPos.Y+90,150,20),
                                                         false,true, guiTerrainToolbar);
	guiTerrainBrushRadiusValue = guienv->addStaticText(L"100",
                                                         myRect(10,mainToolbarPos.Y+130,150,20),
                                                         false,true, guiTerrainToolbar);

	guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_transform_plateau")).c_str(),
                                                         myRect(10,mainToolbarPos.Y+150,150,20),
                                                         false,true, guiTerrainToolbar);


    guiTerrainBrushRadius = guienv->addScrollBar(true,myRect(10,mainToolbarPos.Y+110,150,20),guiTerrainToolbar,SC_ID_TERRAIN_BRUSH_STRENGTH );
    guiTerrainBrushRadius->setMin(0);
    guiTerrainBrushRadius->setMax(200);
    guiTerrainBrushRadius->setPos(100);

	guiTerrainBrushPlateau = guienv->addScrollBar(true,core::rect<s32>(10,mainToolbarPos.Y+170,160,mainToolbarPos.Y+190),guiTerrainToolbar,SC_ID_TERRAIN_BRUSH_PLATEAU);
	guiTerrainBrushPlateau->setMin(-100);
	guiTerrainBrushPlateau->setMax(255);
	guiTerrainBrushPlateau->setPos(-10);

	guiTerrainBrushPlateauValue = guienv->addStaticText(L"0",
                                                         myRect(10,mainToolbarPos.Y+190,150,20),
                                                         false,true, guiTerrainToolbar);

    ///Vegetation toolbar
    guiVegetationToolbar = guienv->addWindow(
		//myRect(driver->getScreenSize().Width - 170,
		myRect(displaywidth - 170,
		//guiMainToolWindow->getAbsoluteClippingRect().getHeight(),
		guiMainToolWindow->getClientRect().getHeight(),
		170,
		//driver->getScreenSize().Height-guiMainToolWindow->getAbsoluteClippingRect().getHeight()),
		displayheight-guiMainToolWindow->getClientRect().getHeight()),
		false,L"Vegetation tool");

    guiVegetationToolbar->getCloseButton()->setVisible(false);

    guiVegetationToolbar->setDraggable(false);
    guiVegetationToolbar->setVisible(false);
	guiVegetationToolbar->setNotClipped(true);
	guiVegetationToolbar->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);


    guiVegetationBrushStrengthLabel = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_transform_vegetation_paint_strength")).c_str(),
                                                         myRect(10,mainToolbarPos.Y+10,150,20),
                                                         false,true, guiVegetationToolbar);

    guiVegetationBrushStrength = guienv->addScrollBar(true,myRect(10,mainToolbarPos.Y+30,150,20),guiVegetationToolbar,SC_ID_VEGETATION_BRUSH_STRENGTH );
    guiVegetationBrushStrength->setMin(0);
    guiVegetationBrushStrength->setMax(200);
    guiVegetationBrushStrength->setPos(100);

    // --- Dynamic Objects Chooser (to choose and place dynamic objects on the scenery)
    rect<s32> windowRect =
#ifdef _wxWIDGET
		myRect(displaywidth - 170, 120, 170, displayheight);
#else
		myRect(displaywidth - 170,
		guiMainToolWindow->getClientRect().getHeight(),
		170,
		displayheight-guiMainToolWindow->getClientRect().getHeight());
#endif

    guiDynamicObjectsWindowChooser = guienv->addWindow(windowRect,false,L"",0,GCW_DYNAMIC_OBJECT_CHOOSER);
    guiDynamicObjectsWindowChooser->setDraggable(false);
    guiDynamicObjectsWindowChooser->getCloseButton()->setVisible(false);
    guiDynamicObjectsWindowChooser->setDrawTitlebar(false);
	guiDynamicObjectsWindowChooser->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

    s32 guiDynamicObjectsWindowChooser_Y = 5;

	IGUIStaticText * ObjectText0 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_dynobjsel")).c_str(),core::rect<s32>(1,1,168,39),false,true,guiDynamicObjectsWindowChooser,-1);
	ObjectText0->setDrawBackground(true);
	ObjectText0->setDrawBorder(true);
	ObjectText0->setBackgroundColor(video::SColor(255,237,242,248));
	ObjectText0->setOverrideColor(video::SColor(255,65,66,174));
	ObjectText0->setOverrideFont(guiFontCourier12);
	ObjectText0->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	IGUIStaticText * ObjectText1 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_dynobjcur")).c_str(),core::rect<s32>(10,190,160,200),false,true,guiDynamicObjectsWindowChooser,-1);
	ObjectText1->setOverrideColor(video::SColor(255,0,0,0));
	ObjectText1->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_CENTER);

   	guiDynamicObjects_NodePreview = new NodePreview(guienv,guiDynamicObjectsWindowChooser,rect<s32>(10,40,160,180),-1);
    guiDynamicObjects_NodePreview->setNode(DynamicObjectsManager::getInstance()->getActiveObject()->getNode());
	guiDynamicObjects_NodePreview->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

    guiDynamicObjectsWindowChooser_Y += 220;
	guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_dynobjcat")).c_str(),core::rect<s32>(10,guiDynamicObjectsWindowChooser_Y,160,guiDynamicObjectsWindowChooser_Y+20),false,true,guiDynamicObjectsWindowChooser,-1);
	guiDynamicObjectsWindowChooser_Y += 20;
	guiDynamicObjects_Category = guienv->addComboBox(myRect(10,guiDynamicObjectsWindowChooser_Y,150,20),guiDynamicObjectsWindowChooser,CO_ID_DYNAMIC_OBJECT_OBJ_CATEGORY);
	guiDynamicObjects_Category->addItem(L"NPC");
	//Since they are not yet implemented, theses will be off for the moment.
	//guiDynamicObjects_Category->addItem(L"INTERACTIVE OBJECTS");
	guiDynamicObjects_Category->addItem(L"PROPS");


	guiDynamicObjectsWindowChooser_Y += 40;
	guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_dynobjitm")).c_str(),core::rect<s32>(10,guiDynamicObjectsWindowChooser_Y,160,guiDynamicObjectsWindowChooser_Y+20),false,true,guiDynamicObjectsWindowChooser,-1);
	guiDynamicObjectsWindowChooser_Y += 20;
	guiDynamicObjects_OBJChooser = guienv->addComboBox(myRect(10,guiDynamicObjectsWindowChooser_Y,150,20),guiDynamicObjectsWindowChooser,CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER);
	UpdateGUIChooser(1);



    guiDynamicObjectsWindowChooser_Y += 25;


	// --- Contextual menu for the dynamic objects
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



	// --- Edit scripts window

    //guiDynamicObjectsWindowEditAction = guienv->addWindow(myRect(100,100,driver->getScreenSize().Width-200,driver->getScreenSize().Height-100),false,L"",0,GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT);
	guiDynamicObjectsWindowEditAction = guienv->addWindow(myRect(25,25,displaywidth-50,displayheight-50),false,L"",0,GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT);
    guiDynamicObjectsWindowEditAction->getCloseButton()->setVisible(false);
    guiDynamicObjectsWindowEditAction->setDrawTitlebar(false);
    guiDynamicObjectsWindowEditAction->setDraggable(false);
	guiDynamicObjectsWindowEditAction->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

	//scripts editor box
    guiDynamicObjects_Script = new CGUIEditBoxIRB(L"",
                       true,
					   true,
                       guienv,
                       guiDynamicObjectsWindowEditAction,
                       EB_ID_DYNAMIC_OBJECT_SCRIPT,
                       //myRect(10,40,driver->getScreenSize().Width-220,driver->getScreenSize().Height-260),
					   myRect(10,40,guiDynamicObjectsWindowEditAction->getClientRect().getWidth()-20,guiDynamicObjectsWindowEditAction->getClientRect().getHeight()-130),
					   App::getInstance()->getDevice());

    guiDynamicObjects_Script->setMultiLine(true);
    guiDynamicObjects_Script->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);
	guiDynamicObjects_Script->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
    //guienv->getSkin()->setColor( gui::EGDC_WINDOW, video::SColor(255, 255, 255, 255) );
    guiDynamicObjects_Script->setOverrideFont(guiFontCourier12);
	
	//Old code now changed to setElementText()
	//guiDynamicObjects_Script->setLineCountButtonText(LANGManager::getInstance()->getText("bt_script_editor_linecount").c_str());
	guiDynamicObjects_Script->setElementText(guiDynamicObjects_Script->BT_LINECOUNT,LANGManager::getInstance()->getText("bt_script_editor_linecount").c_str());

	// Set the IRB commands Highlights
	
	// Allow the code editor to use syntax highlighting based on LUA keywords
	guiDynamicObjects_Script->addLUAKeywords();

	
	// Define custom "Group" keywords, here are "dictionnary" for IRB specific keywords
	guiDynamicObjects_Script->addKeyword("setObjectName",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("chaseObject",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("walkRandomly",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("walkToObject",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("CustomDynamicObjectUpdate",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("programAction",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("CustomDynamicObjectUpdateProgrammedAction",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("hasActionProgrammed",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("enableObject",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("disableObject",SColor(255,128,0,255),true);

	guiDynamicObjects_Script->addKeyword("increasePlayerLife",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("decreasePlayerLife",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("increasePlayerMoney",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("playSound",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("emitSound",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("sleep",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setGlobal",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getGlobal",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("deleteGlobal",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setTimeOfDay",SColor(255,128,0,255),true);

	guiDynamicObjects_Script->addKeyword("setAmbientLight",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getAmbientColor",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("setFogColor",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getFogColor",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setFogRange",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getFogRange",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setPostFX",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setCameraTarget",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getCameraTarget",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getObjectPosition",SColor(255,128,0,255),true);

	guiDynamicObjects_Script->addKeyword("playSound2D",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("playSound3D",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("setSoundListenerPosition",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("setPlayerLife",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("setSoundVolume",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("getPlayerLife",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("setPlayerMoney",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("getPlayerMoney",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("addPlayerItem",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("stopSounds",SColor(255,128,0,255),true);	

	guiDynamicObjects_Script->addKeyword("removePlayerItem",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("usePlayerItem",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("getItemCount",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("showBlackScreen",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("hideBlackScreen",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("showDialogMessage",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("showDialogQuestion",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("saveGame",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("loadGame",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("showObjectLabel",SColor(255,128,0,255),true);	

	guiDynamicObjects_Script->addKeyword("hideObjectLabel",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("setObjectLabel",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("setPosition",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("getPosition",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("setRotation",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("getRotation",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("turn",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("move",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("walkTo",SColor(255,128,0,255),true);	
	guiDynamicObjects_Script->addKeyword("lookAt",SColor(255,128,0,255),true);	

	guiDynamicObjects_Script->addKeyword("lookToObject",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getName",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("distanceFrom",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setEnabled",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setFrameLoop",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setAnimationSpeed",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setAnimation",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setEnemy",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setObject",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setPropertie",SColor(255,128,0,255),true);

	guiDynamicObjects_Script->addKeyword("getPropertie",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("attack",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("onLoad",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("onUpdate",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("step",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("onClicked",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("onAnswer",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getLanguage",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("onCollision",SColor(255,128,0,255),true);

	// Bottom tabcontrol
	//IGUITabControl * tabctrl1 = guienv->addTabControl(myRect(10,driver->getScreenSize().Height-220,driver->getScreenSize().Width-220,110),guiDynamicObjectsWindowEditAction,true,false);
	IGUITabControl * tabctrl1 = guienv->addTabControl(myRect(10,guiDynamicObjectsWindowEditAction->getClientRect().getHeight()-90,displaywidth-220,110),guiDynamicObjectsWindowEditAction,true,false);
	IGUITab * tab1 = tabctrl1->addTab(LANGManager::getInstance()->getText("tab_script_debug").c_str());
	IGUITab * tab2 = tabctrl1->addTab(LANGManager::getInstance()->getText("tab_script_templates").c_str());

	tabctrl1->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);
    s32 X_ScriptToolbar = 10;

    guiDynamicObjects_LoadScriptTemplateCB = guienv->addComboBox(myRect(X_ScriptToolbar,10,400,20),tab2,CO_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE);
	guiDynamicObjects_LoadScriptTemplateCB->setAlignment(EGUIA_UPPERLEFT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);


    this->loadScriptTemplates();

    X_ScriptToolbar+=405;

    guiDynamicObjects_LoadScriptTemplateBT = guienv->addButton(myRect(X_ScriptToolbar,10,200,20),
                      tab2,
                      BT_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE,
                      stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_load_script_template")).c_str() );
	guiDynamicObjects_LoadScriptTemplateBT->setOverrideFont(guiFontC12);
	guiDynamicObjects_LoadScriptTemplateBT->setAlignment(EGUIA_UPPERLEFT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

    X_ScriptToolbar+=160;

    //IGUIButton* validate = guienv->addButton(myRect(driver->getScreenSize().Width-375,5,150,20),
	IGUIButton* validate = guienv->addButton(myRect(displaywidth-375,5,150,20),
                      tab1,
                      BT_ID_DYNAMIC_OBJECT_VALIDATE_SCRIPT,
                      stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_validate_script")).c_str() );
	validate->setOverrideFont(guiFontC12);
	validate->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

	IGUIButton* close = guiDynamicObjects_Script_Close = guienv->addButton(myRect(guiDynamicObjectsWindowEditAction->getClientRect().getWidth()-90,10,82,20),
                      guiDynamicObjectsWindowEditAction,
                      BT_ID_DYNAMIC_OBJECT_SCRIPT_CLOSE,
                      stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_close_script")).c_str() );
	close->setOverrideFont(guiFontC12);
	close->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);


	// Console window
    guiDynamicObjects_Script_Console = guienv->addEditBox(L"",
                                                          //myRect(2,5,driver->getScreenSize().Width-380,70),
														  myRect(2,5,displaywidth-380,70),
                                                          true,
                                                          tab1,
                                                          EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE);

    guiDynamicObjects_Script_Console->setOverrideColor(SColor(255,255,0,0));
    guiDynamicObjects_Script_Console->setEnabled(false);
	guiDynamicObjects_Script_Console->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);


	guiDynamicObjects_LoadScriptTemplateCB->bringToFront(guiDynamicObjects_LoadScriptTemplateCB);
	guiDynamicObjectsWindowEditAction->setVisible(false);



    ///LOAD HELP IMAGES
    helpTerrainTransform = App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/art/help_terrain_transform.png");

    helpVegetationPaint = App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/art/help_vegetation_paint.png");

    helpTerrainSegments = App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/art/help_terrain_segments.png");

    logo1 = App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/art/logo1.png");

    configWindow = new GUIConfigWindow(App::getInstance()->getDevice());

}

#endif

bool GUIManager::isGuiPresent(vector2d<s32> mousepos)
// will tell the caller if he's clicked inside a IRB window
{

	if (guidialog->isVisible() && guidialog->isPointInside(mousepos))
	    return true;

	// This one is special, the gameplay bar has a Health that takes 100% height while the buttons take half
	// of the bottom of the image. So we check if the mouse is in the element and check if it reaches the buttons
	if (gameplay_bar_image->isVisible() && gameplay_bar_image->isPointInside(mousepos))
	{
		s32 startpos = (gameplay_bar_image->getAbsolutePosition().UpperLeftCorner.Y-15) + (gameplay_bar_image->getAbsolutePosition().getHeight()/2);
		if (mousepos.Y>startpos)
			return true;
	}
		

	if (guiWindowItems->isVisible() && guiWindowItems->isPointInside(mousepos))
		return true;
	if (consolewin->isVisible() && consolewin->isPointInside(mousepos))
		return true;
#ifdef EDITOR
	if (guiDynamicObjectsWindowChooser->isVisible() && guiDynamicObjectsWindowChooser->isPointInside(mousepos))
		return true;
	if (guiTerrainToolbar->isVisible() && guiTerrainToolbar->isPointInside(mousepos))
	{
		getScrollBarValue(SC_ID_TERRAIN_BRUSH_RADIUS);
        getScrollBarValue(SC_ID_TERRAIN_BRUSH_STRENGTH);
		getScrollBarValue(SC_ID_TERRAIN_BRUSH_PLATEAU);
		return true;
	}
	if (guiLoaderWindow->isVisible() && guiLoaderWindow->isPointInside(mousepos))
		return true;
	if (guiAboutWindow->isVisible() && guiAboutWindow->isPointInside(mousepos))
		return true;
	if (guiDynamicObjectsWindowEditAction->isVisible() && guiDynamicObjectsWindowEditAction->isPointInside(mousepos))
		return true;
	if (guiDynamicObjects_Context_Menu_Window->isVisible() && guiDynamicObjects_Context_Menu_Window->isPointInside(mousepos))
		return true;

	// old stuff (IRRlicht only editor)
	if (guiMainWindow->isVisible() && guiMainWindow->isPointInside(mousepos))
		return true;
#endif
	if (guiMainToolWindow->isVisible() && guiMainToolWindow->isPointInside(mousepos))
		return true;


	return false;
}

void GUIManager::UpdateGUIChooser(int objType)
{
	guiDynamicObjects_OBJChooser->clear();
	std::vector<stringc> listDynamicObjs = DynamicObjectsManager::getInstance()->getObjectsList((TYPE)objType);

    for (int i=0 ; i<(int)listDynamicObjs.size() ; i++)
    {
    	guiDynamicObjects_OBJChooser->addItem( stringw( listDynamicObjs[i] ).c_str() );
    }
}

void GUIManager::setTextLoader(stringw text)
{
	if (guiLoaderDescription)
	{
		guiLoaderDescription->setText(text.c_str());
		App::getInstance()->quickUpdate();
	}
}

void GUIManager::setupGameplayGUI()
{

    IVideoDriver* driver = App::getInstance()->getDevice()->getVideoDriver();

    fader=guienv->addInOutFader();
	fader->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
    fader->setVisible(false);

	// NEW Create display size since IRRlicht return wrong values
	// Check the current screen size (normally reported by wxWidget or the configuration)

	displayheight=screensize.Height;
	displaywidth=screensize.Width;

	// This is called only in the PLAYER application
	#ifndef EDITOR
	// ----------------------------------------
    //guienv->getSkin()->setFont(guiFontC12);
	guienv->getSkin()->setFont(guiFontCourier12);
	// Load textures
	ITexture* imgLogo = driver->getTexture("../media/art/logo1.png");

	//LOADER WINDOW
	guiLoaderWindow = guienv->addWindow(myRect(driver->getScreenSize().Width/2-300, driver->getScreenSize().Height/2-200,600,400),false,L"Loading...");
	guiLoaderWindow->setDrawTitlebar(false);
	guiLoaderWindow->getCloseButton()->setVisible(false);

	guienv->addImage(imgLogo,vector2d<s32>(200,50),true,guiLoaderWindow);
	guiLoaderDescription = guienv->addStaticText(L"Loading interface graphics...",myRect(10,350,580,40),true,true,guiLoaderWindow,-1,false);
	//printf("The GUI should display from here...\n");
	// quick update
	App::getInstance()->quickUpdate();

	// Buttons
	ITexture* imgCloseProgram = driver->getTexture("../media/art/bt_close_program.png");
	ITexture* imgAbout = driver->getTexture("../media/art/bt_about.png");
	ITexture* imgAbout1 = driver->getTexture("../media/art/bt_about_ghost.png");
	ITexture* imgHelp = driver->getTexture("../media/art/bt_help.png");
	ITexture* imgHelp1 = driver->getTexture("../media/art/bt_help_ghost.png");
	ITexture* imgConfig = driver->getTexture("../media/art/bt_config.png");
	ITexture* imgConfig1 = driver->getTexture("../media/art/bt_config_ghost.png");

	guiMainToolWindow = guienv->addWindow(myRect(driver->getScreenSize().Width-170,0,170,46),false);
	guiMainToolWindow->setDraggable(false);
	guiMainToolWindow->setDrawTitlebar(false);
	guiMainToolWindow->getCloseButton()->setVisible(false);


	//Play Game
	int x = 0;
	mainToolbarPos.Y=5;
    guiPlayGame= guienv->addButton(myRect(10+x,mainToolbarPos.Y,32,32),
                                     guiMainToolWindow,
                                     BT_ID_PLAY_GAME,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_play_game")).c_str());

    guiPlayGame->setImage(driver->getTexture("../media/art/bt_play_game.png"));


    //Stop Game
    guiStopGame= guienv->addButton(myRect(10+x,mainToolbarPos.Y,32,32),
                                     guiMainToolWindow,
                                     BT_ID_STOP_GAME,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_stop_game")).c_str());

    guiStopGame->setImage(driver->getTexture("../media/art/bt_stop_game.png"));
    guiStopGame->setVisible(false);



    //ABOUT BUTTON
	x += 42;
    guiAbout = guienv->addButton(myRect(10+x,mainToolbarPos.Y,32,32),
                                     guiMainToolWindow,
                                     BT_ID_ABOUT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_about")).c_str() );

    guiAbout->setImage(imgAbout);
	guiAbout->setPressedImage(imgAbout1);

	// Help Button
	x += 42;
    guiHelpButton = guienv->addButton(myRect(10+x,mainToolbarPos.Y,32,32),
                                     guiMainToolWindow,
                                     BT_ID_HELP,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_help")).c_str() );

    guiHelpButton->setImage(imgHelp);
    guiHelpButton->setPressedImage(imgHelp1);

	// Close program
	x += 42;
	guiCloseProgram = guienv->addButton(myRect(10+x,mainToolbarPos.Y,32,32),
                                     guiMainToolWindow,
                                     BT_ID_CLOSE_PROGRAM,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_close_program")).c_str() );

    guiCloseProgram->setImage(imgCloseProgram);

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

	// Ask the LANGManager to fill the box with the proper Language of the about text.
	LANGManager::getInstance()->setAboutText(guiAboutText);

	// ---------------------------------------
	#endif
	consolewin = guienv->addWindow(rect<s32>(20,20,800,400),false,L"Console window",0,GCW_CONSOLE);
	consolewin->getCloseButton()->setVisible(false);
	console = guienv->addListBox(myRect(10,30,700,325),consolewin,0,true);
	console->setAutoScrollEnabled(false);
	console->setItemHeight(20);
	consolewin->setVisible(false);

	// --- Active game menu during play
	ITexture* gameplay_bar = driver->getTexture("../media/art/gameplay_bar.png");
	ITexture* circle = driver->getTexture("../media/art/circle.png");
	ITexture* circleMana = driver->getTexture("../media/art/circlemana.png");
	ITexture* topCircle = driver->getTexture("../media/art/circle_top.png");

	// The bottom image of the interface
	gameplay_bar_image = guienv->addImage(gameplay_bar,vector2d<s32>((displaywidth/2)-(gameplay_bar->getSize().Width/2),displayheight-gameplay_bar->getSize().Height),true);
	gameplay_bar_image->setAlignment(EGUIA_CENTER,EGUIA_CENTER,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);

	// The life gauge
	lifegauge = new gui::CGUIGfxStatus(guienv, gameplay_bar_image,myRect((gameplay_bar->getSize().Width/2)-60,gameplay_bar->getSize().Height-128,128,128),-1);
	lifegauge->setImage(circle);
	lifegauge->ViewHalfLeft();

	// The mana gauge
	managauge = new gui::CGUIGfxStatus(guienv, gameplay_bar_image,myRect((gameplay_bar->getSize().Width/2)-60,gameplay_bar->getSize().Height-128,128,128),-1);
	managauge->setImage(circleMana);
	managauge->ViewHalfRight();

	// The image over the circle
	IGUIImage* circle_overlay =
		guienv->addImage(topCircle,vector2d<s32>((gameplay_bar->getSize().Width/2)-64,gameplay_bar->getSize().Height-128),true,gameplay_bar_image);

	gameplay_bar_image->setVisible(false);

    ///DIALOG
    guiDialogImgYes = driver->getTexture("../media/art/img_yes.png");
    guiDialogImgYes_s = driver->getTexture("../media/art/img_yes_s.png");
    guiDialogImgNo = driver->getTexture("../media/art/img_no.png");
    guiDialogImgNo_s = driver->getTexture("../media/art/img_no_s.png");


    //view items
    guiBtViewItems = guienv->addButton(myRect(465,85,48,48),
		//displaywidth/2 + 80,displayheight - 57,48,48),
                                     gameplay_bar_image,
                                     BT_ID_VIEW_ITEMS,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_view_items")).c_str() );

    guiBtViewItems->setImage(driver->getTexture("../media/art/bt_view_items.png"));
    guiBtViewItems->setVisible(false);

    //Items window

    guiWindowItems = guienv->addWindow(myRect(100,100,displaywidth-200,displayheight-150),false,L"",0,GCW_GAMEPLAY_ITEMS);
    guiWindowItems->getCloseButton()->setVisible(false);
    guiWindowItems->setDrawTitlebar(false);
    guiWindowItems->setDraggable(false);
	guiWindowItems->setAlignment(EGUIA_CENTER,EGUIA_CENTER,EGUIA_CENTER,EGUIA_CENTER);
    gameTabCtrl = guienv->addTabControl(core::rect<s32>(10,30,displaywidth-240,displayheight-200),guiWindowItems,false,true,-1);
	IGUITab * tab1 = gameTabCtrl->addTab(L"Character stats");
	IGUITab * tab2 = gameTabCtrl->addTab(L"Inventory");
	IGUITab * tab3 = gameTabCtrl->addTab(L"Skills");
	IGUITab * tab4 = gameTabCtrl->addTab(L"Quests");


	guiPlayerNodePreview = new NodePreview(guienv,tab1,rect<s32>(440,40,740,370),-1);
	guiPlayerNodePreview->drawBackground(false);

	//DynamicObjectsManager::getInstance()->setActiveObject("player_template");

	//guiPlayerNodePreview->setNode(DynamicObjectsManager::getInstance()->getActiveObject()->getNode());
	guiPlayerNodePreview->setNode(Player::getInstance()->getNodeRef());
	DynamicObjectsManager::getInstance()->setActiveObject("peasant");
	//printf("This is the node name: %s\n",DynamicObjectsManager::getInstance()->getActiveObject()->getName());
	guiPlayerNodePreview->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

    guiPlayerItems = guienv->addListBox(myRect(10,30,200,displayheight-340),tab2,LB_ID_PLAYER_ITEMS,true);

    guiBtUseItem = guienv->addButton(myRect(10,displayheight-300,32,32),
                                         tab2,
                                         BT_ID_USE_ITEM,
                                         L"",
                                         stringw(LANGManager::getInstance()->getText("bt_use_item")).c_str());
    guiBtUseItem->setImage(driver->getTexture("../media/art/bt_yes_32.png"));

    guiBtDropItem = guienv->addButton(myRect(52,displayheight-300,32,32),
                                         tab2,
                                         BT_ID_DROP_ITEM,
                                         L"",
                                         stringw(LANGManager::getInstance()->getText("bt_drop_item")).c_str());
    guiBtDropItem->setImage(driver->getTexture("../media/art/bt_no_32.png"));


    guiBtCloseItemsWindow = guienv->addButton(myRect(displaywidth-210-32,displayheight-160 - 32,32,32),
                                         guiWindowItems,
                                         BT_ID_CLOSE_ITEMS_WINDOW,
                                         L"",
                                         stringw(LANGManager::getInstance()->getText("bt_close_items_window")).c_str());
    guiBtCloseItemsWindow->setImage(driver->getTexture("../media/art/bt_arrow_32.png"));
	guiWindowItems->setVisible(false);



	// TExt GUI for player stats

    guiPlayerMoney = guienv->addStaticText(L"GOLD:129",myRect(15,displayheight-300,300,32),false,false,tab1);
    guiPlayerMoney->setOverrideFont(guiFontLarge28);
    guiPlayerMoney->setOverrideColor(SColor(255,255,255,255));

	playerLifeText = LANGManager::getInstance()->getText("txt_player_life");

	guiPlayerLife_Shadow=guienv->addStaticText(stringw(playerLifeText).c_str(),myRect(14,5,600,30),false,false,tab1,-1,false);
    guiPlayerLife_Shadow->setOverrideColor(SColor(255,30,30,30));
    guiPlayerLife_Shadow->setOverrideFont(guiFontLarge28);

    guiPlayerLife=guienv->addStaticText(stringw(playerLifeText).c_str(),myRect(15,6,600,30),false,false,tab1,-1,false);
    guiPlayerLife->setOverrideColor(SColor(255,255,255,100));
    guiPlayerLife->setOverrideFont(guiFontLarge28);

	this->setElementVisible(ST_ID_PLAYER_LIFE, false);


	////// --------------------------------
	///    Define the Dialogs used in the game
	//////

	guidialog = guienv->addWindow(myRect(10,displayheight-200,displaywidth-20,190),true,L"",0,GCW_DIALOG);
	guidialog->getCloseButton()->setVisible(false);
	guidialog->setDrawTitlebar(false);
	guidialog->setDraggable(false);
	guidialog->setDrawBackground(false);
	guidialog->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);

	// Panel background is done with pictures
	IGUIImage* img1 = guienv->addImage(driver->getTexture("../media/art/panel_left.png"),vector2d<s32>(0,0),true,guidialog);
	IGUIImage* img2 = guienv->addImage(driver->getTexture("../media/art/panel_middle.png"),vector2d<s32>(51,0),true,guidialog);
	IGUIImage* img3 = guienv->addImage(driver->getTexture("../media/art/panel_right.png"),vector2d<s32>(581,0),true,guidialog);
	img2->setScaleImage(true);
	img2->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);
	img3->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);

	// Text display of the panel
	rect<s32> textRect = rect<s32>(30,25,600,170);
	txt_dialog = guienv->addStaticText(L"Hello! This is a simple test to see how the text is flowing inside the box. There is a test, test, and test of text we need to make to be sure the flowing is ok",textRect,false,false,guidialog,TXT_ID_DIALOG,false);
	txt_dialog->setOverrideFont(guiFontDialog);
	txt_dialog->setOverrideColor(SColor(255,255,255,255));
	txt_dialog->setWordWrap(true);
	txt_dialog->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);

	guiBtDialogYes = guienv->addButton(myRect(640,30,52,52),
                                         guidialog,
                                         BT_ID_DIALOG_YES,
                                         L"",
                                         stringw(LANGManager::getInstance()->getText("bt_dialog_yes")).c_str());
    guiBtDialogYes->setImage(guiDialogImgYes);
	guiBtDialogYes->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);
	guiBtDialogCancel = guienv->addButton(myRect(640,110,52,52),
                                         guidialog,
                                         BT_ID_DIALOG_CANCEL,
                                         L"",
                                         stringw(LANGManager::getInstance()->getText("bt_dialog_no")).c_str());
    guiBtDialogCancel->setImage(guiDialogImgNo);
	guiBtDialogCancel->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);
	guidialog->setVisible(false);

	//////
	stringw text = L"Current screen size is:";
	text.append((stringw)screensize.Width);
	text.append(L",");
	text.append((stringw)screensize.Height);
	this->setConsoleText(text.c_str(),SColor(255,0,0,255));
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
			if (guiWindowItems)
				guiWindowItems->setVisible(visible);
            break;
        case GCW_ABOUT:
            guiAboutWindow->setVisible(visible);
            break;
        case GCW_TERRAIN_PAINT_VEGETATION:
            guiVegetationToolbar->setVisible(visible);
            break;
		case GCW_DIALOG:
			this->guidialog->setVisible(visible);
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

void GUIManager::updateGuiPositions(dimension2d<u32> screensize)
{
	// This update for the new screen size... Needed by the images GUI
	this->screensize=screensize;
}
// Only in the editor
#ifdef EDITOR

void GUIManager::setEditBoxText(GUI_ID id, stringw text)
{
	switch(id)
    {
        case EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE:
            guiDynamicObjects_Script_Console->setText(text.c_str());
            break;
        case EB_ID_DYNAMIC_OBJECT_SCRIPT:
            guiDynamicObjects_Script->setText(text.c_str());
            break;
    }
}
#endif

void GUIManager::setElementEnabled(GUI_ID id, bool enable)
{
   /* switch(id)///TODO: fazer metodo getElement by ID!!!
    {
#ifdef EDITOR

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
		case BT_ID_NEW_PROJECT:
            guiMainNewProject->setEnabled(enable);
            guiMainNewProject->setPressed(!enable);
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
#endif
        case BT_ID_ABOUT:
            guiAbout->setEnabled(enable);
			guiAbout->setPressed(!enable);
            break;

        case BT_ID_HELP:
            guiHelpButton->setEnabled(enable);
            guiHelpButton->setPressed(!enable);
            break;

    }*/
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
#ifdef EDITOR
			//if (App::getInstance()->wxSystemState==false)
				guiMainWindow->setVisible(!visible);
#endif
			// the bar_image should be made into an element too
			// gameplay_bar_image->setVisible(visible);
            break;
        case ST_ID_PLAYER_LIFE:
            guiPlayerLife->setVisible(visible);
            guiPlayerLife_Shadow->setVisible(visible);
            break;
        case BT_ID_PLAYER_EDIT_SCRIPT:
            //guiPlayerEditScript->setVisible(visible);
            break;
		case IMG_BAR:
			gameplay_bar_image->setVisible(visible);
			break;
		case CONSOLE:
			consolewin->setVisible(visible);
			break;
		case BT_ID_VIEW_ITEMS:
            guiBtViewItems->setVisible(visible);
			// Update the gold items
			stringc playerMoney = LANGManager::getInstance()->getText("txt_player_money");
			playerMoney += Player::getInstance()->getObject()->getMoney();
			this->setStaticTextText(ST_ID_PLAYER_MONEY,playerMoney);
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

    //printf("MESSAGE:%s\n",msg.c_str());

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
	guiFont6 = guienv->getFont("../media/fonts/Arial6.xml");
	guiFont8 = guienv->getFont("../media/fonts/Arial8.xml");
	guiFont9 = guienv->getFont("../media/fonts/Trebuchet10.xml");
	guiFont10 = guienv->getFont("../media/fonts/Arial10.xml");
	guiFont12 = guienv->getFont("../media/fonts/Arial12.xml");
	guiFont14 = guienv->getFont("../media/fonts/Arial14.xml");

	guiFont10->setKerningWidth(-1);
	guiFont9->setKerningWidth(-2);
	guiFont8->setKerningWidth(-1);
	guiFont8->setKerningHeight(-6);
}

void GUIManager::setStaticTextText(GUI_ID id, stringc text)
{
    switch(id)
    {
        case ST_ID_PLAYER_LIFE:
			if (guiWindowItems->isVisible())
			{
				guiPlayerLife->setText(stringw(text).c_str());
				guiPlayerLife_Shadow->setText(stringw(text).c_str());
			}
            break;
        case ST_ID_PLAYER_MONEY:
            guiPlayerMoney->setText(stringw(text).c_str());
            break;
    }
}

void GUIManager::setConsoleText(stringw text, video::SColor color)
// Add text into the output console
// The function manage up to 2000 lines before clearing the buffer
// Using "forcedisplay" will toggle the display of the GUI
{
	u32 maxitem = 2000;
	// If the GUI is not displayed, accumulate the info in a buffer
	if (textevent.size()<maxitem)
	{
		textevent.push_back(text);
		texteventcolor.push_back(color);
#ifdef _wxWIDGET
		// This is sent to the APP Class for the wxWidget console
		App::getInstance()->console_event.push_back(text);
		App::getInstance()->console_event_color.push_back(color);
#endif
	}

	// This part will update the IRRlicht type console
	if (consolewin && consolewin->isVisible())
	{
		for (int a=0; a<(int)textevent.size(); a++)
		{
			if (console->getItemCount()>maxitem-1)
				console->removeItem(maxitem);

			console->insertItem(0,textevent[a].c_str(),0);
			console->setItemOverrideColor(0,texteventcolor[a]);
		}
		textevent.clear();
		texteventcolor.clear();
	}

}

void GUIManager::clearConsole() // The APP Class can clear the buffer because the wxWidget console need it.
{
	textevent.clear();
	texteventcolor.clear();
}

void GUIManager::setConsoleLogger(vector<core::stringw> &text)
{
	u32 maxitem = 500;
	if (consolewin && consolewin->isVisible())
	{
		if (text.size()>0)
		{
			for (int a=0; a<(int)text.size(); a++)
			{

				if (console->getItemCount()>maxitem-1)
					console->removeItem(maxitem);

				//
				console->insertItem(0,text[a].subString(0,90).c_str(),0);
				console->setItemOverrideColor(0,video::SColor(255,0,0,0));

				//text.pop_back();

			}
			text.clear();

		}

	}
}

void GUIManager::stopDialogSound()
{
	 //stop sound when player cancel the dialog
    if(dialogSound)
    {
        dialogSound->stop();
    }

}

void GUIManager::showDialogMessage(stringw text, std::string sound)
{

	//stringw text2 = (stringw)text.c_str();
	txt_dialog->setText(text.c_str());
	if(guiBtDialogCancel->isVisible())
		guiBtDialogCancel->setVisible(false);

	setWindowVisible(GCW_DIALOG,true);
	App::getInstance()->setAppState(APP_WAIT_DIALOG);

	//Play dialog sound (yes you can record voices!)
    dialogSound = NULL;

	if (sound.size()>0)
    //if((sound.c_str() != "") | (sound.c_str() != NULL))
    {
        stringc soundName = "../media/sound/";
        soundName += sound.c_str();
        dialogSound = SoundManager::getInstance()->playSound2D(soundName.c_str());
    }

}

bool GUIManager::showDialogQuestion(stringw text, std::string sound )
{

	//stringw text2 = (stringw)text.c_str();
	txt_dialog->setText(text.c_str());
	if(!guiBtDialogCancel->isVisible())
		guiBtDialogCancel->setVisible(true);

	setWindowVisible(GCW_DIALOG,true);
	App::getInstance()->setAppState(APP_WAIT_DIALOG);

	//Play dialog sound (yes you can record voices!)
    dialogSound = NULL;

	if (sound.size()>0)
    //if((sound.c_str() != "") | (sound.c_str() != NULL))
    {
        stringc soundName = "../media/sound/";
        soundName += sound.c_str();
        dialogSound = SoundManager::getInstance()->playSound2D(soundName.c_str());
    }

	return true;
}

stringc GUIManager::showInputQuestion(stringw text)
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
    vector<stringc> items = Player::getInstance()->getObject()->getItems();

    for(int i = 0; i<(int)items.size(); i++) guiPlayerItems->addItem( stringw(items[i]).c_str() );
}

void GUIManager::flush()
{
#ifdef EDITOR
    guiMainLoadProject->setPressed(false);
    guiMainSaveProject->setPressed(false);
    guiMainNewProject->setPressed(false);
    guiDynamicObjects_LoadScriptTemplateBT->setPressed(false);
#endif
    guiBtViewItems->setPressed(false);
}

void GUIManager::showConfigWindow()
{
    APP_STATE old_State = App::getInstance()->getAppState();
    App::getInstance()->setAppState(APP_EDIT_WAIT_GUI);
    configWindow->showWindow();
    App::getInstance()->setAppState(old_State);
}
