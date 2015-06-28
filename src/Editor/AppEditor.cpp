#include "AppEditor.h"
#include "../App.h"
#include "../tinyXML/tinyxml.h"

#ifndef WIN32
	#include "../xdg-user-dir-lookup.c" //Linux favorites folders XDG
	#include <sys/types.h>
	#include <sys/stat.h>
#else
	#include <winnls.h>
	#include <direct.h>
#endif

AppEditor::AppEditor()
{

	IrrlichtDevice * tempdevice = createDevice(EDT_NULL, dimension2d<u32>(640, 480), 16, false, false, false, 0);
	
	

	core:stringw docpath=L"";
#ifdef WIN32
	// Get the desktop shortcut (places)
	// String buffer for holding the path.
	TCHAR strPath[MAX_PATH];
	int dirwrite = 0;
	bool result = false;

	//get the IRB application path
	pathapplication = (core::stringw)tempdevice->getFileSystem()->getWorkingDirectory();

	// Get the IRB project folder path
	// Get the special folder path. (My documents)
	SHGetSpecialFolderPath(0, strPath, CSIDL_PERSONAL, FALSE);
	docpath = (core::stringw)strPath;
	
	//Check if "mygames" exist in the user folder. If not, create the folder
	pathprojects = docpath.append(L"/my games");
	core::stringc pathproject1 = pathprojects.c_str();
	result = tempdevice->getFileSystem()->existFile(pathproject1.c_str());
	if (!result)
		dirwrite = _mkdir(pathproject1.c_str());
	
	//Check if "IRB_Projects" exits in the "mygames" folder. If not, create the folder
	pathprojects = docpath.append(L"/IRB_Projects");
	pathproject1 = pathprojects.c_str();
	result = tempdevice->getFileSystem()->existFile(pathproject1.c_str());
	if (!result)
		dirwrite = _mkdir(pathproject1.c_str());


		
	//Create a new folder in APPDATA with the IRB name (Editor configuration path)
	stringc allo=getenv("APPDATA");
	
	stringc filename = allo + "/IRB";
	result = tempdevice->getFileSystem()->existFile(filename.c_str());
	if (!result)
		dirwrite = _mkdir(filename.c_str());

	this->pathappdata = stringw(filename);


#else
	char* str = xdg_user_dir_lookup("DOCUMENTS");
	docpath = (core::stringw)str;
#endif

	pathuserdocuments = docpath;

	tempdevice->closeDevice();
	tempdevice->drop();
	
}

//Destructor
AppEditor::~AppEditor()
{
}

//Check if the path exist. Can be used to check also for files.
bool AppEditor::checkPath(stringc path)
{
	IrrlichtDevice * tempdevice = createDevice(EDT_NULL, dimension2d<u32>(640, 480), 16, false, false, false, 0);
	bool result = tempdevice->getFileSystem()->existFile(path.c_str());
	
	tempdevice->closeDevice();
	tempdevice->drop();
	return result;
}
// Load the reference configuration XML 
// And write the information back in the proper path
void AppEditor::copyConfiguration()
{

	TiXmlDocument doc("config.xml");

	if (!doc.LoadFile()) return;

	TiXmlElement* root = doc.FirstChildElement("IrrRPG_Builder_Config");
	int sw = 0;
	int sh = 0;
	stringc camera="";
	stringc fullscreen="";
	stringc resizeable = "";
	stringc vsync = "";
	stringc antialias = "";
	stringc silouette = "";
	stringc xeffect = "";

	stringc language = "";

	stringc model = "";
	stringc density = "";
	stringc mesh = "";
	stringc layer1 = "";
	stringc layer2 = "";
	stringc layer3 = "";
	stringc layer4 = "";
	stringc scale = "";

	stringc normalmap = "";
	stringc reflection = "";

	stringc mapname = "";
	stringc logo = "";

	if (root)
	{
		TiXmlNode* screenXML = root->FirstChildElement("screen");

		if (screenXML)
		{
			stringc result = "";
			sw = atoi(screenXML->ToElement()->Attribute("screen_width"));
			sh = atoi(screenXML->ToElement()->Attribute("screen_height"));

			camera = screenXML->ToElement()->Attribute("camera");
			
			fullscreen = screenXML->ToElement()->Attribute("fullscreen");
			
			resizeable = screenXML->ToElement()->Attribute("resizeable");
			
			vsync = screenXML->ToElement()->Attribute("vsync");
			
			antialias = screenXML->ToElement()->Attribute("antialias");
			
			silouette = screenXML->ToElement()->Attribute("silouette");
			
			xeffect = screenXML->ToElement()->Attribute("xeffects");
			
		}

		TiXmlNode* languageXML = root->FirstChildElement("language");

		if (languageXML)
		{
			language = stringc(languageXML->ToElement()->Attribute("type"));
		}


		TiXmlNode* terrainXML = root->FirstChildElement("terrain");

		if (terrainXML)
		{
			model = terrainXML->ToElement()->Attribute("model");

			density = terrainXML->ToElement()->Attribute("density");
			
			mesh = terrainXML->ToElement()->Attribute("mesh");
			//ebTerrainL0->setText(stringw(terrainXML->ToElement()->Attribute("layer0")).c_str());
			layer1 = terrainXML->ToElement()->Attribute("layer1");
			layer2 = terrainXML->ToElement()->Attribute("layer2");
			layer3 = terrainXML->ToElement()->Attribute("layer3");
			layer4 = terrainXML->ToElement()->Attribute("layer4");
			scale = terrainXML->ToElement()->Attribute("scale");
		}

		TiXmlNode* oceanXML = root->FirstChildElement("ocean");

		if (oceanXML)
		{
			normalmap = oceanXML->ToElement()->Attribute("normalmap");
			reflection = oceanXML->ToElement()->Attribute("reflection");
		}
	}


	//Default MapName
	TiXmlElement* mapXML = root->FirstChildElement("map");
	if (mapXML)
	{
		mapname = mapXML->ToElement()->Attribute("name");
		logo = mapXML->ToElement()->Attribute("logo");
	}
	

	// >>>>>>> Save back the data in the other path
	//TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");

	doc.LinkEndChild(decl);

	TiXmlElement* irb_cfg = new TiXmlElement("IrrRPG_Builder_Config");
	irb_cfg->SetAttribute("version", "0.3");

	TiXmlElement* screenXML = new TiXmlElement("screen");
	screenXML->SetAttribute("screen_width", sw);
	screenXML->SetAttribute("screen_height", sh);
	screenXML->SetAttribute("camera", camera.c_str());
	screenXML->SetAttribute("fullscreen", fullscreen.c_str());
	screenXML->SetAttribute("resizeable", resizeable.c_str());
	screenXML->SetAttribute("vsync", vsync.c_str());
	screenXML->SetAttribute("antialias", antialias.c_str());
	screenXML->SetAttribute("silouette", silouette.c_str());
	screenXML->SetAttribute("xeffects", xeffect.c_str());

	irb_cfg->LinkEndChild(screenXML);

	TiXmlElement* languageXML = new TiXmlElement("language");
	languageXML->SetAttribute("type", language.c_str());

	irb_cfg->LinkEndChild(languageXML);

	TiXmlElement* terrainXML = new TiXmlElement("terrain");

	terrainXML->SetAttribute("model",model.c_str());
	terrainXML->SetAttribute("density", density.c_str());
	terrainXML->SetAttribute("layer1", layer1.c_str());
	terrainXML->SetAttribute("layer2", layer2.c_str());
	terrainXML->SetAttribute("layer3", layer3.c_str());
	terrainXML->SetAttribute("layer4", layer4.c_str());
	terrainXML->SetAttribute("scale", scale.c_str());

	irb_cfg->LinkEndChild(terrainXML);

	TiXmlElement* oceanXML = new TiXmlElement("ocean");
	//oceanXML->SetAttribute("mesh",stringc(ebOceanMesh->getText()).c_str() );
	oceanXML->SetAttribute("normalmap", normalmap.c_str());
	oceanXML->SetAttribute("reflection", reflection.c_str());
	irb_cfg->LinkEndChild(oceanXML);

	mapXML = new TiXmlElement("map");
	mapXML->SetAttribute("name", mapname.c_str());
	mapXML->SetAttribute("logo", logo.c_str());
	irb_cfg->LinkEndChild(mapXML);

	doc.LinkEndChild(irb_cfg);

	core::stringc path = pathappdata.append(L"/config.xml").c_str();
	doc.SaveFile(path.c_str());

}