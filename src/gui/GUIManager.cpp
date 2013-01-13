#include "GUIManager.h"

#include "../LANGManager.h"
#include "../objects/DynamicObjectsManager.h"
#include "../events/EventReceiver.h"
#include "../sound/SoundManager.h"
#include "../objects/Player.h"
#include "../camera/CameraSystem.h"

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
	driver = App::getInstance()->getDevice()->getVideoDriver();
	screensize = App::getInstance()->getScreenSize();

	// init those because they will move on the display.
	guiDynamicObjectsWindowEditAction=NULL;
	guiDynamicObjectsWindowChooser=NULL;
	guiDynamicObjects_NodePreview=NULL;
	guiTerrainToolbar=NULL;
	guiWindowItems=NULL;
	consolewin=NULL;
	guiLoaderDescription = NULL;
	info_none=NULL;
	info_current=NULL;

	//Init the fonts
	guiFontCourier12 = NULL;
    guiFontLarge28 = NULL;
    guiFontDialog = NULL;
	guiFont6 = NULL;
	guiFont8 = NULL;
	guiFont9 = NULL;
	guiFont10 = NULL;
	guiFont12 = NULL;
	guiFont14 = NULL;
	// Load the required font
	guiFontC12 = guienv->getFont("../media/fonts/char12.xml");

	timer = App::getInstance()->getDevice()->getTimer()->getRealTime();
	timer2 = timer;

	for (s32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
    {
            video::SColor col = guienv->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
            col.setAlpha(230);
            guienv->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
    }

	// Fake office style skin colors
	// We should allow creation of skins colors by the users or at least a choice of skins to use
	//guienv->getSkin()->setColor(EGDC_3D_SHADOW,video::SColor(200,140,178,226));
	//guienv->getSkin()->setColor(EGDC_3D_FACE,video::SColor(200,204,227,248));
	//guienv->getSkin()->setColor(EGDC_WINDOW,video::SColor(255,220,220,220));
}

GUIManager::~GUIManager()
{

	if (managauge)
		delete managauge;

	if (lifegauge)
		delete lifegauge;

	if (guiDynamicObjects_NodePreview)
		delete guiDynamicObjects_NodePreview;

	if (guiPlayerNodePreview)
		delete guiPlayerNodePreview;

	if (guiDynamicObjects_Script)
		delete guiDynamicObjects_Script;

	if (configWindow)
		delete configWindow;

	if (consolewin)
		delete consolewin;

	if (guiDynamicObjectsWindowEditAction)
		delete guiDynamicObjectsWindowEditAction;

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
    switch(img)
    {
        case HELP_TERRAIN_TRANSFORM:
            driver->draw2DImage(helpTerrainTransform, position2di(0,screensize.Height-20 - helpTerrainTransform->getSize().Height),
				myRect(0,0,helpTerrainTransform->getSize().Width,helpTerrainTransform->getSize().Height), 0,
				video::SColor(255,255,255,255), true);
            break;
        case HELP_TERRAIN_SEGMENTS:
            driver->draw2DImage(helpTerrainSegments, position2di(0,screensize.Height-20 - helpTerrainSegments->getSize().Height),
				myRect(0,0,helpTerrainSegments->getSize().Width,helpTerrainSegments->getSize().Height), 0,
				video::SColor(255,255,255,255), true);
            break;
        case HELP_VEGETATION_PAINT:
            driver->draw2DImage(helpVegetationPaint, position2di(0,screensize.Height-20 - helpVegetationPaint->getSize().Height),
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
	/*
    switch(id)
    {
        default:
            break;
    }
	*/
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
        default:
            break;
    }
    return 0;
}

stringc GUIManager::getComboBoxItem(GUI_ID id)
{
    switch(id)
    {
	
		case CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER:
			return stringc(guiDynamicObjects_OBJChooser->getListItem(guiDynamicObjects_OBJChooser->getSelected()));
            break;
		case CO_ID_DYNAMIC_OBJECT_OBJ_CATEGORY:
			return stringc(guiDynamicObjects_Category->getItem(guiDynamicObjects_Category->getSelected()));
			break;
        case CO_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE:
            return stringc(guiDynamicObjects_LoadScriptTemplateCB->getItem(guiDynamicObjects_LoadScriptTemplateCB->getSelected()));
            break;
        default:
            break;
    }
    return "";
}

core::stringw GUIManager::getEditCameraString(ISceneNode* node)
{
	// A bug that crash if I a "node" is "there" but pointer invalid. Will have to investigate more on this.
	core::stringw sct =L"";

	// Display this when working with segments or when there no node selected.
	if (!node || App::getInstance()->getAppState()==APP_EDIT_TERRAIN_EMPTY_SEGMENTS)
	{
		sct += L"Camera position: ";
		core::vector3df pos = CameraSystem::getInstance()->getNode()->getPosition();
		sct+=(core::stringw)pos.X;
		sct+=L",";
		sct+=(core::stringw)pos.Y;
		sct+=L",";
		sct+=(core::stringw)pos.Z;
		sct+=L"    Target:  ";
		pos = CameraSystem::getInstance()->getNode()->getTarget();
		sct+=(core::stringw)pos.X;
		sct+=L",";
		sct+=(core::stringw)pos.Y;
		sct+=L",";
		sct+=(core::stringw)pos.Z;
		return sct;
	}
		
	
	// When moving or rotating a dynamic object
	if (App::getInstance()->getAppState()==APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE)
		node=App::getInstance()->lastMousePick.pickedNode;

	// Display this when in object edit mode only.
	if ((App::getInstance()->getAppState()==APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE || App::getInstance()->getAppState()==APP_EDIT_DYNAMIC_OBJECTS_MODE) && node && node->getID()!=100)
		{
			core::vector3df pos = node->getPosition();
			core::vector3df rot = node->getRotation();

			sct += L"Object position: ";
			sct+=(core::stringw)pos.X;
			sct+=L",";
			sct+=(core::stringw)pos.Y;
			sct+=L",";
			sct+=(core::stringw)pos.Z;

			sct+=L"    Rotation:  ";
			sct+=(core::stringw)rot.X;
			sct+=L",";
			sct+=(core::stringw)rot.Y;
			sct+=L",";
			sct+=(core::stringw)rot.Z;
			return sct;
		}
		//} else
		//	printf ("failed to retrieve node\n");
	//}
	return sct;
}

void GUIManager::setupEditorGUI()
{

	
	// Load textures
	//ITexture* imgLogo = driver->getTexture("../media/art/logo1.png");
	ITexture* imgLogo = driver->getTexture("../media/art/title.jpg");
	ITexture* info_none = driver->getTexture("../media/editor/info_none.jpg");
	if (info_none)
		info_current=info_none;

// NEW Create display size since IRRlicht return wrong values

	// Check the current screen size
	displayheight=screensize.Height;
	displaywidth=screensize.Width;

	//LOADER WINDOW
	guiLoaderWindow = guienv->addWindow(myRect(displaywidth/2-300,displayheight/2-200,600,400),false,L"Loading...");
	guiLoaderWindow->setDrawTitlebar(false);
	guiLoaderWindow->getCloseButton()->setVisible(false);
	guiLoaderWindow->setAlignment(EGUIA_CENTER,EGUIA_CENTER,EGUIA_CENTER,EGUIA_CENTER);

	//guienv->addImage(imgLogo,vector2d<s32>(200,50),true,guiLoaderWindow);
	guienv->addImage(imgLogo,vector2d<s32>(5,5),true,guiLoaderWindow);
	guiLoaderDescription = guienv->addStaticText(L"Loading fonts...",myRect(10,350,580,40),true,true,guiLoaderWindow,-1,false);
	App::getInstance()->quickUpdate();

	loadFonts();
	guienv->getSkin()->setFont(guiFont10);
	guiLoaderDescription->setText(L"Loading interface graphics...");

	// quick update of the Irrlicht display while loading.
	App::getInstance()->quickUpdate();

	// loading others images (mostly for buttons)
	backtexture = driver->getTexture("../media/art/back.png");
	imgNewProject = driver->getTexture("../media/art/bt_new_project.png");
	imgNewProject1 = driver->getTexture("../media/art/bt_new_project_ghost.png");

	App::getInstance()->quickUpdate();

	imgLoadProject = driver->getTexture("../media/art/bt_load_project.png");
	imgLoadProject1 = driver->getTexture("../media/art/bt_load_project_ghost.png");
	imgSaveProject = driver->getTexture("../media/art/bt_save_project.png");
	imgSaveProject1 = driver->getTexture("../media/art/bt_save_project_ghost.png");
	imgCloseProgram = driver->getTexture("../media/art/bt_close_program.png");

	App::getInstance()->quickUpdate();

	imgAbout = driver->getTexture("../media/art/bt_about.png");
	imgAbout1 = driver->getTexture("../media/art/bt_about_ghost.png");
	imgHelp = driver->getTexture("../media/art/bt_help.png");
	imgHelp1 = driver->getTexture("../media/art/bt_help_ghost.png");
	imgConfig = driver->getTexture("../media/art/bt_config.png");
	imgConfig1 = driver->getTexture("../media/art/bt_config_ghost.png");

	// Status bar
	guiStatus = guienv->addWindow(myRect(0,displayheight-25,displaywidth,displayheight),false);
	guiStatus->setDraggable(false);
	guiStatus->setDrawTitlebar(false);
	guiStatus->getCloseButton()->setVisible(false);
	guiStatus->setDrawBackground(true);
	guiStatus->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);

	guiStatusText = guienv->addStaticText(L"Welcome to IRR RPG Builder!",myRect(10,4,displaywidth-200,18),false,false,guiStatus);

	guiStatusCameraText = guienv->addStaticText(getEditCameraString(NULL).c_str(),myRect(displaywidth-600,4,displaywidth-700,18),true,false,guiStatus);
	guiStatusCameraText->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);

	// Update and refresh the display
	App::getInstance()->quickUpdate();

	//Create the main toolbar GUI;
	createMainToolbar();
		
	// Update and refresh the display
	App::getInstance()->quickUpdate();

	// Create the about Window GUI
	createAboutWindowGUI();

	// Create the terrain toolbar GUI
	createTerrainToolbar();

	// Create the Dynamic Object Info panel GUI
	createDynamicObjectInfoGUI();
	
	// Create the Dynamic Object Chooser GUI
	createDynamicObjectChooserGUI();
	
	// Create the Editor context menu GUI
	createContextMenuGUI();

	// Create the Code Editor GUI
	createCodeEditorGUI();

    ///LOAD HELP IMAGES
    helpTerrainTransform = App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/art/help_terrain_transform.png");
    helpVegetationPaint = App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/art/help_vegetation_paint.png");
    helpTerrainSegments = App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/art/help_terrain_segments.png");
    
	// Get the logo
	if (imgLogo)
		logo1 = imgLogo;
	else
		logo1 = App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/art/logo1.png");

	// Create the Configuration window (Need to be updated)
    configWindow = new GUIConfigWindow(App::getInstance()->getDevice());

	// Update and refresh the display
	App::getInstance()->quickUpdate();

}

void GUIManager::createMainToolbar()
{
	// Standard Main toolbar
    //guiMainWindow = guienv->addWindow(myRect(0,0,driver->getScreenSize().Width-170,92),false);
	guiMainWindow = guienv->addWindow(myRect(0,0,displaywidth-220,122),false);
    guiMainWindow->setDraggable(false);
    guiMainWindow->setDrawTitlebar(false);
	guiMainWindow->getCloseButton()->setVisible(false);
	guiMainWindow->setDrawBackground(false);
	guiMainWindow->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

	//guiMainToolWindow = guienv->addWindow(myRect(driver->getScreenSize().Width-170,0,170,46),false);
	guiMainToolWindow = guienv->addWindow(myRect(displaywidth-220,0,220,120),false);
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
	
	// Create the tabs of the main toolbar
	createMainTabs();
	
}

void GUIManager::createProjectTab()
{
	// project TAB
	prjTabCtrl = guienv->addTabControl(myRect(5,2,250,112),guiMainWindow,true,true);
	tabProject = prjTabCtrl->addTab(LANGManager::getInstance()->getText("tab_project").c_str());

	// Tab description box text
	IGUIStaticText * projectTabText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des0")).c_str(),
		core::rect<s32>(0,64,250,76),false,true,tabProject,-1);
	//projectTabText->setBackgroundColor(video::SColor(128,237,242,248));
	//projectTabText->setOverrideColor(video::SColor(255,65,66,174));
	projectTabText->setBackgroundColor(video::SColor(255,238,240,242));
	projectTabText->setOverrideColor(video::SColor(255,86,95,109));
	projectTabText->setOverrideFont(guiFont10);
	projectTabText->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	// Buttons
	 s32 x = 0;

	mainToolbarPos.Y=3;
	// Close program
	x += 12;
	guiCloseProgram = guienv->addButton(myRect(x,mainToolbarPos.Y,32,32),
                                     tabProject,
                                     BT_ID_CLOSE_PROGRAM,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_close_program")).c_str() );

    guiCloseProgram->setImage(imgCloseProgram);

	IGUIStaticText * closeText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_close_program")).c_str(),
		core::rect<s32>(x-5,36,x+40,65),false,true,tabProject,-1);
	//closeText->setOverrideColor(video::SColor(255,65,66,174));
	closeText->setOverrideColor(video::SColor(255,64,64,64));
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
	//newPText->setOverrideColor(video::SColor(255,65,66,174));
	newPText->setOverrideColor(video::SColor(255,64,64,64));	
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
	//loadPText->setOverrideColor(video::SColor(255,65,66,174));
	loadPText->setOverrideColor(video::SColor(255,64,64,64));
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
	//savePText->setOverrideColor(video::SColor(255,65,66,174));
	savePText->setOverrideColor(video::SColor(255,64,64,64));
	savePText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	savePText->setOverrideFont(guiFont9);

}

void GUIManager::createPlayTab()
{
	// Play TAB
	mainToolCtrl = guienv->addTabControl(myRect(2,2,164,112),guiMainToolWindow,true,true);
	tabPlayTool = mainToolCtrl->addTab(LANGManager::getInstance()->getText("txt_tool_des4").c_str());

	//Play Game
	s32 x = 12;
	mainToolbarPos.Y=3;
    guiPlayGame= guienv->addButton(myRect(x,mainToolbarPos.Y,32,32),
                                     tabPlayTool,
                                     BT_ID_PLAY_GAME,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_play_game")).c_str());

    guiPlayGame->setImage(driver->getTexture("../media/art/bt_play_game.png"));

	IGUIStaticText * playGText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_play_game")).c_str(),
		core::rect<s32>(x-5,36,x+40,65),false,true,tabPlayTool,-1);
	//playGText->setOverrideColor(video::SColor(255,65,66,174));
	playGText->setOverrideColor(video::SColor(255,64,64,64));
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
	//aboutBText->setOverrideColor(video::SColor(255,65,66,174));
	aboutBText->setOverrideColor(video::SColor(255,64,64,64));
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
	//helpBText->setOverrideColor(video::SColor(255,65,66,174));
	helpBText->setOverrideColor(video::SColor(255,64,64,64));
	helpBText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);

}

void GUIManager::createEnvironmentTab()
{
	mainToolbarPos.Y=3;
	tabEnv = mainTabCtrl->addTab(LANGManager::getInstance()->getText("tab_environment").c_str());
	// Tab description box text
	IGUIStaticText * environmentTabText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des5")).c_str(),
		core::rect<s32>(0,64,180,76),false,true,tabEnv,-1);
	//environmentTabText->setBackgroundColor(video::SColor(128,237,242,248));
	//environmentTabText->setOverrideColor(video::SColor(255,65,66,174));
	environmentTabText->setBackgroundColor(video::SColor(255,238,240,242));
	environmentTabText->setOverrideColor(video::SColor(255,86,95,109));
	environmentTabText->setOverrideFont(guiFont10);
	environmentTabText->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	IGUIStaticText * vegetationTabText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des6")).c_str(),
		core::rect<s32>(190,64,300,76),false,true,tabEnv,-1);
	//vegetationTabText->setBackgroundColor(video::SColor(128,237,242,248));
	//vegetationTabText->setOverrideColor(video::SColor(255,65,66,174));
	vegetationTabText->setBackgroundColor(video::SColor(255,238,240,242));
	vegetationTabText->setOverrideColor(video::SColor(255,86,95,109));
	vegetationTabText->setOverrideFont(guiFont10);
	vegetationTabText->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	//Buttons
	//Transform Terrain
	s32 x=12;
    //Terrain Add Segment
    guiTerrainAddSegment = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabEnv,
                                     BT_ID_TERRAIN_ADD_SEGMENT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_terrain_segments")).c_str());

    guiTerrainAddSegment->setImage(driver->getTexture("../media/art/bt_terrain_add_segment.png"));
	guiTerrainAddSegment->setPressedImage(driver->getTexture("../media/art/bt_terrain_add_segment_ghost.png"));

	IGUIStaticText * terrainSText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_segments")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabEnv,-1);
	//terrainSText->setOverrideColor(video::SColor(255,65,66,174));
	terrainSText->setOverrideColor(video::SColor(255,64,64,64));
	terrainSText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	terrainSText->setOverrideFont(guiFont9);

	 x+= 60;
	//Add empty Segment
	guiTerrainAddEmptySegment = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabEnv,
                                     BT_ID_TERRAIN_ADD_EMPTY_SEGMENT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_terrain_empty_segments")).c_str());

    guiTerrainAddEmptySegment->setImage(driver->getTexture("../media/art/bt_terrain_add_segment.png"));
	guiTerrainAddEmptySegment->setPressedImage(driver->getTexture("../media/art/bt_terrain_add_segment_ghost.png"));

	IGUIStaticText * terrainSText2 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_empty_segments")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabEnv,-1);
	//terrainSText2->setOverrideColor(video::SColor(255,65,66,174));
	terrainSText2->setOverrideColor(video::SColor(255,64,64,64));
	terrainSText2->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	terrainSText2->setOverrideFont(guiFont9);

	 x+= 60;

	guiTerrainTransform = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabEnv,
                                     BT_ID_TERRAIN_TRANSFORM,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_terrain_transform")).c_str());

    guiTerrainTransform->setImage(driver->getTexture("../media/art/bt_terrain_up.png"));
	guiTerrainTransform->setPressedImage(driver->getTexture("../media/art/bt_terrain_up_ghost.png"));

	IGUIStaticText * terrainTText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_transform")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabEnv,-1);
	//terrainTText->setOverrideColor(video::SColor(255,65,66,174));
	terrainTText->setOverrideColor(video::SColor(255,64,64,64));
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
	//paintVText->setOverrideColor(video::SColor(255,65,66,174));
	paintVText->setOverrideColor(video::SColor(255,64,64,64));
	paintVText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	paintVText->setOverrideFont(guiFont9);
}

void GUIManager::createObjectTab()
{
	tabObject = mainTabCtrl->addTab(LANGManager::getInstance()->getText("tab_objects").c_str());
	// Tab description box text
	IGUIStaticText * objectTabText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des1")).c_str(),
		core::rect<s32>(0,64,120,76),false,true,tabObject,-1);
	//objectTabText->setBackgroundColor(video::SColor(128,237,242,248));
	//objectTabText->setOverrideColor(video::SColor(255,65,66,174));
	objectTabText->setBackgroundColor(video::SColor(255,238,240,242));
	objectTabText->setOverrideColor(video::SColor(255,86,95,109));
	objectTabText->setOverrideFont(guiFont10);
	objectTabText->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	// Tab description box text
	IGUIStaticText * objectTabText2 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des2")).c_str(),
		core::rect<s32>(130,64,250,76),false,true,tabObject,-1);
	//objectTabText2->setBackgroundColor(video::SColor(128,237,242,248));
	//objectTabText2->setOverrideColor(video::SColor(255,65,66,174));
	objectTabText2->setBackgroundColor(video::SColor(255,238,240,242));
	objectTabText2->setOverrideColor(video::SColor(255,86,95,109));
	objectTabText2->setOverrideFont(guiFont10);
	objectTabText2->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	// Buttons
	//Dynamic Objects
	s32 x = 12;
    guiDynamicObjectsMode= guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabObject,
                                     BT_ID_DYNAMIC_OBJECTS_MODE,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_mode")).c_str());

    guiDynamicObjectsMode->setImage(driver->getTexture("../media/art/bt_dynamic_objects_mode.png"));
	guiDynamicObjectsMode->setPressedImage(driver->getTexture("../media/art/bt_dynamic_objects_mode_ghost.png"));

	IGUIStaticText * dynObjText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_mode")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabObject,-1);
	//dynObjText->setOverrideColor(video::SColor(255,65,66,174));
	dynObjText->setOverrideColor(video::SColor(255,64,64,64));
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
	//editCharText->setOverrideColor(video::SColor(255,65,66,174));
	editCharText->setOverrideColor(video::SColor(255,64,64,64));
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
	//editCharSText->setOverrideColor(video::SColor(255,65,66,174));
	editCharSText->setOverrideColor(video::SColor(255,64,64,64));
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
	//editGlobSText->setOverrideColor(video::SColor(255,65,66,174));
	editGlobSText->setOverrideColor(video::SColor(255,64,64,64));
	editGlobSText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	editGlobSText->setOverrideFont(guiFont9);

}

void GUIManager::createMainTabs()
{
	createProjectTab();
	createPlayTab();

	mainToolbarPos = position2di(2,2);

	// Main tools TAB
	mainTabCtrl = guienv->addTabControl(myRect(260,2,displaywidth-435,112),guiMainWindow,true,true);
	mainTabCtrl->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

	createEnvironmentTab();

	createObjectTab();

	// No tools at the moment, don't create the panel
	//tabTools = mainTabCtrl->addTab(LANGManager::getInstance()->getText("tab_tools").c_str());
	
	
	tabConfig = mainTabCtrl->addTab(LANGManager::getInstance()->getText("tab_setup").c_str());
	//mainTabCtrl->setTabExtraWidth(25);
	mainTabCtrl->setActiveTab(1);

	//CONFIG BUTTON
	s32 x=12;
    guiConfigButton = guienv->addButton(myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabConfig,
                                     BT_ID_CONFIG,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_config")).c_str() );

	IGUIStaticText * editConfig = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_config")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabConfig,-1);
	//editConfig->setOverrideColor(video::SColor(255,65,66,174));
	editConfig->setOverrideColor(video::SColor(255,64,64,64));
	editConfig->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	editConfig->setOverrideFont(guiFont9);

    guiConfigButton->setImage(imgConfig);
	guiConfigButton->setPressedImage(imgConfig1);
	
}

void GUIManager::createAboutWindowGUI()
{
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
}

void GUIManager::createTerrainToolbar()
{
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
    guiTerrainBrushStrength->setPos(20);
	guiTerrainBrushStrength->setSmallStep(1);
	guiTerrainBrushStrength->setLargeStep(5);


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


    guiTerrainBrushRadius = guienv->addScrollBar(true,myRect(10,mainToolbarPos.Y+110,150,20),guiTerrainToolbar,SC_ID_TERRAIN_BRUSH_RADIUS );
    guiTerrainBrushRadius->setMin(0);
    guiTerrainBrushRadius->setMax(400);
    guiTerrainBrushRadius->setPos(100);
	guiTerrainBrushRadius->setSmallStep(1);
	guiTerrainBrushRadius->setLargeStep(5);

	guiTerrainBrushPlateau = guienv->addScrollBar(true,core::rect<s32>(10,mainToolbarPos.Y+170,160,mainToolbarPos.Y+190),guiTerrainToolbar,SC_ID_TERRAIN_BRUSH_PLATEAU);
	guiTerrainBrushPlateau->setMin(-100);
	guiTerrainBrushPlateau->setMax(1024);
	guiTerrainBrushPlateau->setPos(-10);
	guiTerrainBrushPlateau->setSmallStep(1);
	guiTerrainBrushPlateau->setLargeStep(5);

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
}

void GUIManager::createDynamicObjectInfoGUI()
{
	// --- Dynamic Objects Info panel (display info about the current selected template object)
    rect<s32> windowRect =
	myRect(displaywidth - 540,
	guiMainToolWindow->getClientRect().getHeight()+4,
	320,
	displayheight-guiMainToolWindow->getClientRect().getHeight()-24);

	guiDynamicObjectsWindowInfo = guienv->addWindow(windowRect,false,L"",0,GCW_DYNAMIC_OBJECT_INFO);
	guiDynamicObjectsWindowInfo->setDraggable(false);
    guiDynamicObjectsWindowInfo->getCloseButton()->setVisible(false);
    guiDynamicObjectsWindowInfo->setDrawTitlebar(false);
	guiDynamicObjectsWindowInfo->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

	IGUIScrollBar * infoscroll = guienv->addScrollBar(false, rect<s32>(300,40,318,driver->getScreenSize().Height-150),guiDynamicObjectsWindowInfo,-1);
	infoscroll->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
	infoscroll->setVisible(false);

	IGUIStaticText * infotext = guienv->addStaticText(L"Informations about this object",core::rect<s32>(1,1,318,39),false,true,guiDynamicObjectsWindowInfo,-1);
	infotext->setDrawBackground(true);
	infotext->setDrawBorder(true);
	infotext->setBackgroundColor(video::SColor(255,237,242,248));
	infotext->setOverrideColor(video::SColor(255,65,66,174));
	infotext->setOverrideFont(guiFontCourier12);
	infotext->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	thumbnail=guienv->addImage(info_current,vector2d<s32>(10,50),true,guiDynamicObjectsWindowInfo);

	u32 posy = 260;
	IGUIStaticText * infotext1 = guienv->addStaticText(L"Model name:",core::rect<s32>(10,posy,290,posy+39),false,true,guiDynamicObjectsWindowInfo,-1);
	infotext1->setOverrideFont(guiFont12);

	posy+=15;
	mdl_name = guienv->addStaticText(L"",core::rect<s32>(10,posy,290,posy+20),true,true,guiDynamicObjectsWindowInfo,-1);
	mdl_name->setDrawBackground(true);
	mdl_name->setBackgroundColor(video::SColor(255,237,242,248));
	mdl_name->setOverrideFont(guiFont10);
	mdl_name->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_CENTER);

	posy+=25;

	IGUIStaticText * infotext2 = guienv->addStaticText(L"Description:",core::rect<s32>(10,posy,290,posy+39),false,true,guiDynamicObjectsWindowInfo,-1);
	infotext2->setOverrideFont(guiFont12);

	posy+=15;
	mdl_desc = guienv->addStaticText(L"",core::rect<s32>(10,posy,290,posy+100),true,true,guiDynamicObjectsWindowInfo,-1);
	mdl_desc->setDrawBackground(true);
	mdl_desc->setBackgroundColor(video::SColor(255,237,242,248));
	mdl_desc->setOverrideFont(guiFont10);

	posy+=110;
	IGUIStaticText * infotext3 = guienv->addStaticText(L"Author:",core::rect<s32>(10,posy,290,posy+39),false,true,guiDynamicObjectsWindowInfo,-1);
	infotext3->setOverrideFont(guiFont12);

	posy+=15;
	mdl_auth = guienv->addStaticText(L"",core::rect<s32>(10,posy,290,posy+20),true,true,guiDynamicObjectsWindowInfo,-1);
	mdl_auth->setDrawBackground(true);
	mdl_auth->setBackgroundColor(video::SColor(255,237,242,248));
	mdl_auth->setOverrideFont(guiFont10);
	mdl_auth->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_CENTER);

	posy+=25;
	IGUIStaticText * infotext4 = guienv->addStaticText(L"Licence:",core::rect<s32>(10,posy,290,posy+39),false,true,guiDynamicObjectsWindowInfo,-1);
	infotext4->setOverrideFont(guiFont12);

	posy+=15;
	mdl_lic = guienv->addStaticText(L"",core::rect<s32>(10,posy,290,posy+20),true,true,guiDynamicObjectsWindowInfo,-1);
	mdl_lic->setDrawBackground(true);
	mdl_lic->setBackgroundColor(video::SColor(255,237,242,248));
	mdl_lic->setOverrideFont(guiFont10);
	mdl_lic->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_CENTER);



	guienv->getRootGUIElement()->bringToFront(guiDynamicObjectsWindowInfo);
	guiDynamicObjectsWindowInfo->setVisible(false);

}

void GUIManager::createDynamicObjectChooserGUI()
{
	// --- Dynamic Objects Chooser (to choose and place dynamic objects on the scenery)
    rect<s32> windowRect = myRect(displaywidth - 220,
	guiMainToolWindow->getClientRect().getHeight()+4,
	220,
	displayheight-guiMainToolWindow->getClientRect().getHeight()-24);

    guiDynamicObjectsWindowChooser = guienv->addWindow(windowRect,false,L"",0,GCW_DYNAMIC_OBJECT_CHOOSER);
    guiDynamicObjectsWindowChooser->setDraggable(false);
    guiDynamicObjectsWindowChooser->getCloseButton()->setVisible(false);
    guiDynamicObjectsWindowChooser->setDrawTitlebar(false);
	guiDynamicObjectsWindowChooser->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
	guiDynamicObjectsWindowChooser->setVisible(false);

    s32 guiDynamicObjectsWindowChooser_Y = 5;

	IGUIStaticText * ObjectText0 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_dynobjsel")).c_str(),core::rect<s32>(1,1,218,39),false,true,guiDynamicObjectsWindowChooser,-1);
	ObjectText0->setDrawBackground(true);
	ObjectText0->setDrawBorder(true);
	ObjectText0->setBackgroundColor(video::SColor(255,237,242,248));
	ObjectText0->setOverrideColor(video::SColor(255,65,66,174));
	ObjectText0->setOverrideFont(guiFontCourier12);
	ObjectText0->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	//IGUIStaticText * ObjectText1 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_dynobjcur")).c_str(),core::rect<s32>(10,190,160,200),false,true,guiDynamicObjectsWindowChooser,-1);
	//ObjectText1->setOverrideColor(video::SColor(255,0,0,0));
	//ObjectText1->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_CENTER);

	// Temporary disabled until the new object templates are in place.
   	// guiDynamicObjects_NodePreview = new NodePreview(guienv,guiDynamicObjectsWindowChooser,rect<s32>(10,40,160,180),-1);
    // guiDynamicObjects_NodePreview->setNode(DynamicObjectsManager::getInstance()->getActiveObject()->getNode());
	// guiDynamicObjects_NodePreview->setNode(DynamicObjectsManager::getInstance()->findActiveObject()->getNode());
	// guiDynamicObjects_NodePreview->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

    guiDynamicObjectsWindowChooser_Y += 40;
	guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_objectcol")).c_str(),core::rect<s32>(10,guiDynamicObjectsWindowChooser_Y,210,guiDynamicObjectsWindowChooser_Y+20),false,true,guiDynamicObjectsWindowChooser,-1);
	guiDynamicObjectsWindowChooser_Y += 20;
	guiDynamicObjects_Category = guienv->addComboBox(myRect(10,guiDynamicObjectsWindowChooser_Y,200,20),guiDynamicObjectsWindowChooser,CO_ID_DYNAMIC_OBJECT_OBJ_CATEGORY);
	
	for (int i=0 ; i< (int)DynamicObjectsManager::getInstance()->meshtypename.size() ; i++)
	{
		guiDynamicObjects_Category->addItem(DynamicObjectsManager::getInstance()->meshtypename[i].c_str());
	}
	//Since they are not yet implemented, theses will be off for the moment.
	//guiDynamicObjects_Category->addItem(L"INTERACTIVE OBJECTS");
	//guiDynamicObjects_Category->addItem(L"PROPS");
	

	
	guiDynamicObjectsWindowChooser_Y += 40;
	guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_dynobjcat")).c_str(),core::rect<s32>(10,guiDynamicObjectsWindowChooser_Y,210,guiDynamicObjectsWindowChooser_Y+20),false,true,guiDynamicObjectsWindowChooser,-1);
	guiDynamicObjectsWindowChooser_Y += 20;
	
	guiDynamicObjects_OBJCategory = guienv->addListBox(myRect(10,guiDynamicObjectsWindowChooser_Y,200,160),guiDynamicObjectsWindowChooser, CO_ID_DYNAMIC_OBJECT_OBJLIST_CATEGORY,true);

	guiDynamicObjectsWindowChooser_Y += 185;
	guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_dynobjitm")).c_str(),core::rect<s32>(10,guiDynamicObjectsWindowChooser_Y,210,guiDynamicObjectsWindowChooser_Y+20),false,true,guiDynamicObjectsWindowChooser,-1);
	
	guiDynamicObjectsWindowChooser_Y += 20;
	s32 boxend = screensize.Height-(guiDynamicObjectsWindowChooser_Y+180);
	if (boxend<10)
		boxend=10;

	guiDynamicObjects_OBJChooser = guienv->addListBox(myRect(10,guiDynamicObjectsWindowChooser_Y,200,boxend),guiDynamicObjectsWindowChooser, CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER,true);
	guiDynamicObjects_OBJChooser->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

	guiDynamicObjectsInfo= guienv->addButton(myRect(10,guiDynamicObjectsWindowChooser_Y+boxend+10,200,20),
                                                           guiDynamicObjectsWindowChooser,
                                                           BT_ID_DYNAMIC_OBJECT_INFO,
                                                           L"<< Information panel" );
	guiDynamicObjectsInfo->setOverrideFont(guiFontC12);
	guiDynamicObjectsInfo->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);

	UpdateGUIChooser();
}

void GUIManager::createContextMenuGUI()
{
	// --- Contextual menu for the dynamic objects
    guiDynamicObjects_Context_Menu_Window = guienv->addWindow(myRect(100,100,200,160),false,L"",0,GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU);
    guiDynamicObjects_Context_Menu_Window->getCloseButton()->setVisible(false);
    guiDynamicObjects_Context_Menu_Window->setDraggable(false);
    guiDynamicObjects_Context_Menu_Window->setDrawTitlebar(false);
    guiDynamicObjects_Context_Menu_Window->setVisible(false);

	guiDynamicObjects_Context_btSpawn = guienv->addButton(myRect(5,5,190,20),
                                                           guiDynamicObjects_Context_Menu_Window,
                                                           BT_ID_DYNAMIC_OBJECT_BT_SPAWN,
                                                           L"Create item here");
	guiDynamicObjects_Context_btSpawn->setOverrideFont(guiFontC12);

    guiDynamicObjects_Context_btEditScript = guienv->addButton(myRect(5,30,190,20),
                                                           guiDynamicObjects_Context_Menu_Window,
                                                           BT_ID_DYNAMIC_OBJECT_BT_EDITSCRIPTS,
                                                           stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_edit_script")).c_str() );
	guiDynamicObjects_Context_btEditScript->setOverrideFont(guiFontC12);

    guiDynamicObjects_Context_btMoveRotate= guienv->addButton(myRect(5,55,190,20),
                                                           guiDynamicObjects_Context_Menu_Window,
                                                           BT_ID_DYNAMIC_OBJECT_BT_MOVEROTATE,
                                                           stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_move_rotate")).c_str() );
	guiDynamicObjects_Context_btMoveRotate->setOverrideFont(guiFontC12);

    guiDynamicObjects_Context_btRemove= guienv->addButton(myRect(5,80,190,20),
                                                           guiDynamicObjects_Context_Menu_Window,
                                                           BT_ID_DYNAMIC_OBJECT_BT_REMOVE,
                                                           stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_remove")).c_str() );
	guiDynamicObjects_Context_btRemove->setOverrideFont(guiFontC12);

    guiDynamicObjects_Context_btCancel= guienv->addButton(myRect(5,105,190,20),
                                                           guiDynamicObjects_Context_Menu_Window,
                                                           BT_ID_DYNAMIC_OBJECT_BT_CANCEL,
                                                           stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_cancel")).c_str() );
	guiDynamicObjects_Context_btCancel->setOverrideFont(guiFontC12);

}

void GUIManager::createCodeEditorGUI()
{
	// ---
	// --- Edit scripts window
	// ---

	// Old way of opening the window (fixed size)
	/*guiDynamicObjectsWindowEditAction = guienv->addWindow(myRect(25,25,displaywidth-50,displayheight-50),false,L"",0,GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT);
    guiDynamicObjectsWindowEditAction->getCloseButton()->setVisible(false);
    guiDynamicObjectsWindowEditAction->setDrawTitlebar(false);
    guiDynamicObjectsWindowEditAction->setDraggable(false);
	guiDynamicObjectsWindowEditAction->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);*/

	// NEW (oct 2012) Create a stretching windows for the script editor
	guiDynamicObjectsWindowEditAction=new CGUIStretchWindow(L"Script editor",guienv, guienv->getRootGUIElement(),GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT,myRect(1,120,displaywidth-1,displayheight-140));
	guiDynamicObjectsWindowEditAction->setDevice(App::getInstance()->getDevice());
	guiDynamicObjectsWindowEditAction->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
	guiDynamicObjectsWindowEditAction->getCloseButton()->setVisible(false);

	//scripts editor box
    guiDynamicObjects_Script = new CGUIEditBoxIRB(L"",
                       true,
					   true,
                       guienv,
                       guiDynamicObjectsWindowEditAction,
                       EB_ID_DYNAMIC_OBJECT_SCRIPT,
                       myRect(10,40,driver->getScreenSize().Width-220,driver->getScreenSize().Height-310),
					   //myRect(10,40,guiDynamicObjectsWindowEditAction->getClientRect().getWidth()-20,guiDynamicObjectsWindowEditAction->getClientRect().getHeight()-130),
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
	guiDynamicObjects_Script->addKeyword("setSkydomeTexture",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setSkydomeVisible",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setBackgroundColor",SColor(255,128,0,255),true);


	guiDynamicObjects_Script->addKeyword("setPostFX",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setCameraPosition",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getCameraPosition",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setCameraTarget",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getCameraTarget",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getObjectPosition",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setObjectRotation",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("cutsceneMode",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("gameMode",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setRTSView",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setRPGView",SColor(255,128,0,255),true);

	guiDynamicObjects_Script->addKeyword("showCutsceneText",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setCutsceneText",SColor(255,128,0,255),true);

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
	guiDynamicObjects_Script->addKeyword("hasReached",SColor(255,128,0,255),true);
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
	guiDynamicObjects_Script->addKeyword("setObjectType",SColor(255,128,0,255),true);

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
	IGUITabControl * tabctrl1 = guienv->addTabControl(myRect(10,driver->getScreenSize().Height-270,driver->getScreenSize().Width-220,110),guiDynamicObjectsWindowEditAction,true,false);
	//IGUITabControl * tabctrl1 = guienv->addTabControl(myRect(10,guiDynamicObjectsWindowEditAction->getClientRect().getHeight()-90,displaywidth-220,110),guiDynamicObjectsWindowEditAction,true,false);
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

	//IGUIButton* close = guiDynamicObjects_Script_Close = guienv->addButton(myRect(guiDynamicObjectsWindowEditAction->getClientRect().getWidth()-90,10,82,20),
	IGUIButton* close = guiDynamicObjects_Script_Close = guienv->addButton(myRect(driver->getScreenSize().Width-170,30,82,20),
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

}

bool GUIManager::getVisibleStatus(s32 ID)
{
	if (ID==GCW_DYNAMIC_OBJECT_INFO)
		return guiDynamicObjectsWindowInfo->isVisible();

	if (ID==GCW_CONSOLE)
		return consolewin->isVisible();

	if (ID==GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT)
		return guiDynamicObjectsWindowEditAction->isVisible();

	return false;
}

// Update the Info panel GUI with the information contained in the template name
void GUIManager::getInfoAboutModel()
{

	// Text will return the current item basec on the Dynamic Objects manager "active" object.
	mdl_name->setText(DynamicObjectsManager::getInstance()->activeObject->getName().c_str());
	mdl_desc->setText(DynamicObjectsManager::getInstance()->activeObject->description.c_str());
	mdl_auth->setText(DynamicObjectsManager::getInstance()->activeObject->author.c_str());
	mdl_lic->setText(DynamicObjectsManager::getInstance()->activeObject->licence.c_str());


	core::stringc filename = "../media/dynamic_objects/";
	filename+=DynamicObjectsManager::getInstance()->activeObject->thumbnail;

	this->info_current=driver->getTexture(filename.c_str());
	if (!info_current)
		info_current = driver->getTexture("../media/editor/info_none.jpg");
	
	this->thumbnail->setImage(info_current);
	
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
	if (guiDynamicObjectsWindowInfo->isVisible() && guiDynamicObjectsWindowInfo->isPointInside(mousepos))
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

void GUIManager::UpdateGUIChooser()
{
	core::stringw selected = guiDynamicObjects_Category->getItem(guiDynamicObjects_Category->getSelected());
	
	// Create the category list first
	guiDynamicObjects_OBJCategory->clear();

	std::vector<stringw> listDynamicObjsCat = DynamicObjectsManager::getInstance()->getObjectsListCategories( selected );
	for (int i=0 ; i<(int)listDynamicObjsCat.size() ; i++)
    {
		guiDynamicObjects_OBJCategory->addItem(listDynamicObjsCat[i].c_str());
    }
	guiDynamicObjects_OBJCategory->setSelected(0);




	// Then the list of objects
	guiDynamicObjects_OBJChooser->clear();
	std::vector<stringw> listDynamicObjs = DynamicObjectsManager::getInstance()->getObjectsList(selected,"");

    for (int i=0 ; i<(int)listDynamicObjs.size() ; i++)
    {
		guiDynamicObjects_OBJChooser->addItem(listDynamicObjs[i].c_str());
    }
	guiDynamicObjects_OBJChooser->setSelected(0);
}

void GUIManager::updateCurrentCategory()
{

	core::stringw text="";
	u32 selected = guiDynamicObjects_OBJCategory->getSelected();
	core::stringw selectedcat = guiDynamicObjects_Category->getItem(guiDynamicObjects_Category->getSelected());
	text=guiDynamicObjects_OBJCategory->getListItem(selected);

	// check if "all" is selected, as it's the first choice
	// and empty string mean, that will check for all
	if (selected==0)
		text="";

	guiDynamicObjects_OBJChooser->clear();
	std::vector<stringw> listDynamicObjs = DynamicObjectsManager::getInstance()->getObjectsList(selectedcat,text);

    for (int i=0 ; i<(int)listDynamicObjs.size() ; i++)
    {
		guiDynamicObjects_OBJChooser->addItem(listDynamicObjs[i].c_str());
    }
	guiDynamicObjects_OBJChooser->setSelected(0);
	// Set the "active" object to the selection
#ifdef EDITOR
	DynamicObjectsManager::getInstance()->setActiveObject(getComboBoxItem(CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER));
#endif
}

void GUIManager::setTextLoader(stringw text)
{
	if (guiLoaderDescription)
	{
		guiLoaderDescription->setText(text.c_str());
		App::getInstance()->quickUpdate();
	}
}

// Console window
void GUIManager::createConsole()
{

	core::dimension2d<u32> center = screensize/2;
	// consolewin = guienv->addWindow(rect<s32>(20,20,800,400),false,L"Console window",0,GCW_CONSOLE);
	consolewin = new CGUIStretchWindow(L"Console window", guienv, guienv->getRootGUIElement(),GCW_CONSOLE,rect<s32>(center.Width-400,center.Height-200,center.Width+400,center.Height+200));
	consolewin->setDevice(App::getInstance()->getDevice());
	consolewin->getCloseButton()->setVisible(false);

	// project TAB
	gui::IGUITabControl* control = guienv->addTabControl(myRect(20,40,750,340),consolewin,true,true);
	gui::IGUITab* tab=control->addTab(LANGManager::getInstance()->getText("tab_console_message").c_str());
	gui::IGUITab* tab2=control->addTab(LANGManager::getInstance()->getText("tab_console_log").c_str());
	tab->setBackgroundColor(video::SColor(255,220,220,220));
	tab->setDrawBackground(true);

	control->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
	
	//Message console
	console = guienv->addListBox(myRect(10,15,720,260),tab,0,true);
	console->setAutoScrollEnabled(false);
	console->setItemHeight(20);
	console->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

	//logger console
	consolelog = guienv->addListBox(myRect(10,15,720,260),tab2,0,true);
	consolelog->setAutoScrollEnabled(false);
	consolelog->setItemHeight(20);
	consolelog->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
	
	consolewin->setVisible(false);
}

void GUIManager::setupGameplayGUI()
{

	createConsole();

    fader=guienv->addInOutFader();
	fader->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
    fader->setVisible(false);

	// NEW Create display size since IRRlicht return wrong values
	// Check the current screen size 
	displayheight=screensize.Height;
	displaywidth=screensize.Width;

	// Create a cutscene text 
	guiCutsceneText = guienv->addStaticText(L"This is a standard cutscene text",core::rect<s32>(100,displayheight/2+(displayheight/4),displaywidth-10,displayheight-100),false,true,0,-1,false);
	guiCutsceneText->setOverrideFont(guiFontLarge28);
	guiCutsceneText->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);
	guiCutsceneText->setVisible(false);

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

	// --- Active game menu during play
	ITexture* gameplay_bar = driver->getTexture("../media/art/gameplay_bar.png");
	ITexture* circle = driver->getTexture("../media/art/circle.png");
	ITexture* circleMana = driver->getTexture("../media/art/circlemana.png");
	ITexture* topCircle = driver->getTexture("../media/art/circle_top.png");

	// The bottom image of the interface
	if (gameplay_bar)
	{
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
	}

	

	

    ///DIALOG
    guiDialogImgYes = driver->getTexture("../media/art/img_yes.png");
    guiDialogImgYes_s = driver->getTexture("../media/art/img_yes_s.png");
    guiDialogImgNo = driver->getTexture("../media/art/img_no.png");
    guiDialogImgNo_s = driver->getTexture("../media/art/img_no_s.png");


    //view items
	if (gameplay_bar_image)
	{
		core::stringw text=LANGManager::getInstance()->getText("bt_view_items");
		guiBtViewItems = guienv->addButton(myRect(465,85,48,48),
		//displaywidth/2 + 80,displayheight - 57,48,48),
                                     gameplay_bar_image,
                                     BT_ID_VIEW_ITEMS,L"",
									 text.c_str());

		guiBtViewItems->setImage(driver->getTexture("../media/art/bt_view_items.png"));
		guiBtViewItems->setVisible(false);
	}

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
	//guiPlayerNodePreview->setNode(Player::getInstance()->getNodeRef());
	//DynamicObjectsManager::getInstance()->setActiveObject("Archer");
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

}

void GUIManager::setWindowVisible(GUI_CUSTOM_WINDOW window, bool visible)
{
    switch(window)
    {
#ifdef EDITOR
		case GCW_DYNAMIC_OBJECT_INFO:
			guiDynamicObjectsWindowInfo->setVisible(visible);
			break;

        case GCW_DYNAMIC_OBJECT_CHOOSER:
            guiDynamicObjectsWindowChooser->setVisible(visible);
            break;
        case GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU:
            mouseX = App::getInstance()->getDevice()->getCursorControl()->getPosition().X;
            mouseY = App::getInstance()->getDevice()->getCursorControl()->getPosition().Y;
            guiDynamicObjects_Context_Menu_Window->setRelativePosition(myRect(mouseX,mouseY,200,130));
            guiDynamicObjects_Context_Menu_Window->setVisible(visible);
            break;
        case GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT:
            guiDynamicObjectsWindowEditAction->setVisible(visible);
            break;
        case GCW_TERRAIN_TOOLBAR:
            guiTerrainToolbar->setVisible(visible);
            break;
#endif
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

        default:
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
	// Temporary disabled until the new template system is in place.
	scene::ISceneNode* node = DynamicObjectsManager::getInstance()->findActiveObject();
	//if (node)
	//	guiDynamicObjects_NodePreview->setNode(node);
}

stringc GUIManager::getEditBoxText(GUI_ID id)
{
    switch(id)
    {
        case EB_ID_DYNAMIC_OBJECT_SCRIPT:
            return stringc(guiDynamicObjects_Script->getText());
            break;

        default:
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
        default:
            break;
    }
}
#endif

void GUIManager::setElementEnabled(GUI_ID id, bool enable)
{
	///TODO: fazer metodo getElement by ID!!!
   switch(id)
    {
#ifdef EDITOR
		case BT_ID_TERRAIN_ADD_EMPTY_SEGMENT:
			guiTerrainAddEmptySegment->setEnabled(enable);
			guiTerrainAddEmptySegment->setPressed(!enable);
			break;
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
		case BT_ID_PLAYER_EDIT_SCRIPT:
			guiPlayerEditScript->setEnabled(enable);
			guiPlayerEditScript->setPressed(!enable);
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

        default:
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
#ifdef EDITOR
			guiMainWindow->setVisible(!visible);
			guiStatus->setVisible(!visible);
#endif
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
		{


            guiBtViewItems->setVisible(visible);
			// Update the gold items
			stringc playerMoney = LANGManager::getInstance()->getText("txt_player_money");
			playerMoney += Player::getInstance()->getObject()->getMoney();
			this->setStaticTextText(ST_ID_PLAYER_MONEY,playerMoney);
		}
        break;

        default:
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
        default:
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
	guiFontCourier12 = guienv->getFont("../media/fonts/courier12.xml");
	guiFontLarge28 = guienv->getFont("../media/fonts/large28.xml");
    guiFontDialog = guienv->getFont("../media/fonts/dialog.xml");
	guiFont6 = guienv->getFont("../media/fonts/Arial6.xml");
	guiFont8 = guienv->getFont("../media/fonts/Arial8.xml");
	guiFont9 = guienv->getFont("../media/fonts/Trebuchet10.xml");
	guiFont10 = guienv->getFont("../media/fonts/Arial10.xml");
	guiFont12 = guienv->getFont("../media/fonts/Arial12.xml");
	guiFont14 = guienv->getFont("../media/fonts/Arial14.xml");


	if (guiFont10)
		guiFont10->setKerningWidth(-1);

	if (guiFont9)
		guiFont9->setKerningWidth(-2);


	if (guiFont8)
	{
		guiFont8->setKerningWidth(-1);
	    guiFont8->setKerningHeight(-6);
	}
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

        default:
            break;
    }
}

void GUIManager::setConsoleText(stringw text, video::SColor color)
// Add text into the output console
// The function manage up to 5000 lines before clearing the buffer
// Using "forcedisplay" will toggle the display of the GUI
{
	//Temporary disable of this method to gain speed
	//Will have a toggle to use/not use this in the future


	u32 maxitem = 5000;
	// If the GUI is not displayed, accumulate the info in a buffer
	if (textevent.size()<maxitem)
	{
		textevent.push_back(text);
		texteventcolor.push_back(color);
	} 
	
	// This part will update the IRRlicht type console
	if (console)
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

void GUIManager::clearConsole() 
{
	textevent.clear();
	texteventcolor.clear();
}

void GUIManager::setConsoleLogger(vector<core::stringw> &text)
{
	u32 maxitem = 5000;
	if (consolelog)
	{
		if (text.size()>0)
		{
			for (int a=0; a<(int)text.size(); a++)
			{

				if (consolelog->getItemCount()>maxitem-1)
					consolelog->removeItem(maxitem);

				//
				consolelog->insertItem(0,text[a].subString(0,90).c_str(),0);
				consolelog->setItemOverrideColor(0,video::SColor(255,0,0,0));

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

	//Pause the player during the dialog opening
	Player::getInstance()->getObject()->setAnimation("idle");

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

	//Pause the player during the dialog opening
	Player::getInstance()->getObject()->setAnimation("idle");

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
