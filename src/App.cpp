#include "App.h"
#include "raytests.h"

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

	// Initialize some values
	selector=NULL;
	app_state=APP_EDIT_LOOK;

	textevent.clear();
	lastScannedPick.pickedNode=NULL;
	lastMousePick.pickedNode=NULL;

	selector=NULL;
	saveselector=NULL;
	selectedNode=NULL;
	scriptNode=NULL;
	lastPickedNodeName="";
	lastFilename="";
	timer=0;
	timer2=0;
	timer3=0;
	initRotation=false;
	oldmouse=vector2df(0,0);
	lockcam=false;
	ingamebackground=SColor(0,0,0,0); // Default ingame color is black
	moveupdown = false; // Mouse item move up/down in dynamic object ADD mode
	snapfunction = false;

	levelchange=false;

	overdraw=false;
	tex_occluded=NULL; // Texture to put on the player when he is occluded
	tex_normal=NULL;

	df = DF_PROJECT; // Default state for the fileselector (Dialog)

	toolstate = TOOL_NONE; // no tools activated
	old_do_state = TOOL_DO_ADD; // no tools activated
	toolactivated = false; // no tools activated
	initangle=vector2d<f32>(0,0); //Initialize the initial angle of the RTS camera (Calculated from here)
	raytester=0; // Initialize and the ray tester class

	current_listfilter = OBJECT_TYPE_NONE;//Show all the objects in the object list set as initial value

	combobox_used=false;
	currentsnapping=64.0f; //set the current snapping distance;
	
}

App::~App()
{
	this->cleanWorkspace();

	// Remove the raytester class from memory
	delete raytester;
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
	#ifdef DEBUG
	//GUIManager::getInstance()->drawHelpImage(HELP_IRR_RPG_BUILDER_1);
	#endif
#endif
}


void App::displayGuiConsole()
{
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
	app_state = newAppState;

#ifdef EDITOR

	if (old_app_state != app_state && app_state != APP_EDIT_VIEWDRAG && app_state != APP_EDIT_DYNAMIC_OBJECTS_MODE)
	{
		if (selectedNode) //Unselect and remove the selected node in mode changes
		{
			//GUIManager::getInstance()->setElementVisible(BT_ID_DO_SEL_MODE,false);
			if (app_state != APP_EDIT_DYNAMIC_OBJECTS_SCRIPT)
			{
				selectedNode->setDebugDataVisible(0); 
				selectedNode=NULL;
			}
			

		}
	}

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
		GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_TRANSFORM,false);
		GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
		timer1 = device->getTimer()->getRealTime();
	}
	else
	{
		if (old_app_state == APP_EDIT_TERRAIN_TRANSFORM)
		{
			GUIManager::getInstance()->setWindowVisible(GCW_TERRAIN_TOOLBAR,false);
			ShaderCallBack::getInstance()->setFlagEditingTerrain(false);
			GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_TRANSFORM,true);
			GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
		}
	}

	if(app_state == APP_EDIT_TERRAIN_PAINT_VEGETATION)
	{
		GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_PAINT_VEGETATION,false);
		GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
		if (selectedNode)
		{
			selectedNode->setDebugDataVisible(0);
			selectedNode=NULL;
		}
		timer1 = device->getTimer()->getRealTime();
	}
	else
	{
		if (old_app_state == APP_EDIT_TERRAIN_PAINT_VEGETATION)
		{
			GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_PAINT_VEGETATION,true);
			GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
		}
	}

	if(app_state == APP_EDIT_TERRAIN_SEGMENTS)
	{
		GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_ADD_SEGMENT,false);
		GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
		if (selectedNode)
		{
			selectedNode->setDebugDataVisible(0);
			selectedNode=NULL;	
		}
	}
	else
	{
		if (old_app_state == APP_EDIT_TERRAIN_SEGMENTS)
		{
			GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_ADD_SEGMENT,true);
			GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
		}
	}

	if(app_state == APP_EDIT_TERRAIN_EMPTY_SEGMENTS)
	{
		GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_ADD_EMPTY_SEGMENT,false);
		GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
	}
	else
	{
		if (old_app_state == APP_EDIT_TERRAIN_EMPTY_SEGMENTS)
		{
			GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_ADD_EMPTY_SEGMENT,true);
			GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
		}
	}

	if (app_state == APP_EDIT_TERRAIN_CUSTOM_SEGMENTS)
	{
		GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_ADD_CUSTOM_SEGMENT,false);
		GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
		GUIManager::getInstance()->setWindowVisible(GCW_CUSTOM_SEGMENT_CHOOSER,true);
		if (selectedNode)
		{
			selectedNode->setDebugDataVisible(0);
			selectedNode=NULL;
		}
	}
	else
	{
		if  (old_app_state == APP_EDIT_TERRAIN_CUSTOM_SEGMENTS)
		{
			GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_ADD_CUSTOM_SEGMENT,true);
			GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
			
			if (selectedNode)
			{
				selectedNode->setDebugDataVisible(0);
				selectedNode=NULL;
			}
			toolstate = TOOL_NONE;
			GUIManager::getInstance()->setElementEnabled(BT_ID_TILE_ROT_LEFT,false);
			GUIManager::getInstance()->setElementEnabled(BT_ID_TILE_ROT_RIGHT,false);
		}
	}

	if(old_app_state == APP_EDIT_TERRAIN_CUSTOM_SEGMENTS)
		GUIManager::getInstance()->setWindowVisible(GCW_CUSTOM_SEGMENT_CHOOSER,false);
	
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
	//if(old_app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
	//	GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECT_CHOOSER,false);


	if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
	{
		//If the tools was in move/rotate mode then set back the current mode as the old tool state
		//if (old_app_state == APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE || old_app_state == APP_EDIT_DYNAMIC_OBJECTS_SCRIPT)
			toolstate = old_do_state;

		if (old_app_state != APP_EDIT_VIEWDRAG)
		{
			GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECT_CHOOSER,true);
			GUIManager::getInstance()->setElementEnabled(BT_ID_DYNAMIC_OBJECTS_MODE,false);
			GUIManager::getInstance()->setStatusText(LANGManager::getInstance()->getText("info_dynamic_objects_mode").c_str());
			//If the up/down mode was last used then reset if
			if (moveupdown)
				moveupdown=false;
		}

	}
	else
	{	
		//Reset the tools state if going outside of the dynamic object edit mode
		if (app_state != APP_EDIT_VIEWDRAG)
		{
			GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECT_CHOOSER,false);
			GUIManager::getInstance()->setElementEnabled(BT_ID_DYNAMIC_OBJECTS_MODE,true);
			toolstate = TOOL_NONE;
		}
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
		Player::getInstance()->setHighLight(true);
		GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);
	}
	else
	{
		GUIManager::getInstance()->setElementEnabled(BT_ID_EDIT_CHARACTER,true);
		
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
		GUIManager::getInstance()->setElementEnabled(BT_ID_PLAYER_EDIT_SCRIPT,false);
	}
	else
	{
		GUIManager::getInstance()->setElementEnabled(BT_ID_PLAYER_EDIT_SCRIPT,true);
		// Find a way to set the script once the user change the mode
		if (old_app_state == APP_EDIT_PLAYER_SCRIPT)
			Player::getInstance()->getObject()->setScript(GUIManager::getInstance()->getEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT));
	}
#endif

	if(app_state == APP_GAMEPLAY_NORMAL)
	{
		GUIManager::getInstance()->setElementVisible(BT_ID_PLAY_GAME,false);
		GUIManager::getInstance()->setElementVisible(BT_ID_STOP_GAME,true);
		//GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_ADD_SEGMENT,false);
		//GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_PAINT_VEGETATION,false);
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
#ifdef EDITOR
		guienv->getRootGUIElement()->getElementFromId(CB_SNAPCOMBO,true)->setVisible(false); //Hide the snap box when playing
		guienv->getRootGUIElement()->getElementFromId(CB_SCREENCOMBO,true)->setVisible(false);
#endif
	}
	else if(app_state < APP_STATE_CONTROL)
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
		GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_PAINT_VEGETATION,true);
		GUIManager::getInstance()->setElementEnabled(BT_ID_TERRAIN_TRANSFORM,true);
#ifdef EDITOR
		guienv->getRootGUIElement()->getElementFromId(CB_SNAPCOMBO,true)->setVisible(true); ///Show the snap box when editing
		guienv->getRootGUIElement()->getElementFromId(CB_SCREENCOMBO,true)->setVisible(true);
#endif
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

	DynamicObject* object=NULL;
#ifdef EDITOR
	DynamicObject* selectedObject=NULL;
#endif

	oldcampos = vector3df(0,0,0);
	vector3df oldrotation = vector3df(0,0,0);
	core::stringw oldscript = L"";

	IGUIListBox* box = NULL; // Combo box pointer


	switch (id)
	{

	case BT_ID_NEW_PROJECT:
		lastScannedPick.pickedNode=NULL;
		if (selectedNode)
		{
			selectedNode->setDebugDataVisible(0);
			selectedNode=NULL;
		}
		GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);

#ifdef DEBUG
		if (!GUIManager::getInstance()->isWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU))
			printf("Context menu is hidden!\n");
#endif

		this->createNewProject();
		// Put back the player object in the list of the dynamic objects
		DynamicObjectsManager::getInstance()->setPlayer();
		this->setAppState(APP_EDIT_LOOK);
		GUIManager::getInstance()->buildSceneObjectList(current_listfilter);
		break;

	case BT_ID_LOAD_PROJECT:
		this->loadProject();
		this->setAppState(APP_EDIT_LOOK);
		GUIManager::getInstance()->buildSceneObjectList(current_listfilter);
		break;

	case BT_ID_SAVE_PROJECT:
		this->saveProjectDialog();
		this->setAppState(APP_EDIT_LOOK);
		break;
#ifdef EDITOR
	case BT_ID_TERRAIN_ADD_SEGMENT:
		this->setAppState(APP_EDIT_TERRAIN_SEGMENTS);
		break;

	case BT_ID_TERRAIN_ADD_EMPTY_SEGMENT:
		this->setAppState(APP_EDIT_TERRAIN_EMPTY_SEGMENTS);
		break;

	case BT_ID_TERRAIN_ADD_CUSTOM_SEGMENT:
		DynamicObjectsManager::getInstance()->setActiveObject(GUIManager::getInstance()->getComboBoxItem(CO_ID_CUSTOM_SEGMENT_OBJ_CHOOSER));
		this->setAppState(APP_EDIT_TERRAIN_CUSTOM_SEGMENTS);
		break;

	case BT_ID_TERRAIN_PAINT_VEGETATION:
		this->setAppState(APP_EDIT_TERRAIN_PAINT_VEGETATION);
		break;

	case BT_ID_TERRAIN_TRANSFORM:
		this->setAppState(APP_EDIT_TERRAIN_TRANSFORM);
		break;

	case BT_ID_DYNAMIC_OBJECTS_MODE:
		{
			DynamicObjectsManager::getInstance()->setActiveObject(GUIManager::getInstance()->getComboBoxItem(CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER));
			this->setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
			if (toolstate==TOOL_NONE)
				toolstate=old_do_state;
		}
		break;

	case BT_ID_DYNAMIC_OBJECT_BT_CANCEL:
		break;

	case BT_ID_DYNAMIC_OBJECT_BT_SPAWN: // Create a new item from the last selected item in the dynamic object
		GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);

		DynamicObjectsManager::getInstance()->createActiveObjectAt(lastMousePick.pickedPos);
		GUIManager::getInstance()->buildSceneObjectList(current_listfilter);
		break;

	case BT_ID_DYNAMIC_OBJECT_BT_REPLACE: // Will replace the model with one from the file selector
		GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);
		loadProject(DF_MODEL);
		GUIManager::getInstance()->buildSceneObjectList(current_listfilter);

	break;

	case BT_ID_DYNAMIC_OBJECT_BT_REPLACE2: // Will replace the model with the one selected in the item template

		GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);


		//----
		if (lastMousePick.pickedNode)
		{
			core::stringc nodeName = lastMousePick.pickedNode->getName();
			if( stringc( nodeName.subString(0,14)) == "dynamic_object" || nodeName.subString(0,16) == "dynamic_walkable" )
			{
				//Tell the dynamic Objects Manager to remove the node
				if (lastMousePick.pickedNode)
				{
					lastMousePick.pickedNode->setDebugDataVisible(0);
					selectedNode=NULL;
				}

				// Keep the "good stuff"
				oldrotation = lastMousePick.pickedNode->getRotation();
				oldscript = DynamicObjectsManager::getInstance()->getScript(lastMousePick.pickedNode->getName());

				DynamicObjectsManager::getInstance()->removeObject(lastMousePick.pickedNode->getName());
				// remove the object for the selection
				lastScannedPick.pickedNode=NULL;
				lastMousePick.pickedNode=NULL;		

				// Create the new object from the template and put the old values back in.
				object = DynamicObjectsManager::getInstance()->createActiveObjectAt(lastMousePick.pickedPos);
				object->setScript(oldscript);
				object->setRotation(oldrotation);
				
				//Reselect the replaced object (in the other modes, in ADD mode selection is done automatically)
				if (toolstate!=TOOL_DO_ADD)
				{
					selectedNode=object->getNode();
					selectedNode->setDebugDataVisible(true ? EDS_BBOX | EDS_SKELETON : EDS_OFF);
				}

				//Once the change is done rebuild the list
				GUIManager::getInstance()->buildSceneObjectList(current_listfilter);
			}
			else //Wrong node type selected
			{
				guienv->addMessageBox(L"No object selected",(L"You need to select an object to replace it."),true);
			}
		}
		else //Nothing selected
		{
			guienv->addMessageBox(L"No object selected",(L"You need to select an object to replace it."),true);
		}
		if (app_state!=APP_EDIT_DYNAMIC_OBJECTS_MODE)
			setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
		break;

	case BT_ID_DYNAMIC_OBJECT_BT_EDITSCRIPTS:
		
		GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);
		
		if (lastMousePick.pickedNode)
		{
			core::stringc nodeName = lastMousePick.pickedNode->getName();

			if( stringc( nodeName.subString(0,14)) == "dynamic_object" || nodeName.subString(0,16) == "dynamic_walkable" )
			{
				selectedObject = DynamicObjectsManager::getInstance()->getObjectByName( stringc(nodeName) );
				GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT,selectedObject->getScript());
				GUIManager::getInstance()->setEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE,"");
				this->setAppState(APP_EDIT_DYNAMIC_OBJECTS_SCRIPT);
			}
			else //Wrong node type selected
			{
				guienv->addMessageBox(L"No object selected",(L"You need to select an object to edit it's script."),true);
			}
		}
		else //Nothing selected
		{
			guienv->addMessageBox(L"No object selected",(L"You need to select an object to edit it's script."),true);
		}


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


		if (lastMousePick.pickedNode)
		{
			core::stringc nodeName = lastMousePick.pickedNode->getName();
			if( stringc( nodeName.subString(0,14)) == "dynamic_object" || nodeName.subString(0,16) == "dynamic_walkable" )
			{
				//Tell the dynamic Objects Manager to remove the node
				if (lastMousePick.pickedNode)
				{
					lastMousePick.pickedNode->setDebugDataVisible(0);
					selectedNode=NULL;
				}
				DynamicObjectsManager::getInstance()->removeObject(lastMousePick.pickedNode->getName());
				// remove the object for the selection
				lastScannedPick.pickedNode=NULL;
				lastMousePick.pickedNode=NULL;		
				GUIManager::getInstance()->buildSceneObjectList(current_listfilter);
			}
			else //Wrong node type selected
			{
				guienv->addMessageBox(L"No object selected",(L"You need to select an object to remove it."),true);
			}
		}
		else //Nothing selected
		{
			guienv->addMessageBox(L"No object selected",(L"You need to select an object to remove it."),true);
		}
		if (app_state!=APP_EDIT_DYNAMIC_OBJECTS_MODE)
			setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
		break;

		//Center the view on the selected object
	case BT_ID_DYNAMIC_OBJECT_BT_CENTER:
		GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);
		if (selectedNode)
		{
			core::vector3df pos = selectedNode->getPosition();
			core::vector3df offset = CameraSystem::getInstance()->getNode()->getPosition();
			core::vector3df calc = pos + (pos - offset);

			CameraSystem::getInstance()->setPosition(calc);
			CameraSystem::getInstance()->getNode()->setTarget(pos);
		}
		else
			guienv->addMessageBox(L"No object selected",(L"You need to select the object to center the view on it."),true);

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
			if (selectedNode)
				DynamicObjectsManager::getInstance()->getObjectByName(selectedNode->getName())->setScript(GUIManager::getInstance()->getEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT));
			
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
		//GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT,false);
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

	case BT_ID_DYNAMIC_OBJECT_INFO: // Expand/Retract the pane for the info of the dynamic object with the button
		GUIManager::getInstance()->setWindowVisible(GCW_DYNAMIC_OBJECT_INFO,false);
		break;

	case BT_ID_TILE_ROT_LEFT: // User pressed the rotate tile left toggle button
		if (toolstate != TOOL_TILEROTATE_LEFT)
		{
			toolstate = TOOL_TILEROTATE_LEFT;
			GUIManager::getInstance()->setElementEnabled(BT_ID_TILE_ROT_RIGHT,false);
		}
		else
			toolstate = TOOL_NONE;
		break;

	case BT_ID_TILE_ROT_RIGHT: // User pressed the rotate tile right toggle button
		if (toolstate != TOOL_TILEROTATE_RIGHT)
		{
			toolstate = TOOL_TILEROTATE_RIGHT;
			GUIManager::getInstance()->setElementEnabled(BT_ID_TILE_ROT_LEFT,false);
		}
		else
			toolstate = TOOL_NONE;
		break;

	case BT_ID_DO_ADD_MODE:
		GUIManager::getInstance()->setElementEnabled(BT_ID_DO_ADD_MODE,true);
		toolstate = TOOL_DO_ADD;
		old_do_state = toolstate;
		if (selectedNode)
		{
			selectedNode->setDebugDataVisible(0);
			selectedNode=NULL;
		}
		toolactivated=false;
		moveupdown=false;
		break;

	case BT_ID_DO_SEL_MODE:
		GUIManager::getInstance()->setElementEnabled(BT_ID_DO_SEL_MODE,true);
		toolstate = TOOL_DO_SEL;
		old_do_state = toolstate;
		if (selectedNode)
			GUIManager::getInstance()->updateNodeInfos(selectedNode);
		toolactivated=false;
		moveupdown=false;
		break;

	case BT_ID_DO_MOV_MODE:
		GUIManager::getInstance()->setElementEnabled(BT_ID_DO_MOV_MODE,true);
		toolstate = TOOL_DO_MOV;
		old_do_state = toolstate;
		if (selectedNode)
			GUIManager::getInstance()->updateNodeInfos(selectedNode);
		toolactivated=false;
		moveupdown=false;
		break;

	case BT_ID_DO_ROT_MODE:
		toolstate = TOOL_DO_ROT;
		old_do_state = toolstate;
		GUIManager::getInstance()->setElementEnabled(BT_ID_DO_ROT_MODE,true);
		if (selectedNode)
			GUIManager::getInstance()->updateNodeInfos(selectedNode);
		toolactivated=false;
		moveupdown=false;
		break;

	case BT_ID_DO_SCA_MODE:
		GUIManager::getInstance()->setElementEnabled(BT_ID_DO_SCA_MODE,true);
		if (selectedNode)
			GUIManager::getInstance()->updateNodeInfos(selectedNode);
		toolstate = TOOL_DO_SCA;
		old_do_state = toolstate;
		toolactivated=false;
		moveupdown=false;
		break;

	case BT_PLAYER_START:
		this->playGame();
		break;

	case BT_PLAYER_CONFIG:
		GUIManager::getInstance()->showConfigWindow();

		break;

	default:
		break;
	}
}

// Stuff in editor only
#ifdef EDITOR

void App::hideEditGui()
{
	GUIManager::getInstance()->setConsoleText(L"Console ready!",SColor(255,0,0,255));
}

std::vector<stringw> App::getAbout()
{
	return LANGManager::getInstance()->getAboutText();
}

//Checkbox events
void App::eventGuiCheckbox(s32 id)
{
	switch (id)
	{
		case CB_ID_POS_X:
			break;

		case CB_ID_POS_Y:
			break;

		case CB_ID_POS_Z:
			break;

		case CB_ID_ROT_X:
			break;

		case CB_ID_ROT_Y:
			break;

		case CB_ID_ROT_Z:
			break;

		case CB_ID_SCA_X:
			break;

		case CB_ID_SCA_Y:
			break;

		case CB_ID_SCA_Z:
			break;

		default:
		break;
	}
}

//Check events coming from combo boxes AND LIST BOXES
void App::eventGuiCombobox(s32 id)
{
	s32 index = 0;
	core::stringw item = L"";
	DynamicObject * object = NULL;

	IGUIComboBox* selectedbox = NULL;
	switch (id)
	{
		
	// Selection in the list from the dynamic object selection
	case CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER:
		DynamicObjectsManager::getInstance()->setActiveObject(GUIManager::getInstance()->getComboBoxItem(CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER));
		GUIManager::getInstance()->getInfoAboutModel();
		GUIManager::getInstance()->updateDynamicObjectPreview();
		break;

	case CO_ID_DYNAMIC_OBJECT_OBJLIST_CATEGORY:
		GUIManager::getInstance()->updateCurrentCategory();
		GUIManager::getInstance()->getInfoAboutModel();
		break;

	case CO_ID_DYNAMIC_OBJECT_OBJ_CATEGORY:
		GUIManager::getInstance()->UpdateGUIChooser();
		DynamicObjectsManager::getInstance()->setActiveObject(GUIManager::getInstance()->getComboBoxItem(CO_ID_DYNAMIC_OBJECT_OBJ_CHOOSER));
		GUIManager::getInstance()->getInfoAboutModel();
		GUIManager::getInstance()->updateDynamicObjectPreview();
		break;

	// Selection in the list from the custom segment selection
	case CO_ID_CUSTOM_SEGMENT_OBJ_CHOOSER:
		DynamicObjectsManager::getInstance()->setActiveObject(GUIManager::getInstance()->getComboBoxItem(CO_ID_CUSTOM_SEGMENT_OBJ_CHOOSER));
		GUIManager::getInstance()->getInfoAboutModel(LIST_SEGMENT);
		//GUIManager::getInstance()->updateDynamicObjectPreview();
		break;

	case CO_ID_CUSTOM_TILES_OBJLIST_CATEGORY:
		GUIManager::getInstance()->updateCurrentCategory(LIST_SEGMENT);
		GUIManager::getInstance()->getInfoAboutModel(LIST_SEGMENT);
		break;

	case CO_ID_CUSTOM_SEGMENT_CATEGORY:
		GUIManager::getInstance()->UpdateGUIChooser(LIST_SEGMENT);
		DynamicObjectsManager::getInstance()->setActiveObject(GUIManager::getInstance()->getComboBoxItem(CO_ID_CUSTOM_SEGMENT_OBJ_CHOOSER));
		GUIManager::getInstance()->getInfoAboutModel(LIST_SEGMENT);
		//GUIManager::getInstance()->updateDynamicObjectPreview();
		break;

	case CO_ID_ACTIVE_SCENE_LIST:
		index = GUIManager::getInstance()->getListBox(CO_ID_ACTIVE_SCENE_LIST)->getSelected();
		item = GUIManager::getInstance()->getListBox(CO_ID_ACTIVE_SCENE_LIST)->getListItem(index);
		if (selectedNode)
		{
			selectedNode->setDebugDataVisible(0);
			selectedNode=NULL;
		}

		object = DynamicObjectsManager::getInstance()->getObjectByName(core::stringc(item)); 
		if (object)
		{
			selectedNode = object->getNode();
			lastMousePick.pickedNode = object->getNode();
		} else
			GUIManager::getInstance()->setConsoleText(core::stringw(L"Failed to retrieve this object: ").append(core::stringw(item)));
		
		if (selectedNode)
		{
			selectedNode->setDebugDataVisible(true ? EDS_BBOX | EDS_SKELETON : EDS_OFF);
		}
		break;

	case CO_ID_ACTIVE_LIST_FILTER: // User activate a item filter to get a new list of objects to select
		item = GUIManager::getInstance()->getComboBoxItem(CO_ID_ACTIVE_LIST_FILTER);
		current_listfilter = OBJECT_TYPE_NONE; 
		if (item == core::stringc("NPC"))
			current_listfilter = OBJECT_TYPE_NPC; 
		if (item == core::stringc("Props"))
			current_listfilter = OBJECT_TYPE_NON_INTERACTIVE; 
		if (item == core::stringc("Interactive Props"))
			current_listfilter = OBJECT_TYPE_INTERACTIVE;
		if (item == core::stringc("Walkables"))
			current_listfilter = OBJECT_TYPE_WALKABLE; 
		if (item == core::stringc("Loot"))
			current_listfilter = OBJECT_TYPE_LOOT; 

		GUIManager::getInstance()->buildSceneObjectList(current_listfilter);
		break;

	case CB_SCREENCOMBO:
		selectedbox = ((IGUIComboBox*)guienv->getRootGUIElement()->getElementFromId(CB_SCREENCOMBO,true));
		if (selectedbox)
		{
			combobox_used=true;
			vector3df initpos = CameraSystem::getInstance()->editCamMaya->getAbsolutePosition();
			vector3df inittar = CameraSystem::getInstance()->editCamMaya->getTarget();
			f32 initdist = initpos.getDistanceFrom(inittar);
			u32 value=selectedbox->getItemData(selectedbox->getSelected());
			vector3df newpos = inittar;
#ifdef DEBUG
			printf("Here is the result value of the box: %i\n",value);
#endif
			matrix4 projMat; //MAtrix projection 
			switch (value)
			{
			case 1: //TOP
				//CameraSystem::getInstance()->getNode()->setPosition(vector3df(0,-1000,0));
				//CameraSystem::getInstance()->getNode()->setTarget(vector3df(0,0,0));
				newpos.Y+=initdist;
				newpos.Z-=0.05f;
				CameraSystem::getInstance()->setMAYAPos(newpos);
				CameraSystem::getInstance()->setMAYATarget(inittar);
#ifdef DEBUG
				printf("View selected is TOP\n");
#endif

				break;
			case 2: //Bottom
				newpos.Y-=initdist;
				newpos.Z+=0.05f;
				CameraSystem::getInstance()->setMAYAPos(newpos);
				CameraSystem::getInstance()->setMAYATarget(inittar);
				break;
			case 3: //Left
				newpos.X-=initdist;
				CameraSystem::getInstance()->setMAYAPos(newpos);
				CameraSystem::getInstance()->setMAYATarget(inittar);
				break;
			case 4: // right
				newpos.X+=initdist;
				CameraSystem::getInstance()->setMAYAPos(newpos);
				CameraSystem::getInstance()->setMAYATarget(inittar);
				break;
			case 5: //Front
				newpos.Z-=initdist;
				CameraSystem::getInstance()->setMAYAPos(newpos);
				CameraSystem::getInstance()->setMAYATarget(inittar);
				break;
			case 6: // Back
				newpos.Z+=initdist;
				CameraSystem::getInstance()->setMAYAPos(newpos);
				CameraSystem::getInstance()->setMAYATarget(inittar);
				break;
			case 7: // Orthographic view
				//Will need the change the distance moving since there is no perspective
				projMat.buildProjectionMatrixOrthoLH((f32)device->getVideoDriver()->getScreenSize().Width,(f32)device->getVideoDriver()->getScreenSize().Height,1,15000);
				CameraSystem::getInstance()->getNode()->setProjectionMatrix(projMat,true);
				break;
			case 8: // Back to perspective
				projMat.buildProjectionMatrixPerspectiveFovRH((f32)device->getVideoDriver()->getScreenSize().Width,(f32)device->getVideoDriver()->getScreenSize().Height,1,15000);
				CameraSystem::getInstance()->getNode()->setProjectionMatrix(projMat,false);
				CameraSystem::getInstance()->getNode()->setFOV(0.45f);
				break;

			default: 
				break;
			}
		}

		break;

	case CB_SNAPCOMBO: // Get the combo box data to set the snap distance
		
		selectedbox = ((IGUIComboBox*)guienv->getRootGUIElement()->getElementFromId(CB_SNAPCOMBO,true));
		if (selectedbox)
		{
			combobox_used=true;
			currentsnapping=(f32)selectedbox->getItemData(selectedbox->getSelected());
			if (currentsnapping==0) // if 0 is selected, the snapping is back to default
				currentsnapping=64;
		
			TerrainManager::getInstance()->setEmptyTileGridScale(currentsnapping);
		}
		
		break;


	default:
		break;

	}
}

//Check the ENTER events coming from edit boxes
void App::eventGuiEditBox(s32 id)
{
	core::stringc text="";
	core::vector3df newposition=vector3df(0,0,0);
	core::vector3df newrotation=vector3df(0,0,0);
	core::vector3df newscale=vector3df(1,1,1);
	if (selectedNode)
	{
		newposition=selectedNode->getPosition();
		newrotation=selectedNode->getRotation();
		newscale=selectedNode->getScale();
	}

	switch (id)
	{

	default:
		break;
	}

}



#endif

void App::eventGuiSpinbox(s32 id)
{
	f32 value=0.0f;
	core::vector3df newposition=vector3df(0,0,0);
	core::vector3df newrotation=vector3df(0,0,0);
	core::vector3df newscale=vector3df(1,1,1);
	if (selectedNode)
	{
		newposition=selectedNode->getPosition();
		newrotation=selectedNode->getRotation();
		newscale=selectedNode->getScale();
	}

	switch (id)
	{
	case TI_ID_POS_X:
		value=((IGUISpinBox *)guienv->getRootGUIElement()->getElementFromId(TI_ID_POS_X,true))->getValue();
		newposition.X=value;
		if (selectedNode)
		{
			selectedNode->setPosition(newposition);
		}
		break;

	case TI_ID_POS_Y:

		value=((IGUISpinBox *)guienv->getRootGUIElement()->getElementFromId(TI_ID_POS_Y,true))->getValue();
		newposition.Y=value;
		if (selectedNode)
		{
			selectedNode->setPosition(newposition);
		}
		break;

	case TI_ID_POS_Z:
		value=((IGUISpinBox *)guienv->getRootGUIElement()->getElementFromId(TI_ID_POS_Z,true))->getValue();
		newposition.Z=value;
		if (selectedNode)
		{
			selectedNode->setPosition(newposition);
		}
		break;

	case TI_ID_ROT_X:
		value=((IGUISpinBox *)guienv->getRootGUIElement()->getElementFromId(TI_ID_ROT_X,true))->getValue();
		newrotation.X=value;
		if (selectedNode)
		{
			selectedNode->setRotation(newrotation);
		}
		break;

	case TI_ID_ROT_Y:
		value=((IGUISpinBox *)guienv->getRootGUIElement()->getElementFromId(TI_ID_ROT_Y,true))->getValue();
		newrotation.Y=value;
		if (selectedNode)
		{
			selectedNode->setRotation(newrotation);
		}
		break;

	case TI_ID_ROT_Z:
		value=((IGUISpinBox *)guienv->getRootGUIElement()->getElementFromId(TI_ID_ROT_Z,true))->getValue();
		newrotation.Z=value;
		if (selectedNode)
		{
			selectedNode->setRotation(newrotation);
		}
		break;

	case TI_ID_SCA_X:
		value=((IGUISpinBox *)guienv->getRootGUIElement()->getElementFromId(TI_ID_SCA_X,true))->getValue();
		newscale.X=value;
		//in case the user enter strange input (should get 0). 0 as a scale is not good.
		//if (newscale.X==0)
		//	newscale.X=1.0f;

		if (selectedNode)
		{
			selectedNode->setScale(newscale);
		}
		break;

	case TI_ID_SCA_Y:
		value=((IGUISpinBox *)guienv->getRootGUIElement()->getElementFromId(TI_ID_SCA_Y,true))->getValue();
		newscale.Y=value;
		if (selectedNode)
		{
			selectedNode->setScale(newscale);
		}
		break;

	case TI_ID_SCA_Z:
		value=((IGUISpinBox *)guienv->getRootGUIElement()->getElementFromId(TI_ID_SCA_Z,true))->getValue();
		newscale.Z=value;
		if (selectedNode)
		{
			selectedNode->setScale(newscale);
		}
		break;

	default:
		break;
	}

}

void App::setScreenSize(dimension2d<u32> size)
{
	if (device->run())
	{
		GUIManager::getInstance()->updateGuiPositions(size);
		screensize = size;
		CameraSystem::getInstance()->fixRatio(driver);
	}
}

dimension2d<u32> App::getScreenSize()
{
	return screensize;
}

void App::eventKeyPressed(s32 key)
{
	bool visible=false; //Used to display hide the gameplay toolbar
	switch (key)
	{


	case KEY_KEY_Q:
		if (app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
		{
			toolstate=TOOL_DO_ADD;
			old_do_state = toolstate;
			if (selectedNode)
			{
				selectedNode->setDebugDataVisible(0);
				selectedNode=NULL;
				toolactivated=false;
				moveupdown=false;
			}
			GUIManager::getInstance()->setElementEnabled(BT_ID_DO_ADD_MODE,true);
		}
		break;

	case KEY_KEY_W:
		if (app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
		{
			toolstate=TOOL_DO_SEL;
			old_do_state = toolstate;
			toolactivated=false;
			moveupdown=false;
			GUIManager::getInstance()->setElementEnabled(BT_ID_DO_SEL_MODE,true);
		}
		break;

	case KEY_KEY_E:
		if (app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
		{
			toolstate=TOOL_DO_MOV;
			old_do_state = toolstate;
			toolactivated=false;
			moveupdown=false;
			GUIManager::getInstance()->setElementEnabled(BT_ID_DO_MOV_MODE,true);
		}
		break;

	case KEY_KEY_R:
		if (app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
		{
			toolstate=TOOL_DO_ROT;
			old_do_state = toolstate;
			GUIManager::getInstance()->setElementEnabled(BT_ID_DO_ROT_MODE,true);
			toolactivated=false;
			moveupdown=false;
		}
		break;

	case KEY_KEY_T:
		if (app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
		{
			toolstate=TOOL_DO_SCA;
			old_do_state = toolstate;
			GUIManager::getInstance()->setElementEnabled(BT_ID_DO_SCA_MODE,true);
			toolactivated=false;
			moveupdown=false;
		}
		break;

	case KEY_F5:
		if(app_state == APP_EDIT_DYNAMIC_OBJECTS_SCRIPT && !EventReceiver::getInstance()->isKeyPressed(key))
			LuaGlobalCaller::getInstance()->doScript(GUIManager::getInstance()->getEditBoxText(EB_ID_DYNAMIC_OBJECT_SCRIPT));
		break;

	case KEY_F10: // Clear the test rays
		if (raytester)
			raytester->clearAll();
		break;
	case KEY_F9:
		if (raytester)
			raytester->enable(true);
		break;
	case KEY_F8:
		if (raytester)
			raytester->enable(false);
		break;


	case KEY_RETURN:
		if (app_state == APP_WAIT_DIALOG)
			break;
	

	case KEY_LCONTROL:

		if (EventReceiver::getInstance()->isKeyPressed(KEY_LCONTROL))
			snapfunction=true;
		else
			snapfunction=false;
		break;

	case KEY_ESCAPE:
#ifndef EDITOR
		if (EventReceiver::getInstance()->isKeyPressed(KEY_ESCAPE))
		{
			visible=guienv->getRootGUIElement()->getElementFromId(WIN_GAMEPLAY,true)->isVisible();
			guienv->getRootGUIElement()->getElementFromId(WIN_GAMEPLAY,true)->setVisible(!visible);
		}
#endif

		break;


	default:
		break;
	}
}

// Behaviors with the detected mouse button
// This is called from mouse events
// (08/02/13) This will need to be split in methods as the code is getting too big here.
void App::eventMousePressed(s32 mouse)
{
	switch(mouse)
	{///TODO: colocar acoes mais comuns acima e menos comuns nos elses

	case EMIE_LMOUSE_LEFT_UP:
		{
			if (toolstate==TOOL_DO_MOV || toolstate==TOOL_DO_ROT || toolstate==TOOL_DO_SCA)
			{
				//Deactivate the tool if the mouse buttons are released
				if (toolactivated)
					toolactivated=false;
			}
		}
		break;

	case EMIE_RMOUSE_LEFT_UP:
		{
			if (toolstate==TOOL_DO_MOV || toolstate==TOOL_DO_ROT || toolstate==TOOL_DO_SCA)
			{
				//Deactivate the tool if the mouse buttons are released
				toolactivated=false;
				moveupdown=false;
				
			}
		}
		break;

	case EMIE_LMOUSE_PRESSED_DOWN://Left button (default)
		if( cursorIsInEditArea())
		{
			if(app_state == APP_EDIT_TERRAIN_SEGMENTS)
			{
				TerrainManager::getInstance()->createSegment(this->getMousePosition3D().pickedPos / TerrainManager::getInstance()->getScale());
				return;
			}
			else if(app_state == APP_EDIT_TERRAIN_EMPTY_SEGMENTS)
			{
				TerrainManager::getInstance()->createSegment(this->getMousePosition3D().pickedPos / TerrainManager::getInstance()->getScale(), true);
				return;
			}
			else if(app_state == APP_EDIT_TERRAIN_CUSTOM_SEGMENTS)
			{
				if (toolstate==TOOL_NONE)
				{
					core::stringc meshfile=DynamicObjectsManager::getInstance()->getActiveObject()->meshFile;
					ISceneNode * nod = TerrainManager::getInstance()->createCustomSegment(this->getMousePosition3D().pickedPos / TerrainManager::getInstance()->getScale(),meshfile);
					
					if (DynamicObjectsManager::getInstance()->getActiveObject()->materials.size()>0 && nod)
					{
						ShaderCallBack::getInstance()->setMaterials(nod,DynamicObjectsManager::getInstance()->getActiveObject()->materials);
					}
					return;
				}
				if (toolstate==TOOL_TILEROTATE_LEFT)
				{
					TerrainManager::getInstance()->rotateLeft(this->getMousePosition3D().pickedPos / TerrainManager::getInstance()->getScale());
					return;
				}
				if (toolstate==TOOL_TILEROTATE_RIGHT)
				{
					TerrainManager::getInstance()->rotateRight(this->getMousePosition3D().pickedPos / TerrainManager::getInstance()->getScale());
					return;
				}
			}
			else if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
			{
				MousePick mousePick = getMousePosition3D();

				lastMousePick = mousePick;
				stringc nodeName = "";

				if (combobox_used)
				{
					combobox_used=false;
					return;
				}
				
				// Mouse operation in ADD mode
				if (toolstate==TOOL_DO_ADD)
				{

					if (selectedNode) //Unselect and remove the selected node in mode changes
					{
						selectedNode->setDebugDataVisible(0); //Remove selection
					}

					selectedNode=NULL;

					// To add an object, the picked node need to return at least a tile. 
					// If pickednode is empty then the user clicked outside the area
					if (mousePick.pickedNode)
					{
						nodeName = mousePick.pickedNode->getName();
						//if you click on a Dynamic Object then open the context menu
						if( stringc( nodeName.subString(0,14)) == "dynamic_object" || nodeName.subString(0,16) == "dynamic_walkable" )
						{
							selectedNode=mousePick.pickedNode;
#ifdef DEBUG
							cout << "PROP:" << nodeName.c_str() << endl;
#endif
							// Toggle the context menu
							GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,
								!GUIManager::getInstance()->isWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU));
						}
						else//create a new copy of active dynamic object at the clicked position
						{
							// If the context menu is still open close it since we want to create a object
							if (GUIManager::getInstance()->isWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU))
								GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);

							DynamicObject* tmpDObj = DynamicObjectsManager::getInstance()->createActiveObjectAt(mousePick.pickedPos);
							GUIManager::getInstance()->buildSceneObjectList(current_listfilter);
#ifdef DEBUG
							cout << "DEBUG : DYNAMIC_OBJECTS : NEW " << tmpDObj->getName().c_str() << " CREATED!"  << endl;
#endif
							
						}
					}
					return;
				} 
				
				// Mouse operation in SEL,MOV,ROT and SCA modes
				if (toolstate==TOOL_DO_SEL || toolstate==TOOL_DO_MOV || toolstate==TOOL_DO_ROT || toolstate==TOOL_DO_SCA) // Enable selection for theses modes
				{
					// Since we don't have multi section implemented as of now, the previous node should be NULL
					if (selectedNode) // There was a node selected before
					{
						selectedNode->setDebugDataVisible(0); // Unselect it
					}
					selectedNode=NULL;
					
					// Create the selected node if there was a node picked
					if (mousePick.pickedNode)
					{
						nodeName = mousePick.pickedNode->getName();
					
						// Need to filter some nodes names as "terrain" or other objects might be selected.
						if( stringc( nodeName.subString(0,14)) == "dynamic_object" || nodeName.subString(0,16) == "dynamic_walkable" )
						{
							selectedNode=mousePick.pickedNode;	
							selectedNode->setDebugDataVisible(true ? EDS_BBOX | EDS_SKELETON : EDS_OFF);
							GUIManager::getInstance()->updateNodeInfos(selectedNode); //Put infos
						}
						else //Invalid node for selection
						{
							selectedNode=NULL;
							GUIManager::getInstance()->updateNodeInfos(selectedNode); //Put 0 in the node infos
						}
					} else //Clicked outside on nothing
					{
						selectedNode=NULL;
						GUIManager::getInstance()->updateNodeInfos(selectedNode); //Put 0 in the node infos
					}

					if (selectedNode)
					{
						initialposition=selectedNode->getPosition();
						initialrotation=selectedNode->getRotation();
						initialscale=selectedNode->getScale();
						mousepos=device->getCursorControl()->getPosition();
						toolactivated=!toolactivated; //Toggle the state of the tool
						return;
					}

				}
			}
			else if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE)
			{
				setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
				return;
			}
		}
		break;

	// Right button (Action the same as the left button)
	case EMIE_RMOUSE_PRESSED_DOWN:
		if( cursorIsInEditArea())
		{
			if(app_state == APP_EDIT_TERRAIN_SEGMENTS)
			{
				TerrainManager::getInstance()->removeSegment(this->getMousePosition3D().pickedPos / TerrainManager::getInstance()->getScale());
				return;
			}
			else if(app_state == APP_EDIT_TERRAIN_CUSTOM_SEGMENTS)
			{
				if (toolstate==TOOL_NONE)
					TerrainManager::getInstance()->removeSegment(this->getMousePosition3D().pickedPos / TerrainManager::getInstance()->getScale(), true);
				return;
			}
			else if(app_state == APP_EDIT_TERRAIN_EMPTY_SEGMENTS)
			{
				TerrainManager::getInstance()->removeEmptySegment(this->getMousePosition3D().pickedPos / TerrainManager::getInstance()->getScale());
				return;
			}
			else if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE)
			{
				MousePick mousePick = getMousePosition3D();

				lastMousePick = mousePick;
				stringc nodeName = "";
				// Check for a node to prevent a crash (need to get the name of the node)
				
				
				if (mousePick.pickedNode != NULL && toolstate==TOOL_DO_ADD) // Add mode right button functionnality
				{
					nodeName = mousePick.pickedNode->getName();

					//if you click on a Dynamic Object then open his properties
					if( stringc( nodeName.subString(0,14)) == "dynamic_object" || nodeName.subString(0,16) == "dynamic_walkable" )
					{
						cout << "PROP:" << nodeName.c_str() << endl;

						// Toggle the context menu
						GUIManager::getInstance()->setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,
							!GUIManager::getInstance()->isWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU));
					}
					return;
				}
				
				if ((toolstate==TOOL_DO_MOV || toolstate==TOOL_DO_ROT || toolstate==TOOL_DO_SCA))
				{

					// Since we don't have multi section implemented as of now, the previous node should be NULL
					if (selectedNode) // There was a node selected before
					{
						selectedNode->setDebugDataVisible(0); // Unselect it
					}
					selectedNode=NULL;
					
					// Create the selected node if there was a node picked
					if (mousePick.pickedNode)
					{
						nodeName = mousePick.pickedNode->getName();
					
						// Need to filter some nodes names as "terrain" or other objects might be selected.
						if( stringc( nodeName.subString(0,14)) == "dynamic_object" || nodeName.subString(0,16) == "dynamic_walkable" )
						{
							selectedNode=mousePick.pickedNode;	
							selectedNode->setDebugDataVisible(true ? EDS_BBOX | EDS_SKELETON : EDS_OFF);
							GUIManager::getInstance()->updateNodeInfos(selectedNode); //Put infos
						}
						else //Invalid node for selection
						{
							selectedNode=NULL;
							GUIManager::getInstance()->updateNodeInfos(selectedNode); //Put 0 in the node infos
						}
					} else //Clicked outside on nothing
					{
						selectedNode=NULL;
						GUIManager::getInstance()->updateNodeInfos(selectedNode); //Put 0 in the node infos
					}

					if (selectedNode)
					{
						initialposition=selectedNode->getPosition();
						mousepos=device->getCursorControl()->getPosition();
						toolactivated=true;
						moveupdown=true;
					}
					return;
				}


			}
			else if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE)
			{
				// Move the object up/down using the mouse Y axis
				// Pressing back the button release the mode
				mousepos=device->getCursorControl()->getPosition();
				moveupdown=!moveupdown;
				if (!moveupdown)
					setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
			}
		}
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
		lastMousePick.pickedNode->setRotation(vector3df(0,value*5,0)+oldRot);
	}
	if(app_state == APP_EDIT_CHARACTER)
	{
		vector3df oldRot = Player::getInstance()->getObject()->getRotation();
		Player::getInstance()->getObject()->setRotation(vector3df(0,value*5,0)+oldRot);
	}
	// This will allow zoom in/out in editor mode
	if	(app_state != APP_EDIT_CHARACTER &&
		app_state != APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE &&
		app_state != APP_EDIT_ABOUT &&
		app_state != APP_EDIT_DYNAMIC_OBJECTS_SCRIPT &&
		app_state != APP_EDIT_SCRIPT_GLOBAL &&
		app_state != APP_EDIT_PLAYER_SCRIPT &&
		app_state != APP_EDIT_WAIT_GUI &&
		app_state != APP_WAIT_FILEREQUEST &&
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
		if (app_state>APP_STATE_CONTROL)
			CameraSystem::getInstance()->setCameraHeight(value);
	}
}

void App::eventMessagebox(gui::EGUI_EVENT_TYPE type)
{

	// When editing terrain segment, on "yes" on the message box will 
	// Delete the "tagged" terrain segment
	if (type==EGET_MESSAGEBOX_YES && app_state==APP_EDIT_TERRAIN_SEGMENTS)
		TerrainManager::getInstance()->deleteTaggedSegment();
}

App* App::getInstance()
{
	static App *instance = 0;
	if (!instance) instance = new App();
	return instance;
}
//! Get the 3D mouse coordinate on the ground or object (ray test)
MousePick App::getMousePosition3D(int id)
{

	// Initialize the data
	MousePick result;
	result.pickedNode=NULL;
	result.pickedPos=vector3df(0,0,0);

	core::vector3df intersection=vector3df(0,0,0);
	core::triangle3df hitTriangle=triangle3df(vector3df(0,0,0),vector3df(0,0,0),vector3df(0,0,0));

	ISceneNode* tempNode=NULL;

	// Get the cursor 2D coordinates
	position2d<s32> pos=device->getCursorControl()->getPosition();

	// For the ray test, we should hide the player (And the decors element that we don't want to select)
	Player::getInstance()->getObject()->getNode()->setVisible(false);
	if (app_state== APP_GAMEPLAY_NORMAL)
		DynamicObjectsManager::getInstance()->setObjectsVisible(OBJECT_TYPE_NON_INTERACTIVE, false);	

	line3df ray = smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(pos, smgr->getActiveCamera());
	tempNode = smgr->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray,
		intersection,
		hitTriangle,
		id);
	
	//ray.start = ray.start.getInterpolated(ray.end, 0.5f);
	// Show back the player once the ray test is done
	Player::getInstance()->getObject()->getNode()->setVisible(true);

	if (app_state == APP_GAMEPLAY_NORMAL)
		DynamicObjectsManager::getInstance()->setObjectsVisible(OBJECT_TYPE_NON_INTERACTIVE, true);


	if(tempNode!=NULL)
	{
		// Ray test passed, returning the results
		result.pickedPos = intersection;
		result.pickedNode = tempNode;
		raytester->addRay(ray,true);
		return result;
	
	}
	else
	{
		// Failed the ray test, returning the previous results
		if (intersection!=vector3df(0,0,0))
			result.pickedPos = intersection;
		else
			result.pickedPos = lastMousePick.pickedPos;
		
		result.pickedNode = NULL;

		// Send the ray to the raytester (drawing lines to see the failed ray)
		raytester->addRay(ray,false);
	
		//printf ("Failed the screen ray test! Picking old values., ray len is: %f \n",len);
		return result;
	}
}


//! Display a "debug" box over a selected node
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
			//Should be able to select the walkable in editor mode
			if( stringc( nodeName.subString(0,14)) == "dynamic_object" || nodeName.subString(0,16) == "dynamic_walkable" )
			{
				selectedNode=mousePick.pickedNode;
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
				selectedNode=NULL;
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
	vsync = false;
	antialias = false;
	silouette = false;

	language = "en-us";
	TerrainManager::getInstance()->setTileMeshName("../media/baseTerrain.obj");
	TerrainManager::getInstance()->setTerrainTexture(0,"../media/L0.jpg");
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

#ifdef DEBUG
	cout << "DEBUG : XML : LOADING CONFIGURATION : " << endl;
#endif

	TiXmlElement* root = doc.FirstChildElement( "IrrRPG_Builder_Config" );

	if ( root )
	{
		if( atof(root->Attribute("version"))!=APP_VERSION )
		{
#ifdef DEBUG
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

			stringc vsyncresult = resXML->ToElement()->Attribute("vsync");
			if (vsyncresult=="true")
				vsync=true;

			stringc antialiasresult = resXML->ToElement()->Attribute("antialias");
			if (antialiasresult=="true")
				antialias=true;

			stringc silouetteresult = resXML->ToElement()->Attribute("silouette");
			if (silouetteresult=="true")
				silouette=true;

			if (resizable && fullScreen)
			{
				IrrlichtDevice * tempdevice = createDevice(EDT_NULL,dimension2d<u32>(640,480), 16, false, false, false, 0);
				screensize = tempdevice->getVideoModeList()->getDesktopResolution();
				tempdevice->closeDevice();
				tempdevice->drop();
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
			stringc layer4 = groundXML->ToElement()->Attribute("layer4");
			f32 scale = (f32)atof(groundXML->ToElement()->Attribute("scale"));
			TerrainManager::getInstance()->setTerrainTexture(0,layer0);
			TerrainManager::getInstance()->setTerrainTexture(1,layer1);
			TerrainManager::getInstance()->setTerrainTexture(2,layer2);
			TerrainManager::getInstance()->setTerrainTexture(3,layer3);
			TerrainManager::getInstance()->setTerrainTexture(4,layer4);
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
#ifdef DEBUG
			printf("The map name is: %s\n",mapname.c_str());
#endif
			///TODO: we are just loading ocean seetings, we need to set it!
		}
#endif
	}
	else
	{
#ifdef DEBUG
		cout << "DEBUG : XML : THIS FILE IS NOT A IRRRPG BUILDER PROJECT!" << endl;
#endif

		return false;
	}

#ifdef DEBUG
	cout << "DEBUG : XML : PROJECT LOADED! "<< endl;
#endif

	///TODO:CLEAR PROJECT IF NOT RETURN TRUE ON LOAD PROJECT FROM XML

	return true;
}

void App::setupDevice(IrrlichtDevice* IRRdevice)
{

	loadConfig();
	irr::SIrrlichtCreationParameters deviceConfig;

	if (!IRRdevice)
	{
		if (antialias) // Set 4x antialias mode if supported
			deviceConfig.AntiAlias = 4;
		else 
			deviceConfig.AntiAlias = 0;

		deviceConfig.Bits = 32;
		deviceConfig.DriverType = EDT_OPENGL;
		deviceConfig.Fullscreen = fullScreen;
		deviceConfig.Vsync = vsync;
		deviceConfig.WindowSize = screensize;

		device = createDeviceEx(deviceConfig);
		this->device->setResizable(resizable);
		device->setWindowCaption(L"IrrRPG Builder - Alpha SVN release 0.3 (aug 2014)");
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
#ifdef DEBUG
		printf("ERROR: Device is NULL, please call SetupDevice first!");
#endif
		exit(0);
	}
	return device;
}


void App::playGame()
{

#ifndef EDITOR 
	bool visible=false;
	visible=guienv->getRootGUIElement()->getElementFromId(WIN_LOADER,true)->isVisible();
	guienv->getRootGUIElement()->getElementFromId(WIN_LOADER,true)->setVisible(!visible);
#endif
	
	if (app_state<APP_STATE_CONTROL)
	{
		EffectsManager::getInstance()->updateSkydome();
		TerrainManager::getInstance()->setEmptyTileVisible(false);
		//oldcampos = Player::getInstance()->getObject()->getPosition();
		oldcampos = CameraSystem::getInstance()->editCamMaya->getPosition();
		oldcamtar = CameraSystem::getInstance()->editCamMaya->getTarget();
		LuaGlobalCaller::getInstance()->storeGlobalParams();

		//DynamicObjectsManager::getInstance()->displayShadow(true);
		CameraSystem::getInstance()->setCamera(1);
		// setback the fog as before (will need to check with LUA)
		driver->setFog(SColor(0,220,220,255),EFT_FOG_LINEAR,300,5000);
		smgr->getActiveCamera()->setFarValue(5000.0f);

		old_state = app_state;
		this->setAppState(APP_GAMEPLAY_NORMAL);
		DynamicObjectsManager::getInstance()->showDebugData(false); //DynamicObjectsManager::getInstance()->showDebugData(false);


		
		// Execute the scripts in the dynamic objects
		DynamicObjectsManager::getInstance()->initializeAllScripts();

		//LuaGlobalCaller::getInstance()->usePlayerItem("onLoad");

		// Need to evaluate if it's needed to have displaying debug data for objects (could be done with selection instead)
		//DynamicObjectsManager::getInstance()->showDebugData(false);
		//TerrainManager::getInstance()->showDebugData(false);

		// Reset the last "walk target" as the game restart.
		Player::getInstance()->getObject()->setWalkTarget(Player::getInstance()->getObject()->getPosition());
		DynamicObjectsManager::getInstance()->resetObjectsWalkTarget(OBJECT_TYPE_NPC);


		GUIManager::getInstance()->setElementVisible(ST_ID_PLAYER_LIFE,true);
		LuaGlobalCaller::getInstance()->doScript(scriptGlobal);

	}
}

void App::stopGame()
{
	if (app_state>APP_STATE_CONTROL)
	{
		EffectsManager::getInstance()->turnOffSkydome();
		DynamicObjectsManager::getInstance()->objectsToIdle();
		LuaGlobalCaller::getInstance()->restoreGlobalParams();
		GlobalMap::getInstance()->clearGlobals();


		TerrainManager::getInstance()->setEmptyTileVisible(true);

		DynamicObjectsManager::getInstance()->clearAllScripts();
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
		CameraSystem::getInstance()->editCamMaya->setFarValue(50000.0f);
		//CameraSystem::getInstance()->setPosition(vector3df(oldcampos));

		driver->setFog(SColor(0,255,255,255),EFT_FOG_LINEAR,300,999100);
		DynamicObjectsManager::getInstance()->objectsToIdle();


	}
}

void App::update()
{

	// Attempt to do automatic rezise detection
	if (screensize != driver->getScreenSize())
		this->setScreenSize(driver->getScreenSize());

	if (app_state<APP_STATE_CONTROL)
		background=SColor(0,200,200,200); // Background color in editor
	else
		background=ingamebackground; // Background color ingame


	driver->beginScene(true, true, background);

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
		{
			updateGameplay();
			
			// This will calculate the animation blending for the nodes
			DynamicObjectsManager::getInstance()->updateAnimationBlend();
		}
	}


	// Check for events of the logger
	GUIManager::getInstance()->setConsoleLogger(textevent);

	// Prepare the post FX before rendering all
	EffectsManager::getInstance()->preparePostFX(false);
	smgr->drawAll();

	if (raytester)
		raytester->update();

	// PostFX - render the player in silouette if he's occluded
	// Work with the current model but the code should be improved to support more models (with more than one texture)
	if (silouette) //  && (app_state > APP_STATE_CONTROL))
	{
		driver->runAllOcclusionQueries(false);
		driver->updateAllOcclusionQueries();
		overdraw=driver->getOcclusionQueryResult(Player::getInstance()->getNode())>0;
		overdraw=!overdraw;
		if (overdraw)
		{
			// Draw the player over the rendering so it's not occluded by the scenery
			Player::getInstance()->getNode()->setMaterialTexture(0, tex_occluded);
			Player::getInstance()->getNode()->setMaterialFlag(EMF_ZBUFFER,false);
			Player::getInstance()->getNode()->setMaterialFlag(EMF_LIGHTING,false);
			Player::getInstance()->getNode()->render();
			Player::getInstance()->getNode()->setMaterialFlag(EMF_ZBUFFER,true);
			Player::getInstance()->getNode()->setMaterialFlag(EMF_LIGHTING,true);
		}
		else 
		{
			Player::getInstance()->getNode()->setMaterialTexture(0, tex_normal);
		}
	}

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

	// Start the post process in the FX Manager
	EffectsManager::getInstance()->initPostProcess();
	EffectsManager::getInstance()->skydomeVisible(false); //Force the skydome to appear when the application is initialised; (Default state)

	// Set the proper state if in the EDITOR or only the player application
#ifdef EDITOR
	this->setAppState(APP_EDIT_LOOK);

	// Update the info panel with the current "active object"
	GUIManager::getInstance()->getInfoAboutModel();	
	// Loading is complete
	GUIManager::getInstance()->guiLoaderWindow->setVisible(false);

	CameraSystem::getInstance()->editCamMaya->setUpVector(vector3df(0,1,0));
	CameraSystem::getInstance()->setCamera(2);
	CameraSystem::getInstance()->editCamMaya->setPosition(vector3df(0.0f,1000.0f,-1000.0f));
	CameraSystem::getInstance()->editCamMaya->setTarget(vector3df(0.0f,0.0f,0.0f));
	CameraSystem::getInstance()->editCamMaya->setFarValue(50000.0f);
		//CameraSystem::getInstance()->setPosition(vector3df(oldcampos));

#else
	//EffectsManager::getInstance()->skydomeVisible(true); //Force the skydome to appear when the application is initialised; (Default state)
	//this->setAppState(APP_EDIT_WAIT_GUI);
	this->loadProjectFromXML(mapname);
	//oldcampos = Player::getInstance()->getObject()->getPosition();
	//CameraSystem::getInstance()->setCamera(1);
	//this->setAppState(APP_GAMEPLAY_NORMAL);
	//Player::getInstance()->getObject()->doScript();
	//LuaGlobalCaller::getInstance()->storeGlobalParams();
	//DynamicObjectsManager::getInstance()->initializeAllScripts();
	//DynamicObjectsManager::getInstance()->showDebugData(false);
	//TerrainManager::getInstance()->showDebugData(false);
	//GUIManager::getInstance()->setElementVisible(ST_ID_PLAYER_LIFE,true);
//	LuaGlobalCaller::getInstance()->doScript(scriptGlobal);

#endif

	
	
	// Hide the fog in the editor
	driver->setFog(SColor(0,255,255,255),EFT_FOG_LINEAR,300,999100);

	// Define the occlusion texture for the player (occlusion query)
	tex_occluded=driver->getTexture("../media/player/swordman_red.png");
	tex_normal=Player::getInstance()->getNode()->getMaterial(0).getTexture(0);
	//tex_normal=driver->getTexture("../media/player/swordman.png");

	// Occlusing query
	//driver->addOcclusionQuery(Player::getInstance()->getNode(), ((scene::IMeshSceneNode*)Player::getInstance()->getNode())->getMesh());
	driver->addOcclusionQuery(Player::getInstance()->getNode());

	// Instanciate the ray test class (debug purpose)
	raytester=new raytest();
	raytester->init(device);

	selectedNode=NULL;


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
			core::stringw str = L"IrrRPG Builder - Alpha SVN release 0.3 (aug 2014)";
			if (app_state>APP_STATE_CONTROL)
			{
				str += " FPS:";
				str += fps;
			}

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

	if (guienv->getFocus()!=guienv->getRootGUIElement())  // DEBUG: REset the focus. Problem loosing focus. Need to fix the problem but hack does it.
	{
		if (app_state == APP_EDIT_VIEWDRAG) // Viewdrag mode will reset the focus to the root (pressing spacebar)
		{
			if (cursorIsInEditArea())
				guienv->setFocus(guienv->getRootGUIElement());
#ifdef DEBUG				
			printf("In viewdrag mode\n");
#endif
		}

		if (!guienv->getFocus()) // Focus is pointer to an invalid pointer. Reset it.
		{
//#ifdef DEBUG
//printf("We lost the focus!\n");
//#endif
			guienv->setFocus(guienv->getRootGUIElement());
		}

	}

	// If the app state edit the terrain, then update the terrain
	if(app_state == APP_EDIT_TERRAIN_PAINT_VEGETATION || app_state == APP_EDIT_TERRAIN_TRANSFORM)
		TerrainManager::getInstance()->update();

	if (selectedNode  && app_state!=APP_EDIT_CHARACTER)
		GUIManager::getInstance()->updateEditCameraString(selectedNode);
	else
		GUIManager::getInstance()->updateEditCameraString(NULL);

	if (app_state==APP_EDIT_CHARACTER)
		GUIManager::getInstance()->updateEditCameraString(Player::getInstance()->getNode());
		
	// Trie to display the node as we go with the mouse cursor in edit mode
	if((app_state == APP_EDIT_DYNAMIC_OBJECTS_MODE || app_state==APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE) && cursorIsInEditArea() )
	{
		
		if (toolstate==TOOL_DO_ADD) //Will "preselect" an item only in ADD mode
			setPreviewSelection();
	}

	GUIManager::getInstance()->update(); // Update the GUI when editing objects
	// Enter the refresh after a timer duration OR if the terrain transform is used
	if ((timer2-timer)>17) // (17)1/60th second refresh interval
	{
		timer2 = device->getTimer()->getRealTime();
		if(app_state < APP_STATE_CONTROL)
		{
			
			// --- Drag the view when the spacebar is pressed
			if (app_state != APP_EDIT_DYNAMIC_OBJECTS_SCRIPT 
				&& app_state != APP_EDIT_WAIT_GUI 
				&& app_state != APP_EDIT_PLAYER_SCRIPT
				&& app_state != APP_EDIT_SCRIPT_GLOBAL
				// && app_state != APP_EDIT_CHARACTER
				//&& app_state != APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE
				)
			{
				// Activate "Viewdrag" mode
				// This state is checked by the MAYA camera
				// Will allow to move and rotate the view by the mouse when it's in that mode
				if(EventReceiver::getInstance()->isKeyPressed(KEY_SPACE))
				{
					if (app_state != APP_EDIT_VIEWDRAG)
					{
						old_state = app_state;
						setAppState(APP_EDIT_VIEWDRAG);
#ifdef DEBUG
						printf("Set camera settings...\n");
#endif
					}
				}
			}
			// Return the edit mode to normal after the spacebar is pressed (viewdrag)
			if ((app_state == APP_EDIT_VIEWDRAG) && !(EventReceiver::getInstance()->isKeyPressed(KEY_SPACE)))
			{
				setAppState(old_state);
				guienv->setFocus(guienv->getRootGUIElement()); // reset the focus when we release the spacebar
				return;
			}
			// --- End of code for drag of view


			//Move the player update code
			if(app_state == APP_EDIT_CHARACTER)
			{
				if(EventReceiver::getInstance()->isMousePressed(0) && cursorIsInEditArea())
					Player::getInstance()->getObject()->setPosition(getMousePosition3D(100).pickedPos);
				return;
			}

			//Feature: Take a note of the position of the camera or object into the console
			//Check for the pressing of CTRL+C
			if(app_state < APP_STATE_CONTROL)
			{
				if (EventReceiver::getInstance()->isKeyPressed(KEY_LCONTROL) && EventReceiver::getInstance()->isKeyPressed(KEY_KEY_C))
				{
					if (!keytoggled)
					{
						core::stringw text = L"";
						if (selectedNode && (app_state==APP_EDIT_DYNAMIC_OBJECTS_MODE || app_state==APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE))
							text = core::stringw(L"Note: ").append(GUIManager::getInstance()->getEditCameraString(selectedNode));
						else
							text = core::stringw(L"Note: ").append(GUIManager::getInstance()->getEditCameraString(NULL));

						GUIManager::getInstance()->setConsoleText (text.c_str(), video::SColor(255,11,120,13));
						keytoggled=true;
					}
				}
				else
				{
					keytoggled=false;
					//CameraSystem::getInstance()->getNode()
				}
			}


			
			
			// Move the selected object in ADD mode
			if(app_state == APP_EDIT_DYNAMIC_OBJECTS_MOVE_ROTATE && cursorIsInEditArea())
			{
				if (!moveupdown)
				{
					// Change the ID of the moved mesh so it's won't collision with the ray.
					irr::s32 oldID=lastMousePick.pickedNode->getID();
					lastMousePick.pickedNode->setID(0x0010);

					vector3df newposition = vector3df(0,0,0);
					
					if (snapfunction) // If snapping is activated use the function
						newposition=calculateSnap(getMousePosition3D(100).pickedPos,currentsnapping);
					else
						newposition=getMousePosition3D(100).pickedPos;

					//newposition=lastMousePick.pickedNode->getPosition()+(newposition-lastMousePick.pickedNode->getPosition());
					
					lastMousePick.pickedNode->setPosition(newposition);
					lastMousePick.pickedNode->setID(oldID);
					initialposition=lastMousePick.pickedNode->getPosition();
					return;
				}
				else
				{
					position2d<s32> mousepos2=device->getCursorControl()->getPosition();
					core::vector3df newpos = initialposition;
					//lastMousePick.pickedNode->getPosition();
					newpos.Y=newpos.Y+((mousepos.Y-mousepos2.Y));

					if (snapfunction) // If snapping is activated use the function
						lastMousePick.pickedNode->setPosition(calculateSnap(newpos,currentsnapping));
					else
						lastMousePick.pickedNode->setPosition(newpos);

					return;

				}
			}

			//Refresh the infos about the selections
			if (app_state==APP_EDIT_DYNAMIC_OBJECTS_MODE && toolstate==TOOL_DO_SEL)
			{
				if (selectedNode)
				{
					IGUIStaticText * text = NULL;
					DynamicObject * object = NULL;
					core::stringw objtype = "";
					core::stringw templatename = "";

					templatename = DynamicObjectsManager::getInstance()->getActiveObject()->getName();

					// Get the selected dynamic object
					object = DynamicObjectsManager::getInstance()->getObjectByName(((core::stringc)selectedNode->getName()).c_str());

					if (object)
					{
						TYPE obj = object->getType();
						switch (obj)
						{ 

							case OBJECT_TYPE_PLAYER:
								objtype = LANGManager::getInstance()->getText("objtype_player").c_str();
								break;

							case OBJECT_TYPE_NPC:
								objtype = LANGManager::getInstance()->getText("objtype_NPC").c_str();
								break;

							case OBJECT_TYPE_LOOT:
								objtype = LANGManager::getInstance()->getText("objtype_loot").c_str();
								break;

							case OBJECT_TYPE_INTERACTIVE:
								objtype = LANGManager::getInstance()->getText("objtype_int").c_str();
								break;

							case OBJECT_TYPE_NON_INTERACTIVE:
								objtype = LANGManager::getInstance()->getText("objtype_nint").c_str();
								break;

							case OBJECT_TYPE_WALKABLE:
								objtype = LANGManager::getInstance()->getText("objtype_walkable").c_str();
								break;

						
							default:
							break;
						}
					}

					text = ((IGUIStaticText *)guienv->getRootGUIElement()->getElementFromId(TXT_ID_SELOBJECT,true));
					if (text)
						text->setText(((core::stringw)selectedNode->getName()).c_str());

					text = ((IGUIStaticText *)guienv->getRootGUIElement()->getElementFromId(TXT_ID_SELOBJECT_TYPE,true));
					if (text)
						text->setText(objtype.c_str());

					text = ((IGUIStaticText *)guienv->getRootGUIElement()->getElementFromId(TXT_ID_OBJ_SCRIPT,true));
					if (text && object)
					{
						if (object->getScript().size()>0)
							text->setText(LANGManager::getInstance()->getText("bt_dialog_yes").c_str());
						else
							text->setText(LANGManager::getInstance()->getText("bt_dialog_no").c_str());
					}

					text = ((IGUIStaticText *)guienv->getRootGUIElement()->getElementFromId(TXT_ID_CUR_TEMPLATE,true));
					if (text)
						text->setText(templatename.c_str());
				} else
				{
				
					core::stringw templatename = "";
					IGUIStaticText * text = NULL;

					templatename = DynamicObjectsManager::getInstance()->getActiveObject()->getName();

					text = ((IGUIStaticText *)guienv->getRootGUIElement()->getElementFromId(TXT_ID_SELOBJECT,true));
					if (text)
						text->setText(LANGManager::getInstance()->getText("panel_sel_sel1").c_str());

					text = ((IGUIStaticText *)guienv->getRootGUIElement()->getElementFromId(TXT_ID_SELOBJECT_TYPE,true));
					if (text)
						text->setText(LANGManager::getInstance()->getText("panel_sel_sel1").c_str());

					text = ((IGUIStaticText *)guienv->getRootGUIElement()->getElementFromId(TXT_ID_OBJ_SCRIPT,true));
					if (text)
						text->setText(LANGManager::getInstance()->getText("panel_sel_sel1").c_str());

					text = ((IGUIStaticText *)guienv->getRootGUIElement()->getElementFromId(TXT_ID_CUR_TEMPLATE,true));
					if (text)
						text->setText(templatename.c_str());

				}
			}

			// Tools mode refresh
			if (app_state==APP_EDIT_DYNAMIC_OBJECTS_MODE && toolstate!=TOOL_NONE && toolactivated)
			{				
				if (toolstate==TOOL_DO_MOV && toolactivated) // Will move the object
				{
					if (!moveupdown)
					{
						// Change the ID of the moved mesh so it's won't collision with the ray.
						irr::s32 oldID=selectedNode->getID();
						selectedNode->setID(0x0010);
					
						vector3df newposition = vector3df(0,0,0);

						if (snapfunction) // If snapping is activated use the function
							newposition=calculateSnap(getMousePosition3D(100).pickedPos,currentsnapping);
						else
							newposition=getMousePosition3D(100).pickedPos;

						if (GUIManager::getInstance()->getCheckboxState(CB_ID_POS_X))
							newposition.X = initialposition.X;

						if (GUIManager::getInstance()->getCheckboxState(CB_ID_POS_Y))
							newposition.Y = initialposition.Y;

						if (GUIManager::getInstance()->getCheckboxState(CB_ID_POS_Z))
							newposition.Z = initialposition.Z;

						selectedNode->setPosition(newposition);

						selectedNode->setID(oldID);
						initialposition=selectedNode->getPosition();
						GUIManager::getInstance()->updateNodeInfos(selectedNode);
					}
					else
					{
						position2d<s32> mousepos2=device->getCursorControl()->getPosition();
						core::vector3df newposition = initialposition;
						//lastMousePick.pickedNode->getPosition();
						newposition.Y=newposition.Y+((mousepos.Y-mousepos2.Y));

						if (GUIManager::getInstance()->getCheckboxState(CB_ID_POS_Y))
							return;

						if (snapfunction) // If snapping is activated use the function
							selectedNode->setPosition(calculateSnap(newposition,currentsnapping));
						else
							selectedNode->setPosition(newposition);

						GUIManager::getInstance()->updateNodeInfos(selectedNode);

					}
					return;
				}
				
				if (toolstate==TOOL_DO_ROT && toolactivated) // Will rotate the object
				{
					if (!moveupdown)
					{
						position2d<s32> mousepos2=device->getCursorControl()->getPosition();
						vector3df newrotation = initialrotation;
						
						//Checkboxes define if the axis can be modified
						if (!GUIManager::getInstance()->getCheckboxState(CB_ID_ROT_Y))
							newrotation.Y=initialrotation.Y+(mousepos.X-mousepos2.X);
						

						if (!GUIManager::getInstance()->getCheckboxState(CB_ID_ROT_X))
							newrotation.X=initialrotation.X+(mousepos.Y-mousepos2.Y);

						selectedNode->setRotation(newrotation);
						GUIManager::getInstance()->updateNodeInfos(selectedNode);

					}
					else
					{
						position2d<s32> mousepos2=device->getCursorControl()->getPosition();
						vector3df newrotation = selectedNode->getRotation(); //initialrotation;
						if (!GUIManager::getInstance()->getCheckboxState(CB_ID_ROT_Z))
							newrotation.Z=initialrotation.Z+(mousepos.X-mousepos2.X);
						
						selectedNode->setRotation(newrotation);
						GUIManager::getInstance()->updateNodeInfos(selectedNode);

					}

					return;
				}
				

				if (toolstate==TOOL_DO_SCA && toolactivated) // Will rotate the object
				{
					position2d<s32> mousepos2=device->getCursorControl()->getPosition();
					vector3df newscale = initialscale;

					f32 tscale=-f32(mousepos.X-mousepos2.X)/10.0f;
					
					if ((initialscale.X+tscale)<0.001f)
					{
						initialscale.X=0.001f;
						initialscale.Y=0.001f;
						initialscale.Z=0.001f;
					} else
					{
						if (!GUIManager::getInstance()->getCheckboxState(CB_ID_SCA_X))
							newscale.X=initialscale.X+tscale;

						if (!GUIManager::getInstance()->getCheckboxState(CB_ID_SCA_Y))
							newscale.Y=initialscale.Y+tscale;

						if (!GUIManager::getInstance()->getCheckboxState(CB_ID_SCA_Z))
							newscale.Z=initialscale.Z+tscale;
					}
					selectedNode->setScale(newscale);
					GUIManager::getInstance()->updateNodeInfos(selectedNode);


					return;
				}	
			}
		}
	}
}

#endif


void App::updateGameplay()
{

	timer = device->getTimer()->getRealTime();

	//Levelchange as been asked
	//This is used when we use the lua command to load another project
	if (levelchange && (timer_lua-timer)>1000)
	{
		Player::getInstance()->setTaggedTarget(NULL);
		stopGame();
		cleanWorkspace(); 
		this->loadProjectFromXML(levelfilename);
		playGame();
		levelchange=false;
	}

	vector2d<f32> pom = vector2d<f32>(0,0);
	
	//Left mouse button in gameplay to change the cam direction
	if(EventReceiver::getInstance()->isMousePressed(1) && cursorIsInEditArea() && app_state == APP_GAMEPLAY_NORMAL)
	{
		if (initRotation==false) // when the rotation is initialized, it store the camera position and the cursor relative position
		{
			oldmouse = device->getCursorControl()->getRelativePosition();
			initRotation=true;
			initangle.X=CameraSystem::getInstance()->getAngle().X;
			initangle.Y=CameraSystem::getInstance()->getAngle().Y;
			timer4=device->getTimer()->getRealTime();
			
		}

		if (initRotation && timer-timer4>17)
		{
			// Offset from the stored value
			timer4=device->getTimer()->getRealTime();
			vector2d<f32> pom1 = oldmouse-device->getCursorControl()->getRelativePosition();
			pom.X=initangle.X-(pom1.X*360);
			pom.Y=initangle.Y-(pom1.Y*360);
			
			CameraSystem::getInstance()->SetPointNClickAngle(pom);
			//Hide the mouse pointer while rotation is done
			device->getCursorControl()->setVisible(false);

			// Update the Point&Click camera setup
			CameraSystem::getInstance()->updatePointClickCam();
			// Update all the NPC on the map (including the player)
			DynamicObjectsManager::getInstance()->updateAll();
		}
		return;

	} else
		if((!EventReceiver::getInstance()->isMousePressed(1)) && cursorIsInEditArea() && app_state == APP_GAMEPLAY_NORMAL)
		{
			// Restore the mouse pointer
			device->getCursorControl()->setVisible(true);
			initRotation=false;
		}

		// Update the Point&Click camera setup
		CameraSystem::getInstance()->updatePointClickCam();

		// Refresh the NPC loop
		if ((timer-timer3)>17) // (17 )1/60 second (0 value seem ok for now)
		{
			// Update the NPc refresh
			timer3 = device->getTimer()->getRealTime();

			// Update all the NPC on the map (including the player)
			DynamicObjectsManager::getInstance()->updateAll();

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
				//DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_NON_INTERACTIVE,0x0010);
				DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_NPC,100);
				DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_INTERACTIVE,100);
				DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_WALKABLE,0x0010);

				// Filter only object with the ID=100 to get the resulting node
				MousePick mousePick = getMousePosition3D(100);

				

				// Set back to the defaults
				//DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_NON_INTERACTIVE,100);
				DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_NPC,0x0010);
				DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_INTERACTIVE,0x0010);
				DynamicObjectsManager::getInstance()->setObjectsID(OBJECT_TYPE_WALKABLE,100);

				// Failed to pick something, try to select "walkable"
				if (!mousePick.pickedNode)
				{
					mousePick = getMousePosition3D(100);
					//mousePick.pickedNode = NULL; //Forget about the node since we only need the collision point
				}


				stringc nodeName = "";
				// Check for a node(need to get the name of the node)
				if (mousePick.pickedNode != NULL)
				{
					// Get the name of the object that has been clicked on
					stringc nodeName = mousePick.pickedNode->getName();

					//if you click on a Dynamic Object...
					if( stringc( nodeName.subString(0,14)) == "dynamic_object" )
					{

						DynamicObject* obj = DynamicObjectsManager::getInstance()->getObjectByName(nodeName);
						// TODO: Need to get more accuracy for the distance hardcoded value is not ideal

						//Since an object as been clicked the walktarget of the player is changed
						if (obj)
						{

							vector3df pos = obj->getPosition();
							vector3df pos2 = Player::getInstance()->getObject()->getPosition();
							f32 desiredDistance=50.0f;
							f32 distance = Player::getInstance()->getObject()->getDistanceFrom(pos);
							f32 final = (distance-desiredDistance)/distance;

							vector3df walkTarget = pos.getInterpolated(pos2,final);
							Player::getInstance()->getObject()->setWalkTarget(walkTarget);

							DynamicObjectsManager::getInstance()->getTarget()->setPosition(obj->getPosition()+vector3df(0,0.1f,0));
							DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(true);

							Player::getInstance()->getObject()->lookAt(obj->getPosition());
							Player::getInstance()->setTaggedTarget(obj);
						}

						// If the distance is ok notify the object lua script that it's being clicked
						// Currently set at 100 unit
						if (obj && (obj->getDistanceFrom(Player::getInstance()->getObject()->getPosition()) < 100.0f))
							obj->notifyClick();

						if(obj->getObjectType() == stringc("ENEMY"))
						{
							Player::getInstance()->getObject()->attackEnemy(obj);
						}
						else
						{
							Player::getInstance()->getObject()->clearEnemy();
						}

						return;

					}
					else
					{ // Did not clicked on a NPC or object but on a walkable area
						//mousePick = getMousePosition3D(100);
						if (mousePick.pickedPos!=vector3df(0,0,0))
						{
							Player::getInstance()->getObject()->setWalkTarget(mousePick.pickedPos);
							DynamicObjectsManager::getInstance()->getTarget()->setPosition(mousePick.pickedPos+vector3df(0,0.1f,0));
							DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(true);
							Player::getInstance()->setTaggedTarget(NULL);
							Player::getInstance()->getObject()->clearEnemy();
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

	// Initialize the camera (2) is maya type camera for editing
	CameraSystem::getInstance()->setCamera(2);

	APP_STATE old_state = getAppState();
	setAppState(APP_EDIT_WAIT_GUI);

	//stringc name = GUIManager::getInstance()->showInputQuestion(stringc(LANGManager::getInstance()->getText("msg_new_project_name")).c_str());
	//GUIManager::getInstance()->flush();

	stringc name = "newproject"; //This will hide the question

	/*
	while(name == stringc(""))
	{
		name = GUIManager::getInstance()->showInputQuestion(stringc(LANGManager::getInstance()->getText("msg_new_project_name")).c_str());
		GUIManager::getInstance()->flush();
	}*/

	name += ".XML";

	stringc filename = "../projects/";
	filename += name;

	this->cleanWorkspace();

	CameraSystem::getInstance();

	//TerrainManager::getInstance()->createEmptySegment(vector3df(0,0,0));
	TerrainManager::getInstance()->createEmptySegmentMatrix(50,50);

	//smgr->setAmbientLight(SColorf(0.5,0.5,0.5,0.5));
	//driver->setFog(SColor(255,255,255,255),EFT_FOG_LINEAR,0,12000);

	Player::getInstance();

	this->currentProjectName = name;

	CameraSystem::getInstance()->editCamMaya->setPosition(vector3df(0,1000,-1000));
	CameraSystem::getInstance()->editCamMaya->setTarget(vector3df(0,0,0));
	CameraSystem::getInstance()->setCameraHeight(0); // Refresh the camera

	Player::getInstance()->getNode()->setPosition(vector3df(0.0f,0.0f,0.0f));
	Player::getInstance()->getNode()->setRotation(vector3df(0.0f,0.0f,0.0f));

	//this->saveProject();

	setAppState(old_state);
}

void App::loadProject(DIALOG_FUNCTION function)
{
	old_state = getAppState();

	setAppState(APP_EDIT_WAIT_GUI);
	if (lastScannedPick.pickedNode!=NULL)
	{
			lastScannedPick.pickedNode=NULL;
			selectedNode=NULL;
		}

	// Store the dialog function value and remember it.
	df = function;

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
		//selector->setStartingPath(L"../");
		if (function == DF_PROJECT)
		{
			selector->addFileFilter(L"IRB Project files", L"xml", driver->getTexture("../media/art/wma.png"));
			// Create a "favorite places"
			selector->addPlacePaths(L"IRB Project path",L"../projects",driver->getTexture("../media/art/places_folder.png"));
			// Define in what path the request will open (it accept full or relative paths)
			
		}
		else
		{
			// Create a "favorite places"
			selector->addPlacePaths(L"Dynamic object folder",L"../media/dynamic_objects",driver->getTexture("../media/art/places_folder.png"));
			selector->addFileFilter(L"OBJ Model", L"obj", driver->getTexture("../media/art/wma.png"));
			selector->addFileFilter(L"3DS Model", L"3ds", driver->getTexture("../media/art/wma.png"));
			selector->addFileFilter(L"B3D Model", L"b3d", driver->getTexture("../media/art/wma.png"));
			selector->addFileFilter(L"DirectX Model", L"x", driver->getTexture("../media/art/wma.png"));
		}

		// This is required for the window stretching feature
		selector->setDevice(device);

		
#ifdef WIN32

		// Populate with standard windows favorites paths
		selector->populateWindowsFAV();
#else
		// Add some common linux paths
		selector->populateLinuxFAV();
#endif

		if (function == DF_PROJECT)
			selector->setStartingPath(L"../projects"); // Projects
		else
			selector->setStartingPath(L"../media/dynamic_objects"); // Replacing objects
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

// This need to be reworked as it might be confusing (not only for projects)
// As it's been used for getting the filename and doing actions
void App::loadProjectFile(bool value)
{
	vector3df oldrotation = vector3df(0,0,0);
	core::stringw oldscript = L"";
	DynamicObject* object=NULL;

	if (df==DF_MODEL && value)
	{
		if (selector)
		{
			lastFilename=(core::stringc)selector->getFileName();
			GUIManager::getInstance()->setConsoleText(selector->getFileName());
			GUIManager::getInstance()->setConsoleText(selector->getOnlyFileName());
			// This is a file loader
			if (selector->isSaver()==false)
			{
	
				selector->setVisible(false); // Hide the file selector
				// Keep the "good stuff"
				oldrotation = lastMousePick.pickedNode->getRotation();
				oldscript = DynamicObjectsManager::getInstance()->getScript(lastMousePick.pickedNode->getName());

				//Tell the dynamic Objects Manager to remove the node
				DynamicObjectsManager::getInstance()->removeObject(lastMousePick.pickedNode->getName());

				// remove the object for the selection
				lastScannedPick.pickedNode=NULL;
				lastMousePick.pickedNode=NULL;
				selectedNode=NULL;

				// Create the new object from the template and put the old values back in.
				object = DynamicObjectsManager::getInstance()->createCustomObjectAt(lastMousePick.pickedPos, lastFilename);
				object->setScript(oldscript);
				object->setRotation(oldrotation);
			}
		}
	}
	
	// Project loading
	if (df==DF_PROJECT && value)
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

				//Recreate the empty tile matrix so the user can expand his loaded map.
				TerrainManager::getInstance()->createEmptySegmentMatrix(50,50);

				// Put back the player object in the list of the dynamic objects
				DynamicObjectsManager::getInstance()->setPlayer();
			}
			// This is a file saver
			if (selector->isSaver()==true)
			{
				selector->setVisible(false);
				//printf("Saving project now!\n");
				this->saveProjectToXML(file);
			}
			guienv->setFocus(guienv->getRootGUIElement());
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

			guienv->setFocus(guienv->getRootGUIElement());
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
			guienv->setFocus(guienv->getRootGUIElement());
			selector->remove();
			selector=NULL;
		}
		if (saveselector)
		{
			guienv->setFocus(guienv->getRootGUIElement());
			saveselector->remove();
			saveselector=NULL;
		}
	}

	// Set back the camera after loading the map (could be perhaps improved later, to select the proper camera after loading (ingame loading))
	CameraSystem::getInstance()->setCameraHeight(0); // Refresh the camera
	//setAppState(old_state);
}

// Timed load of the new project, give a chance to LUA to close and start the new thing.
void App::loadProjectGame(irr::core::stringc filename)
{
	this->timer_lua=device->getTimer()->getRealTime();
	levelchange=true;
	levelfilename = filename;
}

void App::saveProjectDialog()
{
	//Save current state, disabled for now
	//APP_STATE old_state = getAppState();
	setAppState(APP_WAIT_FILEREQUEST);

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
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "ISO-8859-1", "" );

	TiXmlElement* irb_project = new TiXmlElement( "IrrRPG_Builder_Project" );
	irb_project->SetAttribute("version","1.0");

	GUIManager::getInstance()->setTextLoader(L"Saving the terrain");
	quickUpdate();
	TerrainManager::getInstance()->saveToXML(irb_project);


	GUIManager::getInstance()->setTextLoader(L"Saving the active dynamic objects");
	quickUpdate();
	DynamicObjectsManager::getInstance()->saveToXML(irb_project);

	GUIManager::getInstance()->setTextLoader(L"Saving the global scripts");
	quickUpdate();

	TiXmlElement* globalScript = new TiXmlElement("global_script");
	globalScript->SetAttribute("script",scriptGlobal.c_str());
	irb_project->LinkEndChild(globalScript);

	// Closing the XML file
	doc.LinkEndChild( decl );
	doc.LinkEndChild( irb_project );

	bool result = doc.SaveFile( filename.c_str() );
#ifdef DEBUG
	if (result) printf("Saved %s OK!\n",filename.c_str());
#endif
	GUIManager::getInstance()->guiLoaderWindow->setVisible(false);

	CameraSystem::getInstance()->setCameraHeight(0); // Refresh the camera	

	guienv->addMessageBox(L"Save report:",(core::stringw("Scene ")
		.append(core::stringw(filename.c_str()))
		.append(LANGManager::getInstance()->getText("msg_saved_ok").c_str()).c_str())
		,true);

#ifdef DEBUG
	cout << "DEBUG : XML : PROJECT SAVED : " << filename.c_str() << endl;
#endif
}

bool App::loadProjectFromXML(stringc filename)
{
	GUIManager::getInstance()->guiLoaderWindow->setVisible(true);
	printf ("Trying to load this map: %s \n",filename.c_str());
	TiXmlDocument doc(filename.c_str());
	if (!doc.LoadFile()) return false;

#ifdef DEBUG
	cout << "DEBUG : XML : LOADING PROJECT : " << filename.c_str() << endl;
#endif

	TiXmlElement* root = doc.FirstChildElement( "IrrRPG_Builder_Project" );

	if ( root )
	{
		if( atof(root->Attribute("version"))!=APP_VERSION )
		{
#ifdef DEBUG
			cout << "DEBUG : XML : INCORRECT VERSION!" << endl;
#endif

			return false;
		}

		TiXmlElement* globalScriptXML = root->FirstChildElement( "global_script" );
		if ( globalScriptXML )
		{
			GUIManager::getInstance()->setTextLoader(L"Loading the scripts");
#ifdef EDITOR  //only update in the editor
			quickUpdate();
#endif
			scriptGlobal = globalScriptXML->ToElement()->Attribute("script");
		}

		TiXmlElement* terrain = root->FirstChildElement( "terrain" );
		if ( terrain )
		{
			GUIManager::getInstance()->setTextLoader(L"Loading the terrain");
#ifdef EDITOR
			quickUpdate();
#endif
			TerrainManager::getInstance()->loadFromXML(terrain);
		}

		TiXmlElement* dynamicObjs = root->FirstChildElement( "dynamic_objects" );
		if ( dynamicObjs )
		{
			GUIManager::getInstance()->setTextLoader(L"Loading the dynamic objects");
#ifdef EDITOR
			quickUpdate();
#endif
			DynamicObjectsManager::getInstance()->loadFromXML(dynamicObjs);
		}

		TiXmlElement* playerXML = root->FirstChildElement( "player" );
		if(playerXML)
		{
			// Player is a dynamic object now.
			// There is no need for now to load from this
		}
		CameraSystem::getInstance()->setCameraHeight(0); // Refresh the camera
#ifndef EDITOR
		GUIManager::getInstance()->setTextLoader(L"");
		guienv->getRootGUIElement()->getElementFromId(BT_PLAYER_START,true)->setVisible(true);
		guienv->getRootGUIElement()->getElementFromId(BT_PLAYER_CONFIG,true)->setVisible(true);
#endif
#ifdef EDITOR
		GUIManager::getInstance()->guiLoaderWindow->setVisible(false);
#endif

	}
	else
	{
#ifdef APP_DEBUG
		cout << "DEBUG : XML : THIS FILE IS NOT A IRRRPG BUILDER PROJECT!" << endl;
#endif
#ifdef EDITOR
		GUIManager::getInstance()->guiLoaderWindow->setVisible(false);
#endif
		return false;
	}

#ifdef APP_DEBUG
	cout << "DEBUG : XML : PROJECT LOADED! "<< endl;
#endif

	///TODO:CLEAR PROJECT IF NOT RETURN TRUE ON LOAD PROJECT FROM XML

#ifdef EDITOR
	guienv->addMessageBox(L"Load report:",(core::stringw("Scene ")
		.append(core::stringw(filename.c_str()))
		.append(LANGManager::getInstance()->getText("msg_loaded_ok").c_str()).c_str())
		,true);
#endif

	return true;
}

void App::initialize()
{

	// Initialize the sound engine
	SoundManager::getInstance();

	// Initialize the GUI class first
	GUIManager::getInstance();

	// Set the ambient light
	//smgr->setAmbientLight(SColorf(0.80f,0.85f,1.0f,1.0f));
	  smgr->setAmbientLight(SColorf(0.5f,0.60f,0.75f,1.0f));

	// Set the fog to be very far when not in gameplay
	driver->setFog(SColor(0,255,255,255),EFT_FOG_LINEAR,0,20000);

	//Create a sun light
	scene::ILightSceneNode * light=smgr->addLightSceneNode(0,vector3df(2500,5000,-5000));
	light->setLightType(ELT_DIRECTIONAL);
	light->setRadius(50000);
	light->setRotation(vector3df(45,45,0));
	

	screensize=driver->getScreenSize();

#ifdef EDITOR
	// Initialize the camera (2) is maya type camera for editing
	CameraSystem::getInstance()->setCamera(2);
	GUIManager::getInstance()->setupEditorGUI();
//TerrainManager::getInstance()->createEmptySegment(vector3df(0,0,0));
	TerrainManager::getInstance()->createEmptySegmentMatrix(50,50);
	quickUpdate();
#endif


	GUIManager::getInstance()->setupGameplayGUI();
	quickUpdate();
	Player::getInstance();
	driver->setMinHardwareBufferVertexCount(0);
	core::stringc vendor = driver->getVendorInfo();
	printf ("Here is the vendor information: %s\n",vendor.c_str());
	this->currentProjectName = "irb_temp_project";
}

void App::shutdown()
{
	// Stuff to do when the device is closed.
	SoundManager::getInstance()->stopEngine();
	cleanWorkspace();
	DynamicObjectsManager::getInstance()->clean(true);
	device->closeDevice();
	device->run();
	device->drop();
	//exit(0);
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


irr::f32 App::getBrushRadius(int number)
{
	f32 radius=0.0f;
#ifdef EDITOR
	
	if (number==0) //main radius
		radius = GUIManager::getInstance()->getScrollBarValue(SC_ID_TERRAIN_BRUSH_RADIUS);
	if (number==1) // inner radius
		radius = GUIManager::getInstance()->getScrollBarValue(SC_ID_TERRAIN_BRUSH_RADIUS2);
#endif
	return radius;
}

// Snapping function
core::vector3df App::calculateSnap(vector3df input, f32 snapvalue)
{
	f32 X1=core::round32(input.X/snapvalue)+0.0f;
	f32 Y1=core::round32(input.Y/snapvalue)+0.0f;
	f32 Z1=core::round32(input.Z/snapvalue)+0.0f;

	X1=X1*snapvalue;
	Y1=Y1*snapvalue;
	Z1=Z1*snapvalue;

	core::vector3df result = core::vector3df(X1,Y1,Z1);
	return result;
}