#include "GUIConfigWindow.h"
#include "../LANGManager.h"
#include "../tinyXML/tinyxml.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

GUIConfigWindow::GUIConfigWindow(IrrlichtDevice* device)
{
	mapname="";
    this->device = device;

    IGUIEnvironment* guienv = device->getGUIEnvironment();

    cfgWindow = guienv->addWindow(myRect(device->getVideoDriver()->getScreenSize().Width/2 - 250,device->getVideoDriver()->getScreenSize().Height/2 - 140,500,460),
                                  false,
                                  stringw(LANGManager::getInstance()->getText("txt_cfg_window_caption")).c_str());

    cfgWindow->getCloseButton()->setVisible(false);

    IGUITabControl* tabCtrl = guienv->addTabControl(rect<s32>(10,30,cfgWindow->getClientRect().LowerRightCorner.X - 10,cfgWindow->getClientRect().LowerRightCorner.Y - 45),cfgWindow,true,true);

    tabSystem = tabCtrl->addTab(stringw(LANGManager::getInstance()->getText("txt_cfg_window_tab_system")).c_str());
#ifdef EDITOR
    tabScenary = tabCtrl->addTab(stringw(LANGManager::getInstance()->getText("txt_cfg_window_tab_scenary")).c_str());
	tabExperimental = tabCtrl->addTab(stringw("Experimental features").c_str());
	tabPlayerApp = tabCtrl->addTab(stringw("Player application specific").c_str());
#else
	tabScenary = NULL;
	tabPlayerApp = NULL;
#endif

    //==========System settings

    int YPos = 10;

    //resolution
	guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_cfg_window_resolution")).c_str(),myRect(10,YPos,tabSystem->getAbsoluteClippingRect().getWidth()-20,25),false,false,tabSystem);

    YPos += 15;

	
    resolutionList = guienv->addComboBox(myRect(10,YPos,450,25),tabSystem,-1);
	YPos += 35;


	 //Game Camera
	guienv->addStaticText(stringw("Default game camera setup").c_str(),myRect(10,YPos,tabSystem->getAbsoluteClippingRect().getWidth()-20,25),false,false,tabSystem);

	 YPos += 15;
	camera = guienv->addComboBox(myRect(10,YPos,450,25),tabSystem,-1);
	camera->addItem(L"Third person RTS style camera");
	camera->addItem(L"Third person RPG style camera");
	camera->addItem(L"First person FPS style camera");




    YPos += 35;

    cbFullscreen = guienv->addCheckBox(false,myRect(10,YPos,190,25),tabSystem,-1,stringw(LANGManager::getInstance()->getText("txt_cfg_window_fullscreen")).c_str());

    cbResizeable = guienv->addCheckBox(false,myRect(200,YPos,190,25),tabSystem,-1,stringw(LANGManager::getInstance()->getText("txt_cfg_window_resizeable")).c_str());

	YPos += 35;
	cbVSync = guienv->addCheckBox(false,myRect(10,YPos,190,25),tabSystem,-1,stringw(LANGManager::getInstance()->getText("txt_cfg_window_vsync")).c_str());

	cbAntialias = guienv->addCheckBox(false,myRect(200,YPos,190,25),tabSystem,-1,stringw(LANGManager::getInstance()->getText("txt_cfg_window_antialias")).c_str());
	YPos += 35;

	cbSilouette = guienv->addCheckBox(false,myRect(10,YPos,380,25),tabSystem,-1,stringw(LANGManager::getInstance()->getText("txt_cfg_window_silouette")).c_str());
	YPos += 35;
    //language
    guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_cfg_window_language")).c_str(),myRect(10,YPos,150,25),false,false,tabSystem);

    YPos += 15;

    languageList = guienv->addComboBox(myRect(10,YPos,450,25),tabSystem,-1);
	YPos += 35;

    this->populateLanguageList();



    btOK = guienv->addButton(myRect(cfgWindow->getClientRect().LowerRightCorner.X - 100,cfgWindow->getClientRect().LowerRightCorner.Y - 35,90,25),
                             cfgWindow,
                             -1,
                             stringw(LANGManager::getInstance()->getText("bt_cfg_window_ok")).c_str());

    btCancel = guienv->addButton(myRect(cfgWindow->getClientRect().LowerRightCorner.X - 200,cfgWindow->getClientRect().LowerRightCorner.Y - 35,90,25),
                                 cfgWindow,
                                 -1,
                                 stringw(LANGManager::getInstance()->getText("bt_cfg_window_cancel")).c_str());

    //==========Scenary seetings
#ifdef EDITOR
	YPos = 10;

    //Terrain mesh and Scale
	guienv->addStaticText(stringw("Terrain tile mesh default save format:").c_str(),myRect(10,YPos,250,25),false,false,tabScenary);
    //guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_cfg_window_terrain_mesh")).c_str(),myRect(10,YPos,250,25),false,false,tabScenary);
    guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_cfg_window_terrain_scale")).c_str(),myRect(370,YPos,100,25),false,false,tabScenary);
	guienv->addStaticText(stringw("Terrain density").c_str(),myRect(270,YPos,90,25),false,false,tabScenary);
    YPos += 15;
    ebTerrainMesh = guienv->addEditBox(L"",myRect(0,0,0,0),true, tabScenary,-1); //Hidden for now.
    ebTerrainScale = guienv->addEditBox(L"",myRect(370,YPos,90,25),true, tabScenary,-1);
	ebTerrainDensity = guienv->addEditBox(L"100",myRect(270,YPos,90,25),true, tabScenary,-1);

	
  
	// Terrain tiles mesh format
	//YPos += 25;
	models = guienv->addComboBox(myRect(10,YPos,250,25), tabScenary, -1);
	models->addItem(stringw(" B3D (Blitz 3D) format").c_str(),0);
	models->addItem(stringw(" OBJ (Wavefront) format").c_str(),1);
	models->addItem(stringw(" DAE (Collada) format").c_str(),2);

	YPos += 35;
    //Terrain layers
    guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_cfg_window_terrain_layers")).c_str(),myRect(10,YPos,250,25),false,false,tabScenary);

    //ebTerrainL0 = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
    YPos += 15;
    ebTerrainL1 = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
    YPos += 25;
    ebTerrainL2 = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
    YPos += 25;
    ebTerrainL3 = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
    YPos += 25;
	ebTerrainL4 = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
	

    //Ocean Mesh
//	YPos += 35;
//    guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_cfg_window_ocean_mesh")).c_str(),myRect(10,YPos,250,25),false,false,tabScenary);
//    YPos += 15;
//    ebOceanMesh = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
   

    //Ocean normalMap
	 YPos += 35;
    guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_cfg_window_ocean_normalmap")).c_str(),myRect(10,YPos,250,25),false,false,tabScenary);
    YPos += 15;
    ebOceanNormalMap= guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
  

    //Ocean Reflection
	  YPos += 35;
    guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_cfg_window_ocean_reflection")).c_str(),myRect(10,YPos,250,25),false,false,tabScenary);
    YPos += 15;
    ebOceanReflection= guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
    YPos += 25;

	// Experimental features
	 YPos = 10;
	cbXEffect = guienv->addCheckBox(false,myRect(10,YPos,400,25),tabExperimental,-1,stringw("XEffect soft shadowing system addon").c_str());


	// Player application specific
	 YPos = 10;
	guienv->addStaticText(stringw("Default project for the player application").c_str(),myRect(10,YPos,350,25),false,false,tabPlayerApp);
	YPos += 15;

	ebDefaultProject = guienv->addEditBox(L"../projects/",myRect(10,YPos,450,25),true, tabPlayerApp,-1);

	YPos += 35;
	guienv->addStaticText(stringw("Image for the Game Logo").c_str(),myRect(10,YPos,350,25),false,false,tabPlayerApp);
	YPos += 15;

	ebLogoImage = guienv->addEditBox(L"../media/",myRect(10,YPos,450,25),true, tabPlayerApp,-1);


#else
	//In the PLayer application the terrain settings are not available so hidden
	//The gui need to be created since we put data in them that goes in the config file.
	YPos = 610;
    ebTerrainMesh = guienv->addEditBox(L"",myRect(0,0,0,0),true, tabSystem,-1); //Hidden for now.
    ebTerrainScale = guienv->addEditBox(L"",myRect(370,YPos,90,25),true, tabSystem,-1);
  	models = guienv->addComboBox(myRect(10,YPos,250,25), tabSystem, -1);
	models->addItem(stringw(" B3D (Blitz 3D) format").c_str(),0);
	models->addItem(stringw(" OBJ (Wavefront) format").c_str(),1);
	models->addItem(stringw(" DAE (Collada) format").c_str(),2);
    YPos += 15;
    ebTerrainL1 = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabSystem,-1);
    YPos += 25;
    ebTerrainL2 = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabSystem,-1);
    YPos += 25;
    ebTerrainL3 = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabSystem,-1);
    YPos += 25;
	ebTerrainL4 = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabSystem,-1);
	YPos += 15;
	ebTerrainDensity = guienv->addEditBox(L"100",myRect(270,YPos,90,25),true, tabSystem,-1);

    //Ocean normalMap
	 YPos += 15;
    ebOceanNormalMap= guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabSystem,-1);
  

    //Ocean Reflection
	
    ebOceanReflection= guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabSystem,-1);
    YPos += 25;

	// Experimental features
	cbXEffect = guienv->addCheckBox(false,myRect(10,YPos,400,25),tabSystem,-1,stringw("XEffect soft shadowing system addon").c_str());

	// Player application specific
	ebDefaultProject = guienv->addEditBox(L"../projects/",myRect(10,YPos,450,25),true, tabSystem,-1);
	YPos += 15;
	ebLogoImage = guienv->addEditBox(L"../media/",myRect(10,YPos,450,25),true, tabSystem,-1);

#endif

    this->populateResolutionList();

    this->loadActualSeetings();

    cfgWindow->setVisible(false);

}

GUIConfigWindow::~GUIConfigWindow()
{
	if (cfgWindow)
	{
		cfgWindow->remove();
		cfgWindow=NULL;
	}
    //dtor
}

void GUIConfigWindow::showWindow()
{
    bool runCfg = true;

    cfgWindow->setVisible(true);

	cfgWindow->setEnabled(true);
	//device->getGUIEnvironment()->setFocus(cfgWindow);

    while(runCfg)
    {
        if(btCancel->isPressed()) runCfg = false;

        if(btOK->isPressed())
        {
            this->saveNewSeetings();
            runCfg = false;
            device->getGUIEnvironment()->addMessageBox(L"",stringw(LANGManager::getInstance()->getText("txt_cfg_window_changes_msg")).c_str(),true);
        }

        device->run();

        device->getVideoDriver()->beginScene(true,true,SColor(0,160,160,160));

        device->getSceneManager()->drawAll();
		
#ifndef EDITOR
			device->getGUIEnvironment()->getRootGUIElement()->bringToFront(cfgWindow);
			device->getGUIEnvironment()->drawAll();
#else
 
			cfgWindow->draw();
#endif		

        device->getVideoDriver()->endScene();
    }

    cfgWindow->setVisible(false);

    btOK->setPressed(false);
    btCancel->setPressed(false);
}

void GUIConfigWindow::populateLanguageList()
{
    TiXmlDocument doc("../media/lang.xml");
	if (!doc.LoadFile()) return;

	TiXmlElement* root = doc.FirstChildElement( "IrrRPG_Builder_LanguageFile" );

    if ( root )
    {
        TiXmlNode* languageXML = root->FirstChildElement( "language" );

        while( languageXML != NULL )
        {
			wchar_t out[255];
			core::utf8ToWchar(languageXML->ToElement()->Attribute("description"),out,255);
			languageList->addItem(out);
            languageListVector.push_back(stringw(languageXML->ToElement()->Attribute("name")));

            languageXML = root->ToElement()->IterateChildren( "language", languageXML );
        }
    }
}

void GUIConfigWindow::populateResolutionList()
{
    IVideoModeList* modeList = device->getVideoModeList();

    //scan available video modes and populate the comboBox
    for ( int i = 0; i != modeList->getVideoModeCount (); ++i )
    {
        u16 d = modeList->getVideoModeDepth ( i );
        if ( d < 16 ) continue;

        u16 w = modeList->getVideoModeResolution ( i ).Width;
        u16 h = modeList->getVideoModeResolution ( i ).Height;

        bool vModeAlreadyExist = false;

        //check if the video mode is unique (16 or 32 bits and refresh rate does not matters)
		for(irr::u16 j=0;j<vModes.size();j++)
        {
            if( vModes[j].X == w && vModes[j].Y == h ) vModeAlreadyExist = true;
        }

        if(!vModeAlreadyExist)
        {
            stringw modeName = stringw(w);
            modeName.append(" x ");
            modeName.append( stringw(h) );

			//Limit the list to the "approved" resolutions
			//A 800x600 display is the minimum for IRB
            if (w>=800 && h>=600)
			{
				resolutionList->addItem(modeName.c_str());
				vector2di v;
				v.X = w;
				v.Y = h;

				vModes.push_back(v);
			}
		}
    }
}

// Load the actual settings to display into the GUI
// Loading is also done in APP that set the application
void GUIConfigWindow::loadActualSeetings()
{

    TiXmlDocument doc("config.xml");

	if (!doc.LoadFile()) return;

	TiXmlElement* root = doc.FirstChildElement( "IrrRPG_Builder_Config" );

    if ( root )
    {
        TiXmlNode* screenXML = root->FirstChildElement( "screen" );

        if(screenXML)
        {
			stringc result = "";
			int sw = atoi(screenXML->ToElement()->Attribute("screen_width"));
			int sh = atoi(screenXML->ToElement()->Attribute("screen_height"));
			
			result = screenXML->ToElement()->Attribute("camera");
			if (result.size()>0)
			{
				if (result == std::string("rts").c_str())
				{
					App::getInstance()->setRTSView();
					camera->setSelected(0);
				}
				if (result == std::string("rpg").c_str())
				{
					App::getInstance()->setRPGView();
					camera->setSelected(1);
				}
				if (result == std::string("fps").c_str())
				{
					App::getInstance()->setFPSView();
					camera->setSelected(2);
				}
			}

			result = screenXML->ToElement()->Attribute("fullscreen");
			if (result.size()>0)
			{
				 bool sFullscreen =  (screenXML->ToElement()->Attribute("fullscreen") == std::string("true") ? true : false);
				 cbFullscreen->setChecked(sFullscreen);
			}
            
			result = screenXML->ToElement()->Attribute("resizeable");
			if (result.size()>0)
			{
				 bool sResizeable =  (screenXML->ToElement()->Attribute("resizeable") == std::string("true") ? true : false);
				 cbResizeable->setChecked(sResizeable);
			}

			result = screenXML->ToElement()->Attribute("vsync");
			if (result.size()>0)
			{
				 bool svSync =  (screenXML->ToElement()->Attribute("vsync") == std::string("true") ? true : false);
				 cbVSync->setChecked(svSync);
			}

			result = screenXML->ToElement()->Attribute("antialias");
			if (result.size()>0)
			{
				 bool svAntialias =  (screenXML->ToElement()->Attribute("antialias") == std::string("true") ? true : false);
				 cbAntialias->setChecked(svAntialias);
			}

			result = screenXML->ToElement()->Attribute("silouette");
			if (result.size()>0)
			{
				bool svSilouette = (screenXML->ToElement()->Attribute("silouette") == std::string("true") ? true : false);
				cbSilouette->setChecked(svSilouette);
			}

			for(irr::u16 i=0;i<vModes.size();i++)
            {
                if(vModes[i].X == sw && vModes[i].Y == sh) resolutionList->setSelected(i);
            }

			result = screenXML->ToElement()->Attribute("xeffects");
			if (result.size()>0)
			{
				bool svxeffects = (screenXML->ToElement()->Attribute("xeffects") == std::string("true") ? true : false);
				cbXEffect->setChecked(svxeffects);
			}

	
        }

        TiXmlNode* languageXML = root->FirstChildElement( "language" );

        if(languageXML)
        {
			for(irr::u16 i=0;i<languageListVector.size();i++)
                if(languageListVector[i] == stringc(languageXML->ToElement()->Attribute("type"))) languageList->setSelected(i);
        }


        TiXmlNode* terrainXML = root->FirstChildElement( "terrain" );

        if(terrainXML)
        {
			stringc result=terrainXML->ToElement()->Attribute("model");
			
			if (result=="B3D")
			{
				models->setSelected(0);
			}

			if (result=="OBJ")
			{
				models->setSelected(1);
			}

			if (result=="DAE")
			{
				models->setSelected(2);
			}
			
			result=terrainXML->ToElement()->Attribute("density");
			u32 den = atoi(result.c_str());
			if (result.size()>0 && den>9 && den<251)
				ebTerrainDensity->setText(stringw(result).c_str());

            ebTerrainMesh->setText(stringw(terrainXML->ToElement()->Attribute("mesh")).c_str());
            //ebTerrainL0->setText(stringw(terrainXML->ToElement()->Attribute("layer0")).c_str());
            ebTerrainL1->setText(stringw(terrainXML->ToElement()->Attribute("layer1")).c_str());
            ebTerrainL2->setText(stringw(terrainXML->ToElement()->Attribute("layer2")).c_str());
            ebTerrainL3->setText(stringw(terrainXML->ToElement()->Attribute("layer3")).c_str());
			ebTerrainL4->setText(stringw(terrainXML->ToElement()->Attribute("layer4")).c_str());
            ebTerrainScale->setText(stringw(terrainXML->ToElement()->Attribute("scale")).c_str());
        }

        TiXmlNode* oceanXML = root->FirstChildElement( "ocean" );

        if(oceanXML)
        {
            //ebOceanMesh->setText(stringw(oceanXML->ToElement()->Attribute("mesh")).c_str());
            ebOceanNormalMap->setText(stringw(oceanXML->ToElement()->Attribute("normalmap")).c_str());
            ebOceanReflection->setText(stringw(oceanXML->ToElement()->Attribute("reflection")).c_str());
        }
	}


	//Default MapName
	TiXmlElement* mapXML = root->FirstChildElement( "map" );
	if ( mapXML )
	{
		mapname = mapXML->ToElement()->Attribute("name");
		if (mapname.size()>0)
			ebDefaultProject->setText(stringw(mapname).c_str());
	}

	ebLogoImage->setText(stringw(App::getInstance()->logoimage).c_str()); //Get the loaded valued from the APP list
}

void GUIConfigWindow::saveNewSeetings()
{
    TiXmlDocument doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );

    doc.LinkEndChild(decl);

    TiXmlElement* irb_cfg = new TiXmlElement( "IrrRPG_Builder_Config" );
	irb_cfg->SetAttribute("version","0.3");

    TiXmlElement* screenXML = new TiXmlElement("screen");
    screenXML->SetAttribute("screen_width",vModes[resolutionList->getSelected()].X);
    screenXML->SetAttribute("screen_height",vModes[resolutionList->getSelected()].Y);
	if (camera)
	{
		if (camera->getSelected()==0)
			screenXML->SetAttribute("camera","rts");
		if (camera->getSelected()==1)
			screenXML->SetAttribute("camera","rpg");
		if (camera->getSelected()==2)
			screenXML->SetAttribute("camera","fps");

	}
    screenXML->SetAttribute("fullscreen", cbFullscreen->isChecked()?"true":"false" );
    screenXML->SetAttribute("resizeable", cbResizeable->isChecked()?"true":"false" );
	screenXML->SetAttribute("vsync", cbVSync->isChecked()?"true":"false" );
	screenXML->SetAttribute("antialias", cbAntialias->isChecked()?"true":"false" );
	screenXML->SetAttribute("silouette", cbSilouette->isChecked()?"true":"false" );
	screenXML->SetAttribute("xeffects", cbXEffect->isChecked()?"true":"false" );
	
    irb_cfg->LinkEndChild(screenXML);

    TiXmlElement* languageXML = new TiXmlElement("language");
    languageXML->SetAttribute("type",languageListVector[languageList->getSelected()].c_str());

    irb_cfg->LinkEndChild(languageXML);

    TiXmlElement* terrainXML = new TiXmlElement("terrain");
    //terrainXML->SetAttribute("mesh",stringc(ebTerrainMesh->getText()).c_str());
	stringc modelstext = "";
	u32 modelselect=models->getSelected();
	if (modelselect==0)
		modelstext="B3D";
	if (modelselect==1)
		modelstext="OBJ";
	if (modelselect==2)
		modelstext="DAE";

	terrainXML->SetAttribute("model",modelstext.c_str());
	stringc result=ebTerrainDensity->getText();
	u32 den = atoi(result.c_str());
	if (den>9 && den<251)
		terrainXML->SetAttribute("density",result.c_str());
	else
		terrainXML->SetAttribute("density","100"); //Default value
    //terrainXML->SetAttribute("layer0",stringc(ebTerrainL0->getText()).c_str());
    terrainXML->SetAttribute("layer1",stringc(ebTerrainL1->getText()).c_str());
    terrainXML->SetAttribute("layer2",stringc(ebTerrainL2->getText()).c_str());
    terrainXML->SetAttribute("layer3",stringc(ebTerrainL3->getText()).c_str());
	terrainXML->SetAttribute("layer4",stringc(ebTerrainL4->getText()).c_str());
    terrainXML->SetAttribute("scale",atoi(stringc(ebTerrainScale->getText()).c_str()));

    irb_cfg->LinkEndChild(terrainXML);

    TiXmlElement* oceanXML = new TiXmlElement("ocean");
    //oceanXML->SetAttribute("mesh",stringc(ebOceanMesh->getText()).c_str() );
    oceanXML->SetAttribute("normalmap",stringc(ebOceanNormalMap->getText()).c_str() );
    oceanXML->SetAttribute("reflection",stringc(ebOceanReflection->getText()).c_str() );
	irb_cfg->LinkEndChild(oceanXML);


	result = stringc(ebDefaultProject->getText());
	TiXmlElement* mapXML = new TiXmlElement("map");
	mapXML->SetAttribute("name",result.c_str());
	mapXML->SetAttribute("logo", core::stringc(ebLogoImage->getText()).c_str() );
	irb_cfg->LinkEndChild(mapXML);

    doc.LinkEndChild(irb_cfg);

	core::stringc path = App::getInstance()->getAppDataPath().c_str();
	path.append("/config.xml");
#ifdef EDITOR
    doc.SaveFile(path.c_str());
#else
	doc.SaveFile("config.xml");
#endif
}

rect<s32> GUIConfigWindow::myRect(s32 x, s32 y, s32 w, s32 h)
{
    return rect<s32>(x,y,x+w,y+h);
}
