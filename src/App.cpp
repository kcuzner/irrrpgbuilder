#include "App.h"

#include "CameraSystem.h"
#include "EventReceiver.h"
#include "GUIManager.h"
#include "TerrainManager.h"
#include "fx/EffectsManager.h"
#include "LANGManager.h"
#include "DynamicObjectsManager.h"
#include "LuaGlobalCaller.h"
#include "SoundManager.h"
#include "Player.h"

#include "tinyXML/tinyxml.h"

#ifdef _wxWIDGET
#include <wx/wx.h>
#endif

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

const float DEG2RAD = 3.14159f/180;


App::App()
{
 wxSystem=false;
}

App::~App()
{
	this->cleanWorkspace();
	SoundManager::getInstance()->stopEngine();
    device->drop();
	exit(0);
}

void App::draw2DImages()
{
#ifdef EDITOR
    if(app_state == APP_EDIT_TERRAIN_TRANSFORM)
    {
        GUIManager::getInstance()->drawHelpImage(HELP_TERRAIN_TRANSFORM);

    }

    if(app_state == APP_EDIT_TERRAIN_PAINT_VEGETATION)
    {
        GUIManager::getInstance()->drawHelpImage(HELP_VEGETATION_PAINT);
    }

    if(app_state == APP_EDIT_TERRAIN_SEGMENTS)
    {
        GUIManager::getInstance()->drawHelpImage(HELP_TERRAIN_SEGMENTS);
    }

    if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
    {

    }

	if (app_state > APP_STATE_CONTROL)
	{
		//GUIManager::getInstance()->drawPlayerStats();
	}
    #ifdef APP_DEBUG
      //GUIManager::getInstance()->drawHelpImage(HELP_IRR_RPG_BUILDER_1);
    #endif
#endif
}

void App::drawBrush()
{
#ifdef EDITOR

	f32 radius = GUIManager::getInstance()->getScrollBarValue(SC_ID_TERRAIN_BRUSH_RADIUS);
	vector3df position = this->getMousePosition3D(100).pickedPos;
	if (position==vector3df(0,0,0))
		return;

	SMaterial m;
	m.Lighting=false;
	driver->setMaterial(m);
	driver->setTransform(video::ETS_WORLD, core::matrix4());


	// Render the size of the brush.
	f32 framesize = 3;
	int step=10;
	for (int i=0; i<(360); i=i+step)
	{
		float degInRad = i*DEG2RAD;
		vector3df pos=position;
		pos.X+=cos(degInRad)*radius;
		pos.Z+=sin(degInRad)*radius;
		pos.Y=TerrainManager::getInstance()->getHeightAt(pos)+2;

		float degInRad2 = (i+step)*DEG2RAD;
		vector3df pos2=position;
		pos2.X+=cos(degInRad2)*radius;
		pos2.Z+=sin(degInRad2)*radius;
		pos2.Y=TerrainManager::getInstance()->getHeightAt(pos2)+2;
		//driver->draw3DLine(pos,pos2,video::SColor(255,255,255,0));

		vector3df pos3=position;
		pos3.X+=cos(degInRad)*(radius+framesize);
		pos3.Z+=sin(degInRad)*(radius+framesize);
		pos3.Y=pos.Y;

		vector3df pos4=position;
		pos4.X+=cos(degInRad2)*(radius+framesize);
		pos4.Z+=sin(degInRad2)*(radius+framesize);
		pos4.Y=pos2.Y;

		driver->draw3DTriangle(triangle3df(pos4,pos3,pos),video::SColor(128,255,255,128));
		driver->draw3DTriangle(triangle3df(pos,pos2,pos4),video::SColor(128,255,255,128));

	  // printf ("Here are the coordinates %d %f,%f,%f \n",i,pos.X,pos3.X,pos.Z);
	}


	radius=5;
	step=30;
	for (int i=0; i<(360-step); i=i+step)
	{
      float degInRad = i*DEG2RAD;
	  vector3df pos=position;
	  pos.X+=cos(degInRad)*radius;
	  pos.Z+=sin(degInRad)*radius;
	  pos.Y=TerrainManager::getInstance()->getHeightAt(pos)+2;


	  float degInRad2 = (i+step)*DEG2RAD;
	  vector3df pos2=position;
	  pos2.X+=cos(degInRad2)*radius;
	  pos2.Z+=sin(degInRad2)*radius;
	  pos2.Y=TerrainManager::getInstance()->getHeightAt(pos2)+2;
	  driver->draw3DLine(pos,pos2,video::SColor(255,255,255,255));
	  //printf ("Here are the coordinates %d %f,%f,%f \n",i,pos.X,pos.Y,pos.Z);
	}

	// Center circle for the brush give the center


#endif
}

void App::displayGuiConsole()
{
	GUIManager::getInstance()->setConsoleText(L"",true);
}
///TODO: mover isso para GUIManager
// Would be nice to only check the tools windows we have opened and check their position / scale
bool App::cursorIsInEditArea()
{
    bool condition = true;
	if (GUIManager::getInstance()->isGuiPresent(device->getCursorControl()->getPosition()))
		condition = false;

    // Perhaps remove this...
	#ifndef _WXMSW
	//is over the main toolbar??
    if(device->getCursorControl()->getPosition().Y < 92 && app_state != APP_GAMEPLAY_NORMAL)  condition = false;
	#endif

	return condition;
}

APP_STATE App::getAppState()
{
    return app_state;
}

void App::setAppState(APP_STATE newAppState)
{

    //just record the state before changing..
    APP_STATE old_app_state = app_state;

    #ifdef APP_DEBUG
    cout << "NEW APP_STATE: " << app_state << endl;
    #endif

    app_state = newAppState;

	#ifdef EDITOR
    if(app_state == APP_EDIT_TERRAIN_TRANSFORM)
    {
        GUIManager::getInstance()->setWindowVisible(GCW_TERRAIN_TOOLBAR,true);
        ShaderCallBack::getInstance()->setFlagEditingTerrain(GUIManager::getInstance()->getCheckboxState(CB_ID_TERRAIN_SHOW_PLAYABLE_AREA));
        GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_TRANSFORM,false);
    }
    else
    {
        GUIManager::getInstance()->setWindowVisible(GCW_TERRAIN_TOOLBAR,false);
        ShaderCallBack::getInstance()->setFlagEditingTerrain(false);
        GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_TRANSFORM,true);
    }

    if(app_state == APP_EDIT_TERRAIN_PAINT_VEGETATION)
    {
        GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_PAINT_VEGETATION,false);
    }
    else
    {
        GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_PAINT_VEGETATION,true);
    }

    if(app_state == APP_EDIT_TERRAIN_SEGMENTS)
    {
        GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_ADD_SEGMENT,false);
    }
    else
    {
        GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_ADD_SEGMENT,true);
    }

    if(app_state == APP_EDIT_TERRAIN_TRANSFORM)
    {
        GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_TRANSFORM,false);
    }
    else
    {
        GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_TRANSFORM,true);
    }

    //if the previous state was DYNAMIC OBJECTS then we need to hide his custom windows
    if(old_app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
        GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECT_CHOOSER,false);

    if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
    {
        GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECT_CHOOSER,true);
        GUIManager::getInstance()->setElementEnabled(BT_ID_DYNAMIC_OBJECTS_MODE,false);
    }
    else
    {
        GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECT_CHOOSER,false);
        GUIManager::getInstance()->setElementEnabled(BT_ID_DYNAMIC_OBJECTS_MODE,true);
    }

    if(app_state != APP_EDIT_ABOUT)
    {
        GUIManager::getInstance()->setWindowVisible(GCW_ABOUT,false);
    }


    if(app_state == APP_EDIT_DYNAMIC_OBJECTS_SCRIPT)
    {
        GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT,true);
    }
    else
    {
        GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT,false);
    }

    if(app_state == APP_EDIT_CHARACTER)
    {
		if (!wxSystem)
		{
			GUIManager::getInstance()->setElementEnabled(BT_ID_EDIT_CHARACTER,false);
			GUIManager::getInstance()->setElementVisible(BT_ID_PLAYER_EDIT_SCRIPT,true);
		}
        Player::getInstance()->setHighLight(true);
        CameraSystem::getInstance()->setPosition(Player::getInstance()->getObject()->getPosition());
		GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);
    }
    else
    {
		if (!wxSystem)
		{
			GUIManager::getInstance()->setElementEnabled(BT_ID_EDIT_CHARACTER,true);
			GUIManager::getInstance()->setElementVisible(BT_ID_PLAYER_EDIT_SCRIPT,false);
		}
		Player::getInstance()->setHighLight(false);

    }

    if(app_state == APP_EDIT_SCRIPT_GLOBAL)
    {
		if (old_app_state == APP_EDIT_PLAYER_SCRIPT)
			Player::getInstance()->getObject()->setScript(GUIManager::getInstance()->getEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT));

        GUIManager::getInstance()->setElementEnabled(BT_ID_EDIT_SCRIPT_GLOBAL,false);
		GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT,scriptGlobal);
        GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE,"");
        GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT,true);
	}
    else
    {
        GUIManager::getInstance()->setElementEnabled(BT_ID_EDIT_SCRIPT_GLOBAL,true);
		if (old_app_state == APP_EDIT_SCRIPT_GLOBAL)
			scriptGlobal = GUIManager::getInstance()->getEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT);


    }

	if (app_state == APP_EDIT_PLAYER_SCRIPT)
	{
		if (old_app_state == APP_EDIT_SCRIPT_GLOBAL)
			scriptGlobal = GUIManager::getInstance()->getEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT);
		GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT,Player::getInstance()->getObject()->getScript());
		GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE,"");
		GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT,true);
		//setAppState(APP_EDIT_CHARACTER);
	}
	else
	{
		// Find a way to set the script once the user change the mode
		if (old_app_state == APP_EDIT_PLAYER_SCRIPT)
			Player::getInstance()->getObject()->setScript(GUIManager::getInstance()->getEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT));
	}



	#endif

    if(app_state == APP_GAMEPLAY_NORMAL)
    {
        GUIManager::getInstance()->setElementVisible(BT_ID_PLAY_GAME,false);
        GUIManager::getInstance()->setElementVisible(BT_ID_STOP_GAME,true);
        GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_ADD_SEGMENT,false);
        GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_PAINT_VEGETATION,false);
        GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_TRANSFORM,false);
        GUIManager::getInstance()->setElementEnabled(BT_ID_DYNAMIC_OBJECTS_MODE,false);
        GUIManager::getInstance()->setElementEnabled(BT_ID_SAVE_PROJECT,false);
        GUIManager::getInstance()->setElementEnabled(BT_ID_LOAD_PROJECT,false);
        GUIManager::getInstance()->setElementEnabled(BT_ID_EDIT_CHARACTER,false);
        GUIManager::getInstance()->setElementEnabled(BT_ID_EDIT_SCRIPT_GLOBAL,false);
        GUIManager::getInstance()->setElementEnabled(BT_ID_ABOUT,false);
        GUIManager::getInstance()->setElementEnabled(BT_ID_NEW_PROJECT,false);
        GUIManager::getInstance()->setElementEnabled(BT_ID_HELP,false);

        GUIManager::getInstance()->setElementVisible(BT_ID_VIEW_ITEMS,true);
    }
    else
    {
        GUIManager::getInstance()->setElementVisible(BT_ID_PLAY_GAME,true);
        GUIManager::getInstance()->setElementVisible(BT_ID_STOP_GAME,false);
        GUIManager::getInstance()->setElementEnabled(BT_ID_SAVE_PROJECT,true);
        GUIManager::getInstance()->setElementEnabled(BT_ID_LOAD_PROJECT,true);
        GUIManager::getInstance()->setElementEnabled(BT_ID_ABOUT,true);
        GUIManager::getInstance()->setElementEnabled(BT_ID_NEW_PROJECT,true);
        GUIManager::getInstance()->setElementEnabled(BT_ID_HELP,true);

        GUIManager::getInstance()->setElementVisible(BT_ID_VIEW_ITEMS,false);
    }
}

void App::eventGuiButton(s32 id)
{
	#ifdef EDITOR

    DynamicObject* selectedObject;

	#endif

	oldcampos = vector3df(0,0,0);
    switch (id)
    {

	#ifdef EDITOR

		case BT_ID_NEW_PROJECT:
            this->createNewProject();
            break;
        case BT_ID_LOAD_PROJECT:
            this->loadProject();
            break;
        case BT_ID_SAVE_PROJECT:
            this->saveProject();
            break;
        case BT_ID_TERRAIN_ADD_SEGMENT:
            this->setAppState(APP_EDIT_TERRAIN_SEGMENTS);
            break;
        case BT_ID_TERRAIN_PAINT_VEGETATION:
            this->setAppState(APP_EDIT_TERRAIN_PAINT_VEGETATION);
            break;
        case BT_ID_TERRAIN_TRANSFORM:
            this->setAppState(APP_EDIT_TERRAIN_TRANSFORM);
            break;
        case BT_ID_DYNAMIC_OBJECTS_MODE:
            this->setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
            break;
        case BT_ID_DYNAMIC_OBJECT_BT_CANCEL:
			GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);
            this->setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
            break;
        case BT_ID_DYNAMIC_OBJECT_BT_EDITSCRIPTS:
            selectedObject = DynamicObjectsManager::getInstance()->getObjectByName( stringc(lastMousePick.pickedNode->getName()) );

            GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);

            GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT,selectedObject->getScript());
            GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE,"");

            setAppState(APP_EDIT_DYNAMIC_OBJECTS_SCRIPT);
            break;
        case BT_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE:
            if(GUIManager::getInstance()->showDialogQuestion(stringc(LANGManager::getInstance()->getText("msg_override_script")).c_str()))
            {
                stringc newScript = "";

                stringc filename = "../media/scripts/";
                filename += GUIManager::getInstance()->getComboBoxItem(CO_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE);

                std::string line;
                ifstream fileScript (filename.c_str());
                if (fileScript.is_open())
                {
                    while (! fileScript.eof() )
                    {
                        getline (fileScript,line);
                        newScript += line.c_str();
                        newScript += '\n';
                    }
                    fileScript.close();
                }

                GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT,newScript);
            }
            break;
        case BT_ID_DYNAMIC_OBJECT_BT_REMOVE:
            GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);
            DynamicObjectsManager::getInstance()->removeObject(lastMousePick.pickedNode->getName());
            setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
            break;
        case BT_ID_DYNAMIC_OBJECT_BT_MOVEROTATE:
            GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);
            setAppState(APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE);
            break;

        case BT_ID_DYNAMIC_OBJECT_VALIDATE_SCRIPT:
            LuaGlobalCaller::getInstance()->doScript(GUIManager::getInstance()->getEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT));
            break;

        case BT_ID_DYNAMIC_OBJECT_SCRIPT_CLOSE:
            if(app_state == APP_EDIT_DYNAMIC_OBJECTS_SCRIPT)
            {
                DynamicObjectsManager::getInstance()->getObjectByName(lastMousePick.pickedNode->getName())->setScript(GUIManager::getInstance()->getEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT));
                setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
            }
            else if(app_state == APP_EDIT_PLAYER_SCRIPT)
            {
                Player::getInstance()->getObject()->setScript(GUIManager::getInstance()->getEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT));
				setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
            }
            else if(app_state == APP_EDIT_SCRIPT_GLOBAL)
            {
                scriptGlobal = GUIManager::getInstance()->getEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT);
                setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
            }
            GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT,false);
            break;
		case BT_ID_EDIT_CHARACTER:
            this->setAppState(APP_EDIT_CHARACTER);
            break;
        case BT_ID_PLAYER_EDIT_SCRIPT:
			this->setAppState(APP_EDIT_PLAYER_SCRIPT);
            break;
		case BT_ID_EDIT_SCRIPT_GLOBAL:
			this->setAppState(APP_EDIT_SCRIPT_GLOBAL);
            break;
		case BT_ID_CONFIG:
            GUIManager::getInstance()->showConfigWindow();
            break;

		#endif

        case BT_ID_PLAY_GAME:
			playGame();
            break;
        case BT_ID_STOP_GAME:
			stopGame();
            break;

        case BT_ID_CLOSE_PROGRAM:
			this->cleanWorkspace();
			SoundManager::getInstance()->stopEngine();
            device->drop();
		    exit(0);
            break;

		case BT_ID_HELP:
			this->displayGuiConsole();
			break;
        case BT_ID_ABOUT:
            GUIManager::getInstance()->setWindowVisible(GCW_ABOUT,true);
            setAppState(APP_EDIT_ABOUT);
            break;
        case BT_ID_ABOUT_WINDOW_CLOSE:
            GUIManager::getInstance()->setWindowVisible(GCW_ABOUT,false);
            setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
            break;
        case BT_ID_VIEW_ITEMS:
            setAppState(APP_GAMEPLAY_VIEW_ITEMS);
            GUIManager::getInstance()->setWindowVisible(GCW_GAMEPLAY_ITEMS,true);
			GUIManager::getInstance()->drawPlayerStats();
            break;
        case BT_ID_USE_ITEM:
            LuaGlobalCaller::getInstance()->usePlayerItem(GUIManager::getInstance()->getActivePlayerItem());
            GUIManager::getInstance()->updateItemsList();
            break;
        case BT_ID_DROP_ITEM:
			Player::getInstance()->getObject()->removeItem(GUIManager::getInstance()->getActivePlayerItem());
            GUIManager::getInstance()->updateItemsList();
            break;

        case BT_ID_CLOSE_ITEMS_WINDOW:
            setAppState(APP_GAMEPLAY_NORMAL);
            GUIManager::getInstance()->setWindowVisible(GCW_GAMEPLAY_ITEMS,false);
            break;

    }
}

// Stuff in editor only
#ifdef EDITOR

void App::hideEditGui()
{
	wxSystem=true;
	GUIManager::getInstance()->setConsoleText(L"Ready now for WXwidget!",false);
	//GUIManager::getInstance()->setElementVisible(BT_ID_WXEditor,false);
}

void App::eventGuiCheckbox(s32 id)
{
    switch (id)
    {
        case CB_ID_TERRAIN_SHOW_PLAYABLE_AREA:
            ShaderCallBack::getInstance()->setFlagEditingTerrain(GUIManager::getInstance()->getCheckboxState(CB_ID_TERRAIN_SHOW_PLAYABLE_AREA));
            break;
    }
}

void App::eventGuiCombobox(s32 id)
{
    switch (id)
    {
        case CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER:
            DynamicObjectsManager::getInstance()->setActiveObject(GUIManager::getInstance()->getComboBoxItem(CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER));
            GUIManager::getInstance()->updateDynamicObjectPreview();
            break;
    }
}

#endif

void App::setScreenSize(dimension2d<u32> size)
{
	if (device->run())
	{
		GUIManager::getInstance()->updateGuiPositions(size);
		screensize = size;
		stringw text = L"Current screen size is:";
		text.append((stringw)screensize.Width);
		text.append(L",");
		text.append((stringw)screensize.Height);
		// Correct the aspect ratio of the camera when the screen is changed.
#ifndef _WXMSW
		CameraSystem::getInstance()->fixRatio(driver);
#endif
		GUIManager::getInstance()->setConsoleText(text.c_str(),false);
	}
}

dimension2d<u32> App::getScreenSize()
{
	return screensize;
}

void App::eventKeyPressed(s32 key)
{
    switch (key)
    {
        case KEY_LEFT:
            break;
        case KEY_UP:
            break;
        case KEY_RIGHT:
            break;
        case KEY_DOWN:
            break;

        case KEY_F5:
            if(app_state == APP_EDIT_DYNAMIC_OBJECTS_SCRIPT && !EventReceiver::getInstance()->isKeyPressed(key))
                LuaGlobalCaller::getInstance()->doScript(GUIManager::getInstance()->getEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT));
            break;

        case KEY_ESCAPE:
            //device->drop();
            break;
    }
}

void App::eventMousePressed(s32 mouse)
{
    //printf("%d",mouse);

    switch(mouse)
    {///TODO: colocar acoes mais comuns acima e menos comuns nos elses
        case 0://LB
            if( cursorIsInEditArea())
            {
                if(app_state == APP_EDIT_TERRAIN_SEGMENTS)
                {
					TerrainManager::getInstance()->createSegment(this->getMousePosition3D().pickedPos / TerrainManager::getInstance()->getScale());
                }
                else if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
                {
                    MousePick mousePick = getMousePosition3D();

					lastMousePick = mousePick;
					stringc nodeName = "";
					// Check for a node to prevent a crash (need to get the name of the node)
					if (mousePick.pickedNode != NULL)
					{
						nodeName = mousePick.pickedNode->getName();

						//if you click on a Dynamic Object then open his properties
						if( stringc( nodeName.subString(0,14)) == "dynamic_object" )
						{
							cout << "PROP:" << nodeName.c_str() << endl;

							GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,true);
							App::getInstance()->setAppState(APP_EDIT_WAIT_GUI);
						}
						else//create a new copy of active dynamic object at the clicked position
						{
							DynamicObject* tmpDObj = DynamicObjectsManager::getInstance()->createActiveObjectAt(mousePick.pickedPos);

							#ifdef APP_DEBUG
							cout << "DEBUG : DYNAMIC_OBJECTS : NEW " << tmpDObj->getName().c_str() << " CREATED!"  << endl;
							#endif
						}
					}
                }
                else if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE)
                {
                    setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
                }
            }
            break;
        case 3:
            break;
    }
}

void App::eventMouseWheel(f32 value)
{
    if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE)
    {
        vector3df oldRot = lastMousePick.pickedNode->getRotation();
        lastMousePick.pickedNode->setRotation(vector3df(0,value*10,0)+oldRot);
    }
	if(app_state == APP_EDIT_CHARACTER)
	{
		vector3df oldRot = Player::getInstance()->getObject()->getRotation();
		Player::getInstance()->getObject()->setRotation(vector3df(0,value*10,0)+oldRot);
	}
	// This will allow zoom in/out in editor mode
	if	(app_state != APP_EDIT_CHARACTER &&
		app_state != APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE &&
		app_state != APP_EDIT_ABOUT &&
		app_state != APP_EDIT_DYNAMIC_OBJECTS_SCRIPT &&
		app_state != APP_EDIT_SCRIPT_GLOBAL &&
		app_state != APP_EDIT_PLAYER_SCRIPT &&
		cursorIsInEditArea())
    {
		if (app_state < 100)
		{
			f32 height = CameraSystem::getInstance()->getPosition().Y;
			CameraSystem::getInstance()->setCameraHeight(value * (height*0.25f));
		}
		else
			CameraSystem::getInstance()->setCameraHeight(value * 50);
	}
}

App* App::getInstance()
{
    static App *instance = 0;
    if (!instance) instance = new App();
    return instance;
}

MousePick App::getMousePosition3D(int id)
{
    position2d<s32> pos=device->getCursorControl()->getPosition();
#ifdef _wxWIDGET
	// Fix to a proper position on wxWidget;
	pos=pos+position2d<s32>(0,22);
#endif

    line3df ray = smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(pos, smgr->getActiveCamera());

    core::vector3df intersection;
    core::triangle3df hitTriangle;

    ISceneNode* tempNode = smgr->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray,
                                                                                                  intersection,
                                                                                                  hitTriangle,
                                                                                                  id);
    MousePick result;

    if(tempNode!=NULL)
    {
        result.pickedPos = intersection;
        result.pickedNode = tempNode;

        return result;
    }
    else
    {
        result.pickedPos = vector3df(0,0,0);
        result.pickedNode = NULL;

        return result;
    }
}

bool App::loadConfig()
{
#ifndef _WXMSW
	screensize.Height = 768;
	screensize.Width = 1024;

# else

//	if (device->run())
//	{
//		screensize = device->getVideoDriver()->getScreenSize();
//	}
	// for some reasons IRRlicht open in 20,20 when used inside a wxWindow
	if (screensize.Height<100)
	{
		screensize.Width = 1008;
		screensize.Height = 596;
	}
#endif
	fullScreen = false;
	resizable = false;
	language = "en-us";
	TerrainManager::getInstance()->setTileMeshName("../media/baseTerrain.obj");
	TerrainManager::getInstance()->setTerrainTexture(1,"../media/L1.jpg");
	TerrainManager::getInstance()->setTerrainTexture(2,"../media/L2.jpg");
	TerrainManager::getInstance()->setTerrainTexture(3,"../media/L3.jpg");
	TerrainManager::getInstance()->setTerrainTexture(4,"../media/L4.jpg");
// Define a default mapname only for the player application
#ifndef EDITOR
	mapname="";
#endif

#ifdef EDITOR
	TiXmlDocument doc("config.xml");
#else
	TiXmlDocument doc("gameconfig.xml");
	printf("Loaded gameconfig for the player app\n");
#endif

	if (!doc.LoadFile()) return false; ///TODO: create the config default file if does not exist

    #ifdef APP_DEBUG
    cout << "DEBUG : XML : LOADING CONFIGURATION : " << endl;
    #endif

    TiXmlElement* root = doc.FirstChildElement( "IrrRPG_Builder_Config" );

    if ( root )
	{
        if( atof(root->Attribute("version"))!=APP_VERSION )
        {
            #ifdef APP_DEBUG
            cout << "DEBUG : XML : INCORRECT VERSION!" << endl;
            #endif

            return false;
        }

        TiXmlElement* resXML = root->FirstChildElement( "screen" );
        if ( resXML )
        {
#ifndef _WXMSW
			screensize.Width = atoi(resXML->ToElement()->Attribute("screen_width"));
			screensize.Height = atoi(resXML->ToElement()->Attribute("screen_height"));
#endif
			stringc full = resXML->ToElement()->Attribute("fullscreen");
			if (full=="true")
				fullScreen=true;
			stringc resize = resXML->ToElement()->Attribute("resizeable");
			if (resize=="true")
				resizable=true;
#ifndef _WXMSW
			if (resizable && fullScreen)
			{
				IrrlichtDevice * tempdevice = createDevice(EDT_NULL,dimension2d<u32>(640,480), 16, false, false, false, 0);
				screensize = tempdevice->getVideoModeList()->getDesktopResolution();
				tempdevice->closeDevice();
			}
#endif
        }
		//Language
		TiXmlElement* langXML = root->FirstChildElement( "language" );
        if ( resXML )
        {
			language=stringc(langXML->ToElement()->Attribute("type")).c_str();

        }
		TiXmlElement* groundXML = root->FirstChildElement( "terrain" );
        if ( groundXML )
        {
            stringc meshname = groundXML->ToElement()->Attribute("mesh");
			TerrainManager::getInstance()->setTileMeshName(meshname);
			stringc layer0 = groundXML->ToElement()->Attribute("layer0");
			stringc layer1 = groundXML->ToElement()->Attribute("layer1");
			stringc layer2 = groundXML->ToElement()->Attribute("layer2");
			stringc layer3 = groundXML->ToElement()->Attribute("layer3");
			f32 scale = (f32)atof(groundXML->ToElement()->Attribute("scale"));
			TerrainManager::getInstance()->setTerrainTexture(1,layer0);
			TerrainManager::getInstance()->setTerrainTexture(2,layer1);
			TerrainManager::getInstance()->setTerrainTexture(3,layer2);
			TerrainManager::getInstance()->setTerrainTexture(4,layer3);
			TerrainManager::getInstance()->setScale(scale);

        }
		TiXmlElement* waterXML = root->FirstChildElement( "ocean" );
        if ( waterXML )
        {
            stringc meshname = waterXML->ToElement()->Attribute("mesh");
			stringc normalmap = waterXML->ToElement()->Attribute("normalmap");
			stringc reflection = waterXML->ToElement()->Attribute("reflection");
            ///TODO: we are just loading ocean seetings, we need to set it!
		}
// Player app. Load the default map from "gameconfig.xml"
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
    else
    {
        #ifdef APP_DEBUG
        cout << "DEBUG : XML : THIS FILE IS NOT A IRRRPG BUILDER PROJECT!" << endl;
        #endif

        return false;
    }

    #ifdef APP_DEBUG
    cout << "DEBUG : XML : PROJECT LOADED! "<< endl;
    #endif

    ///TODO:CLEAR PROJECT IF NOT RETURN TRUE ON LOAD PROJECT FROM XML

    return true;
}

void App::setupDevice(IrrlichtDevice* IRRdevice)
{

	loadConfig();

	if (!IRRdevice)
	{
		device = createDevice(EDT_OPENGL, screensize, 32, fullScreen, false, false, 0);
		this->device->setResizable(resizable);
		device->setWindowCaption(L"IrrRPG Builder - Alpha release 0.2 (apr 2011)");
	} else
		device = IRRdevice;

    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();

    device->setEventReceiver(EventReceiver::getInstance());
	timer = device->getTimer()->getRealTime();
	timer2 = device->getTimer()->getRealTime();
	LANGManager::getInstance()->setDefaultLanguage(language);
	quickUpdate();

}

IrrlichtDevice* App::getDevice()
{
    if(!device)
    {
        printf("ERROR: Device is NULL, please call SetupDevice first!");
        exit(0);
    }
    return device;
}


void App::playGame()
{
	if (app_state<APP_STATE_CONTROL)
	{
		oldcampos = Player::getInstance()->getObject()->getPosition();
		CameraSystem::getInstance()->setCamera(1);
		// setback the fog as before (will need to check with LUA)
		driver->setFog(SColor(0,255,255,255),EFT_FOG_LINEAR,300,9100);
		this->setAppState(APP_GAMEPLAY_NORMAL);
		//Player::getInstance()->getObject()->doScript();
		LuaGlobalCaller::getInstance()->storeGlobalParams();
		DynamicObjectsManager::getInstance()->initializeAllScripts();
		DynamicObjectsManager::getInstance()->showDebugData(false);
		DynamicObjectsManager::getInstance()->startCollisions();
		//DynamicObjectsManager::getInstance()->initializeCollisions();
		TerrainManager::getInstance()->showDebugData(false);
		GUIManager::getInstance()->setElementVisible(ST_ID_PLAYER_LIFE,true);
		LuaGlobalCaller::getInstance()->doScript(scriptGlobal);
	}
}

void App::stopGame()
{
	if (app_state>APP_STATE_CONTROL)
	{
		DynamicObjectsManager::getInstance()->clearAllScripts();
		DynamicObjectsManager::getInstance()->clearCollisions();
		DynamicObjectsManager::getInstance()->showDebugData(true);
		DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(false);
		TerrainManager::getInstance()->showDebugData(true);
		LuaGlobalCaller::getInstance()->restoreGlobalParams();
		SoundManager::getInstance()->stopSounds();
		GUIManager::getInstance()->setElementVisible(ST_ID_PLAYER_LIFE,false);
		GlobalMap::getInstance()->clearGlobals();
		this->setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
		CameraSystem::getInstance()->setCamera(2);
		CameraSystem::getInstance()->setPosition(vector3df(oldcampos));
		driver->setFog(SColor(0,255,255,255),EFT_FOG_LINEAR,300,999100);
	}
}

void App::update()
{
	//while (app_state<APP_STATE_CONTROL)
	{
		// Attempt to do automatic rezise detection
		if (screensize != driver->getScreenSize())
			this->setScreenSize(driver->getScreenSize());

		driver->beginScene(true, true, SColor(0,200,200,200));
		if(app_state < APP_STATE_CONTROL)
		{

			device->yield();

			// This is needed for wxWidget event management
#ifdef _wxWIDGET
			wxYield();
#endif
#ifdef EDITOR
			updateEditMode();//editMode
#endif
		}
		else
		{
			// This is needed for wxWidget event management
#ifdef _wxWIDGET
			wxYield();
#endif

    		updateGameplay();
		}


		// Prepare the RTT for the postFX
		EffectsManager::getInstance()->preparePostFX(false);

		smgr->drawAll();

		// Tries to do an post FX
		EffectsManager::getInstance()->update();

		// bring back the gui after the RTT is done
		video::SMaterial mat;
        driver->setMaterial(mat);

		guienv->drawAll();
		//draw2DImages();

		driver->endScene();
	}
}

void App::quickUpdate()
{
	driver->beginScene(true, true, SColor(0,200,200,200));
	smgr->drawAll();

	guienv->drawAll();
	driver->endScene();
}

void App::run()
{
// Set the proper state if in the EDITOR or only the player application
#ifdef EDITOR
    //this->setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
#else
	//this->setAppState(APP_EDIT_WAIT_GUI);
	this->loadProjectFromXML(mapname);
	//oldcampos = Player::getInstance()->getObject()->getPosition();
	CameraSystem::getInstance()->setCamera(1);
    this->setAppState(APP_GAMEPLAY_NORMAL);
    //Player::getInstance()->getObject()->doScript();
    LuaGlobalCaller::getInstance()->storeGlobalParams();
    DynamicObjectsManager::getInstance()->initializeAllScripts();
    DynamicObjectsManager::getInstance()->showDebugData(false);
	DynamicObjectsManager::getInstance()->startCollisions();
    //DynamicObjectsManager::getInstance()->initializeCollisions();
    TerrainManager::getInstance()->showDebugData(false);
    GUIManager::getInstance()->setElementVisible(ST_ID_PLAYER_LIFE,true);
    LuaGlobalCaller::getInstance()->doScript(scriptGlobal);

#endif
	GUIManager::getInstance()->guiLoaderWindow->setVisible(false);

    int lastFPS = -1;
//	u32 timer = device->getTimer()->getRealTime();
//	u32 timer2 = device->getTimer()->getRealTime();
	bool activated=false;

    while(device->run())
	//while(app_state>99)
    {


		this->update();

        // display frames per second in window title
		int fps = driver->getFPS();
		if (lastFPS != fps)
		{
			core::stringw str = L"IrrRPG Builder - Alpha release 0.2 (apr 2011)";
			str += " FPS:";
			str += fps;

			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
    }

}


// Stuff needed only in the editor
#ifdef EDITOR

void App::updateEditMode()
{
	timer = device->getTimer()->getRealTime();
	if(app_state == APP_EDIT_TERRAIN_TRANSFORM && cursorIsInEditArea() )
		this->drawBrush();

	if ((timer-timer2)>17) // 1/60th second refresh interval
	{
		timer2 = device->getTimer()->getRealTime();
		if(app_state < APP_STATE_CONTROL)
		{
			// --- Drag the view when the spacebar is pressed
			if (app_state != APP_EDIT_DYNAMIC_OBJECTS_SCRIPT &&
				app_state != APP_EDIT_WAIT_GUI &&
				app_state != APP_EDIT_PLAYER_SCRIPT &&
				app_state != APP_EDIT_SCRIPT_GLOBAL &&
				app_state != APP_EDIT_CHARACTER &&
				app_state != APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE)
			{
				if(EventReceiver::getInstance()->isKeyPressed(KEY_SPACE))
				{
					if (app_state != APP_EDIT_VIEWDRAG)
						old_state = app_state;

					app_state = APP_EDIT_VIEWDRAG;
					if(EventReceiver::getInstance()->isMousePressed(0))
					{// TODO: Move the cam based on the cursor position. Current method is buggy.
						vector3df camPosition = this->getMousePosition3D(100).pickedPos;
						CameraSystem::getInstance()->setPosition(camPosition);
					}

					return;
				}
			}
			// Return the edit mode to normal after the spacebar is pressed (viewdrag)
			if (app_state == APP_EDIT_VIEWDRAG)
				app_state = old_state;
			// --- End of code for drag of view

			if(app_state == APP_EDIT_TERRAIN_TRANSFORM && cursorIsInEditArea() )
			{
				if(EventReceiver::getInstance()->isKeyPressed(KEY_LCONTROL))
				{
					if(EventReceiver::getInstance()->isMousePressed(0))
					{
						TerrainManager::getInstance()->transformSegmentsToValue(this->getMousePosition3D(100),
                                                                           GUIManager::getInstance()->getScrollBarValue(SC_ID_TERRAIN_BRUSH_RADIUS),
                                                                           GUIManager::getInstance()->getScrollBarValue(SC_ID_TERRAIN_BRUSH_STRENGTH)*0.0005f,
																		   GUIManager::getInstance()->getScrollBarValue(SC_ID_TERRAIN_BRUSH_PLATEAU));
					}
				}
				else
				{
					if(EventReceiver::getInstance()->isMousePressed(0))
					{
						TerrainManager::getInstance()->transformSegments(this->getMousePosition3D(100),
                                                                     GUIManager::getInstance()->getScrollBarValue(SC_ID_TERRAIN_BRUSH_RADIUS),
																	 GUIManager::getInstance()->getScrollBarValue(SC_ID_TERRAIN_BRUSH_STRENGTH)*0.0005f);
					}
					else if(EventReceiver::getInstance()->isMousePressed(1) )
					{
						TerrainManager::getInstance()->transformSegments(this->getMousePosition3D(100),
                                                                     GUIManager::getInstance()->getScrollBarValue(SC_ID_TERRAIN_BRUSH_RADIUS),
                                                                     -GUIManager::getInstance()->getScrollBarValue(SC_ID_TERRAIN_BRUSH_STRENGTH)*0.0005f);
					}
				}
			}


			if(app_state == APP_EDIT_TERRAIN_PAINT_VEGETATION && cursorIsInEditArea())
			{
				//Add vegetation to the terrain
				if(EventReceiver::getInstance()->isMousePressed(0))
				{
					TerrainManager::getInstance()->paintVegetation(this->getMousePosition3D(100), false);
				}
				//Erase vegetation from the terrain
				if(EventReceiver::getInstance()->isMousePressed(1))
				{
					TerrainManager::getInstance()->paintVegetation(this->getMousePosition3D(100), true);
				}
			}

			if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE && cursorIsInEditArea())
			{
				lastMousePick.pickedNode->setPosition(getMousePosition3D(100).pickedPos);
			}

			if(app_state == APP_EDIT_CHARACTER)
			{
				if(EventReceiver::getInstance()->isMousePressed(0) && cursorIsInEditArea())
					Player::getInstance()->getObject()->setPosition(getMousePosition3D(100).pickedPos);
			}


			if(app_state == APP_EDIT_TERRAIN_SEGMENTS ||
			app_state == APP_EDIT_TERRAIN_TRANSFORM ||
			app_state == APP_EDIT_TERRAIN_PAINT_VEGETATION||
			app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE||
			app_state == APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE||
			app_state == APP_EDIT_CHARACTER)
			{

				//Update Editor Camera Position
				if(EventReceiver::getInstance()->isKeyPressed(KEY_LEFT))
				{
					CameraSystem::getInstance()->moveCamera(vector3df(-10.0f,0,0));
				}
				else if (EventReceiver::getInstance()->isKeyPressed(KEY_RIGHT))
				{
					CameraSystem::getInstance()->moveCamera(vector3df(10.0f,0,0));
				}
				if(EventReceiver::getInstance()->isKeyPressed(KEY_UP))
				{
					CameraSystem::getInstance()->moveCamera(vector3df(0,0,10.0f));
				}
				else if (EventReceiver::getInstance()->isKeyPressed(KEY_DOWN))
				{
					CameraSystem::getInstance()->moveCamera(vector3df(0,0,-10.0f));
				}
			}
		}
	}
}

#endif


void App::updateGameplay()
{

	timer = device->getTimer()->getRealTime();
	//DynamicObjectsManager::getInstance()->updateAnimators();
	DynamicObjectsManager::getInstance()->updateAll(); // This one should be timed now.

	// the update for the player. It's a dynamic object now.
	Player::getInstance()->update(); // This one is timed now.
	CameraSystem::getInstance()->setPosition(Player::getInstance()->getObject()->getPosition());

	// This update the player events and controls at specific time intervals
    if ((timer-timer2)>0) // (17 )1/60 second
	{
		timer2 = device->getTimer()->getRealTime();

		if(EventReceiver::getInstance()->isMousePressed(0) && cursorIsInEditArea() && app_state == APP_GAMEPLAY_NORMAL)
		{
			MousePick mousePick = getMousePosition3D();
			stringc nodeName = "";
			// Check for a node to prevent a crash (need to get the name of the node)
			if (mousePick.pickedNode != NULL)
			{
				stringc nodeName = mousePick.pickedNode->getName();

				//if you click on a Dynamic Object...
				if( stringc( nodeName.subString(0,14)) == "dynamic_object" )
				{
					DynamicObject* obj = DynamicObjectsManager::getInstance()->getObjectByName(nodeName);

					// TODO: Need to get more accuracy for the distance hardcoded value is not ideal

					if(obj->getObjectType() == stringc("ENEMY"))
					{
						Player::getInstance()->getObject()->attackEnemy(obj);
					}
					else
					{
						if(obj->getDistanceFrom(Player::getInstance()->getObject()->getPosition()) < 72.0f)
							obj->notifyClick();
						Player::getInstance()->getObject()->clearEnemy();
					}
					DynamicObjectsManager::getInstance()->getTarget()->setPosition(obj->getPosition()+vector3df(0,0.1f,0));
					DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(true);

					Player::getInstance()->getObject()->setWalkTarget(obj->getPosition());
					Player::getInstance()->getObject()->lookAt(obj->getPosition());
					Player::getInstance()->setTaggedTarget(obj);
					return;

				}
				else
				{
					mousePick = getMousePosition3D(100);
					Player::getInstance()->getObject()->setWalkTarget(mousePick.pickedPos);
					DynamicObjectsManager::getInstance()->getTarget()->setPosition(mousePick.pickedPos+vector3df(0,0.1f,0));
					DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(true);
					Player::getInstance()->setTaggedTarget(NULL);
					//Player::getInstance()->getObject()->clearEnemy();
					return;
				}
			}
		}
	}
}

void App::cleanWorkspace()
{
    CameraSystem::getInstance()->setPosition(vector3df(0,0,0));

    TerrainManager::getInstance()->clean();

    DynamicObjectsManager::getInstance()->clean(false);

    scriptGlobal="";
}

void App::createNewProject()
{
    APP_STATE old_state = getAppState();
    setAppState(APP_EDIT_WAIT_GUI);
#ifndef _WXMSW
    stringc name = GUIManager::getInstance()->showInputQuestion(stringc(LANGManager::getInstance()->getText("msg_new_project_name")).c_str());
    GUIManager::getInstance()->flush();

    while(name == stringc(""))
    {
        name = GUIManager::getInstance()->showInputQuestion(stringc(LANGManager::getInstance()->getText("msg_new_project_name")).c_str());
        GUIManager::getInstance()->flush();
    }

    name += ".XML";

    stringc filename = "../projects/";
    filename += name;
#else
	stringc name="irb_temp_project.XML";
#endif

    this->cleanWorkspace();

    CameraSystem::getInstance();

    TerrainManager::getInstance()->createSegment(vector3df(0,0,0));

    //smgr->setAmbientLight(SColorf(0.5,0.5,0.5,0.5));
    //driver->setFog(SColor(255,255,255,255),EFT_FOG_LINEAR,0,12000);

    Player::getInstance();

    this->currentProjectName = name;

    //this->saveProject();

    setAppState(old_state);
}

void App::loadProject()
{
    APP_STATE old_state = getAppState();
    setAppState(APP_EDIT_WAIT_GUI);

    bool ansSave = GUIManager::getInstance()->showDialogQuestion(stringc(LANGManager::getInstance()->getText("msg_override_project")).c_str());
    GUIManager::getInstance()->flush();
    if(ansSave)
    {
		stringc filename = "../projects/";
		filename += currentProjectName;
        saveProjectToXML(filename);
        GUIManager::getInstance()->showDialogQuestion(stringc(LANGManager::getInstance()->getText("msg_saved_ok")).c_str());
        GUIManager::getInstance()->flush();
    }

    stringc name = GUIManager::getInstance()->showInputQuestion(stringc(LANGManager::getInstance()->getText("msg_new_project_name")).c_str());
    GUIManager::getInstance()->flush();

    stringc filename = "../projects/";
    filename += name;
    filename += ".XML";

    currentProjectName = name;
    currentProjectName += ".XML";

    if(this->loadProjectFromXML(filename))
        GUIManager::getInstance()->showDialogMessage(stringc(LANGManager::getInstance()->getText("msg_loaded_ok")).c_str());
    else
        GUIManager::getInstance()->showDialogMessage(stringc(LANGManager::getInstance()->getText("msg_loaded_error")).c_str());

    GUIManager::getInstance()->flush();

    //this->loadProject("");
    setAppState(old_state);
}

/*
void App::loadProject(stringc filename)
{
    this->cleanWorkspace();
    if(!this->loadProjectFromXML("../projects/myProjectTiny.xml")) this->createNewProject("temp_project");
}
*/

void App::saveProject()
{
    APP_STATE old_state = getAppState();
    setAppState(APP_EDIT_WAIT_GUI);

    if(currentProjectName == stringc("irb_temp_project"))
    {
        currentProjectName = GUIManager::getInstance()->showInputQuestion(stringc(LANGManager::getInstance()->getText("msg_new_project_name")).c_str());
        GUIManager::getInstance()->flush();
        EventReceiver::getInstance()->flushKeys();
        currentProjectName += ".XML";
    }

	stringc filename = "../projects/";
    filename += currentProjectName;
    this->saveProjectToXML(filename);
    GUIManager::getInstance()->showDialogMessage(stringc(LANGManager::getInstance()->getText("msg_saved_ok")).c_str());
    GUIManager::getInstance()->flush();

    setAppState(old_state);
}

stringc App::getProjectName()
{
	return this->currentProjectName;
}

void App::saveProjectToXML(stringc filename)
{

    TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );

	TiXmlElement* irb_project = new TiXmlElement( "IrrRPG_Builder_Project" );
	irb_project->SetAttribute("version","1.0");

    TerrainManager::getInstance()->saveToXML(irb_project);
    DynamicObjectsManager::getInstance()->saveToXML(irb_project);
    Player::getInstance()->getObject()->saveToXML(irb_project);

    TiXmlElement* globalScript = new TiXmlElement("global_script");
    globalScript->SetAttribute("script",scriptGlobal.c_str());
    irb_project->LinkEndChild(globalScript);

	doc.LinkEndChild( decl );
	doc.LinkEndChild( irb_project );
	doc.SaveFile( filename.c_str() );


    #ifdef APP_DEBUG
    cout << "DEBUG : XML : PROJECT SAVED : " << filename.c_str() << endl;
    #endif
}

bool App::loadProjectFromXML(stringc filename)
{
	GUIManager::getInstance()->guiLoaderWindow->setVisible(true);
	printf ("Trying to load this map: %s \n",filename.c_str());
    TiXmlDocument doc(filename.c_str());
	if (!doc.LoadFile()) return false;

    #ifdef APP_DEBUG
    cout << "DEBUG : XML : LOADING PROJECT : " << filename.c_str() << endl;
    #endif

    TiXmlElement* root = doc.FirstChildElement( "IrrRPG_Builder_Project" );

    if ( root )
    {
        if( atof(root->Attribute("version"))!=APP_VERSION )
        {
            #ifdef APP_DEBUG
            cout << "DEBUG : XML : INCORRECT VERSION!" << endl;
            #endif

            return false;
        }

        TiXmlElement* globalScriptXML = root->FirstChildElement( "global_script" );
        if ( globalScriptXML )
        {
			GUIManager::getInstance()->setTextLoader(L"Loading the scripts");
			quickUpdate();
            scriptGlobal = globalScriptXML->ToElement()->Attribute("script");
        }

        TiXmlElement* terrain = root->FirstChildElement( "terrain" );
        if ( terrain )
        {
			GUIManager::getInstance()->setTextLoader(L"Loading the terrain");
			quickUpdate();
            TerrainManager::getInstance()->loadFromXML(terrain);
        }

        TiXmlElement* dynamicObjs = root->FirstChildElement( "dynamic_objects" );
        if ( dynamicObjs )
        {
			GUIManager::getInstance()->setTextLoader(L"Loading the dynamic objects");
			quickUpdate();
            DynamicObjectsManager::getInstance()->loadFromXML(dynamicObjs);
        }

        TiXmlElement* playerXML = root->FirstChildElement( "player" );
        if(playerXML)
        {
			// Temporary down since the player is a dynamic object now.
			// Will need to fix this.
            //Player::getInstance()->getObject()->loadFromXML(playerXML);
        }
    }
    else
    {
        #ifdef APP_DEBUG
        cout << "DEBUG : XML : THIS FILE IS NOT A IRRRPG BUILDER PROJECT!" << endl;
        #endif
		GUIManager::getInstance()->guiLoaderWindow->setVisible(false);
        return false;
    }

    #ifdef APP_DEBUG
    cout << "DEBUG : XML : PROJECT LOADED! "<< endl;
    #endif

    ///TODO:CLEAR PROJECT IF NOT RETURN TRUE ON LOAD PROJECT FROM XML
	GUIManager::getInstance()->guiLoaderWindow->setVisible(false);
    return true;
}

void App::initialize()
{
	CameraSystem::getInstance()->setPosition(vector3df(0,0,0));
	smgr->setAmbientLight(SColorf(0.80f,0.85f,1.0f,1.0f));
    driver->setFog(SColor(0,255,255,255),EFT_FOG_LINEAR,300,999100);
	quickUpdate();

	screensize=driver->getScreenSize();

	#ifdef EDITOR
		GUIManager::getInstance()->setupEditorGUI();
		TerrainManager::getInstance()->createSegment(vector3df(0,0,0));
		quickUpdate();
	#endif

	GUIManager::getInstance()->setupGameplayGUI();


	quickUpdate();



    Player::getInstance();
	driver->setMinHardwareBufferVertexCount(0);


    this->currentProjectName = "irb_temp_project";
// Hide the loading windows if the WX Widget is present
#ifdef _wxWIDGET
	this->setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
	GUIManager::getInstance()->guiLoaderWindow->setVisible(false);
#endif
}

void App::shutdown()
{
	// Stuff to do when the device is closed.
	SoundManager::getInstance()->stopEngine();
	cleanWorkspace();
	DynamicObjectsManager::getInstance()->clean(true);
	device->closeDevice();

}

stringw App::getLangText(irr::core::stringc node)
{
	return LANGManager::getInstance()->getText(node);
}
