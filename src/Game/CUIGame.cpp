#include "GUIGame.h"
#include "../App.h"
#include "../gui/CGUIGfxStatus.h"
#include "../objects/DynamicObjectsManager.h"
#include <algorithm>    // std::sort

//Init
GUIGame::GUIGame()
{
	guienv=GUIManager::getInstance()->getGuiEnv();
	driver=GUIManager::getInstance()->getDriver();
	screensize=App::getInstance()->getScreenSize();

	timer = App::getInstance()->getDevice()->getTimer()->getRealTime();
	timer2 = timer;
	dialogSound=NULL;
}

//Destructor
GUIGame::~GUIGame()
{
	if (nodepreview)
		delete nodepreview;

	if (configWindow)
		delete configWindow;

}

GUIGame* GUIGame::getInstance()
{
    static GUIGame *instance = 0;
    if (!instance) instance = new GUIGame();
    return instance;
}

// Set up the gameplay interface GUI
void GUIGame::setupGameplayGUI()
{

	driver=App::getInstance()->getDevice()->getVideoDriver();
	guienv=App::getInstance()->getDevice()->getGUIEnvironment();

	GUIManager::getInstance()->createConsole();

	IGUIInOutFader* fader=guienv->addInOutFader();
	fader->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
    fader->setVisible(false);
	fader->setID(GUIManager::ID_FADER);

	// NEW Create display size since IRRlicht return wrong values
	// Check the current screen size
	displayheight=screensize.Height;
	displaywidth=screensize.Width;

	// Create a cutscene text
	IGUIStaticText* guiCutsceneText = guienv->addStaticText(L"This is a standard cutscene text",core::rect<s32>(100,displayheight/2+(displayheight/4),displaywidth-10,displayheight-100),false,true,0,-1,false);
	guiCutsceneText->setOverrideFont(GUIManager::getInstance()->guiFontLarge28);
	guiCutsceneText->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);
	guiCutsceneText->setVisible(false);
	guiCutsceneText->setID(GUIManager::ST_ID_CUTSCENE_TEXT);

	// This is called only in the PLAYER application
	#ifndef EDITOR
	// ----------------------------------------
	guienv->getSkin()->setFont(GUIManager::getInstance()->guiFontC12);
	//guienv->getSkin()->setFont(guiFontCourier12);
	// Load textures
	ITexture* imgLogo = driver->getTexture("../media/art/title.jpg");

	//LOADER WINDOW
	IGUIWindow* guiLoaderWindow = guienv->addWindow(GUIManager::getInstance()->myRect(driver->getScreenSize().Width/2-300, driver->getScreenSize().Height/2-200,600,400),false,L"Loading...",0,GUIManager::WIN_LOADER);
	guiLoaderWindow->setDrawTitlebar(false);
	guiLoaderWindow->getCloseButton()->setVisible(false);

	guienv->addImage(imgLogo,vector2d<s32>(5,5),true,guiLoaderWindow);
    IGUIStaticText* guiLoaderDescription = guienv->addStaticText(L"Loading fonts...",
		GUIManager::getInstance()->myRect(10,350,580,40),
		true,true,guiLoaderWindow,
		GUIManager::TXT_ID_LOADER,false);

	//Define 2 buttons to place in the loader windows (player only)
	IGUIButton* guiBtGamePlay = guienv->addButton(core::rect<s32>(400,360,580,380),guiLoaderWindow, GUIManager::BT_PLAYER_START, L"PLAY GAME NOW!");
	guiBtGamePlay->setVisible(false);

	IGUIButton* guiBtGameConfig = guienv->addButton(core::rect<s32>(20,360,200,380),guiLoaderWindow, GUIManager::BT_PLAYER_CONFIG, L"EDIT CONFIGURATION");
	guiBtGameConfig->setVisible(false);

	App::getInstance()->quickUpdate();

	GUIManager::getInstance()->loadFonts();
	guiLoaderDescription->setText(L"Loading interface graphics...");
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

	IGUIWindow* guiMainToolWindow = guienv->addWindow(GUIManager::getInstance()->myRect(driver->getScreenSize().Width-170,0,170,46),false,0,0,GUIManager::WIN_GAMEPLAY);
	guiMainToolWindow->setDraggable(false);
	guiMainToolWindow->setDrawTitlebar(false);
	guiMainToolWindow->getCloseButton()->setVisible(false);
	guiMainToolWindow->setVisible(false);


	//Play Game
	int x = 0;
	mainToolbarPos.Y=5;
	IGUIButton* guiPlayGame= guienv->addButton(GUIManager::getInstance()->myRect(10+x,mainToolbarPos.Y,32,32),
                                     guiMainToolWindow,
									 GUIManager::BT_ID_PLAY_GAME,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_play_game")).c_str());

    guiPlayGame->setImage(driver->getTexture("../media/art/bt_play_game.png"));


    //Stop Game
    IGUIButton* guiStopGame= guienv->addButton(GUIManager::getInstance()->myRect(10+x,mainToolbarPos.Y,32,32),
                                     guiMainToolWindow,
									 GUIManager::BT_ID_STOP_GAME,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_stop_game")).c_str());

    guiStopGame->setImage(driver->getTexture("../media/art/bt_stop_game.png"));
    guiStopGame->setVisible(false);



    //ABOUT BUTTON
	x += 42;
    IGUIButton* guiAbout = guienv->addButton(GUIManager::getInstance()->myRect(10+x,mainToolbarPos.Y,32,32),
                                     guiMainToolWindow,
									 GUIManager::BT_ID_ABOUT,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_about")).c_str() );

    guiAbout->setImage(imgAbout);
	guiAbout->setPressedImage(imgAbout1);

	// Help Button
	x += 42;
    IGUIButton* guiHelpButton = guienv->addButton(GUIManager::getInstance()->myRect(10+x,mainToolbarPos.Y,32,32),
                                     guiMainToolWindow,
									 GUIManager::BT_ID_HELP,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_help")).c_str() );

    guiHelpButton->setImage(imgHelp);
    guiHelpButton->setPressedImage(imgHelp1);

	// Close program
	x += 42;
	IGUIButton* guiCloseProgram = guienv->addButton(GUIManager::getInstance()->myRect(10+x,mainToolbarPos.Y,32,32),
                                     guiMainToolWindow,
									 GUIManager::BT_ID_CLOSE_PROGRAM,L"",
                                     stringw(LANGManager::getInstance()->getText("bt_close_program")).c_str() );

    guiCloseProgram->setImage(imgCloseProgram);

	//ABOUT WINDOW
	IGUIWindow* guiAboutWindow = guienv->addWindow(GUIManager::getInstance()->myRect(driver->getScreenSize().Width/2 - 300,driver->getScreenSize().Height/2 - 200,600,400),false);
    guiAboutWindow->setDraggable(false);
    guiAboutWindow->setDrawTitlebar(false);
    guiAboutWindow->getCloseButton()->setVisible(false);
    guiAboutWindow->setVisible(false);

    guienv->addImage(driver->getTexture("../media/art/logo1.png"),position2di(guiAboutWindow->getAbsoluteClippingRect().getWidth()/2-100,10),true,guiAboutWindow);

	IGUIButton* guiAboutClose = guienv->addButton(GUIManager::getInstance()->myRect(guiAboutWindow->getAbsoluteClippingRect().getWidth() - 37,guiAboutWindow->getAbsoluteClippingRect().getHeight() - 37,32,32),guiAboutWindow,GUIManager::BT_ID_ABOUT_WINDOW_CLOSE);

    guiAboutClose->setImage(driver->getTexture("../media/art/bt_yes_32.png"));

	IGUIListBox* guiAboutText = guienv ->addListBox(GUIManager::getInstance()->myRect(guiAboutWindow->getAbsoluteClippingRect().getWidth()/2-250,160,500,200),guiAboutWindow);

	// Ask the LANGManager to fill the box with the proper Language of the about text.
	LANGManager::getInstance()->setAboutText(guiAboutText);

	// Create the Configuration window (Need to be updated)
	configWindow = new GUIConfigWindow(App::getInstance()->getDevice());

	// ---------------------------------------
	#endif

	// --- Active game menu during play

	ITexture* gameplay_bar = driver->getTexture("../media/art/gameplay_bar.png");
	ITexture* circle = driver->getTexture("../media/art/circle.png");
	ITexture* circleMana = driver->getTexture("../media/art/circlemana.png");
	ITexture* topCircle = driver->getTexture("../media/art/circle_top.png");
	IGUIImage* gameplay_bar_image = NULL;
	
	if (gameplay_bar)
	{
		gameplay_bar_image = guienv->addImage(gameplay_bar,vector2d<s32>((displaywidth/2)-(gameplay_bar->getSize().Width/2),displayheight-gameplay_bar->getSize().Height),true);
		gameplay_bar_image->setAlignment(EGUIA_CENTER,EGUIA_CENTER,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);
		gameplay_bar_image->setID(GUIManager::IMG_BAR);

		// The life gauge
		CGUIGfxStatus* lifegauge = new gui::CGUIGfxStatus(guienv, gameplay_bar_image,GUIManager::getInstance()->myRect((gameplay_bar->getSize().Width/2)-60,gameplay_bar->getSize().Height-128,128,128),-1);
		lifegauge->setImage(circle);
		lifegauge->ViewHalfLeft();
		lifegauge->setID(GUIManager::IMG_LIFEGAUGE);

		// The mana gauge
		CGUIGfxStatus* managauge = new gui::CGUIGfxStatus(guienv, gameplay_bar_image,GUIManager::getInstance()->myRect((gameplay_bar->getSize().Width/2)-60,gameplay_bar->getSize().Height-128,128,128),-1);
		managauge->setImage(circleMana);
		managauge->ViewHalfRight();
		managauge->setID(GUIManager::IMG_MANAGAUGE);

		// The image over the circle
		IGUIImage* circle_overlay =	guienv->addImage(topCircle,vector2d<s32>((gameplay_bar->getSize().Width/2)-64,gameplay_bar->getSize().Height-128),true,gameplay_bar_image);
	}	

	gameplay_bar_image->setVisible(false);

    ///DIALOG
	guiDialogImgYes = driver->getTexture("../media/art/img_yes.png");
    guiDialogImgYes_s = driver->getTexture("../media/art/img_yes_s.png");
    guiDialogImgNo = driver->getTexture("../media/art/img_no.png");
    guiDialogImgNo_s = driver->getTexture("../media/art/img_no_s.png");


    //view items
	if (gameplay_bar_image)
	{
		core::stringw text=LANGManager::getInstance()->getText("bt_view_items");
		IGUIButton* guiBtViewItems = guienv->addButton(GUIManager::getInstance()->myRect(465,85,48,48),
		//displaywidth/2 + 80,displayheight - 57,48,48),
                                     gameplay_bar_image,
									 GUIManager::BT_ID_VIEW_ITEMS,L"",
									 text.c_str());

		guiBtViewItems->setImage(driver->getTexture("../media/art/bt_view_items.png"));
		guiBtViewItems->setVisible(true);
		
	}

	

    //Items window

	IGUIWindow* guiWindowItems = guienv->addWindow(GUIManager::getInstance()->myRect(100,100,displaywidth-200,displayheight-150),false,L"",0,GUIManager::GCW_GAMEPLAY_ITEMS);
    guiWindowItems->getCloseButton()->setVisible(false);
    guiWindowItems->setDrawTitlebar(false);
    guiWindowItems->setDraggable(false);
	guiWindowItems->setAlignment(EGUIA_CENTER,EGUIA_CENTER,EGUIA_CENTER,EGUIA_CENTER);
    IGUITabControl * gameTabCtrl = guienv->addTabControl(core::rect<s32>(10,30,displaywidth-240,displayheight-200),guiWindowItems,false,true,-1);
	IGUITab * tab1 = gameTabCtrl->addTab(LANGManager::getInstance()->getText("game_stats_title").c_str());
	IGUITab * tab2 = gameTabCtrl->addTab(LANGManager::getInstance()->getText("game_inventory_title").c_str());
	IGUITab * tab3 = gameTabCtrl->addTab(LANGManager::getInstance()->getText("game_skills_title").c_str());
	IGUITab * tab4 = gameTabCtrl->addTab(LANGManager::getInstance()->getText("game_quests_title").c_str());


	nodepreview = new NodePreview(guienv,tab1,rect<s32>(440,40,740,370),-1);
	nodepreview->drawBackground(false);

	//DynamicObjectsManager::getInstance()->setActiveObject("player_template");

	//guiPlayerNodePreview->setNode(DynamicObjectsManager::getInstance()->getActiveObject()->getNode());
	//guiPlayerNodePreview->setNode(Player::getInstance()->getNodeRef());
	//DynamicObjectsManager::getInstance()->setActiveObject("Archer");
	//printf("This is the node name: %s\n",DynamicObjectsManager::getInstance()->getActiveObject()->getName());
	nodepreview->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);

	IGUIListBox* guiPlayerItems = guienv->addListBox(GUIManager::getInstance()->myRect(10,30,200,displayheight-340),tab2,GUIManager::LB_ID_PLAYER_ITEMS,true);

	ITexture* info_none = driver->getTexture("../media/editor/info_none.jpg");

	IGUIImage* guiPlayerLootImage = NULL;

	if (info_none)
		guiPlayerLootImage = guienv->addImage(info_none,vector2d<s32>(220,30),true,tab2,GUIManager::IMG_LOOT);

	guienv->addStaticText(L"",core::rect<s32>(220,250,520,410),true,true,tab2,GUIManager::TXT_ID_LOOT_DESCRIPTION,true);

	//guienv->addImage(info_none,vector2d<s32>(5,5),true,tab2);
	core::stringc filename = "../media/dynamic_objects/";

	IGUIButton* guiBtUseItem = guienv->addButton(GUIManager::getInstance()->myRect(10,displayheight-300,32,32),
                                         tab2,
										 GUIManager::BT_ID_USE_ITEM,
                                         L"",
                                         stringw(LANGManager::getInstance()->getText("bt_use_item")).c_str());
    guiBtUseItem->setImage(driver->getTexture("../media/art/bt_yes_32.png"));

    IGUIButton* guiBtDropItem = guienv->addButton(GUIManager::getInstance()->myRect(52,displayheight-300,32,32),
                                         tab2,
										 GUIManager::BT_ID_DROP_ITEM,
                                         L"",
                                         stringw(LANGManager::getInstance()->getText("bt_drop_item")).c_str());
    guiBtDropItem->setImage(driver->getTexture("../media/art/bt_no_32.png"));


    IGUIButton* guiBtCloseItemsWindow = guienv->addButton(GUIManager::getInstance()->myRect(displaywidth-210-32,displayheight-160 - 32,32,32),
                                         guiWindowItems,
										 GUIManager::BT_ID_CLOSE_ITEMS_WINDOW,
                                         L"",
                                         stringw(LANGManager::getInstance()->getText("bt_close_items_window")).c_str());
    guiBtCloseItemsWindow->setImage(driver->getTexture("../media/art/bt_arrow_32.png"));
	guiWindowItems->setVisible(false);



	// TExt GUI for player stats

	IGUIStaticText* guiPlayerMoney = guienv->addStaticText(L"GOLD:129",GUIManager::getInstance()->myRect(15,displayheight-300,300,32),false,false,tab1, GUIManager::ST_ID_PLAYER_MONEY);
	guiPlayerMoney->setOverrideFont(GUIManager::getInstance()->guiFontLarge28);
    guiPlayerMoney->setOverrideColor(SColor(255,255,255,255));

	stringc playerLifeText = LANGManager::getInstance()->getText("txt_player_life");


	IGUIStaticText* guiPlayerLife=guienv->addStaticText(stringw(playerLifeText).c_str(),GUIManager::getInstance()->myRect(15,6,600,30),false,false,tab1,-1,false);
    guiPlayerLife->setOverrideColor(SColor(255,255,255,100));
    guiPlayerLife->setOverrideFont(GUIManager::getInstance()->guiFontLarge28);
	guiPlayerLife->setID(GUIManager::ST_ID_PLAYER_LIFE);

	//Have to rework this. Currently hidden.
	IGUIStaticText* guiPlayerLife_Shadow=guienv->addStaticText(stringw(playerLifeText).c_str(),GUIManager::getInstance()->myRect(14,5,600,30),false,false,tab1,-1,false);
    guiPlayerLife_Shadow->setOverrideColor(SColor(255,30,30,30));
	guiPlayerLife_Shadow->setOverrideFont(GUIManager::getInstance()->guiFontLarge28);
	guiPlayerLife_Shadow->setVisible(false);
	
	GUIManager::getInstance()->setElementVisible(GUIManager::ST_ID_PLAYER_LIFE, false);


	////// --------------------------------
	///    Define the Dialogs used in the game
	//////

	IGUIWindow* guidialog = guienv->addWindow(GUIManager::getInstance()->myRect(10,displayheight-200,displaywidth-20,190),true,L"",0,GUIManager::GCW_DIALOG);
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
	IGUIStaticText* txt_dialog = guienv->addStaticText(L"Hello! This is a simple test to see how the text is flowing inside the box. There is a test, test, and test of text we need to make to be sure the flowing is ok",textRect,false,false,guidialog,GUIManager::TXT_ID_DIALOG,false);
	txt_dialog->setOverrideFont(GUIManager::getInstance()->guiFontDialog);
	txt_dialog->setOverrideColor(SColor(255,255,255,255));
	txt_dialog->setWordWrap(true);
	txt_dialog->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);

	IGUIButton* guiBtDialogYes = guienv->addButton(GUIManager::getInstance()->myRect(640,30,52,52),
                                         guidialog,
										 GUIManager::BT_ID_DIALOG_YES,
                                         L"",
                                         stringw(LANGManager::getInstance()->getText("bt_dialog_yes")).c_str());
    guiBtDialogYes->setImage(guiDialogImgYes);
	guiBtDialogYes->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);
	IGUIButton* guiBtDialogCancel = guienv->addButton(GUIManager::getInstance()->myRect(640,110,52,52),
                                         guidialog,
										 GUIManager::BT_ID_DIALOG_CANCEL,
                                         L"",
                                         stringw(LANGManager::getInstance()->getText("bt_dialog_no")).c_str());
    guiBtDialogCancel->setImage(guiDialogImgNo);
	guiBtDialogCancel->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);
	guidialog->setVisible(false);

}

//Set the cutscene text ingame GUI
void GUIGame::setCutsceneText(core::stringw text)
{
	IGUIStaticText* guitext=(IGUIStaticText*)GUIManager::getInstance()->getGUIElement(GUIManager::ST_ID_CUTSCENE_TEXT);
	guitext->setText(text.c_str());
}

//Display/Hide the cutscene text ingame GUI
void GUIGame::showCutsceneText(bool visible)
{ 
	IGUIStaticText* guitext=(IGUIStaticText*)GUIManager::getInstance()->getGUIElement(GUIManager::ST_ID_CUTSCENE_TEXT);
	guitext->setVisible(visible);
}

//Update the player stats
void GUIGame::drawPlayerStats()
{
//	IVideoDriver * driver = App::getInstance()->getDevice()->getVideoDriver();
	// Text display
	// Update the GUI display
	DynamicObject* playerObject = DynamicObjectsManager::getInstance()->getPlayer();

	stringc playerLife;
	playerLife += playerObject->getProperties().life;
	playerLife += "/";
	playerLife += playerObject->getProperties().maxlife;
	playerLife += " Experience:";
	stringc playerxp = (stringc)playerObject->getProperties().experience;
	playerLife += playerxp;
	playerLife += " Level:";
	playerLife += playerObject->getProperties().level;
	//+(stringc)properties.experience;
	IGUIStaticText* text = (IGUIStaticText*)GUIManager::getInstance()->getGUIElement(GUIManager::ST_ID_PLAYER_LIFE);
	if (text)
		text->setText(((core::stringw)playerLife).c_str());
	
	s32 hp = DynamicObjectsManager::getInstance()->getPlayer()->getProperties().life;
	s32 max = DynamicObjectsManager::getInstance()->getPlayer()->getProperties().maxlife;
	CGUIGfxStatus* lifegauge = (CGUIGfxStatus*)GUIManager::getInstance()->getGUIElement(GUIManager::IMG_LIFEGAUGE);
	lifegauge->setCurrentValue(hp);
	lifegauge->setMaxValue(max);

	s32 mana = DynamicObjectsManager::getInstance()->getPlayer()->getProperties().mana;
	s32 maxmana = DynamicObjectsManager::getInstance()->getPlayer()->getProperties().maxmana;
	CGUIGfxStatus* managauge = (CGUIGfxStatus*)GUIManager::getInstance()->getGUIElement(GUIManager::IMG_MANAGAUGE);
	managauge->setCurrentValue(mana);
	managauge->setMaxValue(maxmana);
}

// Black screen fadeout in gameplay
void GUIGame::showBlackScreen(stringc text)
{
	IGUIInOutFader* fader=(IGUIInOutFader*)GUIManager::getInstance()->getGUIElement(GUIManager::ID_FADER);
    fader->setVisible(true);
    fader->fadeOut(1000);

    while(!fader->isReady() && App::getInstance()->getDevice()->run())
    {
        App::getInstance()->getDevice()->getVideoDriver()->beginScene(true, true, SColor(0,200,200,200));
        App::getInstance()->getDevice()->getSceneManager()->drawAll();

        guienv->drawAll();

		GUIManager::getInstance()->guiFontLarge28->draw(stringw(text),GUIManager::getInstance()->myRect(0,0,
                                             App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width,
                                             App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height ),
                     SColor(255,255,255,255),true,true);


        App::getInstance()->getDevice()->getVideoDriver()->endScene();
    }
}

// Black screen fadeout in gameplay
void GUIGame::hideBlackScreen()
{
	IGUIInOutFader* fader=(IGUIInOutFader*)GUIManager::getInstance()->getGUIElement(GUIManager::ID_FADER);
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

// Display a ingame message
void GUIGame::showDialogMessage(stringw text, std::string sound)
{

	IGUIStaticText* txt_dialog=(IGUIStaticText*)GUIManager::getInstance()->getGUIElement(GUIManager::TXT_ID_DIALOG);
	IGUIButton* guiBtDialogYes=(IGUIButton*)GUIManager::getInstance()->getGUIElement(GUIManager::BT_ID_DIALOG_YES);
	IGUIButton* guiBtDialogCancel=(IGUIButton*)GUIManager::getInstance()->getGUIElement(GUIManager::BT_ID_DIALOG_CANCEL);
	
	//stringw text2 = (stringw)text.c_str();
	txt_dialog->setText(text.c_str());
	if(guiBtDialogCancel->isVisible())
		guiBtDialogCancel->setVisible(false);

	GUIManager::getInstance()->setWindowVisible(GUIManager::GCW_DIALOG,true);
	App::getInstance()->setAppState(App::APP_WAIT_DIALOG);

	//Play dialog sound (yes you can record voices!)
    dialogSound = NULL;

	//Pause the player during the dialog opening
	DynamicObjectsManager::getInstance()->getPlayer()->setAnimation("idle");

	if (sound.size()>0)
    //if((sound.c_str() != "") | (sound.c_str() != NULL))
    {
        stringc soundName = "../media/sound/";
        soundName += sound.c_str();
        dialogSound = SoundManager::getInstance()->playSound2D(soundName.c_str());
    }

}
//Display a ingame question
bool GUIGame::showDialogQuestion(stringw text, std::string sound )
{

	IGUIStaticText* txt_dialog=(IGUIStaticText*)GUIManager::getInstance()->getGUIElement(GUIManager::TXT_ID_DIALOG);
	IGUIButton* guiBtDialogCancel=(IGUIButton*)GUIManager::getInstance()->getGUIElement(GUIManager::BT_ID_DIALOG_CANCEL);
	//Pause the player during the dialog opening
	DynamicObjectsManager::getInstance()->getPlayer()->setAnimation("idle");

	//stringw text2 = (stringw)text.c_str();
	txt_dialog->setText(text.c_str());
	if(!guiBtDialogCancel->isVisible())
		guiBtDialogCancel->setVisible(true);

	GUIManager::getInstance()->setWindowVisible(GUIManager::GCW_DIALOG,true);
	App::getInstance()->setAppState(App::APP_WAIT_DIALOG);

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

//Need to be reworked: BAD. Render loop is done here!
stringc GUIGame::showInputQuestion(stringw text)
{
    std::string newtxt = "";

    bool mouseExit = false;

	
    while(!App::getInstance()->isKeyPressed(KEY_RETURN) && mouseExit==false && App::getInstance()->getDevice()->run())
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

		GUIManager::getInstance()->guiFontDialog->draw(realTxt.c_str(),textRect,SColor(255,255,255,255),false,false,&textRect);


        //draw YES GREEN button
        position2di buttonYesPosition = position2di(App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width - 58,
                    App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height - 58);

        App::getInstance()->getDevice()->getVideoDriver()->draw2DImage(guiDialogImgYes,buttonYesPosition,
                                                                   rect<s32>(0,0,48,48),0,SColor(255,255,255,255),true);

        //check mouse click on OK button
        position2di mousePos = App::getInstance()->getDevice()->getCursorControl()->getPosition();
		if(mousePos.getDistanceFrom(buttonYesPosition+position2di(16,16)) < 16 && App::getInstance()->isMousePressed(0)) mouseExit = true;


        //verify pressed chars and add it to the string

        if(timercheck-timer > 160)
        {
            //process all keycodes [0-9] and [A-Z]
            for(int i=0x30;i<0x5B;i++)
            {
				if(App::getInstance()->isKeyPressed(i))
                {
                    newtxt += i;
                    timer = timercheck;
                }
            }

            //process delete and backspace (same behavior for both of them -> remove the last char)
            if(App::getInstance()->isKeyPressed(KEY_BACK) || App::getInstance()->isKeyPressed(KEY_DELETE))
            {
                newtxt = newtxt.substr(0,newtxt.size()-1);
				timer = timercheck;
            }
        }
        App::getInstance()->getDevice()->getVideoDriver()->endScene();
    }

    //EventReceiver::getInstance()->flushKeys();
    //EventReceiver::getInstance()->flushMouse();
	GUIManager::getInstance()->flush();

    return stringc(newtxt.c_str());
}

//stop sound when player cancel the dialog
void GUIGame::stopDialogSound()
{
    if(dialogSound)
    {
        dialogSound->stop();
    }

}

//Return the current active player item
stringc GUIGame::getActivePlayerItem()
{
	IGUIListBox* guiPlayerItems =(IGUIListBox*)GUIManager::getInstance()->getGUIElement(GUIManager::LB_ID_PLAYER_ITEMS);

    return stringc(guiPlayerItems->getListItem(guiPlayerItems->getSelected()));
}

//Return the currently loot item. (Should be moved back in the gui manager)
DynamicObject* GUIGame::getActiveLootItem()
{
	vector<DynamicObject*> lootitems = DynamicObjectsManager::getInstance()->getPlayer()->getLootItems();
	IGUIListBox* guiPlayerItems =(IGUIListBox*)GUIManager::getInstance()->getGUIElement(GUIManager::LB_ID_PLAYER_ITEMS);

	s32 item=guiPlayerItems->getSelected();
	if (item>-1 && lootitems.size()>0)
		return lootitems[item];
	else
		return NULL;
}

//Update the items list in gameplay
void GUIGame::updateItemsList()
{
	IGUIImage* guiPlayerLootImage = (IGUIImage*)GUIManager::getInstance()->getGUIElement(GUIManager::IMG_LOOT);
	IGUIListBox* guiPlayerItems =(IGUIListBox*)GUIManager::getInstance()->getGUIElement(GUIManager::LB_ID_PLAYER_ITEMS);
    guiPlayerItems->clear();
   
	vector<DynamicObject*> lootitems = DynamicObjectsManager::getInstance()->getPlayer()->getLootItems();
	std::sort(lootitems.begin(), lootitems.end());

    
	for(int i = 0; i<(int)lootitems.size(); i++)
	{
		//internalname is the Alias name of the object. If undefined it use the internal name
		guiPlayerItems->addItem(stringw(lootitems[i]->internalname).c_str());
	}

	if (guiPlayerItems->getSelected()<0)
	{
		ITexture* info_none = driver->getTexture("../media/editor/info_none.jpg");
		guiPlayerLootImage->setImage(info_none);
		((IGUIStaticText*)guienv->getRootGUIElement()->getElementFromId(GUIManager::TXT_ID_LOOT_DESCRIPTION,true))->setText(L"");
	}
}
