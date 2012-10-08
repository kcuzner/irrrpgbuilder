#include "App.h"

#include "camera/CameraSystem.h"
#include "events/EventReceiver.h"
#include "gui/GUIManager.h"
#include "gui/CGUIFileSelector.h"
#include "terrain/TerrainManager.h"
#include "fx/EffectsManager.h"
#include "LANGManager.h"
#include "objects/DynamicObjectsManager.h"


#include "sound/SoundManager.h"
#include "objects/Player.h"



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
	wxSystemState=false;
	selector=NULL;
	app_state=APP_EDIT_LOOK;
	textevent.clear();
	lastScannedPick.pickedNode=NULL;
	selector=NULL;
	saveselector=NULL;
	lastPickedNodeName="";
	timer=0;
	timer2=0;
	timer3=0;
}

App::~App()
{
	this->cleanWorkspace();
	SoundManager::getInstance()->stopEngine();
	device->drop();
	// exit(0);
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


void App::displayGuiConsole()
{
	// This was the old console
	// This need to be improved with scaling.
	bool result=!guienv->getRootGUIElement()->getElementFromId(GCW_CONSOLE,true)->isVisible();
	GUIManager::getInstance()->setElementVisible(CONSOLE,result);
	GUIManager::getInstance()->setConsoleText(L"",true);
}
///TODO: mover isso para GUIManager
// Would be nice to only check the tools windows we have opened and check their position / scale
bool App::cursorIsInEditArea()
{
	bool condition = true;
	if (GUIManager::getInstance()->isGuiPresent(device->getCursorControl()->getPosition()))
		condition = false;

	// New code (nov 2011)
	if(device->getCursorControl()->getPosition().Y < 92 && app_state != APP_GAMEPLAY_NORMAL)  condition = false;

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

	if (old_app_state == APP_EDIT_TERRAIN_TRANSFORM && app_state != APP_EDIT_TERRAIN_TRANSFORM)
	{
		// Change the props to be collidable with the ray test
		DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_NON_INTERACTIVE,100);
	}

	if(app_state == APP_EDIT_TERRAIN_TRANSFORM)
	{
		// Change the props to be non-collidable with the ray test
		DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_NON_INTERACTIVE,0x0010);

		GUIManager::getInstance()->setWindowVisible(GCW_TERRAIN_TOOLBAR,true);
		ShaderCallBack::getInstance()->setFlagEditingTerrain(GUIManager::getInstance()->getCheckboxState(CB_ID_TERRAIN_SHOW_PLAYABLE_AREA));
		GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_TRANSFORM,false);
		GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
	}
	else
	{
		GUIManager::getInstance()->setWindowVisible(GCW_TERRAIN_TOOLBAR,false);
		ShaderCallBack::getInstance()->setFlagEditingTerrain(false);
		GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_TRANSFORM,true);
		GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
	}

	if(app_state == APP_EDIT_TERRAIN_PAINT_VEGETATION)
	{
		GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_PAINT_VEGETATION,false);
		GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
	}
	else
	{
		GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_PAINT_VEGETATION,true);
		GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
	}

	if(app_state == APP_EDIT_TERRAIN_SEGMENTS)
	{
		GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_ADD_SEGMENT,false);
		GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
	}
	else
	{
		GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_ADD_SEGMENT,true);
		GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
	}

	if(app_state == APP_EDIT_TERRAIN_TRANSFORM)
	{
		GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_TRANSFORM,false);
		GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
	}
	else
	{
		GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_TRANSFORM,true);
	}

	//if the previous state was DYNAMIC OBJECTS then we need to hide his custom windows
	if(old_app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
		GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECT_CHOOSER,false);
#endif

	if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
	{
		GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECT_CHOOSER,true);
		GUIManager::getInstance()->setElementEnabled(BT_ID_DYNAMIC_OBJECTS_MODE,false);
		GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
	}
	else
	{
		GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECT_CHOOSER,false);
		GUIManager::getInstance()->setElementEnabled(BT_ID_DYNAMIC_OBJECTS_MODE,true);
	}
#ifdef EDITOR
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
		CameraSystem::getInstance()->setPosition(Player::getInstance()->getObject()->getPosition());
		GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);
	}
	else
	{
		GUIManager::getInstance()->setElementEnabled(BT_ID_EDIT_CHARACTER,true);
		GUIManager::getInstance()->setElementVisible(BT_ID_PLAYER_EDIT_SCRIPT,false);
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
		//This current button is for the console
		//GUIManager::getInstance()->setElementEnabled(BT_ID_HELP,false);
		GUIManager::getInstance()->setElementVisible(IMG_BAR,true);
		GUIManager::getInstance()->setElementVisible(BT_ID_VIEW_ITEMS,true);
	}
	else if(app_state < 100)
	{
		GUIManager::getInstance()->setElementVisible(BT_ID_PLAY_GAME,true);
		GUIManager::getInstance()->setElementVisible(BT_ID_STOP_GAME,false);
		GUIManager::getInstance()->setElementEnabled(BT_ID_SAVE_PROJECT,true);
		GUIManager::getInstance()->setElementEnabled(BT_ID_LOAD_PROJECT,true);
		GUIManager::getInstance()->setElementEnabled(BT_ID_ABOUT,true);
		GUIManager::getInstance()->setElementEnabled(BT_ID_NEW_PROJECT,true);
		GUIManager::getInstance()->setElementEnabled(BT_ID_HELP,true);
		GUIManager::getInstance()->setElementVisible(IMG_BAR,false);
		GUIManager::getInstance()->setElementVisible(BT_ID_VIEW_ITEMS,false);
	} else if(app_state == APP_WAIT_DIALOG)
	{
		GUIManager::getInstance()->setElementVisible(BT_ID_VIEW_ITEMS,false);
		GUIManager::getInstance()->setElementVisible(IMG_BAR,false);
	}



	if (app_state == APP_EDIT_VIEWDRAG)
	{
		GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_drag").c_str());
	}
}

void App::eventGuiButton(s32 id)
{
	DynamicObject* selectedObject;
	oldcampos = vector3df(0,0,0);


	switch (id)
	{

	case BT_ID_NEW_PROJECT:
		lastScannedPick.pickedNode=NULL;
		this->createNewProject();
		break;

	case BT_ID_LOAD_PROJECT:
		this->loadProject();
		/* // Load a new project but not when the loader window is visible
		if (!GUIManager::getInstance()->guiLoaderWindow->isVisible())
		{
		GUIRequestManager::getInstance()->FileSelector(core::dimension2d<u32>(640,400),L"Loading a project file");
		old_state=app_state;
		app_state=APP_WAIT_FILEREQUEST;
		}*/

		//this->setAppState(APP_EDIT_LOOK);
		break;

	case BT_ID_SAVE_PROJECT:
		this->saveProject();
		this->setAppState(APP_EDIT_LOOK);
		break;
#ifdef EDITOR
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
		{
			this->setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
		}
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

		this->setAppState(APP_EDIT_DYNAMIC_OBJECTS_SCRIPT);
		break;

	case BT_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE:
		//if(GUIManager::getInstance()->showDialogQuestion(stringc(LANGManager::getInstance()->getText("msg_override_script")).c_str()))
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

		// remove the object for the selection
		lastScannedPick.pickedNode=NULL;
		lastMousePick.pickedNode=NULL;

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
		this->shutdown();

		/*this->cleanWorkspace();
		SoundManager::getInstance()->stopEngine();
		device->closeDevice();
		//device->drop();
		//exit(0);*/
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
		DynamicObjectsManager::getInstance()->freezeAll();
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
		DynamicObjectsManager::getInstance()->unFreezeAll();
		break;

	case BT_ID_DIALOG_YES:
		GUIManager::getInstance()->setWindowVisible(GCW_DIALOG,false);
		if (app_state> APP_STATE_CONTROL)
		{
			//Player::getInstance()->getObject()->notifyAnswer(true);
			if (DynamicObjectsManager::getInstance()->getDialogCaller())
				DynamicObjectsManager::getInstance()->getDialogCaller()->notifyAnswer(true);
			setAppState(APP_GAMEPLAY_NORMAL);
			GUIManager::getInstance()->stopDialogSound();
		}
		break;

	case BT_ID_DIALOG_CANCEL:
		GUIManager::getInstance()->setWindowVisible(GCW_DIALOG,false);
		if (app_state> APP_STATE_CONTROL)
		{
			//Player::getInstance()->getObject()->notifyAnswer(false);
			if (DynamicObjectsManager::getInstance()->getDialogCaller())
				DynamicObjectsManager::getInstance()->getDialogCaller()->notifyAnswer(false);
			setAppState(APP_GAMEPLAY_NORMAL);
			GUIManager::getInstance()->stopDialogSound();
		}
		break;

	default:
		break;
	}
}

// Stuff in editor only
#ifdef EDITOR

void App::hideEditGui()
{
	wxSystemState=true;
	GUIManager::getInstance()->setConsoleText(L"Ready now for WXwidget!",SColor(255,0,0,255));
	//GUIManager::getInstance()->setElementVisible(BT_ID_WXEditor,false);
}

std::vector<stringw> App::getAbout()
{
	return LANGManager::getInstance()->getAboutText();
}

void App::eventGuiCheckbox(s32 id)
{
	switch (id)
	{
	case CB_ID_TERRAIN_SHOW_PLAYABLE_AREA:
		ShaderCallBack::getInstance()->setFlagEditingTerrain(GUIManager::getInstance()->getCheckboxState(CB_ID_TERRAIN_SHOW_PLAYABLE_AREA));
		break;
	default:
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

	case CO_ID_DYNAMIC_OBJECT_OBJ_CATEGORY:
		std::string result = (std::string)GUIManager::getInstance()->getComboBoxItem(CO_ID_DYNAMIC_OBJECT_OBJ_CATEGORY).c_str();
		TYPE choice = OBJECT_TYPE_NPC;

		if (result.find("NPC") != std::string::npos)
			choice = OBJECT_TYPE_NPC;

		if (result.find("INTERACTIVE OBJECTS") != std::string::npos)
			choice = OBJECT_TYPE_INTERACTIVE;

		if (result.find("PROPS") != std::string::npos)
			choice = OBJECT_TYPE_NON_INTERACTIVE;

		GUIManager::getInstance()->UpdateGUIChooser(choice);
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

		CameraSystem::getInstance()->fixRatio(driver);

		GUIManager::getInstance()->setConsoleText(text.c_str(),SColor(255,0,0,255));
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


	case KEY_RETURN:
		if (app_state == APP_WAIT_DIALOG)
			break;
	case KEY_ESCAPE:
		//device->drop();
		break;

	default:
		break;
	}
}

void App::eventMousePressed(s32 mouse)
{
	switch(mouse)
	{///TODO: colocar acoes mais comuns acima e menos comuns nos elses
	case EMIE_LMOUSE_PRESSED_DOWN://Left button (default)
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
	case EMIE_RMOUSE_PRESSED_DOWN:
		// Right button (Action the same as the left button)
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
					}
				}
			}
			else if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE)
			{
				setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
			}
		}
		break;

	case 3: //Mousewheel pressed
		break;

	default:
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
			// not in viewdrag mode then enable the cam, then set the camera height then disable it again
			if (app_state != APP_EDIT_VIEWDRAG)
			{
				// Go directly
				CameraSystem::getInstance()->setCameraHeight(-value);
			}
			else// in viewdrag mode
				CameraSystem::getInstance()->setCameraHeight(-value);

		}
		else // ingame camera
			CameraSystem::getInstance()->setCameraHeight(value);
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

	// For the ray test, we should hide the player
	Player::getInstance()->getObject()->getNode()->setVisible(false);

	line3df ray = smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(pos, smgr->getActiveCamera());

	core::vector3df intersection;
	core::triangle3df hitTriangle;

	ISceneNode* tempNode = smgr->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray,
		intersection,
		hitTriangle,
		id);
	MousePick result;
	// Show back the player once the ray test is done
	Player::getInstance()->getObject()->getNode()->setVisible(true);


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


void App::setPreviewSelection()
{
	// Will get a toggle selection
	MousePick mousePick = getMousePosition3D();


	stringc nodeName = "";
	// Check for a node to prevent a crash (need to get the name of the node)
	if (mousePick.pickedNode != NULL)
	{
		if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
		{
			nodeName = mousePick.pickedNode->getName();
			//If the mouse hover the object it will be toggled in debug data (bounding box, etc)
			if( stringc( nodeName.subString(0,14)) == "dynamic_object" )
			{
				if (nodeName!=lastPickedNodeName && lastScannedPick.pickedNode!=NULL)
					lastScannedPick.pickedNode->setDebugDataVisible(0);

				lastScannedPick = mousePick;
				lastPickedNodeName=nodeName;
				if (mousePick.pickedNode!=NULL)
					mousePick.pickedNode->setDebugDataVisible(true ? EDS_BBOX | EDS_SKELETON : EDS_OFF);
			}
			//if the mouse is not over the object anymore then it will "deselect" it
			else if (lastScannedPick.pickedNode!=NULL)
			{
				lastScannedPick.pickedNode->setDebugDataVisible(0);
			}
		}
	}
}

bool App::loadConfig()
{

	screensize.Height = 768;
	screensize.Width = 1024;

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
	// File to load if it's the editor
	TiXmlDocument doc("config.xml");
#else
	// File to load if it's the player build
	TiXmlDocument doc("gameconfig.xml");
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
			screensize.Width = atoi(resXML->ToElement()->Attribute("screen_width"));
			screensize.Height = atoi(resXML->ToElement()->Attribute("screen_height"));
			stringc full = resXML->ToElement()->Attribute("fullscreen");
			if (full=="true")
			{
				fullScreen=true;
			}
			stringc resize = resXML->ToElement()->Attribute("resizeable");
			if (resize=="true")
				resizable=true;

			if (resizable && fullScreen)
			{
				IrrlichtDevice * tempdevice = createDevice(EDT_NULL,dimension2d<u32>(640,480), 16, false, false, false, 0);
				screensize = tempdevice->getVideoModeList()->getDesktopResolution();
				tempdevice->closeDevice();
			}

		}
		//Language
		TiXmlElement* langXML = root->FirstChildElement( "language" );
		if ( langXML )
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
		device->setWindowCaption(L"IrrRPG Builder - Alpha release 0.21 (February 2012)");
	} else
		device = IRRdevice;

	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();

	device->setEventReceiver(EventReceiver::getInstance());
	timer = device->getTimer()->getRealTime();
	timer2 = device->getTimer()->getRealTime();
	timer3 = device->getTimer()->getRealTime();
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
		//oldcampos = Player::getInstance()->getObject()->getPosition();
		oldcampos = CameraSystem::getInstance()->editCamMaya->getPosition();
		oldcamtar = CameraSystem::getInstance()->editCamMaya->getTarget();
		LuaGlobalCaller::getInstance()->storeGlobalParams();

		//DynamicObjectsManager::getInstance()->displayShadow(true);
		CameraSystem::getInstance()->setCamera(1);
		// setback the fog as before (will need to check with LUA)
		driver->setFog(SColor(0,255,255,255),EFT_FOG_LINEAR,300,9100);
		old_state = app_state;
		this->setAppState(APP_GAMEPLAY_NORMAL);
		DynamicObjectsManager::getInstance()->showDebugData(false);

		DynamicObjectsManager::getInstance()->initializeAllScripts();
		// Need to evaluate if it's needed to have displaying debug data for objects (could be done with selection instead)
		//DynamicObjectsManager::getInstance()->showDebugData(false);
		//TerrainManager::getInstance()->showDebugData(false);

		DynamicObjectsManager::getInstance()->startCollisions();
		//DynamicObjectsManager::getInstance()->initializeCollisions();

		// Reset the last "walk target" as the game restart.
		Player::getInstance()->getObject()->setWalkTarget(Player::getInstance()->getObject()->getPosition());

		GUIManager::getInstance()->setElementVisible(ST_ID_PLAYER_LIFE,true);
		LuaGlobalCaller::getInstance()->doScript(scriptGlobal);

	}
}

void App::stopGame()
{
	if (app_state>APP_STATE_CONTROL)
	{
		DynamicObjectsManager::getInstance()->objectsToIdle();
		LuaGlobalCaller::getInstance()->restoreGlobalParams();
		GlobalMap::getInstance()->clearGlobals();

		DynamicObjectsManager::getInstance()->clearAllScripts();
		DynamicObjectsManager::getInstance()->clearCollisions();
		//DynamicObjectsManager::getInstance()->displayShadow(false);
		// Need to evaluate if it's needed to have displaying debug data for objects (could be done with selection instead)
		// DynamicObjectsManager::getInstance()->showDebugData(true);
		// TerrainManager::getInstance()->showDebugData(true);
		DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(false);


		SoundManager::getInstance()->stopSounds();
		GUIManager::getInstance()->setElementVisible(ST_ID_PLAYER_LIFE,false);

		this->setAppState(old_state); //APP_EDIT_LOOK


		CameraSystem::getInstance()->editCamMaya->setUpVector(vector3df(0,1,0));
		CameraSystem::getInstance()->setCamera(2);
		CameraSystem::getInstance()->editCamMaya->setPosition(vector3df(0.0f,1000.0f,-1000.0f));
		CameraSystem::getInstance()->editCamMaya->setTarget(vector3df(0.0f,0.0f,0.0f));
		//CameraSystem::getInstance()->setPosition(vector3df(oldcampos));

		driver->setFog(SColor(0,255,255,255),EFT_FOG_LINEAR,300,999100);


	}
}

void App::update()
{

	// Attempt to do automatic rezise detection
	if (screensize != driver->getScreenSize())
		this->setScreenSize(driver->getScreenSize());

	driver->beginScene(true, true, SColor(0,200,200,200));

	// Terrain transform mode MUSt use all the CPU/Refresh it can get for performance
	if(app_state < APP_STATE_CONTROL)
	{

		if (app_state!=APP_EDIT_TERRAIN_TRANSFORM)
			device->yield();

#ifdef EDITOR

		updateEditMode();//editMode
#endif
	}
	else
	{
		// Do not update the gameplay if we "paused" the game for a reason
		if(app_state < APP_GAMEPLAY_VIEW_ITEMS)
			updateGameplay();
	}


	// Check for events of the logger
	//GUIManager::getInstance()->setConsoleLogger(textevent);

	// This will calculate the animation blending for the nodes
	DynamicObjectsManager::getInstance()->updateAnimationBlend();

	// Prepare the post FX before rendering all
	EffectsManager::getInstance()->preparePostFX(false);
	smgr->drawAll();

	// Tries to do an post FX
	EffectsManager::getInstance()->update();

	guienv->drawAll();
	draw2DImages();

	driver->endScene();
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
	this->setAppState(APP_EDIT_LOOK);
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

	// This is the core loop
	while(device->run())
	{
		this->update();
		// display frames per second in window title
		int fps = driver->getFPS();
		if (lastFPS != fps)
		{
			core::stringw str = L"IrrRPG Builder - Alpha release 0.21 (september 2012)";
			str += " FPS:";
			str += fps;

			//GUIManager::getInstance()->setStatusText(str.c_str());
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

	// Draw the brush in realtime
	if(app_state == APP_EDIT_TERRAIN_TRANSFORM && cursorIsInEditArea() )
		TerrainManager::getInstance()->drawBrush();

	// Trie to display the node as we go with the mouse cursor in edit mode
	if((app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE || app_state==APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE) && cursorIsInEditArea() )
		setPreviewSelection();

	// Enter the refresh after a timer duration OR if the terrain transform is used
	if ((timer-timer2)>17 || APP_EDIT_TERRAIN_TRANSFORM) // (17)1/60th second refresh interval
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

					setAppState(APP_EDIT_VIEWDRAG);

					{// TODO: Move the cam based on the cursor position. Current method is buggy.
						// vector3df camPosition = this->getMousePosition3D(100).pickedPos;
						// Unlock the maya camera (Need to be improved)
						if (!CameraSystem::getInstance()->editCamMaya->isInputReceiverEnabled())
							CameraSystem::getInstance()->editCamMaya->setInputReceiverEnabled(true);
					}

					return;
				}
			}
			// Return the edit mode to normal after the spacebar is pressed (viewdrag)
			if (app_state == APP_EDIT_VIEWDRAG)
			{
				// lock the maya camera (Need to be improved)
				if (CameraSystem::getInstance()->editCamMaya->isInputReceiverEnabled())
					CameraSystem::getInstance()->editCamMaya->setInputReceiverEnabled(false);
				setAppState(old_state);
			}
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
				// Change the ID of the moved mesh so it's won't collision with the ray.
				irr::s32 oldID=lastMousePick.pickedNode->getID();
				lastMousePick.pickedNode->setID(0x0010);
				lastMousePick.pickedNode->setPosition(getMousePosition3D(100).pickedPos);
				lastMousePick.pickedNode->setID(oldID);
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
				app_state == APP_EDIT_CHARACTER ||
				app_state == APP_EDIT_LOOK)
			{

				//Update Editor Camera Position
				// Enabled again with the new editor camera
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
	// Refresh the NPC loop
	if ((timer-timer3)>0) // (17 )1/60 second (0 value seem ok for now)
	{
		// Update the NPc refresh
		timer3 = device->getTimer()->getRealTime();

		// Update all the NPC on the map (including the player)
		DynamicObjectsManager::getInstance()->updateAll();

		// Update the Point&Click camera setup
		CameraSystem::getInstance()->updatePointClickCam();

		// Update the combat system (mostly for damage over time management (dot))
		Combat::getInstance()->update();
	}


	// This update the player events and controls at specific time intervals
	if ((timer-timer2)>34)
	{
		timer2 = device->getTimer()->getRealTime();

		if(EventReceiver::getInstance()->isMousePressed(0) && cursorIsInEditArea() && app_state == APP_GAMEPLAY_NORMAL)
		{
			// Try a new trick to pick up only the NPC and the ground (AS object can walk on other objects)
			DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_NON_INTERACTIVE,0x0010);
			DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_NPC,100);

			// Filter only object with the ID=100 to get the resulting node
			MousePick mousePick = getMousePosition3D(100);

			// Set back to the defaults
			DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_NON_INTERACTIVE,100);
			DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_NPC,0x0010);

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
					if (mousePick.pickedPos!=vector3df(0,0,0))
					{
						Player::getInstance()->getObject()->setWalkTarget(mousePick.pickedPos);
						DynamicObjectsManager::getInstance()->getTarget()->setPosition(mousePick.pickedPos+vector3df(0,0.1f,0));
						DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(true);
						Player::getInstance()->setTaggedTarget(NULL);
					}
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
	old_state = getAppState();

	// Have to rethink how to do it. It used the gameplay dialog.
	//bool ansSave = GUIManager::getInstance()->showDialogQuestion(stringc(LANGManager::getInstance()->getText("msg_override_project")).c_str());
	//GUIManager::getInstance()->flush();
	/*bool ansSave=false;
	if(ansSave)
	{
	stringc filename = "../projects/";
	filename += currentProjectName;
	saveProjectToXML(filename);
	GUIManager::getInstance()->showDialogQuestion(LANGManager::getInstance()->getText("msg_saved_ok").c_str());
	GUIManager::getInstance()->flush();
	}

	stringc name = GUIManager::getInstance()->showInputQuestion(LANGManager::getInstance()->getText("msg_new_project_name").c_str());
	GUIManager::getInstance()->flush();

	this->cleanWorkspace();

	stringc filename = "../projects/";
	filename += name;
	filename += ".XML";

	currentProjectName = name;
	currentProjectName += ".XML";

	setAppState(APP_EDIT_WAIT_GUI);
	if(this->loadProjectFromXML(filename))
	GUIManager::getInstance()->showDialogMessage(LANGManager::getInstance()->getText("msg_loaded_ok"));
	else
	GUIManager::getInstance()->showDialogMessage(LANGManager::getInstance()->getText("msg_loaded_error"));

	//this->loadProject("");
	setAppState(old_state);
	//setAppState(APP_STATE_CONTROL);*/


	// (2/12/12) new filerequester method
	// The event manager will receive the event and load the file.
	setAppState(APP_WAIT_FILEREQUEST);
	if (!selector)
	{
		// Create a load file selector
		selector = new CGUIFileSelector(getLangText("msg_prj_lp0").c_str(), guienv, guienv->getRootGUIElement(), 1, CGUIFileSelector::EFST_OPEN_DIALOG);
		// Create a base icon for the files
		selector->setCustomFileIcon(driver->getTexture("../media/art/file.png"));
		// Create a base icon for the folders
		selector->setCustomDirectoryIcon(driver->getTexture("../media/art/folder.png"));
		// Add a new file filters (Normally for what is required to load)
		selector->addFileFilter(L"IRB Project files", L"xml", driver->getTexture("../media/art/wma.png"));

		// This is required for the window stretching feature
		selector->setDevice(device);

		// Create a "favorite places"
		selector->addPlacePaths(L"IRB Project path",L"../projects",driver->getTexture("../media/art/places_folder.png"));
#ifdef WIN32

		// Populate with standard windows favorites paths
		selector->populateWindowsFAV();
#else
		// Add some common linux paths
		selector->populateLinuxFAV();
#endif

		// Define in what path the request will open (it accept full or relative paths)
		selector->setStartingPath(L"../projects");

	}

}

/*
void App::loadProject(stringc filename)
{
this->cleanWorkspace();
if(!this->loadProjectFromXML("../projects/myProjectTiny.xml")) this->createNewProject("temp_project");
}
*/

// This will load the project contained in the selector
// Call is coming directly from the event manager
// Since we're using our new file selector and the method uses the events
// It will be useful to rename that method (obsolete name)
void App::loadProjectFile(bool value)
{
	if (value)
	{
		// Close and drop the file selector

		//Clean up the current world and load the scene

		// Here if it's the load file selector
		if (selector)
		{
			core::stringc file=(core::stringc)selector->getFileName();


			// This is a file loader
			if (selector->isSaver()==false)
			{
				//printf("Loading project now!\n");
				cleanWorkspace();
				selector->setVisible(false);
				this->loadProjectFromXML(file);
			}
			// This is a file saver
			if (selector->isSaver()==true)
			{
				selector->setVisible(false);
				//printf("Saving project now!\n");
				this->saveProjectToXML(file);
			}

			//Destroy the selector
			selector->remove();
			selector=NULL;
			setAppState(old_state);
			//printf ("The returned string is %s\n",file);
		}

		// Here is the save file selector
		else if (saveselector)
		{
			core::stringc file = (core::stringc)saveselector->getFileName();

// For windows put as backslash.
#ifdef WIN32
			file.replace('/','\\');
#endif
			saveselector->setVisible(false);

			this->saveProjectToXML(file);

			saveselector->remove();
			saveselector=NULL;
			setAppState(old_state);
		}
	}

	else
	// User cancelled the file selector. remove them
	{
		setAppState(old_state);
		if (selector)
		{
			selector->remove();
			selector=NULL;
		}
		if (saveselector)
		{
			saveselector->remove();
			saveselector=NULL;
		}
	}

	//setAppState(old_state);
}

void App::saveProject()
{
	APP_STATE old_state = getAppState();
	setAppState(APP_EDIT_WAIT_GUI);

	// Old method of request for save file (only a text input)
/*
	if(currentProjectName == stringc("irb_temp_project"))
	{
		currentProjectName = GUIManager::getInstance()->showInputQuestion(LANGManager::getInstance()->getText("msg_new_project_name"));
		GUIManager::getInstance()->flush();
		EventReceiver::getInstance()->flushKeys();
		currentProjectName += ".XML";
	}

	stringc filename = "../projects/";
	filename += currentProjectName;
	this->saveProjectToXML(filename);
	GUIManager::getInstance()->showDialogMessage(LANGManager::getInstance()->getText("msg_saved_ok"));
	GUIManager::getInstance()->flush();*/

	if (!saveselector)
	{
		// Create a save file selector // EFST_OPEN_DIALOG // EFST_SAVE_DIALOG
		saveselector = new CGUIFileSelector(getLangText("msg_prj_sp0").c_str(), guienv, guienv->getRootGUIElement(), 1, CGUIFileSelector::EFST_SAVE_DIALOG);
		// Create a base icon for the files
		saveselector->setCustomFileIcon(driver->getTexture("../media/art/file.png"));
		// Create a base icon for the folders
		saveselector->setCustomDirectoryIcon(driver->getTexture("../media/art/folder.png"));
		// Add a new file filters (Normally for what is required to load)
		saveselector->addFileFilter(L"IRB Project files", L"xml", driver->getTexture("../media/art/wma.png"));

		// This is required for the window stretching feature
		saveselector->setDevice(device);

		// Create a "favorite places"
		saveselector->addPlacePaths(L"IRB Project path",L"../projects",driver->getTexture("../media/art/places_folder.png"));
#ifdef WIN32

		// Populate with standard windows favorites paths
		saveselector->populateWindowsFAV();
#else
		// Add some common linux paths
		saveselector->populateLinuxFAV();
#endif

		// Define in what path the request will open (it accept full or relative paths)
		saveselector->setStartingPath(L"../projects");
	}


	//setAppState(old_state);
}

stringc App::getProjectName()
{
	return this->currentProjectName;
}

void App::saveProjectToXML(stringc filename)
{

	GUIManager::getInstance()->guiLoaderWindow->setVisible(true);
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );

	TiXmlElement* irb_project = new TiXmlElement( "IrrRPG_Builder_Project" );
	irb_project->SetAttribute("version","1.0");

	GUIManager::getInstance()->setTextLoader(L"Saving the terrain");
	quickUpdate();
	TerrainManager::getInstance()->saveToXML(irb_project);

	GUIManager::getInstance()->setTextLoader(L"Saving the active dynamic objects");
	quickUpdate();
	DynamicObjectsManager::getInstance()->saveToXML(irb_project);

	// Old command should be removed.
	//Player::getInstance()->getObject()->saveToXML(irb_project);

	GUIManager::getInstance()->setTextLoader(L"Saving the global scripts");
	quickUpdate();
	TiXmlElement* globalScript = new TiXmlElement("global_script");
	globalScript->SetAttribute("script",scriptGlobal.c_str());

	irb_project->LinkEndChild(globalScript);

	doc.LinkEndChild( decl );
	doc.LinkEndChild( irb_project );
	bool result = doc.SaveFile( filename.c_str() );
	if (result) printf("Save OK!\n");
	GUIManager::getInstance()->guiLoaderWindow->setVisible(false);

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
			// Player is a dynamic object now.
			// There is no need for now to load from this
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
	// Initialize the GUI class first
	GUIManager::getInstance();

	// Set the ambient light
	smgr->setAmbientLight(SColorf(0.80f,0.85f,1.0f,1.0f));

	// Set the fog to be very far when not in gameplay
	driver->setFog(SColor(0,255,255,255),EFT_FOG_LINEAR,0,20000);

	quickUpdate();
	screensize=driver->getScreenSize();

#ifdef EDITOR
	GUIManager::getInstance()->setupEditorGUI();
	TerrainManager::getInstance()->createSegment(vector3df(0,0,0));
	quickUpdate();
#endif

	CameraSystem::getInstance()->setPosition(vector3df(0,0,0));

	GUIManager::getInstance()->setupGameplayGUI();


	quickUpdate();



	Player::getInstance();
	driver->setMinHardwareBufferVertexCount(0);


	this->currentProjectName = "irb_temp_project";

}

void App::shutdown()
{
	// Stuff to do when the device is closed.
	SoundManager::getInstance()->stopEngine();
	cleanWorkspace();
	DynamicObjectsManager::getInstance()->clean(true);
	device->closeDevice();
	device->drop();
	exit(0);

}

std::vector<stringw> App::getConsoleText()
{
	return console_event;
}

std::vector<SColor> App::getConsoleColor()
{
	return console_event_color;
}

void App::clearConsole()
{
	console_event.clear();
	console_event_color.clear();
	GUIManager::getInstance()->clearConsole();
}

stringw App::getLangText(irr::core::stringc node)
{
	return LANGManager::getInstance()->getText(node);
}


irr::f32 App::getBrushRadius()
{
	f32 radius = GUIManager::getInstance()->getScrollBarValue(SC_ID_TERRAIN_BRUSH_RADIUS);
	return radius;
}