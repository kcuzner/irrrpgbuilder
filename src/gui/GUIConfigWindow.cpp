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

    IGUITab* tabSystem = tabCtrl->addTab(stringw(LANGManager::getInstance()->getText("txt_cfg_window_tab_system")).c_str());
    IGUITab* tabScenary = tabCtrl->addTab(stringw(LANGManager::getInstance()->getText("txt_cfg_window_tab_scenary")).c_str());


    //==========System seetings

    int YPos = 10;

    //resolution
	guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_cfg_window_resolution")).c_str(),myRect(10,YPos,tabSystem->getAbsoluteClippingRect().getWidth()-20,25),false,false,tabSystem);

    YPos += 15;

    resolutionList = guienv->addComboBox(myRect(10,YPos,450,25),tabSystem,-1);

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

    YPos = 10;

    //Terrain mesh and Scale
    guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_cfg_window_terrain_mesh")).c_str(),myRect(10,YPos,250,25),false,false,tabScenary);
    guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_cfg_window_terrain_scale")).c_str(),myRect(370,YPos,100,25),false,false,tabScenary);
    YPos += 15;
    ebTerrainMesh = guienv->addEditBox(L"",myRect(10,YPos,350,25),true, tabScenary,-1);
    ebTerrainScale = guienv->addEditBox(L"",myRect(370,YPos,90,25),true, tabScenary,-1);
    YPos += 25;


    //Terrain layers
    guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_cfg_window_terrain_layers")).c_str(),myRect(10,YPos,250,25),false,false,tabScenary);

    YPos += 15;
    ebTerrainL0 = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
    YPos += 25;
    ebTerrainL1 = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
    YPos += 25;
    ebTerrainL2 = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
    YPos += 25;
    ebTerrainL3 = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
    YPos += 25;
	ebTerrainL4 = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
	YPos += 25;

    //Ocean Mesh
    guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_cfg_window_ocean_mesh")).c_str(),myRect(10,YPos,250,25),false,false,tabScenary);
    YPos += 15;
    ebOceanMesh = guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
    YPos += 25;

    //Ocean normalMap
    guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_cfg_window_ocean_normalmap")).c_str(),myRect(10,YPos,250,25),false,false,tabScenary);
    YPos += 15;
    ebOceanNormalMap= guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
    YPos += 25;

    //Ocean Reflection
    guienv->addStaticText(stringw(LANGManager::getInstance()->getText("txt_cfg_window_ocean_reflection")).c_str(),myRect(10,YPos,250,25),false,false,tabScenary);
    YPos += 15;
    ebOceanReflection= guienv->addEditBox(L"",myRect(10,YPos,450,25),true, tabScenary,-1);
    YPos += 25;

    this->populateResolutionList();

    this->loadActualSeetings();

    cfgWindow->setVisible(false);
}

GUIConfigWindow::~GUIConfigWindow()
{
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

        device->getVideoDriver()->beginScene(true,true,SColor(0,200,200,200));

        device->getSceneManager()->drawAll();
        cfgWindow->draw();
		

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
				resolutionList->addItem(modeName.c_str());

            vector2di v;
            v.X = w;
            v.Y = h;

            vModes.push_back(v);
        }
    }
}

// Load the actual settings to display into the GUI
// Loading is also done in APP that set the application
void GUIConfigWindow::loadActualSeetings()
{
#ifdef EDITOR
    TiXmlDocument doc("config.xml");

#else
	TiXmlDocument doc("gameconfig.xml");
#endif
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
            ebTerrainMesh->setText(stringw(terrainXML->ToElement()->Attribute("mesh")).c_str());
            ebTerrainL0->setText(stringw(terrainXML->ToElement()->Attribute("layer0")).c_str());
            ebTerrainL1->setText(stringw(terrainXML->ToElement()->Attribute("layer1")).c_str());
            ebTerrainL2->setText(stringw(terrainXML->ToElement()->Attribute("layer2")).c_str());
            ebTerrainL3->setText(stringw(terrainXML->ToElement()->Attribute("layer3")).c_str());
			ebTerrainL4->setText(stringw(terrainXML->ToElement()->Attribute("layer4")).c_str());
            ebTerrainScale->setText(stringw(terrainXML->ToElement()->Attribute("scale")).c_str());
        }

        TiXmlNode* oceanXML = root->FirstChildElement( "ocean" );

        if(oceanXML)
        {
            ebOceanMesh->setText(stringw(oceanXML->ToElement()->Attribute("mesh")).c_str());
            ebOceanNormalMap->setText(stringw(oceanXML->ToElement()->Attribute("normalmap")).c_str());
            ebOceanReflection->setText(stringw(oceanXML->ToElement()->Attribute("reflection")).c_str());
        }
#ifndef EDITOR
		TiXmlElement* mapXML = root->FirstChildElement( "map" );
		if ( mapXML )
		{
			mapname = mapXML->ToElement()->Attribute("name");
			printf("The map name is: %s\n",mapname.c_str());
			///TODO: we are just loading ocean seetings, we need to set it!
		}
#endif
    }
}

void GUIConfigWindow::saveNewSeetings()
{
    TiXmlDocument doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );

    doc.LinkEndChild(decl);

    TiXmlElement* irb_cfg = new TiXmlElement( "IrrRPG_Builder_Config" );
	irb_cfg->SetAttribute("version","1.0");

    TiXmlElement* screenXML = new TiXmlElement("screen");
    screenXML->SetAttribute("screen_width",vModes[resolutionList->getSelected()].X);
    screenXML->SetAttribute("screen_height",vModes[resolutionList->getSelected()].Y);
    screenXML->SetAttribute("fullscreen", cbFullscreen->isChecked()?"true":"false" );
    screenXML->SetAttribute("resizeable", cbResizeable->isChecked()?"true":"false" );
	screenXML->SetAttribute("vsync", cbVSync->isChecked()?"true":"false" );
	screenXML->SetAttribute("antialias", cbAntialias->isChecked()?"true":"false" );
	screenXML->SetAttribute("silouette", cbSilouette->isChecked()?"true":"false" );

    irb_cfg->LinkEndChild(screenXML);

    TiXmlElement* languageXML = new TiXmlElement("language");
    languageXML->SetAttribute("type",languageListVector[languageList->getSelected()].c_str());

    irb_cfg->LinkEndChild(languageXML);

    TiXmlElement* terrainXML = new TiXmlElement("terrain");
    terrainXML->SetAttribute("mesh",stringc(ebTerrainMesh->getText()).c_str());
    terrainXML->SetAttribute("layer0",stringc(ebTerrainL0->getText()).c_str());
    terrainXML->SetAttribute("layer1",stringc(ebTerrainL1->getText()).c_str());
    terrainXML->SetAttribute("layer2",stringc(ebTerrainL2->getText()).c_str());
    terrainXML->SetAttribute("layer3",stringc(ebTerrainL3->getText()).c_str());
	terrainXML->SetAttribute("layer4",stringc(ebTerrainL4->getText()).c_str());
    terrainXML->SetAttribute("scale",atoi(stringc(ebTerrainScale->getText()).c_str()));

    irb_cfg->LinkEndChild(terrainXML);

    TiXmlElement* oceanXML = new TiXmlElement("ocean");
    oceanXML->SetAttribute("mesh",stringc(ebOceanMesh->getText()).c_str() );
    oceanXML->SetAttribute("normalmap",stringc(ebOceanNormalMap->getText()).c_str() );
    oceanXML->SetAttribute("reflection",stringc(ebOceanReflection->getText()).c_str() );
	irb_cfg->LinkEndChild(oceanXML);

#ifndef EDITOR
	TiXmlElement* mapXML = new TiXmlElement("map");
	mapXML->SetAttribute("name",mapname.c_str());
	irb_cfg->LinkEndChild(mapXML);
#endif

    

    doc.LinkEndChild(irb_cfg);
#ifdef EDITOR
    doc.SaveFile("config.xml");
#else
	doc.SaveFile("gameconfig.xml");
#endif
}

rect<s32> GUIConfigWindow::myRect(s32 x, s32 y, s32 w, s32 h)
{
    return rect<s32>(x,y,x+w,y+h);
}
