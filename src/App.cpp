#include "App.h"

#include "EditorCamera.h"
#include "EventReceiver.h"
#include "GUIManager.h"
#include "TerrainManager.h"
#include "LANGManager.h"
#include "DynamicObjectsManager.h"
#include "LuaGlobalCaller.h"
#include "SoundManager.h"
#include "Player.h"

#include "tinyXML/tinyxml.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

App::App()
{

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
        GUIManager::getInstance()->drawNodePreview();
    }

    #ifdef APP_DEBUG
    //GUIManager::getInstance()->drawHelpImage(HELP_IRR_RPG_BUILDER_1);
    #endif
}

///TODO: mover isso para GUIManager
bool App::cursorIsInEditArea()
{
    bool condition = true;

    //is over the main toolbar??
    if(device->getCursorControl()->getPosition().Y < 36)   condition = false;

    //Is over terrain toolbar??
    if(device->getCursorControl()->getPosition().Y < 90  && device->getCursorControl()->getPosition().X < 200 && (app_state == APP_EDIT_TERRAIN_TRANSFORM || app_state == APP_EDIT_CHARACTER) )
        condition = false;

    //is over the dynamic objects chooser window??
    if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
    {
        if( device->getCursorControl()->getPosition().X > driver->getScreenSize().Width - 160.0 )
        {
            condition = false;
        }
    }

    //gameplay has a toolbar in bottom of the screen (items)
    if(app_state == APP_GAMEPLAY_NORMAL)
    {
        if( device->getCursorControl()->getPosition().Y > driver->getScreenSize().Height - 50.0 )
        {
            condition = false;
        }
    }

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
        GUIManager::getInstance()->setElementEnabled(BT_ID_EDIT_CHARACTER,false);
        GUIManager::getInstance()->setElementVisible(BT_ID_PLAYER_EDIT_SCRIPT,true);
        Player::getInstance()->setHighLight(true);
        EditorCamera::getInstance()->setPosition(Player::getInstance()->getPosition());
    }
    else
    {
        GUIManager::getInstance()->setElementEnabled(BT_ID_EDIT_CHARACTER,true);
        GUIManager::getInstance()->setElementVisible(BT_ID_PLAYER_EDIT_SCRIPT,false);
        Player::getInstance()->setHighLight(false);
    }

    if(app_state == APP_EDIT_SCRIPT_GLOBAL)
    {
        GUIManager::getInstance()->setElementEnabled(BT_ID_EDIT_SCRIPT_GLOBAL,false);
    }
    else
    {
        GUIManager::getInstance()->setElementEnabled(BT_ID_EDIT_SCRIPT_GLOBAL,true);
    }

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

        GUIManager::getInstance()->setElementVisible(BT_ID_VIEW_ITEMS,true);
    }
    else
    {
		
        GUIManager::getInstance()->setElementVisible(BT_ID_PLAY_GAME,true);
        GUIManager::getInstance()->setElementVisible(BT_ID_STOP_GAME,false);
        GUIManager::getInstance()->setElementEnabled(BT_ID_SAVE_PROJECT,true);
        GUIManager::getInstance()->setElementEnabled(BT_ID_LOAD_PROJECT,true);
        GUIManager::getInstance()->setElementEnabled(BT_ID_ABOUT,true);

        GUIManager::getInstance()->setElementVisible(BT_ID_VIEW_ITEMS,false);
    }
}

void App::eventGuiButton(s32 id)
{
    DynamicObject* selectedObject;
	vector3df oldcampos = vector3df(0,0,0);
    switch (id)
    {
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
            setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
            break;
        case BT_ID_DYNAMIC_OBJECT_BT_EDITSCRIPTS:
            selectedObject = DynamicObjectsManager::getInstance()->getObjectByName( stringc(lastMousePick.pickedNode->getName()) );

            GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);

            GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT,selectedObject->getScript());
            GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE,"");

            setAppState(APP_EDIT_DYNAMIC_OBJECTS_SCRIPT);
            break;
        case BT_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE:
            if(GUIManager::getInstance()->showDialogQuestion(LANGManager::getInstance()->getText("msg_override_script").c_str()))
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
                Player::getInstance()->setScript(GUIManager::getInstance()->getEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT));
                setAppState(APP_EDIT_CHARACTER);
            }
            else if(app_state == APP_EDIT_SCRIPT_GLOBAL)
            {
                scriptGlobal = GUIManager::getInstance()->getEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT);
                setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
            }
            GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT,false);
            break;
        case BT_ID_PLAY_GAME:
			oldcampos = Player::getInstance()->getPosition();
			EditorCamera::getInstance()->setCamera(1);
            this->setAppState(APP_GAMEPLAY_NORMAL);
            Player::getInstance()->doScript();
            LuaGlobalCaller::getInstance()->storeGlobalParams();
            DynamicObjectsManager::getInstance()->initializeAllScripts();
            DynamicObjectsManager::getInstance()->showDebugData(false);
            DynamicObjectsManager::getInstance()->initializeCollisions();
            TerrainManager::getInstance()->showDebugData(false);
            GUIManager::getInstance()->setElementVisible(ST_ID_PLAYER_LIFE,true);
            LuaGlobalCaller::getInstance()->doScript(scriptGlobal);
            break;
        case BT_ID_STOP_GAME:
			
			Player::getInstance()->clearScripts();
            DynamicObjectsManager::getInstance()->clearAllScripts();
            DynamicObjectsManager::getInstance()->clearCollisions();
            DynamicObjectsManager::getInstance()->showDebugData(true);
            TerrainManager::getInstance()->showDebugData(true);
            LuaGlobalCaller::getInstance()->restoreGlobalParams();
            SoundManager::getInstance()->stopSounds();
            GUIManager::getInstance()->setElementVisible(ST_ID_PLAYER_LIFE,false);
            GlobalMap::getInstance()->clearGlobals();
            this->setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
			EditorCamera::getInstance()->setCamera(2);
			EditorCamera::getInstance()->setPosition(vector3df(oldcampos));
			
            break;
        case BT_ID_EDIT_CHARACTER:
            this->setAppState(APP_EDIT_CHARACTER);
            break;
        case BT_ID_PLAYER_EDIT_SCRIPT:
            GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT,Player::getInstance()->getScript());
            GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE,"");
            setAppState(APP_EDIT_PLAYER_SCRIPT);
            GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT,true);
            break;
        case BT_ID_CLOSE_PROGRAM:
			this->cleanWorkspace();
			SoundManager::getInstance()->stopEngine();
            device->drop();
            exit(0);
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
            break;
        case BT_ID_USE_ITEM:
            LuaGlobalCaller::getInstance()->usePlayerItem(GUIManager::getInstance()->getActivePlayerItem());
            GUIManager::getInstance()->updateItemsList();
            break;
        case BT_ID_DROP_ITEM:
            Player::getInstance()->removeItem(GUIManager::getInstance()->getActivePlayerItem());
            GUIManager::getInstance()->updateItemsList();
            break;
        case BT_ID_EDIT_SCRIPT_GLOBAL:
            GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT,scriptGlobal);
            GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE,"");
            setAppState(APP_EDIT_SCRIPT_GLOBAL);
            GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT,true);
            break;
        case BT_ID_CLOSE_ITEMS_WINDOW:
            setAppState(APP_GAMEPLAY_NORMAL);
            GUIManager::getInstance()->setWindowVisible(GCW_GAMEPLAY_ITEMS,false);
            break;
    }
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
		vector3df oldRot = Player::getInstance()->getRotation();
		Player::getInstance()->setRotation(vector3df(0,value*10,0)+oldRot);
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
		EditorCamera::getInstance()->setCameraHeight(value * 50);
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
	screenW = 1024;
	screenH = 768;
	fullScreen = 0;
	TerrainManager::getInstance()->setTileMesh("../media/baseTerrain.obj");
	TerrainManager::getInstance()->setTerrainTexture(1,"../media/L1.jpg");
	TerrainManager::getInstance()->setTerrainTexture(2,"../media/L2.jpg");
	TerrainManager::getInstance()->setTerrainTexture(3,"../media/L3.jpg");
	TerrainManager::getInstance()->setTerrainTexture(4,"../media/L4.jpg");

	TiXmlDocument doc("config.xml");
	if (!doc.LoadFile()) return false;

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

        TiXmlElement* resXML = root->FirstChildElement( "Screen" );
        if ( resXML )
        {
            screenW = atoi(resXML->ToElement()->Attribute("ScreenWide"));
			screenH = atoi(resXML->ToElement()->Attribute("ScreenHeight"));
			fullScreen = atoi(resXML->ToElement()->Attribute("Fullscreen"));
        }
		TiXmlElement* groundXML = root->FirstChildElement( "Terrain" );
        if ( groundXML )
        {
            stringc meshname = groundXML->ToElement()->Attribute("Mesh");
			TerrainManager::getInstance()->setTileMesh(meshname);
			stringc layer1 = groundXML->ToElement()->Attribute("layer1");
			stringc layer2 = groundXML->ToElement()->Attribute("layer2");
			stringc layer3 = groundXML->ToElement()->Attribute("layer3");
			stringc layer4 = groundXML->ToElement()->Attribute("layer4");
			TerrainManager::getInstance()->setTerrainTexture(1,layer1);
			TerrainManager::getInstance()->setTerrainTexture(2,layer2);
			TerrainManager::getInstance()->setTerrainTexture(3,layer3);
			TerrainManager::getInstance()->setTerrainTexture(4,layer4);

        }

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

void App::setupDevice()
{

	loadConfig();
    device = createDevice(EDT_OPENGL, dimension2d<u32>(screenW, screenH), 32, fullScreen, false, false, 0);
    device->setWindowCaption(L"IrrRPG Builder - By Andres Jesse Porfirio - www.andresjesse.com");

    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();

    device->setEventReceiver(EventReceiver::getInstance());
	timer = device->getTimer()->getRealTime();
	timer2 = device->getTimer()->getRealTime();

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


void App::run()
{
    this->setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);

    int lastFPS = -1;
	u32 timer = device->getTimer()->getRealTime();
	u32 timer2 = device->getTimer()->getRealTime();
	
    while(device->run())
    {
		
        driver->beginScene(true, true, SColor(0,200,200,200));
        if(app_state < APP_STATE_CONTROL)
		{
			device->yield();
            updateEditMode();//editMode
		}
        else
		{ 		
			updateGameplay();	
		}
		
		smgr->drawAll();

		guienv->drawAll();

        draw2DImages();

        driver->endScene();

        // display frames per second in window title
		int fps = driver->getFPS();
		if (lastFPS != fps)
		{
			core::stringw str = L"Irr RPG Builder - By Andres Jesse Porfirio - www.andresjesse.com";
			str += " FPS:";
			str += fps;

			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
    }
	// Stuff to do when the device is closed.
	cleanWorkspace();
	SoundManager::getInstance()->stopEngine();
}

void App::saveProjectToXML()
{
    stringc filename = "../projects/";
    filename += currentProjectName;

    TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );

	TiXmlElement* irb_project = new TiXmlElement( "IrrRPG_Builder_Project" );
	irb_project->SetAttribute("version","1.0");

    TerrainManager::getInstance()->saveToXML(irb_project);
    DynamicObjectsManager::getInstance()->saveToXML(irb_project);
    Player::getInstance()->saveToXML(irb_project);

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
            scriptGlobal = globalScriptXML->ToElement()->Attribute("script");
        }

        TiXmlElement* terrain = root->FirstChildElement( "terrain" );
        if ( terrain )
        {
            TerrainManager::getInstance()->loadFromXML(terrain);
        }

        TiXmlElement* dynamicObjs = root->FirstChildElement( "dynamic_objects" );
        if ( dynamicObjs )
        {
            DynamicObjectsManager::getInstance()->loadFromXML(dynamicObjs);
        }

        TiXmlElement* playerXML = root->FirstChildElement( "player" );
        if(playerXML)
        {
            Player::getInstance()->loadFromXML(playerXML);
        }
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

void App::updateEditMode()
{
	timer = device->getTimer()->getRealTime();
	if ((timer-timer2)>17) // 1/60th second refresh interval
	{
		timer2 = device->getTimer()->getRealTime();
		if(app_state < APP_STATE_CONTROL)
		{
			if(EventReceiver::getInstance()->isKeyPressed(KEY_SPACE))
			{
				vector3df camPosition = this->getMousePosition3D(101).pickedPos;
				EditorCamera::getInstance()->setPosition(camPosition);

			}

			if(app_state == APP_EDIT_TERRAIN_TRANSFORM && cursorIsInEditArea() )
			{
				if(EventReceiver::getInstance()->isKeyPressed(KEY_LCONTROL))
				{
					if(EventReceiver::getInstance()->isMousePressed(0))
					{
						TerrainManager::getInstance()->transformSegmentsToZero(this->getMousePosition3D(100),
                                                                           GUIManager::getInstance()->getScrollBarValue(SC_ID_TERRAIN_BRUSH_RADIUS),
                                                                           GUIManager::getInstance()->getScrollBarValue(SC_ID_TERRAIN_BRUSH_STRENGTH)*0.0005f);
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
					Player::getInstance()->setPosition(getMousePosition3D().pickedPos);
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
					EditorCamera::getInstance()->moveCamera(vector3df(-10.0f,0,0));
				}
				else if (EventReceiver::getInstance()->isKeyPressed(KEY_RIGHT))
				{
					EditorCamera::getInstance()->moveCamera(vector3df(10.0f,0,0));
				}
				if(EventReceiver::getInstance()->isKeyPressed(KEY_UP))
				{
					EditorCamera::getInstance()->moveCamera(vector3df(0,0,10.0f));
				}
				else if (EventReceiver::getInstance()->isKeyPressed(KEY_DOWN))
				{
					EditorCamera::getInstance()->moveCamera(vector3df(0,0,-10.0f));
				}
			}
		}
	}
}

void App::updateGameplay()
{
	timer = device->getTimer()->getRealTime();
	//DynamicObjectsManager::getInstance()->updateAnimators();
	DynamicObjectsManager::getInstance()->updateAll(); // This one should be timed now.
	Player::getInstance()->update(); // This one is timed now.
	EditorCamera::getInstance()->setPosition(Player::getInstance()->getPosition());

	// This update the player events and controls at specific time intervals
    if ((timer-timer2)>17) // 1/60 second
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

					if(obj->getDistanceFrom(Player::getInstance()->getPosition()) < 72.0f)
					{
						if(obj->getObjectType() == stringc("ENEMY"))
						{
							Player::getInstance()->attackEnemy(obj);
							obj->notifyClick();
						}
						else
						{
							Player::getInstance()->lookAt(obj->getPosition());
							obj->notifyClick();
						}
					}
					else
					{
						Player::getInstance()->setWalkTarget(vector3df(mousePick.pickedPos.X,0,mousePick.pickedPos.Z));
					}
				}
				else
				{
					Player::getInstance()->setWalkTarget(mousePick.pickedPos);
				}
			}
			else//No action
			{
				if(Player::getInstance()->getAnimation() != PLAYER_ANIMATION_WALK) Player::getInstance()->setAnimation(PLAYER_ANIMATION_IDLE);
			}
		}
	
	stringc playerMoney = LANGManager::getInstance()->getText("txt_player_money");
	playerMoney += Player::getInstance()->getMoney();
	GUIManager::getInstance()->setStaticTextText(ST_ID_PLAYER_MONEY,playerMoney);
	}
}

void App::cleanWorkspace()
{
    EditorCamera::getInstance()->setPosition(vector3df(0,0,0));

    TerrainManager::getInstance()->clean();

    DynamicObjectsManager::getInstance()->clean();

    Player::getInstance()->setPosition(vector3df(0,0,0));
    Player::getInstance()->setRotation(vector3df(0,0,0));
    Player::getInstance()->setScript("");

    scriptGlobal="";
}

void App::createNewProject()
{
    APP_STATE old_state = getAppState();
    setAppState(APP_EDIT_WAIT_GUI);

    stringc name = GUIManager::getInstance()->showInputQuestion(LANGManager::getInstance()->getText("msg_new_project_name").c_str());
    GUIManager::getInstance()->flush();

    while(name == stringc(""))
    {
        name = GUIManager::getInstance()->showInputQuestion(LANGManager::getInstance()->getText("msg_new_project_name").c_str());
        GUIManager::getInstance()->flush();
    }

    name += ".XML";

    stringc filename = "../projects/";
    filename += name;

    this->cleanWorkspace();

    EditorCamera::getInstance();

    TerrainManager::getInstance()->createSegment(vector3df(0,0,0));

    smgr->setAmbientLight(SColorf(0.5,0.5,0.5,0.5));
    driver->setFog(SColor(255,255,255,255),EFT_FOG_LINEAR,0,12000);

    Player::getInstance();

    this->currentProjectName = name;

    this->saveProject();

    setAppState(old_state);
}

void App::loadProject()
{
    APP_STATE old_state = getAppState();
    setAppState(APP_EDIT_WAIT_GUI);

    bool ansSave = GUIManager::getInstance()->showDialogQuestion(LANGManager::getInstance()->getText("msg_override_project").c_str());
    GUIManager::getInstance()->flush();
    if(ansSave)
    {
        saveProjectToXML();
        GUIManager::getInstance()->showDialogQuestion(LANGManager::getInstance()->getText("msg_saved_ok").c_str());
        GUIManager::getInstance()->flush();
    }

    stringc name = GUIManager::getInstance()->showInputQuestion(LANGManager::getInstance()->getText("msg_new_project_name").c_str());
    GUIManager::getInstance()->flush();

    stringc filename = "../projects/";
    filename += name;
    filename += ".XML";

    currentProjectName = name;
    currentProjectName += ".XML";

    if(this->loadProjectFromXML(filename))
        GUIManager::getInstance()->showDialogMessage(LANGManager::getInstance()->getText("msg_loaded_ok").c_str());
    else
        GUIManager::getInstance()->showDialogMessage(LANGManager::getInstance()->getText("msg_loaded_error").c_str());

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
        currentProjectName = GUIManager::getInstance()->showInputQuestion(LANGManager::getInstance()->getText("msg_new_project_name").c_str());
        GUIManager::getInstance()->flush();
        EventReceiver::getInstance()->flushKeys();
        currentProjectName += ".XML";
    }

    this->saveProjectToXML();
    GUIManager::getInstance()->showDialogMessage(LANGManager::getInstance()->getText("msg_saved_ok").c_str());
    GUIManager::getInstance()->flush();

    setAppState(old_state);
}

void App::initialize()
{
    GUIManager::getInstance()->setupEditorGUI();

    EditorCamera::getInstance()->setPosition(vector3df(0,0,0));

    TerrainManager::getInstance()->createSegment(vector3df(0,0,0));

    smgr->setAmbientLight(SColorf(0.80f,0.85f,1.0f,1.0f));
    driver->setFog(SColor(255,255,255,255),EFT_FOG_LINEAR,300,9100);

    Player::getInstance();
	driver->setMinHardwareBufferVertexCount(0);

    this->currentProjectName = "irb_temp_project";
}
