#include "GUIEditor.h"

#include "../objects/DynamicObjectsManager.h"
#include "../terrain/TerrainManager.h" //Needed for the vegatation and terrain tools
#include "../camera/CameraSystem.h" //Informations of the camera for the GUI

#include <algorithm>    // std::sort

//Init
GUIEditor::GUIEditor()
{
	guienv=GUIManager::getInstance()->getGuiEnv();
	driver=GUIManager::getInstance()->getDriver();
	screensize=App::getInstance()->getScreenSize();
	device=App::getInstance()->getDevice(); //Get a fresh copy of the device

	info_current=NULL;
	info_current1=NULL;
	
	// Textures
	backtexture=NULL;
	imgNewProject=NULL;
	imgNewProject1=NULL;
	imgLoadProject=NULL;
	imgLoadProject1=NULL;
	imgSaveProject=NULL;
	imgSaveProject1=NULL;
	imgCloseProgram=NULL;
	imgAbout=NULL;
	imgAbout1=NULL;
	imgHelp=NULL;
	imgHelp1=NULL;
	imgConfig=NULL;
	imgConfig1=NULL;

	///IrrRPG Builder LOGO
    logo1=NULL;;

	helpTerrainTransform=NULL;;
    helpTerrainSegments=NULL;;
	helpVegetationPaint=NULL;;

	configWindow=NULL;;
}

//Destructor
GUIEditor::~GUIEditor()
{
	if (configWindow)
		delete configWindow;
	
}

GUIEditor* GUIEditor::getInstance()
{
    static GUIEditor *instance = 0;
    if (!instance) instance = new GUIEditor();
    return instance;
}

//--------------------------------------------------------------------------------------------------------------------------------
// Functions for creating the GUI

//This is the comparison function needed by STD:SORT() so it sort on the lowercase version of the strings
bool compareString(const core::stringw &a, const core::stringw &b)
{
	core::stringw a1=a;
	core::stringw b1=b;
	return (a1.make_lower() < b1.make_lower());
}

void GUIEditor::setupEditorGUI()
{


	// Load textures
	//ITexture* imgLogo = driver->getTexture("../media/art/logo1.png");
	ITexture* imgLogo = driver->getTexture("../media/art/title.jpg");
	ITexture* info_none = driver->getTexture("../media/editor/info_none.jpg");
	// Default textures for the info window
	if (info_none)
	{
		info_current=info_none;
		info_current1=info_none;
	}

	// Check the current screen size
	displayheight=screensize.Height;
	displaywidth=screensize.Width;

	//LOADER WINDOW
	//printf("Creating the loader window.\n");
	IGUIWindow* guiLoaderWindow = guienv->addWindow(
		GUIManager::getInstance()->myRect(displaywidth/2-300,displayheight/2-200,600,400),
		false,LANGManager::getInstance()->getText("loader_gen").c_str(),
		0,GUIManager::WIN_LOADER);

	guiLoaderWindow->setDrawTitlebar(false);
	guiLoaderWindow->getCloseButton()->setVisible(false);
	guiLoaderWindow->setAlignment(EGUIA_CENTER,EGUIA_CENTER,EGUIA_CENTER,EGUIA_CENTER);

	//guienv->addImage(imgLogo,vector2d<s32>(200,50),true,guiLoaderWindow);
	guienv->addImage(imgLogo,vector2d<s32>(5,5),true,guiLoaderWindow);
	IGUIStaticText* guiLoaderDescription = guienv->addStaticText(LANGManager::getInstance()->getText("loader_font").c_str(),
		GUIManager::getInstance()->myRect(10,350,580,40),
		true,true,guiLoaderWindow,
		GUIManager::TXT_ID_LOADER,false);

	App::getInstance()->quickUpdate();

	GUIManager::getInstance()->loadFonts();
	guienv->getSkin()->setFont(GUIManager::getInstance()->guiFont10);
	guiLoaderDescription->setText(LANGManager::getInstance()->getText("loader_graphics").c_str());

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
	IGUIWindow* guiStatus = guienv->addWindow(GUIManager::getInstance()->myRect(0,displayheight-25,displaywidth,displayheight),false,0,0,GUIManager::GCW_STATUSBAR);
	guiStatus->setDraggable(false);
	guiStatus->setDrawTitlebar(false);
	guiStatus->getCloseButton()->setVisible(false);
	guiStatus->setDrawBackground(true);
	guiStatus->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);

	IGUIStaticText* guiStatusText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("status_title")).c_str(),
		GUIManager::getInstance()->myRect(10,4,displaywidth-200,18),
		false,false,guiStatus, GUIManager::TXT_ID_STATUS_TEXT);

	IGUIStaticText* guiStatusCameraText = guienv->addStaticText(getEditCameraString(NULL).c_str(),
		GUIManager::getInstance()->myRect(displaywidth-600,4,displaywidth-700,18),true,false,guiStatus, GUIManager::TXT_ID_STATUS_CAMERA);
	
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

	// Create the tree toolbar GUI
	createVegetationToolbar();

	// Create the Dynamic Object Chooser GUI
	createDynamicObjectChooserGUI();

	// Create the Editor context menu GUI
	createContextMenuGUI();

	// Create the Code Editor GUI
	createCodeEditorGUI();

    ///LOAD HELP IMAGES
    helpTerrainTransform = driver->getTexture("../media/art/help_terrain_transform.png");
    helpVegetationPaint = driver->getTexture("../media/art/help_vegetation_paint.png");
    helpTerrainSegments = driver->getTexture("../media/art/help_terrain_segments.png");

	// Get the logo
	if (imgLogo)
		logo1 = imgLogo;
	else
		logo1 = driver->getTexture("../media/art/logo1.png");

#ifdef EDITOR
	// Create the Configuration window (Need to be updated)
    configWindow = new GUIConfigWindow(App::getInstance()->getDevice());
	configWindow->setID(GUIManager::GCW_CONFIG_WINDOW);
	// Give back to the GUI Manager since it's needed for both applications
	GUIManager::getInstance()->setConfigWindow(configWindow);
#endif
	
	// Update and refresh the display
	App::getInstance()->quickUpdate();

	createDisplayOptionsGUI();

}


void GUIEditor::createNewProjectGUI()
{
	//Create a new windows to create projects
	core::dimension2d<u32> center = screensize / 2;
	IGUIWindow* newproj = guienv->addWindow(rect<s32>(center.Width - 400, center.Height - 200, center.Width + 400, center.Height + 200), true, L"Create a new project", 0, GUIManager::GCW_NEWPROJECT);
	newproj->getCloseButton()->setVisible(false);
	newproj->setMinSize(core::dimension2du(140, 70));

	u32 posx = 40; u32 posy = 40;
	IGUIStaticText * projectNameText = guienv->addStaticText(L"Project name",
		core::rect<s32>(posx, posy, 250, 76), false, true, newproj, -1);
	projectNameText->setOverrideColor(video::SColor(255, 20, 20, 20));
	projectNameText->setOverrideFont(GUIManager::getInstance()->guiFont10);

	//New list for current project in the folder
	guienv->addStaticText(L"", core::rect<s32>(posx + 500, posy, posx + 740, posy + 290), true, false, newproj, -1, true);
	guienv->addStaticText(L"Current projects", core::rect<s32>(posx + 510, posy+10, posx + 700, posy + 30), false, false, newproj, -1);
	projectlist = guienv->addListBox(core::rect<s32>(posx + 510, posy+30, posx + 730, posy + 240), newproj, -1, true);
	IGUIButton* loadproj = guienv->addButton(rect<s32>(posx+510, posy+250, posx+ 730, posy+280), newproj, GUIManager::BT_ID_LOAD_PROJECT, L"Load existing project");

	posy += 20;
	guienv->addEditBox(App::getInstance()->getCurrentProjectName().c_str(), rect<s32>(posx, posy, 500, posy + 20), true, newproj, GUIManager::TXT_ID_PROJECT_NAME);

	posy += 30;
	IGUIStaticText * projectPathText = guienv->addStaticText(L"Project path",
		core::rect<s32>(posx, posy, 250, posy+12), false, true, newproj, -1);
	projectPathText->setOverrideColor(video::SColor(255, 20, 20, 20));
	projectPathText->setOverrideFont(GUIManager::getInstance()->guiFont10);
	posy += 20;
	//App::getInstance()->path + App::getInstance()->projectpath;
	core::stringw projpath = (core::stringw)device->getFileSystem()->getAbsolutePath(App::getInstance()->projectpath.c_str());
	guienv->addStaticText(projpath.c_str(), rect<s32>(posx, posy, 500, posy + 20), true, false, newproj, -1, true);
	//guienv->addEditBox(projpath.c_str(), rect<s32>(posx, posy, 500, posy + 20), true, newproj, -1);

	posy += 50;
	
	IGUIStaticText * firstMapText = guienv->addStaticText(L"First map name",
		core::rect<s32>(posx, posy, 250, posy + 12), false, true, newproj, -1);
	firstMapText->setOverrideColor(video::SColor(255, 20, 20, 20));
	firstMapText->setOverrideFont(GUIManager::getInstance()->guiFont10);
	posy += 20;
	guienv->addEditBox(App::getInstance()->getCurrentMapName().c_str(), rect<s32>(posx, posy, 500, posy + 20), true, newproj, GUIManager::TXT_ID_FIRST_MAP_NAME);

	posy += 30;
	IGUIStaticText * mapDescriptionText = guienv->addStaticText(L"Description",
		core::rect<s32>(posx, posy, 250, posy + 12), false, true, newproj, -1);
	mapDescriptionText->setOverrideColor(video::SColor(255, 20, 20, 20));
	mapDescriptionText->setOverrideFont(GUIManager::getInstance()->guiFont10);
	posy += 20;
	guienv->addEditBox(App::getInstance()->getCurrentMapDesc().c_str(), rect<s32>(posx, posy, 500, posy + 80), true, newproj, GUIManager::TXT_ID_FIRST_MAP_DESC);

	//Buttons
	IGUIButton* quit = NULL;
	IGUIButton* cancel = NULL;
	
	IGUIButton* createproj = guienv->addButton(rect<s32>(30, 400 - 60, 250, 400 - 30), newproj, GUIManager::BT_ID_CREATE_PROJECT, L"Create a new project");
	quit = guienv->addButton(rect<s32>(260 + 30, 400 - 60, 260 + 250, 400 - 30), newproj, GUIManager::BT_ID_CLOSE_PROGRAM, L"Quit");

	buildProjectList();
	if (projectlist->getItemCount()==0)
		guienv->addStaticText(L"No projects", core::rect<s32>(posx + 515, 40 + 30, posx + 695, 40 + 60), false, false, newproj, -1);


}

void GUIEditor::createDisplayOptionsGUI()
{

	IGUIWindow* win = guienv->addWindow(rect<s32>(0,120,displaywidth,142),false,0,0,GUIManager::GCW_VIEW_MENU);
	win->getCloseButton()->setVisible(false);
	win->setDrawTitlebar(false);
	win->setDrawBackground(false);
	win->setDraggable(false);
	win->setEnabled(false);
	win->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);
	IGUIStaticText* text=guienv->addStaticText(L"",rect<s32>(0,0,displaywidth,30),false,false,win,-1,true);
	text->setBackgroundColor(video::SColor(255,42,42,42));
	text->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

	IGUIComboBox* snappingcombo = guienv->addComboBox(GUIManager::getInstance()->myRect(240,0,140,20),win ,GUIManager::CB_SNAPCOMBO);
	snappingcombo->setText(LANGManager::getInstance()->getText("sgrid_default").c_str());
	snappingcombo->addItem(LANGManager::getInstance()->getText("sgrid_default").c_str(),0);
	snappingcombo->addItem(LANGManager::getInstance()->getText("sgrid2").c_str(),2);
	snappingcombo->addItem(LANGManager::getInstance()->getText("sgrid4").c_str(),4);
	snappingcombo->addItem(LANGManager::getInstance()->getText("sgrid8").c_str(),8);
	snappingcombo->addItem(LANGManager::getInstance()->getText("sgrid16").c_str(),16);
	snappingcombo->addItem(LANGManager::getInstance()->getText("sgrid32").c_str(),32);
	snappingcombo->addItem(LANGManager::getInstance()->getText("sgrid64").c_str(),64);
	snappingcombo->addItem(LANGManager::getInstance()->getText("sgrid128").c_str(),128);
	snappingcombo->addItem(LANGManager::getInstance()->getText("sgrid256").c_str(),256);
	snappingcombo->addItem(LANGManager::getInstance()->getText("sgrid512").c_str(),512);
	snappingcombo->addItem(LANGManager::getInstance()->getText("sgrid1024").c_str(),1024);
	snappingcombo->setMaxSelectionRows(12);
	guienv->getRootGUIElement()->sendToBack(snappingcombo);


	IGUIComboBox* screencombo = guienv->addComboBox(GUIManager::getInstance()->myRect(30,0,200,20),win,GUIManager::CB_SCREENCOMBO);
	screencombo->setText(LANGManager::getInstance()->getText("view_gen").c_str());
	screencombo->setToolTipText(LANGManager::getInstance()->getText("view_gen").c_str());
	screencombo->addItem(LANGManager::getInstance()->getText("view_gen").c_str(),0);
	screencombo->addItem(LANGManager::getInstance()->getText("view_top").c_str(),1);
	screencombo->addItem(LANGManager::getInstance()->getText("view_bottom").c_str(),2);
	screencombo->addItem(LANGManager::getInstance()->getText("view_left").c_str(),3);
	screencombo->addItem(LANGManager::getInstance()->getText("view_right").c_str(),4);
	screencombo->addItem(LANGManager::getInstance()->getText("view_front").c_str(),5);
	screencombo->addItem(LANGManager::getInstance()->getText("view_back").c_str(),6);
	screencombo->addItem(LANGManager::getInstance()->getText("view_ortho").c_str(),7);
	screencombo->addItem(LANGManager::getInstance()->getText("view_pers").c_str(),8);
	screencombo->setMaxSelectionRows(9);
	screencombo->setEnabled(true);
	guienv->getRootGUIElement()->sendToBack(win);

}

void GUIEditor::createMainToolbar()
{
	// Standard Main toolbar
	IGUIWindow* guiMainWindow = guienv->addWindow(GUIManager::getInstance()->myRect(0,0,displaywidth-220,122),false,0,0,GUIManager::GCW_TOP_WINDOW);
    guiMainWindow->setDraggable(false);
    guiMainWindow->setDrawTitlebar(false);
	guiMainWindow->getCloseButton()->setVisible(false);
	guiMainWindow->setDrawBackground(false);
	guiMainWindow->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

	IGUIWindow* guiMainToolWindow = guienv->addWindow(GUIManager::getInstance()->myRect(displaywidth-220,0,220,120),false,0,0,GUIManager::WIN_GAMEPLAY);
	guiMainToolWindow->setDraggable(false);
	guiMainToolWindow->setDrawTitlebar(false);
	guiMainToolWindow->getCloseButton()->setVisible(false);
	guiMainToolWindow->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

	IGUIImage* guiBackImage2=guienv->addImage(backtexture,vector2d<s32>(0,0),true,guiMainToolWindow);
	guiBackImage2->setScaleImage(true);
	guiBackImage2->setMaxSize(dimension2du(2048,120));
	guiBackImage2->setMinSize(dimension2du(2048,120));
	guiBackImage2->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

	IGUIImage* guiBackImage = guienv->addImage(backtexture,vector2d<s32>(0,0),true,guiMainWindow);
	guiBackImage->setScaleImage(true);
	guiBackImage->setMaxSize(dimension2du(2048,120));
	guiBackImage->setMinSize(dimension2du(2048,120));
	guiBackImage->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

	// Create the tabs of the main toolbar
	createMainTabs();

}

void GUIEditor::createMainTabs()
{
	//Get the handle of the top main window.
	IGUIWindow* guiMainWindow = (IGUIWindow*)GUIManager::getInstance()->getGUIElement(GUIManager::GCW_TOP_WINDOW);

	createProjectTab();
	createPlayTab();

	mainToolbarPos = position2di(2,2);

	// Main tools TAB
	IGUITabControl* mainTabCtrl = guienv->addTabControl(GUIManager::getInstance()->myRect(260,2,displaywidth-435,112),guiMainWindow,true,true, GUIManager::ID_TABCONTROL_MAIN);
	mainTabCtrl->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

	createEnvironmentTab();

	createObjectTab();

	// No tools at the moment, don't create the panel
	//tabTools = mainTabCtrl->addTab(LANGManager::getInstance()->getText("tab_tools").c_str());


	IGUITab* tabConfig = mainTabCtrl->addTab(LANGManager::getInstance()->getText("tab_setup").c_str());
	//mainTabCtrl->setTabExtraWidth(25);
	mainTabCtrl->setActiveTab(1);

	//CONFIG BUTTON
	s32 x=12;
	IGUIButton* guiConfigButton = guienv->addButton(GUIManager::getInstance()->myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabConfig,
									 GUIManager::BT_ID_CONFIG,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_config")).c_str() );

	IGUIStaticText * editConfig = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_config")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabConfig,-1);
	//editConfig->setOverrideColor(video::SColor(255,65,66,174));
	editConfig->setOverrideColor(video::SColor(255,64,64,64));
	editConfig->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	editConfig->setOverrideFont(GUIManager::getInstance()->guiFont9);

    guiConfigButton->setImage(imgConfig);
	guiConfigButton->setPressedImage(imgConfig1);

}

void GUIEditor::createProjectTab()
{
	//Get the handle of the top main window.
	IGUIWindow* guiMainWindow = (IGUIWindow*)GUIManager::getInstance()->getGUIElement(GUIManager::GCW_TOP_WINDOW);
	// project TAB
	IGUITabControl* prjTabCtrl = guienv->addTabControl(GUIManager::getInstance()->myRect(5,2,250,112),guiMainWindow,true,true);
	IGUITab* tabProject = prjTabCtrl->addTab(LANGManager::getInstance()->getText("tab_project").c_str());

	// Tab description box text
	IGUIStaticText * projectTabText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des0")).c_str(),
		core::rect<s32>(0,64,250,76),false,true,tabProject,-1);
	//projectTabText->setBackgroundColor(video::SColor(128,237,242,248));
	//projectTabText->setOverrideColor(video::SColor(255,65,66,174));
	projectTabText->setBackgroundColor(video::SColor(255,238,240,242));
	projectTabText->setOverrideColor(video::SColor(255,86,95,109));
	projectTabText->setOverrideFont(GUIManager::getInstance()->guiFont10);
	projectTabText->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	// Buttons
	 s32 x = 0;

	mainToolbarPos.Y=3;
	// Close program
	x += 12;
	IGUIButton* guiCloseProgram = guienv->addButton(GUIManager::getInstance()->myRect(x,mainToolbarPos.Y,32,32),
                                     tabProject,
									 GUIManager::BT_ID_CLOSE_PROGRAM,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_close_program")).c_str() );

    guiCloseProgram->setImage(imgCloseProgram);

	IGUIStaticText * closeText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_close_program")).c_str(),
		core::rect<s32>(x-5,36,x+40,65),false,true,tabProject,-1);
	//closeText->setOverrideColor(video::SColor(255,65,66,174));
	closeText->setOverrideColor(video::SColor(255,64,64,64));
	closeText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	closeText->setOverrideFont(GUIManager::getInstance()->guiFont9);

    //New Project
	x+=60;
	IGUIButton* guiMainNewProject = guienv->addButton(GUIManager::getInstance()->myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabProject,
									 GUIManager::BT_ID_NEW_PROJECT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_new_project")).c_str() );

    guiMainNewProject->setImage(imgNewProject);
	guiMainNewProject->setPressedImage(imgNewProject1);

	IGUIStaticText * newPText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_new_project")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabProject,-1);
	//newPText->setOverrideColor(video::SColor(255,65,66,174));
	newPText->setOverrideColor(video::SColor(255,64,64,64));
	newPText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	newPText->setOverrideFont(GUIManager::getInstance()->guiFont9);


    //Load Project
	x+=60;
	IGUIButton* guiMainLoadProject = guienv->addButton(GUIManager::getInstance()->myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabProject,
									 GUIManager::BT_ID_LOAD_PROJECT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_load_project")).c_str() );

    guiMainLoadProject->setImage(imgLoadProject);
	guiMainLoadProject->setPressedImage(imgLoadProject1);

	IGUIStaticText * loadPText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_load_project")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabProject,-1);
	//loadPText->setOverrideColor(video::SColor(255,65,66,174));
	loadPText->setOverrideColor(video::SColor(255,64,64,64));
	loadPText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	loadPText->setOverrideFont(GUIManager::getInstance()->guiFont9);


	//Save Project
	x+=60;
	IGUIButton* guiMainSaveProject = guienv->addButton(GUIManager::getInstance()->myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabProject,
									 GUIManager::BT_ID_SAVE_PROJECT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_save_project")).c_str() );

    guiMainSaveProject->setImage(imgSaveProject);
	guiMainSaveProject->setPressedImage(imgSaveProject1);

	IGUIStaticText * savePText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_save_project")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabProject,-1);
	//savePText->setOverrideColor(video::SColor(255,65,66,174));
	savePText->setOverrideColor(video::SColor(255,64,64,64));
	savePText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	savePText->setOverrideFont(GUIManager::getInstance()->guiFont9);

}

void GUIEditor::createPlayTab()
{
	//Get the handle of the top main window (other part that have the play buttons).
	IGUIWindow* guiMainToolWindow = (IGUIWindow*)GUIManager::getInstance()->getGUIElement(GUIManager::WIN_GAMEPLAY);
	// Play TAB
	IGUITabControl* mainToolCtrl = guienv->addTabControl(GUIManager::getInstance()->myRect(2,2,164,112),guiMainToolWindow,true,true);
	IGUITab* tabPlayTool = mainToolCtrl->addTab(LANGManager::getInstance()->getText("txt_tool_des4").c_str());

	//Play Game
	s32 x = 12;
	mainToolbarPos.Y=3;
	IGUIButton* guiPlayGame= guienv->addButton(GUIManager::getInstance()->myRect(x,mainToolbarPos.Y,32,32),
                                     tabPlayTool,
									 GUIManager::BT_ID_PLAY_GAME,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_play_game")).c_str());

    guiPlayGame->setImage(driver->getTexture("../media/art/bt_play_game.png"));

	IGUIStaticText * playGText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_play_game")).c_str(),
		core::rect<s32>(x-5,36,x+40,65),false,true,tabPlayTool,-1);
	//playGText->setOverrideColor(video::SColor(255,65,66,174));
	playGText->setOverrideColor(video::SColor(255,64,64,64));
	playGText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	playGText->setOverrideFont(GUIManager::getInstance()->guiFont9);


    //Stop Game
	IGUIButton* guiStopGame= guienv->addButton(GUIManager::getInstance()->myRect(+x,mainToolbarPos.Y,32,32),
                                     tabPlayTool,
									 GUIManager::BT_ID_STOP_GAME,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_stop_game")).c_str());

    guiStopGame->setImage(driver->getTexture("../media/art/bt_stop_game.png"));
    guiStopGame->setVisible(false);



    //ABOUT BUTTON
	x += 50;
	IGUIButton* guiAbout = guienv->addButton(GUIManager::getInstance()->myRect(x,mainToolbarPos.Y,32,32),
                                     tabPlayTool,
									 GUIManager::BT_ID_ABOUT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_about")).c_str() );

    guiAbout->setImage(imgAbout);
	guiAbout->setPressedImage(imgAbout1);

	IGUIStaticText * aboutBText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_about")).c_str(),
		core::rect<s32>(x-5,36,x+40,65),false,true,tabPlayTool,-1);
	//aboutBText->setOverrideColor(video::SColor(255,65,66,174));
	aboutBText->setOverrideColor(video::SColor(255,64,64,64));
	aboutBText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	aboutBText->setOverrideFont(GUIManager::getInstance()->guiFont9);

	// Help Button
	x += 50;
	IGUIButton* guiHelpButton = guienv->addButton(GUIManager::getInstance()->myRect(x,mainToolbarPos.Y,32,32),
                                     tabPlayTool,
									 GUIManager::BT_ID_HELP,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_help")).c_str() );

    guiHelpButton->setImage(imgHelp);
    guiHelpButton->setPressedImage(imgHelp1);

	IGUIStaticText * helpBText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_help")).c_str(),
		core::rect<s32>(x-5,36,x+40,65),false,true,tabPlayTool,-1);
	//helpBText->setOverrideColor(video::SColor(255,65,66,174));
	helpBText->setOverrideColor(video::SColor(255,64,64,64));
	helpBText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);

}

void GUIEditor::createEnvironmentTab()
{
	//Get the handle of the main tab controls 
	IGUITabControl* mainTabCtrl = (IGUITabControl*)GUIManager::getInstance()->getGUIElement(GUIManager::ID_TABCONTROL_MAIN);

	mainToolbarPos.Y=3;
	IGUITab* tabEnv = mainTabCtrl->addTab(LANGManager::getInstance()->getText("tab_environment").c_str());
	// Tab description box text
	IGUIStaticText * tileTabText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des_tile")).c_str(),
		core::rect<s32>(0,64,120,76),false,true,tabEnv,-1);
	//environmentTabText->setBackgroundColor(video::SColor(128,237,242,248));
	//environmentTabText->setOverrideColor(video::SColor(255,65,66,174));
	tileTabText->setBackgroundColor(video::SColor(255,238,240,242));
	tileTabText->setOverrideColor(video::SColor(255,86,95,109));
	tileTabText->setOverrideFont(GUIManager::getInstance()->guiFont10);
	tileTabText->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	IGUIStaticText * environmentTabText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des5")).c_str(),
		core::rect<s32>(130,64,240,76),false,true,tabEnv,-1);
	//environmentTabText->setBackgroundColor(video::SColor(128,237,242,248));
	//environmentTabText->setOverrideColor(video::SColor(255,65,66,174));
	environmentTabText->setBackgroundColor(video::SColor(255,238,240,242));
	environmentTabText->setOverrideColor(video::SColor(255,86,95,109));
	environmentTabText->setOverrideFont(GUIManager::getInstance()->guiFont10);
	environmentTabText->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	IGUIStaticText * vegetationTabText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des6")).c_str(),
		core::rect<s32>(250,64,360,76),false,true,tabEnv,-1);
	//vegetationTabText->setBackgroundColor(video::SColor(128,237,242,248));
	//vegetationTabText->setOverrideColor(video::SColor(255,65,66,174));
	vegetationTabText->setBackgroundColor(video::SColor(255,238,240,242));
	vegetationTabText->setOverrideColor(video::SColor(255,86,95,109));
	vegetationTabText->setOverrideFont(GUIManager::getInstance()->guiFont10);
	vegetationTabText->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	//Buttons
	//Transform Terrain

	//Add empty Segment
	s32 x=12;
	
	IGUIButton* guiTerrainAddEmptySegment = guienv->addButton(GUIManager::getInstance()->myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabEnv,
									 GUIManager::BT_ID_TERRAIN_ADD_EMPTY_SEGMENT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_terrain_empty_segments")).c_str());

    guiTerrainAddEmptySegment->setImage(driver->getTexture("../media/art/bt_terrain_add_segment.png"));
	guiTerrainAddEmptySegment->setPressedImage(driver->getTexture("../media/art/bt_terrain_add_segment_ghost.png"));

	IGUIStaticText * terrainSText2 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_empty_segments")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabEnv,-1);
	//terrainSText2->setOverrideColor(video::SColor(255,65,66,174));
	terrainSText2->setOverrideColor(video::SColor(255,64,64,64));
	terrainSText2->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	terrainSText2->setOverrideFont(GUIManager::getInstance()->guiFont9);


	//Terrain Add Segment
	x+= 60;
	IGUIButton* guiTerrainAddSegment = guienv->addButton(GUIManager::getInstance()->myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabEnv,
									 GUIManager::BT_ID_TERRAIN_ADD_SEGMENT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_terrain_segments")).c_str());

    guiTerrainAddSegment->setImage(driver->getTexture("../media/art/bt_terrain_add_segment.png"));
	guiTerrainAddSegment->setPressedImage(driver->getTexture("../media/art/bt_terrain_add_segment_ghost.png"));

	IGUIStaticText * terrainSText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_segments")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabEnv,-1);
	//terrainSText->setOverrideColor(video::SColor(255,65,66,174));
	terrainSText->setOverrideColor(video::SColor(255,64,64,64));
	terrainSText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	terrainSText->setOverrideFont(GUIManager::getInstance()->guiFont9);

	 x+= 100;

	 IGUIButton* guiTerrainTransform = guienv->addButton(GUIManager::getInstance()->myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabEnv,
									 GUIManager::BT_ID_TERRAIN_TRANSFORM,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_terrain_transform")).c_str());

    guiTerrainTransform->setImage(driver->getTexture("../media/art/bt_terrain_up.png"));
	guiTerrainTransform->setPressedImage(driver->getTexture("../media/art/bt_terrain_up_ghost.png"));

	IGUIStaticText * terrainTText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_transform")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabEnv,-1);
	//terrainTText->setOverrideColor(video::SColor(255,65,66,174));
	terrainTText->setOverrideColor(video::SColor(255,64,64,64));
	terrainTText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	terrainTText->setOverrideFont(GUIManager::getInstance()->guiFont9);

     x+= 70;
	  x+= 40;

    //Terrain Add Segment
	  IGUIButton* guiTerrainPaintVegetation = guienv->addButton(GUIManager::getInstance()->myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabEnv,
									 GUIManager::BT_ID_TERRAIN_PAINT_VEGETATION,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_paint_vegetation")).c_str());

    guiTerrainPaintVegetation->setImage(driver->getTexture("../media/art/bt_terrain_paint_vegetation.png"));
	guiTerrainPaintVegetation->setPressedImage(driver->getTexture("../media/art/bt_terrain_paint_vegetation_ghost.png"));

	IGUIStaticText * paintVText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_paint_vegetation")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabEnv,-1);
	//paintVText->setOverrideColor(video::SColor(255,65,66,174));
	paintVText->setOverrideColor(video::SColor(255,64,64,64));
	paintVText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	paintVText->setOverrideFont(GUIManager::getInstance()->guiFont9);
}

void GUIEditor::createObjectTab()
{
	//Get the handle of the main tab controls 
	IGUITabControl* mainTabCtrl = (IGUITabControl*)GUIManager::getInstance()->getGUIElement(GUIManager::ID_TABCONTROL_MAIN);

	IGUITab* tabObject = mainTabCtrl->addTab(LANGManager::getInstance()->getText("tab_objects").c_str());
	// Tab description box text
	IGUIStaticText * objectTabText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des1")).c_str(),
		core::rect<s32>(0,64,300,76),false,true,tabObject,-1);
	//objectTabText->setBackgroundColor(video::SColor(128,237,242,248));
	//objectTabText->setOverrideColor(video::SColor(255,65,66,174));
	objectTabText->setBackgroundColor(video::SColor(255,238,240,242));
	objectTabText->setOverrideColor(video::SColor(255,86,95,109));
	objectTabText->setOverrideFont(GUIManager::getInstance()->guiFont10);
	objectTabText->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	// Buttons
	//Dynamic Objects
	s32 x = 12;

	//Edit Character
	IGUIButton* guiEditCharacter = guienv->addButton(GUIManager::getInstance()->myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabObject,
									 GUIManager::BT_ID_EDIT_CHARACTER,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_edit_character")).c_str());

    guiEditCharacter->setImage(driver->getTexture("../media/art/bt_edit_character.png"));
	guiEditCharacter->setPressedImage(driver->getTexture("../media/art/bt_edit_character_ghost.png"));

	IGUIStaticText * editCharText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_edit_character")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabObject,-1);
	//editCharText->setOverrideColor(video::SColor(255,65,66,174));
	editCharText->setOverrideColor(video::SColor(255,64,64,64));
	editCharText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	editCharText->setOverrideFont(GUIManager::getInstance()->guiFont9);

	x += 60;
	IGUIButton* guiDynamicObjectsMode= guienv->addButton(GUIManager::getInstance()->myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabObject,
									 GUIManager::BT_ID_DYNAMIC_OBJECTS_MODE,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_mode")).c_str());

    guiDynamicObjectsMode->setImage(driver->getTexture("../media/art/bt_edit_npc.png"));
	guiDynamicObjectsMode->setPressedImage(driver->getTexture("../media/art/bt_edit_npc_ghost.png"));

	IGUIStaticText * dynObjText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_mode")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabObject,-1);
	//dynObjText->setOverrideColor(video::SColor(255,65,66,174));
	dynObjText->setOverrideColor(video::SColor(255,64,64,64));
	dynObjText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	dynObjText->setOverrideFont(GUIManager::getInstance()->guiFont9);

	x += 60;

	IGUIButton* guiDynamicObjectsProps= guienv->addButton(GUIManager::getInstance()->myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabObject,
									 GUIManager::BT_ID_DYNAMIC_OBJECTS_PROPS,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_prop")).c_str());

    guiDynamicObjectsProps->setImage(driver->getTexture("../media/art/bt_edit_prop.png"));
	guiDynamicObjectsProps->setPressedImage(driver->getTexture("../media/art/bt_edit_prop_ghost.png"));

	IGUIStaticText * dynObjText1 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_prop")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabObject,-1);
	//dynObjText->setOverrideColor(video::SColor(255,65,66,174));
	dynObjText1->setOverrideColor(video::SColor(255,64,64,64));
	dynObjText1->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	dynObjText1->setOverrideFont(GUIManager::getInstance()->guiFont9);

	x += 60;

	IGUIButton* guiDynamicObjectsLoot= guienv->addButton(GUIManager::getInstance()->myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabObject,
									 GUIManager::BT_ID_DYNAMIC_OBJECTS_LOOT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_loot")).c_str());

	guiDynamicObjectsLoot->setImage(driver->getTexture("../media/art/bt_edit_loot.png"));
	guiDynamicObjectsLoot->setPressedImage(driver->getTexture("../media/art/bt_edit_loot_ghost.png"));

	IGUIStaticText * dynObjText2 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_loot")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabObject,-1);
	//dynObjText->setOverrideColor(video::SColor(255,65,66,174));
	dynObjText2->setOverrideColor(video::SColor(255,64,64,64));
	dynObjText2->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	dynObjText2->setOverrideFont(GUIManager::getInstance()->guiFont9);

  	//x += 70;

    //-- Add custom segment (Custom Tiles button)
	x+= 60;
	IGUIButton* guiTerrainAddCustomSegment = guienv->addButton(GUIManager::getInstance()->myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
									 tabObject,
									 GUIManager::BT_ID_TERRAIN_ADD_CUSTOM_SEGMENT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_terrain_custom_segments")).c_str());

	guiTerrainAddCustomSegment->setImage(driver->getTexture("../media/art/bt_terrain_add_segment.png"));
	guiTerrainAddCustomSegment->setPressedImage(driver->getTexture("../media/art/bt_terrain_add_segment_ghost.png"));

	IGUIStaticText * terrainSText3 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_custom_segments")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabObject,-1);
	//terrainSText2->setOverrideColor(video::SColor(255,65,66,174));
	terrainSText3->setOverrideColor(video::SColor(255,64,64,64));
	terrainSText3->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	terrainSText3->setOverrideFont(GUIManager::getInstance()->guiFont9);

	//For 0.3 this will be removed. Will take until 0.4 until this is fully removed.
	//All scripting derives from object (dynamic_object). And this increase complexity for almost nothing.
	//Will only have to check for "globals" and theses can be useful to have objects "talks" to each others.
	//But could find something in C++ to do that. Since ALL is based on dynamic objects.

	//All the functionnality of the "global script" can be achieved by using another dynamic object.
	//I'm thinking of a special dynamic object that could be called "scene controller". in witch
	//You could monitor the scene, and use it for example to spawn characters and create waves,
	//Monitor the quests, etc. (This could be done with a simple hidden frog! :)

	//This special object, could be made from an empty node (so no rendering overhead)
	//Use the onUpdate() callback to start monitoring.
	//Then I could put back the button again to access it directly. An in essence would be a "editor" type object.

    //Edit Items Script
	IGUIButton* guiEditScriptGlobal = guienv->addButton(GUIManager::getInstance()->myRect(mainToolbarPos.X + x,mainToolbarPos.Y,32,32),
                                     tabObject,
									 GUIManager::BT_ID_EDIT_SCRIPT_GLOBAL,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_edit_script_global")).c_str());

	guiEditScriptGlobal->setVisible(false);
	/*

    guiEditScriptGlobal->setImage(driver->getTexture("../media/art/bt_edit_script_global.png"));
	guiEditScriptGlobal->setPressedImage(driver->getTexture("../media/art/bt_edit_script_global_ghost.png"));

	IGUIStaticText * editGlobSText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_edit_script_global")).c_str(),
		core::rect<s32>(x-10,36,x+45,65),false,true,tabObject,-1);
	//editGlobSText->setOverrideColor(video::SColor(255,65,66,174));
	editGlobSText->setOverrideColor(video::SColor(255,64,64,64));
	editGlobSText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	editGlobSText->setOverrideFont(guiFont9); */

}

void GUIEditor::createAboutWindowGUI()
{
	//ABOUT WINDOW
    //guiAboutWindow = guienv->addWindow(myRect(driver->getScreenSize().Width/2 - 300,driver->getScreenSize().Height/2 - 200,600,400),false);
	IGUIWindow* guiAboutWindow = guienv->addWindow(GUIManager::getInstance()->myRect(displaywidth/2 - 300,displayheight/2 - 200,600,400),false,0,0,GUIManager::GCW_ABOUT);
    guiAboutWindow->setDraggable(false);
    guiAboutWindow->setDrawTitlebar(false);
    guiAboutWindow->getCloseButton()->setVisible(false);
    guiAboutWindow->setVisible(false);
	guiAboutWindow->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

    //guienv->addImage(driver->getTexture("../media/art/logo1.png"),position2di(guiAboutWindow->getAbsoluteClippingRect().getWidth()/2-100,10),true,guiAboutWindow);
	IGUIImage * logo = guienv->addImage(driver->getTexture("../media/art/logo1.png"),position2di(guiAboutWindow->getClientRect().getWidth()/2-100,10),true,guiAboutWindow);

	logo->setAlignment(EGUIA_CENTER,EGUIA_CENTER,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
    //guiAboutClose = guienv->addButton(myRect(guiAboutWindow->getAbsoluteClippingRect().getWidth() - 37,guiAboutWindow->getAbsoluteClippingRect().getHeight() - 37,32,32),guiAboutWindow,BT_ID_ABOUT_WINDOW_CLOSE);
	IGUIButton* guiAboutClose = guienv->addButton(GUIManager::getInstance()->myRect(guiAboutWindow->getClientRect().getWidth() - 37,guiAboutWindow->getClientRect().getHeight() - 37,32,32),guiAboutWindow,GUIManager::BT_ID_ABOUT_WINDOW_CLOSE);
    guiAboutClose->setImage(driver->getTexture("../media/art/bt_yes_32.png"));
	guiAboutClose->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);

	//guiAboutText = guienv ->addListBox(myRect(guiAboutWindow->getAbsoluteClippingRect().getWidth()/2-250,160,500,200),guiAboutWindow);
	IGUIListBox* guiAboutText = guienv ->addListBox(GUIManager::getInstance()->myRect(guiAboutWindow->getClientRect().getWidth()/2-250,160,500,200),guiAboutWindow);
	guiAboutText->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

	// Ask the LANGManager to fill the box with the proper Language of the about text.
	LANGManager::getInstance()->setAboutText(guiAboutText);
}

void GUIEditor::createTerrainToolbar()
{
	//Get the handle of the main tool windows (aligned with the gameplay tool)
	IGUIWindow* guiMainToolWindow = (IGUIWindow*)GUIManager::getInstance()->getGUIElement(GUIManager::WIN_GAMEPLAY);
	
	///TERRAIN TOOLBAR
    IGUIWindow* guiTerrainToolbar = guienv->addWindow(
		//myRect(driver->getScreenSize().Width - 170,
		GUIManager::getInstance()->myRect(displaywidth - 220,
		//guiMainToolWindow->getAbsoluteClippingRect().getHeight(),
		guiMainToolWindow->getClientRect().getHeight()+3,
		220,
		//driver->getScreenSize().Height-guiMainToolWindow->getAbsoluteClippingRect().getHeight()),
		displayheight-guiMainToolWindow->getClientRect().getHeight()-28),
		false,stringw(LANGManager::getInstance()->getText("bt_terrain_brush")).c_str(),0,
		GUIManager::GCW_TERRAIN_TOOLBAR);


    guiTerrainToolbar->getCloseButton()->setVisible(false);

    guiTerrainToolbar->setDraggable(false);
    guiTerrainToolbar->setVisible(false);
	guiTerrainToolbar->setNotClipped(true);
	guiTerrainToolbar->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

	//Show Playable Area (areas with no Y == 0 will be red)
	mainToolbarPos.Y=20;

	// Display the brush strength
	IGUIStaticText* guiTerrainBrushStrengthLabel = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_transform_brush_strength_label")).c_str(),
		GUIManager::getInstance()->myRect(10,mainToolbarPos.Y+30,200,20),
		false,true, guiTerrainToolbar);
	
	IGUIStaticText* guiTerrainBrushStrengthValue = guienv->addStaticText(L"100",
		GUIManager::getInstance()->myRect(10,mainToolbarPos.Y+70,200,20),
		false,true, guiTerrainToolbar, GUIManager::TXT_ID_TERRAIN_BRUSH_STRENGTH);

	IGUIScrollBar* guiTerrainBrushStrength = guienv->addScrollBar(true,
		GUIManager::getInstance()->myRect(10,mainToolbarPos.Y+50,200,20),
		guiTerrainToolbar,GUIManager::SC_ID_TERRAIN_BRUSH_STRENGTH);

    guiTerrainBrushStrength->setMin(0);
    guiTerrainBrushStrength->setMax(400);
    guiTerrainBrushStrength->setPos(25);
	guiTerrainBrushStrength->setSmallStep(1);
	guiTerrainBrushStrength->setLargeStep(5);


	// Display the brush radius
	IGUIStaticText* guiTerrainBrushRadiusLabel = guienv->addStaticText(
		stringw(LANGManager::getInstance()->getText("bt_terrain_transform_brush_radius_label")).c_str(),
		GUIManager::getInstance()->myRect(10,mainToolbarPos.Y+90,200,20),
		false,true, guiTerrainToolbar);

	guienv->addStaticText(LANGManager::getInstance()->getText("bt_terrain_transform_brush_radius_inner").c_str(),
		GUIManager::getInstance()->myRect(10,mainToolbarPos.Y+150,150,20),
		false,true, guiTerrainToolbar);

	IGUIStaticText* guiTerrainBrushRadiusValue = guienv->addStaticText(L"100",
		GUIManager::getInstance()->myRect(10,mainToolbarPos.Y+130,200,20),
		false,true, guiTerrainToolbar, GUIManager::TXT_ID_TERRAIN_RADIUS);

	IGUIStaticText* guiTerrainBrushRadiusValue2 = guienv->addStaticText(L"100",
		GUIManager::getInstance()->myRect(10,mainToolbarPos.Y+190,200,20),
		false,true, guiTerrainToolbar, GUIManager::TXT_ID_TERRAIN_RADIUS1);

	guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_terrain_transform_plateau")).c_str(),
		GUIManager::getInstance()->myRect(10,mainToolbarPos.Y+210,200,20),
		false,true, guiTerrainToolbar);


    IGUIScrollBar* guiTerrainBrushRadius = guienv->addScrollBar(true,
		GUIManager::getInstance()->myRect(10,mainToolbarPos.Y+110,200,20),
		guiTerrainToolbar, GUIManager::SC_ID_TERRAIN_BRUSH_RADIUS );

    guiTerrainBrushRadius->setMin(0);
    guiTerrainBrushRadius->setMax(1024);
    guiTerrainBrushRadius->setPos(400);
	guiTerrainBrushRadius->setSmallStep(1);
	guiTerrainBrushRadius->setLargeStep(5);

	IGUIScrollBar* guiTerrainBrushRadius2 = guienv->addScrollBar(true,
		GUIManager::getInstance()->myRect(10,mainToolbarPos.Y+170,200,20),
		guiTerrainToolbar, GUIManager::SC_ID_TERRAIN_BRUSH_RADIUS2 );

    guiTerrainBrushRadius2->setMin(5);
    guiTerrainBrushRadius2->setMax(100);
    guiTerrainBrushRadius2->setPos(5);
	guiTerrainBrushRadius2->setSmallStep(1);
	guiTerrainBrushRadius2->setLargeStep(5);

	IGUIScrollBar* guiTerrainBrushPlateau = guienv->addScrollBar(true,
		core::rect<s32>(10,mainToolbarPos.Y+230,210,mainToolbarPos.Y+250),
		guiTerrainToolbar, GUIManager::SC_ID_TERRAIN_BRUSH_PLATEAU);

	guiTerrainBrushPlateau->setMin(-256);
	guiTerrainBrushPlateau->setMax(768);
	guiTerrainBrushPlateau->setPos(0);
	guiTerrainBrushPlateau->setSmallStep(1);
	guiTerrainBrushPlateau->setLargeStep(5);

	IGUIStaticText* guiTerrainBrushPlateauValue = guienv->addStaticText(L"0",
		GUIManager::getInstance()->myRect(10,mainToolbarPos.Y+250,200,20),
		false,true, guiTerrainToolbar, GUIManager::TXT_ID_TERRAIN_PLATEAU);

}

void GUIEditor::createVegetationToolbar()
{

	//Get the handle of the main tool windows (aligned with the gameplay tool)
	IGUIWindow* guiMainToolWindow = (IGUIWindow*)GUIManager::getInstance()->getGUIElement(GUIManager::WIN_GAMEPLAY);

	/*
	///Vegetation toolbar (Not yet implemented, will have a choice of vegetation and brush)
    guiVegetationToolbar = guienv->addWindow(
		//myRect(driver->getScreenSize().Width - 170,
		myRect(displaywidth - 170,
		//guiMainToolWindow->getAbsoluteClippingRect().getHeight(),
		guiMainToolWindow->getClientRect().getHeight()+3,
		170,
		//driver->getScreenSize().Height-guiMainToolWindow->getAbsoluteClippingRect().getHeight()),
		displayheight-guiMainToolWindow->getClientRect().getHeight()-28),
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
    guiVegetationBrushStrength->setPos(100);*/

	IGUIWindow* guiVegetationToolbar = guienv->addWindow(
		//myRect(driver->getScreenSize().Width - 170,
		GUIManager::getInstance()->myRect(displaywidth - 220,
		//guiMainToolWindow->getAbsoluteClippingRect().getHeight(),
		guiMainToolWindow->getClientRect().getHeight()+3,
		220,
		//driver->getScreenSize().Height-guiMainToolWindow->getAbsoluteClippingRect().getHeight()),
		displayheight-guiMainToolWindow->getClientRect().getHeight()-28),
		false,stringw(LANGManager::getInstance()->getText("bt_paint_vegetation")).c_str(),0, GUIManager::GCW_VEGE_TOOLBAR);


    guiVegetationToolbar->getCloseButton()->setVisible(false);

	guiVegetationToolbar->setDraggable(false);
    guiVegetationToolbar->setVisible(false);
	guiVegetationToolbar->setNotClipped(true);
	guiVegetationToolbar->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

	u32 x=10;
	u32 y=40;
	
	guienv->addImage(driver->getTexture("../media/vegetation/vege0.jpg"),vector2d<s32>(x,y),true,guiVegetationToolbar,GUIManager::VEGE_IMAGE);
	guienv->addCheckBox(true,core::rect<s32>(x+70,y,x+90,y+20),guiVegetationToolbar, GUIManager::VEGE_CHECKBOX);
	guienv->addStaticText(L"Enabled?",
		GUIManager::getInstance()->myRect(x+90,y+5,x+170,y+20),false,true, guiVegetationToolbar);
	y+=70;
	IGUIListBox* vegelistbox=guienv->addListBox(GUIManager::getInstance()->myRect(x,y,x+190,y+120),guiVegetationToolbar,GUIManager::VEGE_LISTBOX, true);

	//Populate the vegetation toolbar with the names of the vegetation items
	
	vector<stringw> names = TerrainManager::getInstance()->getVegetationNames();
	if (vegelistbox)
	{
		for(int i = 0; i<(int)names.size(); i++)
		{
			vegelistbox->addItem(names[i].c_str());
		}
	} 

	vegelistbox->setSelected(0); //Select the first tree in the list.

	/*IGUIButton* button = guienv->addButton(rect<s32>(x,y,x+64,y+64),guiVegetationToolbar,
                                     BT_VEGE_BASE,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_camera_rts")).c_str());
	button->setIsPushButton(true);
	button->setPressed(true);
	button->setImage(driver->getTexture("../media/vegetation/vege0.jpg"));
	button->setPressedImage(driver->getTexture("../media/vegetation/vege0dn.jpg"));
	
	y+=74;
	IGUIButton* button1 = guienv->addButton(rect<s32>(x,y,x+64,y+64),guiVegetationToolbar,
                                     BT_VEGE_BASE+1,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_camera_rts")).c_str());
	button1->setIsPushButton(true);
	button1->setPressed(true);
	button1->setImage(driver->getTexture("../media/vegetation/vege1.jpg"));
	button1->setPressedImage(driver->getTexture("../media/vegetation/vege1dn.jpg"));

	y+=74;
	IGUIButton* button2 = guienv->addButton(rect<s32>(x,y,x+64,y+64),guiVegetationToolbar,
                                     BT_VEGE_BASE+2,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_camera_rts")).c_str());
	button2->setIsPushButton(true);
	button2->setPressed(true);
	button2->setImage(driver->getTexture("../media/vegetation/vege2.jpg"));
	button2->setPressedImage(driver->getTexture("../media/vegetation/vege2dn.jpg"));

	y+=74;
	IGUIButton* button3 = guienv->addButton(rect<s32>(x,y,x+64,y+64),guiVegetationToolbar,
                                     BT_VEGE_BASE+3,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_camera_rts")).c_str());
	button3->setIsPushButton(true);
	button3->setPressed(true);
	button3->setImage(driver->getTexture("../media/vegetation/vege3.jpg"));
	button3->setPressedImage(driver->getTexture("../media/vegetation/vege3dn.jpg"));*/

	y+=74;
}

void GUIEditor::createDynamicObjectChooserGUI()
{

	//Get the handle of the main tool windows (aligned with the gameplay tool)
	IGUIWindow* guiMainToolWindow = (IGUIWindow*)GUIManager::getInstance()->getGUIElement(GUIManager::WIN_GAMEPLAY);

	// --- Dynamic Objects Chooser (to choose and place dynamic objects on the scenery)
	rect<s32> windowRect = GUIManager::getInstance()->myRect(displaywidth - 220,
	guiMainToolWindow->getClientRect().getHeight()+3,
	220,
	displayheight-guiMainToolWindow->getClientRect().getHeight()-28);


	CGUIExtWindow* guiDynamicPlayerWindowChooser = new CGUIExtWindow(stringw(LANGManager::getInstance()->getText("txt_player_info")).c_str(),
		guienv,guienv->getRootGUIElement(),GUIManager::GCW_DYNAMIC_PLAYER_EDIT,windowRect);

    guiDynamicPlayerWindowChooser->setDraggable(false);
    guiDynamicPlayerWindowChooser->getCloseButton()->setVisible(false);
   //guiDynamicObjectsWindowChooser->setDrawTitlebar(false);
	guiDynamicPlayerWindowChooser->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
	guiDynamicPlayerWindowChooser->setVisible(false);

	guiDynamicPlayerWindowChooser->setDevice(App::getInstance()->getDevice());
	guiDynamicPlayerWindowChooser->enableleft=false;
	guiDynamicPlayerWindowChooser->setMaxSize(core::dimension2du(545,2000));
	guiDynamicPlayerWindowChooser->setMinSize(core::dimension2du(220,10));


	//-- inner window
	rect<s32> winRect;
	winRect.UpperLeftCorner.X=10;
	winRect.UpperLeftCorner.Y=40;
	winRect.LowerRightCorner.X=windowRect.getWidth()-10;
	winRect.LowerRightCorner.Y=windowRect.getHeight()-10;

	//----------------------------------------------
	IGUIWindow* in = guienv->addWindow(winRect,false,L"",guiDynamicPlayerWindowChooser,0);
	in->setDraggable(false);
	in->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
	in->getCloseButton()->setVisible(false);
    in->setDrawTitlebar(false);
	in->setDrawBackground(false);

	//Backdrop
	IGUIStaticText * back1 = guienv->addStaticText(stringw("").c_str(),
		core::rect<s32>(5,0,195,105),false,true,in,-1);
	back1->setDrawBackground(true);
	back1->setDrawBorder(false);

	IGUIStaticText * back2 = guienv->addStaticText(stringw("").c_str(),
		core::rect<s32>(5,120,195,120+76),false,true,in,-1);
	back2->setDrawBackground(true);
	back2->setDrawBorder(false);
	//-- Buttons
	u32 x = 20; u32 y = 10;
	IGUIButton* cam1 = guienv->addButton(rect<s32>(x,y,x+32,y+32),
		in,
		GUIManager::BT_CAMERA_RTS,L"",
		stringw(LANGManager::getInstance()->getText("bt_camera_rts")).c_str());

	cam1->setImage(driver->getTexture("../media/art/bt_camera_rts.png"));
	cam1->setPressedImage(driver->getTexture("../media/art/bt_camera_rts_pressed.png"));

	IGUIStaticText * playGText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_camera_rts")).c_str(),
		core::rect<s32>(x,y+36,x+50,y+85),false,true,in,-1);

	cam1->setIsPushButton(true);
	cam1->setPressed(true);

	x+=60;
	IGUIButton* cam2 = guienv->addButton(rect<s32>(x,y,x+32,y+32),
		in,
		GUIManager::BT_CAMERA_RPG,L"",
		stringw(LANGManager::getInstance()->getText("bt_camera_rpg")).c_str());

	cam2->setImage(driver->getTexture("../media/art/bt_camera_rpg.png"));
	cam2->setPressedImage(driver->getTexture("../media/art/bt_camera_rpg_pressed.png"));

	IGUIStaticText * playGText2 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_camera_rpg")).c_str(),
		core::rect<s32>(x,y+36,x+50,y+85),false,true,in,-1);
	
	cam2->setIsPushButton(true);


	x+=60;
	IGUIButton* cam3 = guienv->addButton(rect<s32>(x,y,x+32,y+32),
		in,
		GUIManager::BT_CAMERA_FPS,L"",
		stringw(LANGManager::getInstance()->getText("bt_camera_fps")).c_str());

	cam3->setImage(driver->getTexture("../media/art/bt_camera_fps.png"));
    cam3->setPressedImage(driver->getTexture("../media/art/bt_camera_fps_pressed.png"));

	IGUIStaticText * playGText3 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_camera_fps")).c_str(),
		core::rect<s32>(x,y+36,x+50,y+85),false,true,in,-1);
	
	cam3->setIsPushButton(true);
	
	// Tab description box text
	x = 20;
	IGUIStaticText * objectTabText1 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des7")).c_str(),
		core::rect<s32>(x-15,y+20+64,x+175,y+20+76),false,true,in,-1);
	//objectTabText2->setBackgroundColor(video::SColor(128,237,242,248));
	//objectTabText2->setOverrideColor(video::SColor(255,65,66,174));
	objectTabText1->setBackgroundColor(video::SColor(255,238,240,242));
	objectTabText1->setOverrideColor(video::SColor(255,86,95,109));
	objectTabText1->setOverrideFont(GUIManager::getInstance()->guiFont10);
	objectTabText1->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);


	y+=120;
	///EDIT CHARACTER
	IGUIButton* guiPlayerEditScript = guienv->addButton(core::rect<s32>(x,y,x+32,y+32),
		in ,
		GUIManager::BT_ID_PLAYER_EDIT_SCRIPT,L"",
        stringw(LANGManager::getInstance()->getText("bt_player_edit_script")).c_str() );

	guiPlayerEditScript->setOverrideFont(GUIManager::getInstance()->guiFontC12);
    guiPlayerEditScript->setImage(driver->getTexture("../media/art/bt_player_edit_script.png"));
	guiPlayerEditScript->setPressedImage(driver->getTexture("../media/art/bt_player_edit_script_ghost.png"));

	IGUIStaticText * editCharSText = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("bt_player_edit_script")).c_str(),
		core::rect<s32>(x-10,y+36,x+45,y+65),false,true,in,-1);
	//editCharSText->setOverrideColor(video::SColor(255,65,66,174));
	editCharSText->setOverrideColor(video::SColor(255,64,64,64));
	editCharSText->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	editCharSText->setOverrideFont(GUIManager::getInstance()->guiFont9);

	// Tab description box text
	IGUIStaticText * objectTabText2 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_tool_des2")).c_str(),
		core::rect<s32>(x-15,y+64,x+175,y+76),false,true,in,-1);
	//objectTabText2->setBackgroundColor(video::SColor(128,237,242,248));
	//objectTabText2->setOverrideColor(video::SColor(255,65,66,174));
	objectTabText2->setBackgroundColor(video::SColor(255,238,240,242));
	objectTabText2->setOverrideColor(video::SColor(255,86,95,109));
	objectTabText2->setOverrideFont(GUIManager::getInstance()->guiFont10);
	objectTabText2->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	//guiPlayerEditScript->setNotClipped(true);

    //guiPlayerEditScript->setVisible(false);



	//-------------------- DYNAMIC OBJECT CHOOSER

    //guiDynamicObjectsWindowChooser = guienv->addWindow(windowRect,false,L"",0,GCW_DYNAMIC_OBJECT_CHOOSER);
	CGUIExtWindow* guiDynamicObjectsWindowChooser = new CGUIExtWindow(stringw(LANGManager::getInstance()->getText("txt_dynobjsel")).c_str(),
		guienv,guienv->getRootGUIElement(),
		GUIManager::GCW_DYNAMIC_OBJECT_CHOOSER,windowRect);

    guiDynamicObjectsWindowChooser->setDraggable(false);
    guiDynamicObjectsWindowChooser->getCloseButton()->setVisible(false);
   //guiDynamicObjectsWindowChooser->setDrawTitlebar(false);
	guiDynamicObjectsWindowChooser->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
	guiDynamicObjectsWindowChooser->setVisible(false);

	guiDynamicObjectsWindowChooser->setDevice(App::getInstance()->getDevice());
	guiDynamicObjectsWindowChooser->enableleft=true;
	guiDynamicObjectsWindowChooser->setMaxSize(core::dimension2du(545,2000));
	guiDynamicObjectsWindowChooser->setMinSize(core::dimension2du(220,10));

	// Enable manual dragging of the left portion of the pane

	/*guiDynamicObjectsWindowChooser->enablebottom=true;
	guiDynamicObjectsWindowChooser->enableright=true;
	guiDynamicObjectsWindowChooser->enabletop=true;*/


	// Mode select for dynamic object editing
	 s32 pos_Y = 40;
	 s32 pos_X = 10;

	//guiDynamicObjectEditModesPanel->setRelativePosition(core::position2di(45,35));
	//guiDynamicObjectEditModesPanel->setMaxSize(core::dimension2du(110,40));
	// Buttons

	 device=App::getInstance()->getDevice(); //Get a fresh copy of the device
	 IGUIButton* guiDOAddMode = guienv->addButton(core::rect<s32>(pos_X,pos_Y,pos_X+40,pos_Y+40),
		 guiDynamicObjectsWindowChooser,
		 GUIManager::BT_ID_DO_ADD_MODE, L"",
		 LANGManager::getInstance()->getText("btn_obj_add_mode").c_str());

	 guiDOAddMode->setImage(driver->getTexture("../media/art/DO_ADD.jpg"));
	 guiDOAddMode->setPressedImage(driver->getTexture("../media/art/DO_ADD1.jpg"));
	 guiDOAddMode->setIsPushButton(true);
	 guiDOAddMode->setUseAlphaChannel(true);
	 guiDOAddMode->setPressed(true);

	 pos_X += 40;
	 IGUIButton* guiDOSelMode = guienv->addButton(core::rect<s32>(pos_X,pos_Y,pos_X+40,pos_Y+40),
		 guiDynamicObjectsWindowChooser,
		 GUIManager::BT_ID_DO_SEL_MODE, L"",
		 LANGManager::getInstance()->getText("btn_obj_select_mode").c_str());

	 guiDOSelMode->setImage(driver->getTexture("../media/art/DO_SEL.jpg"));
	 guiDOSelMode->setPressedImage(driver->getTexture("../media/art/DO_SEL1.jpg"));
	 guiDOSelMode->setIsPushButton(true);
	 guiDOSelMode->setUseAlphaChannel(true);
	 guiDOSelMode->setPressed(false);

	 pos_X += 40;
	 IGUIButton* guiDOMovMode = guienv->addButton(core::rect<s32>(pos_X,pos_Y,pos_X+40,pos_Y+40),
		 guiDynamicObjectsWindowChooser,
		 GUIManager::BT_ID_DO_MOV_MODE, L"",
		 LANGManager::getInstance()->getText("btn_obj_move_mode").c_str());

	 guiDOMovMode->setImage(driver->getTexture("../media/art/DO_MOV.jpg"));
	 guiDOMovMode->setPressedImage(driver->getTexture("../media/art/DO_MOV1.jpg"));
	 guiDOMovMode->setIsPushButton(true);
	 guiDOMovMode->setUseAlphaChannel(true);
	 guiDOMovMode->setPressed(false);
	 //guiDOMovMode->setEnabled(false);

	 pos_X += 40;
	 IGUIButton* guiDORotMode = guienv->addButton(core::rect<s32>(pos_X,pos_Y,pos_X+40,pos_Y+40),
		 guiDynamicObjectsWindowChooser,
		 GUIManager::BT_ID_DO_ROT_MODE, L"",
		 LANGManager::getInstance()->getText("btn_obj_rotate_mode").c_str());

	 guiDORotMode->setImage(driver->getTexture("../media/art/DO_ROT.jpg"));
	 guiDORotMode->setPressedImage(driver->getTexture("../media/art/DO_ROT1.jpg"));
	 guiDORotMode->setIsPushButton(true);
	 guiDORotMode->setUseAlphaChannel(true);
	 guiDORotMode->setPressed(false);
	 //guiDORotMode->setEnabled(false);

	 pos_X += 40;
	 IGUIButton* guiDOScaMode = guienv->addButton(core::rect<s32>(pos_X,pos_Y,pos_X+40,pos_Y+40),
		 guiDynamicObjectsWindowChooser,
		 GUIManager::BT_ID_DO_SCA_MODE, L"",
		 LANGManager::getInstance()->getText("btn_obj_scale_mode").c_str());

	 guiDOScaMode->setImage(driver->getTexture("../media/art/DO_SCA.jpg"));
	 guiDOScaMode->setPressedImage(driver->getTexture("../media/art/DO_SCA1.jpg"));
	 guiDOScaMode->setIsPushButton(true);
	 guiDOScaMode->setUseAlphaChannel(true);
	 guiDOScaMode->setPressed(false);
	 //guiDOScaMode->setEnabled(false);

	//-- inner window
	rect<s32> windowRect2;
	windowRect2.UpperLeftCorner.X=10;
	windowRect2.UpperLeftCorner.Y=100;
	windowRect2.LowerRightCorner.X=windowRect.getWidth()-10;
	windowRect2.LowerRightCorner.Y=windowRect.getHeight()-10;

	//---------------------------------------------- ADD MODE content.
	IGUIWindow* InnerChooser = guienv->addWindow(windowRect2,false,L"",guiDynamicObjectsWindowChooser,GUIManager::GCW_DYNAMIC_OBJECT_CHOOSER_INNER);
	InnerChooser->setDraggable(false);
	InnerChooser->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
	InnerChooser->getCloseButton()->setVisible(false);
    InnerChooser->setDrawTitlebar(false);
	InnerChooser->setDrawBackground(false);

	pos_Y = 0;
    //guiDynamicObjectsWindowChooser_Y += 10;
	IGUIStaticText* text1 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_objectcol")).c_str(),core::rect<s32>(5,pos_Y,210,pos_Y+20),false,true,InnerChooser,-1);
	text1->setOverrideFont(GUIManager::getInstance()->guiFont12);

	pos_Y += 20;
	IGUIComboBox* guiDynamicObjects_Category = guienv->addComboBox(GUIManager::getInstance()->myRect(5,pos_Y,190,20),
		InnerChooser,
		GUIManager::CO_ID_DYNAMIC_OBJECT_OBJ_CATEGORY);

	guiDynamicObjects_Category->setMaxSelectionRows(24);

	// Populate a list of collection that contain only dynamic objects. (Default to all)
	for (int i=0 ; i< (int)DynamicObjectsManager::getInstance()->getObjectsCollections(GUIManager::LIST_NPC).size() ; i++)
	{
		core::stringw result = DynamicObjectsManager::getInstance()->getObjectsCollections(GUIManager::LIST_NPC)[i].c_str();
		if (result!=L"") //Collection with no name filtering
			guiDynamicObjects_Category->addItem(result.c_str());
	}

	pos_Y += 80;
	gui::IGUIStaticText* text2 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_dynobjcat")).c_str(),core::rect<s32>(5,pos_Y,210,pos_Y+20),false,true,InnerChooser,-1);
	text2->setOverrideFont(GUIManager::getInstance()->guiFont12);

	pos_Y += 20;
	IGUIListBox* guiDynamicObjects_OBJCategory = guienv->addListBox(GUIManager::getInstance()->myRect(5,pos_Y,190,80),
		InnerChooser,
		GUIManager::CO_ID_DYNAMIC_OBJECT_OBJLIST_CATEGORY,true);
	//guiDynamicObjects_OBJCategory->setDrawBackground(false);

	pos_Y += 95;
	gui::IGUIStaticText* text3 = guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_dynobjitm")).c_str(),core::rect<s32>(5,pos_Y,210,pos_Y+20),false,true,InnerChooser,-1);
	text3->setOverrideFont(GUIManager::getInstance()->guiFont12);

	pos_Y += 20;
	s32 boxend = screensize.Height-(pos_Y+325);
	if (boxend<10)
		boxend=10;

	IGUIListBox* guiDynamicObjects_OBJChooser = guienv->addListBox(GUIManager::getInstance()->myRect(5,pos_Y,190,boxend),
		InnerChooser,
		GUIManager::CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER,true);

	//guiDynamicObjects_OBJChooser->setDrawBackground(false);
	guiDynamicObjects_OBJChooser->setAlignment(EGUIA_UPPERLEFT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

	IGUIButton* guiDynamicObjectsInfo= guienv->addButton(GUIManager::getInstance()->myRect(5,pos_Y+boxend+10,190,20),
		InnerChooser,
		GUIManager::BT_ID_DYNAMIC_OBJECT_INFO,
		LANGManager::getInstance()->getText("panel_infobut").c_str());

	guiDynamicObjectsInfo->setOverrideFont(GUIManager::getInstance()->guiFontC12);
	guiDynamicObjectsInfo->setAlignment(EGUIA_UPPERLEFT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);

	
	//---------------------------------------------- Info portions - ADD MODE
	pos_X = 200;
	pos_Y = 5;
	IGUIStaticText * infotext = guienv->addStaticText(LANGManager::getInstance()->getText("panel_infotext").c_str(),core::rect<s32>(pos_X+5,pos_Y,pos_X+310,20),false,true,InnerChooser,-1);
	infotext->setDrawBackground(true);
	infotext->setDrawBorder(true);
	infotext->setBackgroundColor(video::SColor(255,237,242,248));
	infotext->setOverrideColor(video::SColor(255,65,66,174));
	infotext->setOverrideFont(GUIManager::getInstance()->guiFontCourier12);
	infotext->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);

	IGUIImage* thumbnail=guienv->addImage(info_current,vector2d<s32>(pos_X+5,pos_Y+20),true,InnerChooser, GUIManager::IMG_THUMBNAIL);

	pos_Y+=220;

	IGUIStaticText * infotext1 = guienv->addStaticText(LANGManager::getInstance()->getText("panel_infomodel").c_str(),core::rect<s32>(pos_X+5,pos_Y,pos_X+310,pos_Y+39),false,true,InnerChooser,-1);
	infotext1->setOverrideFont(GUIManager::getInstance()->guiFont12);

	pos_Y+=15;
	IGUIStaticText* mdl_name = guienv->addStaticText(L"",core::rect<s32>(pos_X+5,pos_Y,pos_X+310,pos_Y+20),
		true,true,InnerChooser,GUIManager::TXT_ID_MDL_NAME);

	mdl_name->setDrawBackground(true);
	mdl_name->setBackgroundColor(video::SColor(255,237,242,248));
	mdl_name->setOverrideFont(GUIManager::getInstance()->guiFont10);
	mdl_name->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_CENTER);

	pos_Y+=25;

	IGUIStaticText * infotext2 = guienv->addStaticText(LANGManager::getInstance()->getText("panel_infodesc").c_str(),core::rect<s32>(pos_X+5,pos_Y,pos_X+310,pos_Y+39),false,true,InnerChooser,-1);
	infotext2->setOverrideFont(GUIManager::getInstance()->guiFont12);

	pos_Y+=15;
	IGUIStaticText* mdl_desc = guienv->addStaticText(L"",core::rect<s32>(pos_X+5,pos_Y,pos_X+310,pos_Y+100),
		true,true,InnerChooser, GUIManager::TXT_ID_MDL_DESC);

	mdl_desc->setDrawBackground(true);
	mdl_desc->setBackgroundColor(video::SColor(255,237,242,248));
	mdl_desc->setOverrideFont(GUIManager::getInstance()->guiFont10);

	pos_Y+=110;
	IGUIStaticText * infotext3 = guienv->addStaticText(LANGManager::getInstance()->getText("panel_infoauthor").c_str(),core::rect<s32>(pos_X+5,pos_Y,pos_X+310,pos_Y+39),false,true,InnerChooser,-1);
	infotext3->setOverrideFont(GUIManager::getInstance()->guiFont12);

	pos_Y+=15;
	IGUIStaticText* mdl_auth = guienv->addStaticText(L"",core::rect<s32>(pos_X+5,pos_Y,pos_X+310,pos_Y+20),
		true,true,InnerChooser,GUIManager::TXT_ID_MDL_AUTH);

	mdl_auth->setDrawBackground(true);
	mdl_auth->setBackgroundColor(video::SColor(255,237,242,248));
	mdl_auth->setOverrideFont(GUIManager::getInstance()->guiFont10);
	mdl_auth->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_CENTER);

	pos_Y+=25;
	IGUIStaticText * infotext4 = guienv->addStaticText(LANGManager::getInstance()->getText("panel_infolicence").c_str(),core::rect<s32>(pos_X+5,pos_Y,pos_X+310,pos_Y+39),false,true,InnerChooser,-1);
	infotext4->setOverrideFont(GUIManager::getInstance()->guiFont12);

	pos_Y+=15;
	IGUIStaticText* mdl_lic = guienv->addStaticText(L"",core::rect<s32>(pos_X+5,pos_Y,pos_X+310,pos_Y+20),true,true,InnerChooser,GUIManager::TXT_ID_MDL_LIC);
	
	mdl_lic->setDrawBackground(true);
	mdl_lic->setBackgroundColor(video::SColor(255,237,242,248));
	mdl_lic->setOverrideFont(GUIManager::getInstance()->guiFont10);
	mdl_lic->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_CENTER);
	// -- end info portions
	UpdateGUIChooser(GUIManager::LIST_NPC);

	//---------------------------------------------- SELECT MODE PORTION
	/// Define the portion when in select mode
	IGUIWindow* InnerChooser1 = guienv->addWindow(windowRect2,false,L"",guiDynamicObjectsWindowChooser, GUIManager::GCW_DYNAMIC_OBJECT_CHOOSER_INNER1);
	InnerChooser1->setDraggable(false);
	//InnerChooser1->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
	InnerChooser1->getCloseButton()->setVisible(false);
    InnerChooser1->setDrawTitlebar(false);
	InnerChooser1->setDrawBackground(false);
	InnerChooser1->setSubElement(false);

	pos_X = 10;
	pos_Y = 5;
	IGUIButton * button = NULL;
	//Script editor button

	IGUIStaticText * background = guienv->addStaticText(L"",core::rect<s32>(pos_X-5,pos_Y,pos_X+185,pos_Y+140),true,false,InnerChooser1,-1,true); //Box border & background for infos
	background->setBackgroundColor(video::SColor(255,240,240,240));

	IGUIStaticText * objinfotext1 =  guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_object").c_str(),
		core::rect<s32>(pos_X,pos_Y,pos_X+180,pos_Y+30),false,true,InnerChooser1);

	objinfotext1->setOverrideFont(GUIManager::getInstance()->guiFont12);

	pos_Y+=15;
	IGUIStaticText * objtext1 =  guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_sel1").c_str(),
		core::rect<s32>(pos_X,pos_Y,pos_X+180,pos_Y+30),
		false,true,InnerChooser1,GUIManager::TXT_ID_SELOBJECT);

	objtext1->setOverrideColor(video::SColor(255,60,129,220));

	pos_Y+=20;
	IGUIStaticText * objinfotext2 =  guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_type").c_str(),
		core::rect<s32>(pos_X,pos_Y,pos_X+180,pos_Y+30),
		false,true,InnerChooser1);

	objinfotext2->setOverrideFont(GUIManager::getInstance()->guiFont12);

	pos_Y+=15;
	IGUIStaticText * objtext2 =  guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_sel1").c_str(),
		core::rect<s32>(pos_X,pos_Y,pos_X+180,pos_Y+30),
		false,true,InnerChooser1,GUIManager::TXT_ID_SELOBJECT_TYPE);

	objtext2->setOverrideColor(video::SColor(255,60,129,220));

	pos_Y+=20;
	IGUIStaticText * objinfotext4 =  guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_script").c_str(),
		core::rect<s32>(pos_X,pos_Y,pos_X+180,pos_Y+30),
		false,true,InnerChooser1);

	objinfotext4->setOverrideFont(GUIManager::getInstance()->guiFont12);

	pos_Y+=15;
	IGUIStaticText * objtext4 =  guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_sel1").c_str(),
		core::rect<s32>(pos_X,pos_Y,pos_X+180,pos_Y+30),
		false,true,InnerChooser1,GUIManager::TXT_ID_OBJ_SCRIPT);

	objtext4->setOverrideColor(video::SColor(255,60,129,220));

	pos_Y+=20;
	IGUIStaticText * objinfotext3 =  guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_template").c_str(),
		core::rect<s32>(pos_X,pos_Y,pos_X+180,pos_Y+30),
		false,true,InnerChooser1);

	objinfotext3->setOverrideFont(GUIManager::getInstance()->guiFont12);

	pos_Y+=15;
	IGUIStaticText * objtext3 =  guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_sel1").c_str(),
		core::rect<s32>(pos_X,pos_Y,pos_X+185,pos_Y+30),
		false,true,InnerChooser1,GUIManager::TXT_ID_CUR_TEMPLATE);

	objtext3->setOverrideColor(video::SColor(255,60,129,220));

	pos_X-=5;
	pos_Y+=30;
	button = guienv->addButton(GUIManager::getInstance()->myRect(pos_X,pos_Y,190,20), 
		InnerChooser1, GUIManager::BT_ID_DYNAMIC_OBJECT_BT_EDITSCRIPTS,
		stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_edit_script")).c_str() );

	pos_Y+=30;
	button=guienv->addButton(GUIManager::getInstance()->myRect(pos_X,pos_Y,190,20),
		InnerChooser1, GUIManager::BT_ID_DYNAMIC_OBJECT_BT_REMOVE,
		stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_remove")).c_str() );

	pos_Y+=30;
	button=guienv->addButton(GUIManager::getInstance()->myRect(pos_X,pos_Y,190,20),
		InnerChooser1, GUIManager::BT_ID_DYNAMIC_OBJECT_BT_REPLACE2,
		LANGManager::getInstance()->getText("bt_dynamic_objects_replace").c_str());

	pos_Y+=30;
	button=guienv->addButton(GUIManager::getInstance()->myRect(pos_X,pos_Y,190,20),
		InnerChooser1, GUIManager::BT_ID_DYNAMIC_OBJECT_BT_CENTER,
		LANGManager::getInstance()->getText("bt_dynamic_objects_centerview").c_str());

	InnerChooser1->setVisible(false);

	//---------------------------------------------- SELECT - MOVE - ROTATE - SCALE MODES RIGHT PORTION
	/// Define the portion when in move/rotate/scale
	IGUIWindow* InnerChooser2 = guienv->addWindow(windowRect2,false,L"",guiDynamicObjectsWindowChooser, GUIManager::GCW_DYNAMIC_OBJECT_CHOOSER_INNER2);
	InnerChooser2->setDraggable(false);
	InnerChooser2->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
	InnerChooser2->getCloseButton()->setVisible(false);
    InnerChooser2->setDrawTitlebar(false);
	InnerChooser2->setDrawBackground(false);
	InnerChooser2->setSubElement(true);

	//Line 1 Position
	guienv->addStaticText(L"",GUIManager::getInstance()->myRect(5,5,190,90),true,true,InnerChooser2);
	guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_position").c_str(),
		GUIManager::getInstance()->myRect(10,10,160,20),false,false,InnerChooser2);

	//position X axis
	guienv->addStaticText(L"X",GUIManager::getInstance()->myRect(10,28,15,20),
		false,false,InnerChooser2);

	IGUISpinBox* pos_x_text = guienv->addSpinBox(L"X:",GUIManager::getInstance()->myRect(25,25,100,20),
		true, InnerChooser2, GUIManager::TI_ID_POS_X);

		//guienv->addEditBox(L"X:",myRect(25,25,100,20), true, InnerChooser2, TI_ID_POS_X);
	guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_lock").c_str(),
		GUIManager::getInstance()->myRect(130,28,50,20),
		false,false,InnerChooser2);

	//pos_x_text->setMultiLine(false);
	//pos_x_text->setText(L"0.000000");
	pos_x_text->setValue(0.0f);
	IGUICheckBox* pos_x_lock = guienv->addCheckBox(false,GUIManager::getInstance()->myRect(160,25,20,20),
		InnerChooser2,GUIManager::CB_ID_POS_X);

	//position Y axis
	guienv->addStaticText(L"Y",
		GUIManager::getInstance()->myRect(10,48,15,20),
		false,false,InnerChooser2);

	IGUISpinBox* pos_y_text = guienv->addSpinBox(L"X:",
		GUIManager::getInstance()->myRect(25,45,100,20),
		true, InnerChooser2, GUIManager::TI_ID_POS_Y);

	guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_lock").c_str(),
		GUIManager::getInstance()->myRect(130,48,50,20),
		false,false,InnerChooser2);

	//pos_y_text->setMultiLine(false);
	//pos_y_text->setText(L"0.000000");
	pos_y_text->setValue(0.0f);
	IGUICheckBox* pos_y_lock = guienv->addCheckBox(false,GUIManager::getInstance()->myRect(160,45,20,20),
		InnerChooser2, GUIManager::CB_ID_POS_Y);

	//position Z axis
	guienv->addStaticText(L"Z",
		GUIManager::getInstance()->myRect(10,68,15,20),
		false,false,InnerChooser2);

	IGUISpinBox* pos_z_text = guienv->addSpinBox(L"X:",
		GUIManager::getInstance()->myRect(25,65,100,20),
		true, InnerChooser2, GUIManager::TI_ID_POS_Z);

	guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_lock").c_str(),
		GUIManager::getInstance()->myRect(130,68,50,20),
		false,false,InnerChooser2);

	//pos_z_text->setMultiLine(false);
	//pos_z_text->setText(L"0.000000");
	pos_z_text->setValue(0.0f);
	IGUICheckBox* pos_z_lock = guienv->addCheckBox(false,
		GUIManager::getInstance()->myRect(160,65,20,20),
		InnerChooser2, GUIManager::CB_ID_POS_Z);

	//Rotation
	guienv->addStaticText(L"",
		GUIManager::getInstance()->myRect(5,100,190,90),
		true,true,InnerChooser2);

	guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_rotation").c_str(),
		GUIManager::getInstance()->myRect(10,110,160,20),
		false,false,InnerChooser2);

	//rotation X axis
	guienv->addStaticText(L"X",
		GUIManager::getInstance()->myRect(10,128,15,20),
		false,false,InnerChooser2);

	IGUISpinBox* rot_x_text = guienv->addSpinBox(L"X:",
		GUIManager::getInstance()->myRect(25,125,100,20),
		true, InnerChooser2, GUIManager::TI_ID_ROT_X);

	guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_lock").c_str(),
		GUIManager::getInstance()->myRect(130,128,50,20),
		false,false,InnerChooser2);

	//rot_x_text->setMultiLine(false);
	//rot_x_text->setText(L"0.000000");
	rot_x_text->setValue(0.0f);
	IGUICheckBox* rot_x_lock = guienv->addCheckBox(true,
		GUIManager::getInstance()->myRect(160,125,20,20),
		InnerChooser2, GUIManager::CB_ID_ROT_X);

	//rotation Y axis
	guienv->addStaticText(L"Y",GUIManager::getInstance()->myRect(10,148,15,20),
		false,false,InnerChooser2);

	IGUISpinBox* rot_y_text = guienv->addSpinBox(L"X:",
		GUIManager::getInstance()->myRect(25,145,100,20),
		true, InnerChooser2, GUIManager::TI_ID_ROT_Y);

	guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_lock").c_str(),
		GUIManager::getInstance()->myRect(130,148,50,20),
		false,false,InnerChooser2);

	//rot_y_text->setMultiLine(false);
	//rot_y_text->setText(L"0.000000");
	rot_y_text->setValue(0.0f);
	IGUICheckBox* rot_y_lock = guienv->addCheckBox(false,
		GUIManager::getInstance()->myRect(160,145,20,20),
		InnerChooser2, GUIManager::CB_ID_ROT_Y);

	//rotation Z axis
	guienv->addStaticText(L"Z",
		GUIManager::getInstance()->myRect(10,168,15,20),
		false,false,InnerChooser2);

	IGUISpinBox* rot_z_text = guienv->addSpinBox(L"X:",
		GUIManager::getInstance()->myRect(25,165,100,20),
		true, InnerChooser2, GUIManager::TI_ID_ROT_Z);

	guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_lock").c_str(),
		GUIManager::getInstance()->myRect(130,168,50,20),
		false,false,InnerChooser2);

	//rot_z_text->setMultiLine(false);
	//rot_z_text->setText(L"0.000000");
	rot_z_text->setValue(0.0f);
	IGUICheckBox* rot_z_lock = guienv->addCheckBox(false,
		GUIManager::getInstance()->myRect(160,165,20,20),
		InnerChooser2, GUIManager::CB_ID_ROT_Z);

	guienv->addStaticText(L"",
		GUIManager::getInstance()->myRect(5,200,190,90),
		true,true,InnerChooser2);

	guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_scale").c_str(),
		GUIManager::getInstance()->myRect(10,210,160,20),
		false,false,InnerChooser2);

	//Scale X axis
	guienv->addStaticText(L"X",
		GUIManager::getInstance()->myRect(10,228,15,20),
		false,false,InnerChooser2);

	IGUISpinBox* sca_x_text = guienv->addSpinBox(L"X:",
		GUIManager::getInstance()->myRect(25,225,100,20),
		true, InnerChooser2, GUIManager::TI_ID_SCA_X);

	guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_lock").c_str(),
		GUIManager::getInstance()->myRect(130,228,50,20),
		false,false,InnerChooser2);

	//sca_x_text->setMultiLine(false);
	//sca_x_text->setText(L"0.000000");
	sca_x_text->setValue(0.0f);
	IGUICheckBox* sca_x_lock = guienv->addCheckBox(false,
		GUIManager::getInstance()->myRect(160,225,20,20),
		InnerChooser2, GUIManager::CB_ID_SCA_X);

	//Scale Y axis
	guienv->addStaticText(L"Y",
		GUIManager::getInstance()->myRect(10,248,15,20),
		false,false,InnerChooser2);

	IGUISpinBox* sca_y_text = guienv->addSpinBox(L"X:",
		GUIManager::getInstance()->myRect(25,245,100,20),
		true, InnerChooser2, GUIManager::TI_ID_SCA_Y);

	guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_lock").c_str(),
		GUIManager::getInstance()->myRect(130,248,50,20),
		false,false,InnerChooser2);

	//sca_y_text->setMultiLine(false);
	//sca_y_text->setText(L"0.000000");
	sca_y_text->setValue(0.0f);
	IGUICheckBox* sca_y_lock = guienv->addCheckBox(false,
		GUIManager::getInstance()->myRect(160,245,20,20),
		InnerChooser2, GUIManager::CB_ID_SCA_Y);

	//Scale Z axis
	guienv->addStaticText(L"Z",
		GUIManager::getInstance()->myRect(10,268,15,20),
		false,false,InnerChooser2);

	IGUISpinBox* sca_z_text = guienv->addSpinBox(L"X:",
		GUIManager::getInstance()->myRect(25,265,100,20),
		true, InnerChooser2, GUIManager::TI_ID_SCA_Z);

	guienv->addStaticText(LANGManager::getInstance()->getText("panel_sel_lock").c_str(),
		GUIManager::getInstance()->myRect(130,268,50,20),
		false,false,InnerChooser2);

	//sca_z_text->setMultiLine(false);
	//sca_z_text->setText(L"0.000000");
	sca_z_text->setValue(0.0f);
	IGUICheckBox* sca_z_lock = guienv->addCheckBox(false,
		GUIManager::getInstance()->myRect(160,265,20,20),
		InnerChooser2, GUIManager::CB_ID_SCA_Z);

	InnerChooser2->setVisible(false);

	//---------------------------------------------- SELECT MODE RIGHT portion
	// Right portion of the GUI in SELECT MODE will contain a way to select object by list of object types
	windowRect2.UpperLeftCorner.X=220;
	windowRect2.UpperLeftCorner.Y=35;
	windowRect2.LowerRightCorner.X=540;
	windowRect2.LowerRightCorner.Y=windowRect.getHeight()-10;

	IGUIWindow* InnerChooser3 = guienv->addWindow(windowRect2,false,L"",guiDynamicObjectsWindowChooser, GUIManager::GCW_DYNAMIC_OBJECT_CHOOSER_INNER3);
	InnerChooser3->getCloseButton()->setVisible(false);
	InnerChooser3->setDrawTitlebar(true);
	InnerChooser3->setDrawBackground(false);
	InnerChooser3->setDraggable(false);
	InnerChooser3->setSubElement(true);
	//InnerChooser3->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

	pos_X=0;
	pos_Y=5;
	//Elements of this windows
	IGUIStaticText * it_1 = guienv->addStaticText(LANGManager::getInstance()->getText("panel_sellist").c_str(),
		core::rect<s32>(pos_X+5,pos_Y,pos_X+310,pos_Y+39),
		false,true,InnerChooser3,-1);

	it_1->setOverrideFont(GUIManager::getInstance()->guiFont12);

	pos_Y+=20;
	guienv->addStaticText(LANGManager::getInstance()->getText("panel_sellist_filter").c_str(),
		core::rect<s32>(pos_X+5,pos_Y,pos_X+100,pos_Y+39),
		false,true,InnerChooser3);

	IGUIComboBox* guiDynamicObjects_listfilter = guienv->addComboBox(
		GUIManager::getInstance()->myRect(120,pos_Y+5,180,20),
		InnerChooser3, GUIManager::CO_ID_ACTIVE_LIST_FILTER);

	guiDynamicObjects_listfilter->setMaxSelectionRows(24);
	guiDynamicObjects_listfilter->addItem(LANGManager::getInstance()->getText("panel_selcombo_all").c_str());
	guiDynamicObjects_listfilter->addItem(LANGManager::getInstance()->getText("panel_selcombo_npc").c_str());
	guiDynamicObjects_listfilter->addItem(LANGManager::getInstance()->getText("panel_selcombo_loot").c_str());
	guiDynamicObjects_listfilter->addItem(LANGManager::getInstance()->getText("panel_selcombo_prop").c_str());
	guiDynamicObjects_listfilter->addItem(LANGManager::getInstance()->getText("panel_selcombo_intprop").c_str());
	guiDynamicObjects_listfilter->addItem(LANGManager::getInstance()->getText("panel_selcombo_walk").c_str());

	pos_Y+=60;
	IGUIListBox* guiSceneObjectList = guienv->addListBox(
		GUIManager::getInstance()->myRect(5,pos_Y,260,320),
		InnerChooser3, GUIManager::CO_ID_ACTIVE_SCENE_LIST,true);

	guiSceneObjectList->addItem(LANGManager::getInstance()->getText("panel_sel_noscene").c_str());
	guiSceneObjectList->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
	InnerChooser3->setVisible(false);

}

// --- Contextual menu for the dynamic objects
void GUIEditor::createContextMenuGUI()
{

    IGUIWindow* guiDynamicObjects_Context_Menu_Window = guienv->addWindow(
		GUIManager::getInstance()->myRect(0,0,200,240),
		false,L"",0,GUIManager::GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU);

    guiDynamicObjects_Context_Menu_Window->getCloseButton()->setVisible(false);
    guiDynamicObjects_Context_Menu_Window->setDraggable(false);
    guiDynamicObjects_Context_Menu_Window->setDrawTitlebar(false);
    guiDynamicObjects_Context_Menu_Window->setVisible(false);

	IGUIStaticText* contexttitle = guienv->addStaticText(LANGManager::getInstance()->getText("txt_context_title").c_str(),core::rect<s32>(0,5,200,30),false,true,guiDynamicObjects_Context_Menu_Window,-1);
	contexttitle->setOverrideFont(GUIManager::getInstance()->guiFont14);
	contexttitle->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
	//contexttitle->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);


	u32 pby = 30; //vertical initial position of the button
	IGUIButton* guiDynamicObjects_Context_btMoveRotate= guienv->addButton(
		GUIManager::getInstance()->myRect(5,pby,190,20),
		guiDynamicObjects_Context_Menu_Window,
		GUIManager::BT_ID_DYNAMIC_OBJECT_BT_MOVEROTATE,
		stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_move_rotate")).c_str() );

	guiDynamicObjects_Context_btMoveRotate->setOverrideFont(GUIManager::getInstance()->guiFontC12);
	pby+=25;

	IGUIButton* guiDynamicObjects_Context_btEditScript = guienv->addButton(
		GUIManager::getInstance()->myRect(5,pby,190,20),
		guiDynamicObjects_Context_Menu_Window,
		GUIManager::BT_ID_DYNAMIC_OBJECT_BT_EDITSCRIPTS,
		stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_edit_script")).c_str() );

	guiDynamicObjects_Context_btEditScript->setOverrideFont(GUIManager::getInstance()->guiFontC12);
	pby+=30;

	IGUIButton* guiDynamicObjects_Context_btSpawn = guienv->addButton(
		GUIManager::getInstance()->myRect(5,pby,190,20),
		guiDynamicObjects_Context_Menu_Window,
		GUIManager::BT_ID_DYNAMIC_OBJECT_BT_SPAWN,
		LANGManager::getInstance()->getText("txt_context_spawn").c_str());

	guiDynamicObjects_Context_btSpawn->setOverrideFont(GUIManager::getInstance()->guiFontC12);
	pby+=25;

	IGUIButton* guiDynamicObjects_Context_btReplace = guienv->addButton(
		GUIManager::getInstance()->myRect(5,pby,190,20),
		guiDynamicObjects_Context_Menu_Window,
		GUIManager::BT_ID_DYNAMIC_OBJECT_BT_REPLACE,
		LANGManager::getInstance()->getText("txt_context_replace_file").c_str());

	guiDynamicObjects_Context_btReplace->setOverrideFont(GUIManager::getInstance()->guiFontC12);
	guiDynamicObjects_Context_btReplace->setEnabled(true);
	pby+=25;

	IGUIButton* guiDynamicObjects_Context_btReplace2 = guienv->addButton(
		GUIManager::getInstance()->myRect(5,pby,190,20),
		guiDynamicObjects_Context_Menu_Window,
		GUIManager::BT_ID_DYNAMIC_OBJECT_BT_REPLACE2,
		LANGManager::getInstance()->getText("bt_dynamic_objects_replace").c_str());

	guiDynamicObjects_Context_btReplace2->setOverrideFont(GUIManager::getInstance()->guiFontC12);
	pby+=30;

	IGUIButton* guiDynamicObjects_Context_btRemove= guienv->addButton(
		GUIManager::getInstance()->myRect(5,pby,190,20),
		guiDynamicObjects_Context_Menu_Window,
		GUIManager::BT_ID_DYNAMIC_OBJECT_BT_REMOVE,
		stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_remove")).c_str() );

	guiDynamicObjects_Context_btRemove->setOverrideFont(GUIManager::getInstance()->guiFontC12);

	pby+=30;

	IGUIButton * button = guienv->addButton(
		GUIManager::getInstance()->myRect(5,pby,190,20),
		guiDynamicObjects_Context_Menu_Window,
		GUIManager::BT_ID_DYNAMIC_OBJECT_BT_CENTER,
		LANGManager::getInstance()->getText("bt_dynamic_objects_centerview").c_str());

	button->setOverrideFont(GUIManager::getInstance()->guiFontC12);

	pby+=25;

	// The windows now close dynamicaly.
    /*guiDynamicObjects_Context_btCancel= guienv->addButton(myRect(5,pby,190,20),
                                                           guiDynamicObjects_Context_Menu_Window,
                                                           BT_ID_DYNAMIC_OBJECT_BT_CANCEL,
                                                           stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_cancel")).c_str() );
	guiDynamicObjects_Context_btCancel->setOverrideFont(guiFontC12);
	pby+=25;*/

	// Second context menu
	IGUIWindow* guiDynamicObjects_Context_Menu_Window1 = guienv->addWindow(
		GUIManager::getInstance()->myRect(0,100,200,60),
		false,L"",0,GUIManager::GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU1);

    guiDynamicObjects_Context_Menu_Window1->getCloseButton()->setVisible(false);
    guiDynamicObjects_Context_Menu_Window1->setDraggable(false);
    guiDynamicObjects_Context_Menu_Window1->setDrawTitlebar(false);
    guiDynamicObjects_Context_Menu_Window1->setVisible(false);

	IGUIStaticText* contexttitle1 = guienv->addStaticText(LANGManager::getInstance()->getText("txt_context_title").c_str(),
		core::rect<s32>(0,5,200,30),
		false,true,guiDynamicObjects_Context_Menu_Window1,-1);

	contexttitle1->setOverrideFont(GUIManager::getInstance()->guiFont14);
	contexttitle1->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
	pby = 30;

	IGUIButton * buttoncenter = guienv->addButton(
		GUIManager::getInstance()->myRect(5,pby,190,20),
		guiDynamicObjects_Context_Menu_Window1,
		GUIManager::BT_ID_DYNAMIC_VIEW_BT_CENTER,
		LANGManager::getInstance()->getText("bt_centerview").c_str());

	button->setOverrideFont(GUIManager::getInstance()->guiFontC12);

	pby+=25;


}

void GUIEditor::createCodeEditorGUI()
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
	CGUIExtWindow* guiDynamicObjectsWindowEditAction=new CGUIExtWindow(LANGManager::getInstance()->getText("script_window_title").c_str(),
		guienv, guienv->getRootGUIElement(),
		GUIManager::GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT,
		GUIManager::getInstance()->myRect(1,120,displaywidth-1,displayheight-140));

	guiDynamicObjectsWindowEditAction->setDevice(device);
	guiDynamicObjectsWindowEditAction->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
	//guiDynamicObjectsWindowEditAction->getCloseButton()->setVisible(false);
	guiDynamicObjectsWindowEditAction->setCloseHide(true); // Not now as it need to check for buttons states and other things
	guiDynamicObjectsWindowEditAction->setStretchable(true); // Use this window as a streachable windows (all directions)
	guiDynamicObjectsWindowEditAction->setMinSize(core::dimension2du(640,256));
	guiDynamicObjectsWindowEditAction->getCloseButton()->setVisible(false);

	IGUIButton* guiDynamicObjects_Script_Close = guienv->addButton(
		GUIManager::getInstance()->myRect(driver->getScreenSize().Width-40,0,30,20),
		guiDynamicObjectsWindowEditAction,
		GUIManager::BT_ID_DYNAMIC_OBJECT_SCRIPT_CLOSE,
		stringw(L"X").c_str() );

	guiDynamicObjects_Script_Close->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);



	//scripts editor box
    CGUIEditBoxIRB* guiDynamicObjects_Script = new CGUIEditBoxIRB(L"",
                       true,
					   true,
                       guienv,
                       guiDynamicObjectsWindowEditAction,
					   GUIManager::EB_ID_DYNAMIC_OBJECT_SCRIPT,
					   GUIManager::getInstance()->myRect(6,32,driver->getScreenSize().Width-15,driver->getScreenSize().Height-312),
					   //myRect(10,40,guiDynamicObjectsWindowEditAction->getClientRect().getWidth()-20,guiDynamicObjectsWindowEditAction->getClientRect().getHeight()-130),
					   App::getInstance()->getDevice());

    guiDynamicObjects_Script->setMultiLine(true);
    guiDynamicObjects_Script->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);
	guiDynamicObjects_Script->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
    //guienv->getSkin()->setColor( gui::EGDC_WINDOW, video::SColor(255, 255, 255, 255) );
    guiDynamicObjects_Script->setOverrideFont(GUIManager::getInstance()->guiFontCourier10);

	//Old code now changed to setElementText()
	//guiDynamicObjects_Script->setLineCountButtonText(LANGManager::getInstance()->getText("bt_script_editor_linecount").c_str());
	guiDynamicObjects_Script->setElementText(guiDynamicObjects_Script->BT_LINECOUNT,LANGManager::getInstance()->getText("bt_script_editor_linecount").c_str());
	guiDynamicObjects_Script->setElementText(guiDynamicObjects_Script->CM_COPY,LANGManager::getInstance()->getText("script_editor_copy").c_str());
	guiDynamicObjects_Script->setElementText(guiDynamicObjects_Script->CM_CUT,LANGManager::getInstance()->getText("script_editor_cut").c_str());
	guiDynamicObjects_Script->setElementText(guiDynamicObjects_Script->CM_DELETE,LANGManager::getInstance()->getText("script_editor_delete").c_str());
	guiDynamicObjects_Script->setElementText(guiDynamicObjects_Script->CM_PASTE,LANGManager::getInstance()->getText("script_editor_paste").c_str());
	guiDynamicObjects_Script->setElementText(guiDynamicObjects_Script->CM_REDO,LANGManager::getInstance()->getText("script_editor_redo").c_str());
	guiDynamicObjects_Script->setElementText(guiDynamicObjects_Script->CM_UNDO,LANGManager::getInstance()->getText("script_editor_undo").c_str());

	// Set the IRB commands Highlights


	// Allow the code editor to use syntax highlighting based on LUA keywords
	guiDynamicObjects_Script->addLUAKeywords();
	guiDynamicObjects_Script->addKeyword("then",SColor(255,0,0,200),true);

	// Define custom "Group" keywords, here are "dictionnary" for IRB specific keywords
	guiDynamicObjects_Script->addKeyword("setObjectName",SColor(255,128,0,0),true); //REd as internal, non recommended
	guiDynamicObjects_Script->addKeyword("chaseObject",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("walkRandomly",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("walkToObject",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("CustomDynamicObjectUpdate",SColor(255,128,0,0),true);
	guiDynamicObjects_Script->addKeyword("programAction",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("CustomDynamicObjectUpdateProgrammedAction",SColor(255,128,0,0),true);
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
	guiDynamicObjects_Script->addKeyword("getCameraRange",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setCameraRange",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getCameraRTSRotation",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setCameraRTSRotation",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getCameraZoom",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setCameraZoom",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getCameraAngleLimit",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setCameraAngleLimit",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getObjectPosition",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setObjectRotation",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("cutsceneMode",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("gameMode",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setRTSView",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setRTSFixedView",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setRPGView",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setFPSView",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("defineKeys",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setCameraAttachment",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setCameraOffset",SColor(255,128,0,255),true);


	guiDynamicObjects_Script->addKeyword("showCutsceneText",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setCutsceneText",SColor(255,128,0,255),true);

	guiDynamicObjects_Script->addKeyword("playSound2D",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("playSound3D",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setSoundListenerPosition",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setSoundVolume",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setPlayerLife",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getPlayerLife",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getObjectLife",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setObjectLife",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setPlayerMoney",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getPlayerMoney",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("addPlayerItem",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("stopSounds",SColor(255,128,0,255),true);

	guiDynamicObjects_Script->addKeyword("removePlayerItem",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("useGlobalFunction",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getObjectItemCount",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("moveObjectLoot",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("addLoot",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("addObjectLoot",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("spawn",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setEnemy",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getEnemyCount",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("attachObject",SColor(255,128,0,255),true);


	guiDynamicObjects_Script->addKeyword("showBlackScreen",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("hideBlackScreen",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("showDialogMessage",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("showDialogQuestion",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("printToConsole",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("saveGame",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("loadGame",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("showObjectLabel",SColor(255,128,0,255),true);

	guiDynamicObjects_Script->addKeyword("hideObjectLabel",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setObjectLabel",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setPosition",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getPosition",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setRotation",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getRotation",SColor(255,128,0,255),true);

	guiDynamicObjects_Script->addKeyword("getParentRotation",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getParentPosition",SColor(255,128,0,255),true);

	guiDynamicObjects_Script->addKeyword("turn",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("move",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("walkTo",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("hasReached",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("lookAt",SColor(255,128,0,255),true);

	guiDynamicObjects_Script->addKeyword("lookToObject",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getName",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setName",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("distanceFrom",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setEnabled",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setFrameLoop",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setAnimationSpeed",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setAnimation",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setEnemy",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setObject",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setProperty",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setObjectType",SColor(255,128,0,255),true);

	guiDynamicObjects_Script->addKeyword("getProperty",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setObjectProperty",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getObjectProperty",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("checkObjectItem",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("destroyObjectItem",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("setObjectVisible",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("isObjectVisible",SColor(255,128,0,255),true);

	guiDynamicObjects_Script->addKeyword("attack",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("onLoad",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("onUpdate",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("onUse",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("onWear",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("onKeypressed",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("isKeypressed",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("step",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("onClicked",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("onAnswer",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("getLanguage",SColor(255,128,0,255),true);
	guiDynamicObjects_Script->addKeyword("onCollision",SColor(255,128,0,255),true);

	guiDynamicObjects_Script->addKeyword("findInSphere",SColor(255,128,0,255),true);




	// Bottom tabcontrol
	IGUITabControl * tabctrl1 = guienv->addTabControl(
		GUIManager::getInstance()->myRect(6,driver->getScreenSize().Height-290,driver->getScreenSize().Width-16,144),
		guiDynamicObjectsWindowEditAction,true,false);

	//IGUITabControl * tabctrl1 = guienv->addTabControl(myRect(10,guiDynamicObjectsWindowEditAction->getClientRect().getHeight()-90,displaywidth-220,110),guiDynamicObjectsWindowEditAction,true,false);
	IGUITab * tab1 = tabctrl1->addTab(LANGManager::getInstance()->getText("tab_script_debug").c_str());
	IGUITab * tab2 = tabctrl1->addTab(LANGManager::getInstance()->getText("tab_script_templates").c_str());

	tabctrl1->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);
    s32 X_ScriptToolbar = 10;

    IGUIComboBox* guiDynamicObjects_LoadScriptTemplateCB = guienv->addComboBox(
		GUIManager::getInstance()->myRect(X_ScriptToolbar,10,400,20),
		tab2,GUIManager::CO_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE);

	guiDynamicObjects_LoadScriptTemplateCB->setAlignment(EGUIA_UPPERLEFT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);


    this->loadScriptTemplates();

    X_ScriptToolbar+=405;

    IGUIButton* guiDynamicObjects_LoadScriptTemplateBT = guienv->addButton(
		GUIManager::getInstance()->myRect(X_ScriptToolbar,10,200,20),
		tab2, GUIManager::BT_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE,
		stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_load_script_template")).c_str() );

	guiDynamicObjects_LoadScriptTemplateBT->setOverrideFont(GUIManager::getInstance()->guiFontC12);
	guiDynamicObjects_LoadScriptTemplateBT->setAlignment(EGUIA_UPPERLEFT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

    X_ScriptToolbar+=160;

    //IGUIButton* validate = guienv->addButton(myRect(driver->getScreenSize().Width-375,5,150,20),
	IGUIButton* validate = guienv->addButton(
		GUIManager::getInstance()->myRect(displaywidth-174,5,150,20),
		tab1, GUIManager::BT_ID_DYNAMIC_OBJECT_VALIDATE_SCRIPT,
		stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_validate_script")).c_str() );

	validate->setOverrideFont(GUIManager::getInstance()->guiFontC12);
	validate->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

	//IGUIButton* close = guiDynamicObjects_Script_Close = guienv->addButton(myRect(guiDynamicObjectsWindowEditAction->getClientRect().getWidth()-90,10,82,20),
	/*IGUIButton* close = guiDynamicObjects_Script_Close = guienv->addButton(myRect(driver->getScreenSize().Width-170,30,82,20),
                      guiDynamicObjectsWindowEditAction,
                      BT_ID_DYNAMIC_OBJECT_SCRIPT_CLOSE,
                      stringw(LANGManager::getInstance()->getText("bt_dynamic_objects_close_script")).c_str() );
	close->setOverrideFont(guiFontC12);
	close->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);*/


	// Console window
    IGUIEditBox* guiDynamicObjects_Script_Console = guienv->addEditBox(L"",
		GUIManager::getInstance()->myRect(2,5,displaywidth-180,104),
		true,tab1, GUIManager::EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE);

    guiDynamicObjects_Script_Console->setOverrideColor(SColor(255,255,0,0));
	guiDynamicObjects_Script_Console->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);
	guiDynamicObjects_Script_Console->setTextAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	guiDynamicObjects_Script_Console->setEnabled(false);


	guiDynamicObjects_LoadScriptTemplateCB->bringToFront(guiDynamicObjects_LoadScriptTemplateCB);
	guiDynamicObjectsWindowEditAction->setVisible(false);

}

//--------------------------------------------------------------------------------------------------------------------------------
// Functions for using the GUI

void GUIEditor::drawHelpImage(u32 img)
{
    switch(img)
    {
	case GUIManager::HELP_TERRAIN_TRANSFORM:
            driver->draw2DImage(helpTerrainTransform, position2di(0,screensize.Height-20 - helpTerrainTransform->getSize().Height),
				GUIManager::getInstance()->myRect(0,0,helpTerrainTransform->getSize().Width,helpTerrainTransform->getSize().Height), 0,
				video::SColor(255,255,255,255), true);
            break;
	case GUIManager::HELP_TERRAIN_SEGMENTS:
            driver->draw2DImage(helpTerrainSegments, position2di(0,screensize.Height-20 - helpTerrainSegments->getSize().Height),
				GUIManager::getInstance()->myRect(0,0,helpTerrainSegments->getSize().Width,helpTerrainSegments->getSize().Height), 0,
				video::SColor(255,255,255,255), true);
            break;
	case GUIManager::HELP_VEGETATION_PAINT:
            driver->draw2DImage(helpVegetationPaint, position2di(0,screensize.Height-20 - helpVegetationPaint->getSize().Height),
				GUIManager::getInstance()->myRect(0,0,helpVegetationPaint->getSize().Width,helpVegetationPaint->getSize().Height), 0,
				video::SColor(255,255,255,255), true);
            break;
	case GUIManager::HELP_IRR_RPG_BUILDER_1:
			driver->draw2DImage(logo1, position2di(screensize.Width - logo1->getSize().Width,screensize.Height - logo1->getSize().Height),
				GUIManager::getInstance()->myRect(0,0,logo1->getSize().Width,logo1->getSize().Height), 0,
				video::SColor(255,255,255,255), true);
            break;
    }
}

f32 GUIEditor::getScrollBarValue(u32 id)
{

	//Get the handle of the main tool windows (aligned with the gameplay tool)
	IGUIScrollBar* scrollbar = (IGUIScrollBar*)GUIManager::getInstance()->getGUIElement(id);
	IGUIScrollBar* scrollbar2 = NULL; //Used to change 2 scrollsbars at the same time (special case)
	IGUIStaticText* displaytext = NULL;

    switch(id)
    {
	case GUIManager::SC_ID_TERRAIN_BRUSH_STRENGTH :
			{
				stringw text = stringw(scrollbar->getPos()).c_str();
				text=text+L"\"";

				displaytext = (IGUIStaticText*)GUIManager::getInstance()->getGUIElement(GUIManager::TXT_ID_TERRAIN_BRUSH_STRENGTH);
				displaytext->setText(text.c_str());
				return (f32)scrollbar->getPos();
			}
            break;
	case GUIManager::SC_ID_TERRAIN_BRUSH_RADIUS :
			{
				stringw text = stringw(scrollbar->getPos()).c_str();
				text=text+L"\"";
				
				displaytext = (IGUIStaticText*)GUIManager::getInstance()->getGUIElement(GUIManager::TXT_ID_TERRAIN_RADIUS);
				displaytext->setText(text.c_str());

				scrollbar2 = (IGUIScrollBar*)GUIManager::getInstance()->getGUIElement(GUIManager::SC_ID_TERRAIN_BRUSH_RADIUS2);
				scrollbar2->setMax(scrollbar->getPos());

				if (scrollbar2->getPos()>scrollbar->getPos())
					scrollbar2->setPos(scrollbar->getPos());

				return (f32)scrollbar->getPos();
			}
			break;
	case GUIManager::SC_ID_TERRAIN_BRUSH_RADIUS2 :
			{
				stringw text = stringw(scrollbar->getPos()).c_str();
				text=text+L"\"";

				displaytext = (IGUIStaticText*)GUIManager::getInstance()->getGUIElement(GUIManager::TXT_ID_TERRAIN_RADIUS1);
				displaytext->setText(text.c_str());
				return (f32)scrollbar->getPos();
			}
			break;
	case GUIManager::SC_ID_TERRAIN_BRUSH_PLATEAU :
			{
				stringw text = stringw(scrollbar->getPos()).c_str();
				text=text+L"\"";

				displaytext = (IGUIStaticText*)GUIManager::getInstance()->getGUIElement(GUIManager::TXT_ID_TERRAIN_PLATEAU);
				displaytext->setText(text.c_str());
				return (f32)scrollbar->getPos();

			}
			break;
	case GUIManager::SC_ID_VEGETATION_BRUSH_STRENGTH : //Currently not used
            {
                //return (f32)guiVegetationBrushStrength->getPos();
				return 1.0f;
            }
            break;
        default:
            break;
    }
    return 0;
}

// Reshesh the GUI informations inside a window
void GUIEditor::UpdateGUIChooser(u32 type)
{

	IGUIComboBox* category = (IGUIComboBox*)GUIManager::getInstance()->getGUIElement(GUIManager::CO_ID_DYNAMIC_OBJECT_OBJ_CATEGORY);
	IGUIListBox* categorylist = (IGUIListBox*)GUIManager::getInstance()->getGUIElement(GUIManager::CO_ID_DYNAMIC_OBJECT_OBJLIST_CATEGORY);
	IGUIListBox* objectlist = (IGUIListBox*)GUIManager::getInstance()->getGUIElement(GUIManager::CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER);

	//LIST_TYPE type=LIST_OBJ;
	//DynamicObject::TYPE objtype
	if (type==GUIManager::LIST_NPC) // Dynamic object panel
	{
		core::stringw selected = category->getItem(category->getSelected());

		// Create the category list first
		categorylist->clear();

		std::vector<stringw> listDynamicObjsCat = DynamicObjectsManager::getInstance()->getObjectsListCategories(GUIManager::LIST_NPC, selected );
		std::sort(listDynamicObjsCat.begin(), listDynamicObjsCat.end(), compareString);

		for (int i=0 ; i<(int)listDynamicObjsCat.size() ; i++)
		{
			categorylist->addItem(listDynamicObjsCat[i].c_str());
		}
		categorylist->setSelected(0);

		// Then the list of objects
		objectlist->clear();
		std::vector<stringw> listDynamicObjs = DynamicObjectsManager::getInstance()->getObjectsList(GUIManager::LIST_NPC,selected,"");
		std::sort(listDynamicObjs.begin(), listDynamicObjs.end(), compareString);
		
		for (int i=0 ; i<(int)listDynamicObjs.size() ; i++)
		{
			objectlist->addItem(listDynamicObjs[i].c_str());
		}
		objectlist->setSelected(0);
		
		DynamicObjectsManager::getInstance()->setActiveObject(
			GUIManager::getInstance()->getComboBoxItem(GUIManager::CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER));

		return;
		getInfoAboutModel(GUIManager::LIST_NPC);
		return;
	}
	if (type==GUIManager::LIST_SEGMENT) // Get a list for the special CUSTOM SEGMENT meshes
	{
		//We unified the segment list with the object list.
		/*core::stringw selected = guiCustom_Segment_Category->getItem(guiCustom_Segment_Category->getSelected());

		// Create the category list first
		guiCustom_Segment_OBJCategory->clear();

		std::vector<stringw> listDynamicObjsCat = DynamicObjectsManager::getInstance()->getObjectsListCategories( selected, DynamicObject::SPECIAL_SEGMENT );
		for (int i=0 ; i<(int)listDynamicObjsCat.size() ; i++)
		{
			guiCustom_Segment_OBJCategory->addItem(listDynamicObjsCat[i].c_str());
		}
		guiCustom_Segment_OBJCategory->setSelected(0);

		// Then the list of objects
		guiCustom_Segment_OBJChooser->clear();
		std::vector<stringw> listDynamicObjs = DynamicObjectsManager::getInstance()->getObjectsList(selected,"", DynamicObject::SPECIAL_SEGMENT);

		for (int i=0 ; i<(int)listDynamicObjs.size() ; i++)
		{
			guiCustom_Segment_OBJChooser->addItem(listDynamicObjs[i].c_str());
		}
		guiCustom_Segment_OBJChooser->setSelected(0);*/

		core::stringw selected = category->getItem(category->getSelected());

		// Create the category list first
		categorylist->clear();

		std::vector<stringw> listDynamicObjsCat = DynamicObjectsManager::getInstance()->getObjectsListCategories(GUIManager::LIST_SEGMENT, selected );
		std::sort(listDynamicObjsCat.begin(), listDynamicObjsCat.end(), compareString);
		
		for (int i=0 ; i<(int)listDynamicObjsCat.size() ; i++)
		{
			categorylist->addItem(listDynamicObjsCat[i].c_str());
		}
		categorylist->setSelected(0);

		// Then the list of objects
		objectlist->clear();
		std::vector<stringw> listDynamicObjs = DynamicObjectsManager::getInstance()->getObjectsList(GUIManager::LIST_SEGMENT, selected,"", DynamicObject::SPECIAL_SEGMENT);
		std::sort(listDynamicObjs.begin(), listDynamicObjs.end(), compareString);

		for (int i=0 ; i<(int)listDynamicObjs.size() ; i++)
		{
			objectlist->addItem(listDynamicObjs[i].c_str());
		}
		objectlist->setSelected(0);
		DynamicObjectsManager::getInstance()->setActiveObject(
			GUIManager::getInstance()->getComboBoxItem(GUIManager::CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER));

		getInfoAboutModel(GUIManager::LIST_SEGMENT);
		return;
	}
	if (type==GUIManager::LIST_PROP) // Get a list for the special CUSTOM SEGMENT meshes
	{
		core::stringw selected = category->getItem(category->getSelected());

		// Create the category list first
		categorylist->clear();

		std::vector<stringw> listDynamicObjsCat = DynamicObjectsManager::getInstance()->getObjectsListCategories(GUIManager::LIST_PROP, selected );
		std::sort(listDynamicObjsCat.begin(), listDynamicObjsCat.end(), compareString);
		for (int i=0 ; i<(int)listDynamicObjsCat.size() ; i++)
		{
			categorylist->addItem(listDynamicObjsCat[i].c_str());
		}
		categorylist->setSelected(0);

		// Then the list of objects
		objectlist->clear();
		std::vector<stringw> listDynamicObjs = DynamicObjectsManager::getInstance()->getObjectsList(GUIManager::LIST_PROP, selected,"");
		std::sort(listDynamicObjs.begin(), listDynamicObjs.end(), compareString);

		for (int i=0 ; i<(int)listDynamicObjs.size() ; i++)
		{
			objectlist->addItem(listDynamicObjs[i].c_str());
		}
		objectlist->setSelected(0);
		DynamicObjectsManager::getInstance()->setActiveObject(
			GUIManager::getInstance()->getComboBoxItem(GUIManager::CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER));
		getInfoAboutModel(GUIManager::LIST_PROP);
		return;

	}
	if (type==GUIManager::LIST_LOOT) // Get a list for the special CUSTOM SEGMENT meshes
	{
		core::stringw selected = category->getItem(category->getSelected());

		// Create the category list first
		categorylist->clear();

		std::vector<stringw> listDynamicObjsCat = DynamicObjectsManager::getInstance()->getObjectsListCategories(GUIManager::LIST_LOOT, selected );
		std::sort(listDynamicObjsCat.begin(), listDynamicObjsCat.end(), compareString);

		for (int i=0 ; i<(int)listDynamicObjsCat.size() ; i++)
		{
			categorylist->addItem(listDynamicObjsCat[i].c_str());
		}
		categorylist->setSelected(0);

		// Then the list of objects
		objectlist->clear();
		std::vector<stringw> listDynamicObjs = DynamicObjectsManager::getInstance()->getObjectsList(GUIManager::LIST_LOOT, selected,"",DynamicObject::SPECIAL_LOOT);
		std::sort(listDynamicObjs.begin(), listDynamicObjs.end(), compareString);

		for (int i=0 ; i<(int)listDynamicObjs.size() ; i++)
		{
			objectlist->addItem(listDynamicObjs[i].c_str());
		}
		objectlist->setSelected(0);
		DynamicObjectsManager::getInstance()->setActiveObject(
			GUIManager::getInstance()->getComboBoxItem(GUIManager::CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER));
		getInfoAboutModel(GUIManager::LIST_LOOT);
		return;

	}
}

// Refresh gui information inside a panel type
void GUIEditor::updateCurrentCategory(u32 type)
{
	IGUIComboBox* guiDynamicObjects_Category = (IGUIComboBox*)GUIManager::getInstance()->getGUIElement(GUIManager::CO_ID_DYNAMIC_OBJECT_OBJ_CATEGORY);
	IGUIListBox* guiDynamicObjects_OBJCategory = (IGUIListBox*)GUIManager::getInstance()->getGUIElement(GUIManager::CO_ID_DYNAMIC_OBJECT_OBJLIST_CATEGORY);
	IGUIListBox* guiDynamicObjects_OBJChooser = (IGUIListBox*)GUIManager::getInstance()->getGUIElement(GUIManager::CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER);

	core::stringw text="";
	if (type == GUIManager::LIST_NPC) // Dynamic objects
	{
		u32 selected = guiDynamicObjects_OBJCategory->getSelected();
		core::stringw selectedcat = guiDynamicObjects_Category->getItem(guiDynamicObjects_Category->getSelected());
		text=guiDynamicObjects_OBJCategory->getListItem(selected);

		// check if "all" is selected, as it's the first choice
		// and empty string mean, that will check for all
		if (selected==0)
			text="";

		guiDynamicObjects_OBJChooser->clear();
		std::vector<stringw> listDynamicObjs = DynamicObjectsManager::getInstance()->getObjectsList(GUIManager::LIST_NPC, selectedcat,text);
		std::sort(listDynamicObjs.begin(), listDynamicObjs.end(), compareString);


		for (int i=0 ; i<(int)listDynamicObjs.size() ; i++)
		{
			guiDynamicObjects_OBJChooser->addItem(listDynamicObjs[i].c_str());
		}
		guiDynamicObjects_OBJChooser->setSelected(0);
		// Set the "active" object to the selection
#ifdef EDITOR
		DynamicObjectsManager::getInstance()->setActiveObject(GUIManager::getInstance()->getComboBoxItem(GUIManager::CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER));
#endif
		return;
	}
	if (type == GUIManager::LIST_PROP) // Dynamic objects
	{
		u32 selected = guiDynamicObjects_OBJCategory->getSelected();
		core::stringw selectedcat = guiDynamicObjects_Category->getItem(guiDynamicObjects_Category->getSelected());
		text=guiDynamicObjects_OBJCategory->getListItem(selected);

		// check if "all" is selected, as it's the first choice
		// and empty string mean, that will check for all
		if (selected==0)
			text="";

		guiDynamicObjects_OBJChooser->clear();
		std::vector<stringw> listDynamicObjs = DynamicObjectsManager::getInstance()->getObjectsList(GUIManager::LIST_PROP, selectedcat,text);
		std::sort(listDynamicObjs.begin(), listDynamicObjs.end(), compareString);

		for (int i=0 ; i<(int)listDynamicObjs.size() ; i++)
		{
			guiDynamicObjects_OBJChooser->addItem(listDynamicObjs[i].c_str());
		}
		guiDynamicObjects_OBJChooser->setSelected(0);
		// Set the "active" object to the selection
#ifdef EDITOR
		DynamicObjectsManager::getInstance()->setActiveObject(GUIManager::getInstance()->getComboBoxItem(GUIManager::CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER));
#endif
		return;
	}
	if (type == GUIManager::LIST_LOOT) // Dynamic objects
	{
		u32 selected = guiDynamicObjects_OBJCategory->getSelected();
		core::stringw selectedcat = guiDynamicObjects_Category->getItem(guiDynamicObjects_Category->getSelected());
		text=guiDynamicObjects_OBJCategory->getListItem(selected);

		// check if "all" is selected, as it's the first choice
		// and empty string mean, that will check for all
		if (selected==0)
			text="";

		guiDynamicObjects_OBJChooser->clear();
		std::vector<stringw> listDynamicObjs = DynamicObjectsManager::getInstance()->getObjectsList(GUIManager::LIST_LOOT, selectedcat,text);
		std::sort(listDynamicObjs.begin(), listDynamicObjs.end(), compareString);

		for (int i=0 ; i<(int)listDynamicObjs.size() ; i++)
		{
			guiDynamicObjects_OBJChooser->addItem(listDynamicObjs[i].c_str());
		}
		guiDynamicObjects_OBJChooser->setSelected(0);
		// Set the "active" object to the selection
#ifdef EDITOR
		DynamicObjectsManager::getInstance()->setActiveObject(GUIManager::getInstance()->getComboBoxItem(GUIManager::CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER));
#endif
		return;
	}
	if (type==GUIManager::LIST_SEGMENT) // Get a list for the special CUSTOM SEGMENT meshes
	{
		u32 selected = guiDynamicObjects_OBJCategory->getSelected();
		core::stringw selectedcat = guiDynamicObjects_Category->getItem(guiDynamicObjects_Category->getSelected());
		text=guiDynamicObjects_OBJCategory->getListItem(selected);

		// check if "all" is selected, as it's the first choice
		// and empty string mean, that will check for all
		if (selected==0)
			text="";

		guiDynamicObjects_OBJChooser->clear();
		std::vector<stringw> listDynamicObjs = DynamicObjectsManager::getInstance()->getObjectsList(GUIManager::LIST_SEGMENT, selectedcat,text,DynamicObject::SPECIAL_SEGMENT);
		std::sort(listDynamicObjs.begin(), listDynamicObjs.end(), compareString);

		for (int i=0 ; i<(int)listDynamicObjs.size() ; i++)
		{
			guiDynamicObjects_OBJChooser->addItem(listDynamicObjs[i].c_str());
		}
		guiDynamicObjects_OBJChooser->setSelected(0);
		// Set the "active" object to the selection
#ifdef EDITOR
		DynamicObjectsManager::getInstance()->setActiveObject(GUIManager::getInstance()->getComboBoxItem(GUIManager::CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER));
#endif
		/*u32 selected = guiCustom_Segment_OBJCategory->getSelected();
		core::stringw selectedcat = guiCustom_Segment_Category->getItem(guiCustom_Segment_Category->getSelected());
		text=guiCustom_Segment_OBJCategory->getListItem(selected);

		// check if "all" is selected, as it's the first choice
		// and empty string mean, that will check for all
		if (selected==0)
			text="";

		guiCustom_Segment_OBJChooser->clear();
		std::vector<stringw> listDynamicObjs = DynamicObjectsManager::getInstance()->getObjectsList(selectedcat,text, DynamicObject::SPECIAL_SEGMENT);

		for (int i=0 ; i<(int)listDynamicObjs.size() ; i++)
		{
			guiCustom_Segment_OBJChooser->addItem(listDynamicObjs[i].c_str());
		}
		guiCustom_Segment_OBJChooser->setSelected(0);
		// Set the "active" object to the selection
#ifdef EDITOR
		DynamicObjectsManager::getInstance()->setActiveObject(getComboBoxItem(CO_ID_CUSTOM_SEGMENT_OBJ_CHOOSER));
#endif
		*/
		return;

	}
}

void GUIEditor::getInfoAboutModel(u32 type)
{

	IGUIStaticText* mdl_name = (IGUIStaticText*)GUIManager::getInstance()->getGUIElement(GUIManager::TXT_ID_MDL_NAME);
	IGUIStaticText* mdl_desc = (IGUIStaticText*)GUIManager::getInstance()->getGUIElement(GUIManager::TXT_ID_MDL_DESC);
	IGUIStaticText* mdl_auth = (IGUIStaticText*)GUIManager::getInstance()->getGUIElement(GUIManager::TXT_ID_MDL_AUTH);
	IGUIStaticText* mdl_lic = (IGUIStaticText*)GUIManager::getInstance()->getGUIElement(GUIManager::TXT_ID_MDL_LIC);
	// Text will return the current item basec on the Dynamic Objects manager "active" object.
	if (mdl_name)
		mdl_name->setText(DynamicObjectsManager::getInstance()->activeObject->getName().c_str());
	
	if (mdl_desc)
		mdl_desc->setText(DynamicObjectsManager::getInstance()->activeObject->description.c_str());
	
	if (mdl_auth)
		mdl_auth->setText(DynamicObjectsManager::getInstance()->activeObject->author.c_str());
	
	if (mdl_lic)
		mdl_lic->setText(DynamicObjectsManager::getInstance()->activeObject->licence.c_str());


	core::stringc filename = "../media/dynamic_objects/";
	filename+=DynamicObjectsManager::getInstance()->activeObject->thumbnail;

	info_current=driver->getTexture(filename.c_str());
	if (!info_current)
		info_current = driver->getTexture("../media/editor/info_none.jpg");

	IGUIImage* thumbnail=(IGUIImage*)GUIManager::getInstance()->getGUIElement(GUIManager::IMG_THUMBNAIL);
	if (thumbnail)
		thumbnail->setImage(info_current);
}

void GUIEditor::UpdateCollections(u32 type)
{
	IGUIComboBox* guiDynamicObjects_Category = (IGUIComboBox*)GUIManager::getInstance()->getGUIElement(GUIManager::CO_ID_DYNAMIC_OBJECT_OBJ_CATEGORY);
	guiDynamicObjects_Category->clear();

	GUIManager::LIST_TYPE ftype = (GUIManager::LIST_TYPE)type;
	std::vector<stringw> listCollection = DynamicObjectsManager::getInstance()->getObjectsCollections(ftype);
	std::sort(listCollection.begin(), listCollection.end(), compareString);

	// Populate a list of collection that contain only dynamic objects. (Default to all)
	for (int i=0 ; i< (int)listCollection.size() ; i++)
	{
		core::stringw result = listCollection[i].c_str();
		if (result!=L"") //Collection with no name filtering
			guiDynamicObjects_Category->addItem(result.c_str());
	}
}

void GUIEditor::buildSceneObjectList(DynamicObject::TYPE objtype)
{
	IGUIListBox* listbox=(IGUIListBox*)guienv->getRootGUIElement()->getElementFromId(GUIManager::CO_ID_ACTIVE_SCENE_LIST,true);
	if (listbox)
	{
		if (listbox->getItemCount()>0)
		{
			listbox->clear();
		}
		std::vector<stringw> listDynamicObjsCat = DynamicObjectsManager::getInstance()->getObjectsSceneListAlias(objtype);
		std::sort(listDynamicObjsCat.begin(), listDynamicObjsCat.end(), compareString);

		for (int i=0 ; i<(int)listDynamicObjsCat.size() ; i++)
		{
			listbox->addItem(listDynamicObjsCat[i].c_str());
		}
		listbox->setSelected(0);
	}
	
}

void GUIEditor::buildProjectList()
{
	core::stringw test2 = L"";
	core::stringw projpath = (core::stringw)device->getFileSystem()->getAbsolutePath(App::getInstance()->projectpath.c_str());
	device->getFileSystem()->changeWorkingDirectoryTo(core::stringc(projpath).c_str());
	io::IFileList * list = App::getInstance()->getDevice()->getFileSystem()->createFileList();
	list->sort();
	for (u32 i = 0; i < list->getFileCount(); ++i)
	{
		
		test2 = list->getFileName(i);
		
		// We just want a list of directories and those matching the file filter
		if (list->isDirectory(i))
		{
			if (test2 != L"." && test2 != L"..")
				projectlist->addItem(test2.c_str());
		}
	}
	App::getInstance()->getDevice()->getFileSystem()->changeWorkingDirectoryTo(App::getInstance()->getAppPath().c_str());
	if (projectlist->getItemCount()>0)
		projectlist->setSelected(projectlist->getListItem(0));
}

// Load a script template list for the script editor GUI
void GUIEditor::loadScriptTemplates()
{
	//Get back the GUI that was defined previously.
	IGUIComboBox* guiDynamicObjects_LoadScriptTemplateCB=(IGUIComboBox*)GUIManager::getInstance()->getGUIElement(GUIManager::CO_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE);

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

core::stringw GUIEditor::getEditCameraString(scene::ISceneNode *node)
{
	// A bug that crash if I a "node" is "there" but pointer invalid. Will have to investigate more on this.
	core::stringw sct =L"";

	// Display this when working with segments or when there no node selected.
	if (!node || App::getInstance()->getAppState()==App::APP_EDIT_TERRAIN_EMPTY_SEGMENTS)
	{
		sct += LANGManager::getInstance()->getText("status_campos");
		core::vector3df pos = CameraSystem::getInstance()->getNode()->getPosition();
		sct+=(core::stringw)pos.X;
		sct+=L",";
		sct+=(core::stringw)pos.Y;
		sct+=L",";
		sct+=(core::stringw)pos.Z;
		sct+=LANGManager::getInstance()->getText("status_target");
		pos = CameraSystem::getInstance()->getNode()->getTarget();
		sct+=(core::stringw)pos.X;
		sct+=L",";
		sct+=(core::stringw)pos.Y;
		sct+=L",";
		sct+=(core::stringw)pos.Z;
		return sct;
	}


	// When moving or rotating a dynamic object
	if (App::getInstance()->getAppState()==App::APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE)
		node=App::getInstance()->lastMousePick.pickedNode;

	// Display this when in object edit mode only.
	if ((App::getInstance()->getAppState()==App::APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE || App::getInstance()->getAppState()==App::APP_EDIT_DYNAMIC_OBJECTS_MODE) && node && node->getID()!=100)
		{
			core::vector3df pos = node->getPosition();
			core::vector3df rot = node->getRotation();

			sct += LANGManager::getInstance()->getText("status_objpos");
			sct+=(core::stringw)pos.X;
			sct+=L",";
			sct+=(core::stringw)pos.Y;
			sct+=L",";
			sct+=(core::stringw)pos.Z;

			sct+=LANGManager::getInstance()->getText("status_objrot");
			sct+=(core::stringw)rot.X;
			sct+=L",";
			sct+=(core::stringw)rot.Y;
			sct+=L",";
			sct+=(core::stringw)rot.Z;
			return sct;
		}
	return sct;
}

void GUIEditor::updateEditCameraString(scene::ISceneNode * node)
{
	IGUIStaticText* guiStatusCameraText=(IGUIStaticText*)GUIManager::getInstance()->getGUIElement(GUIManager::TXT_ID_STATUS_CAMERA); 
	if (guiStatusCameraText){guiStatusCameraText->setText(getEditCameraString(node).c_str());}
}