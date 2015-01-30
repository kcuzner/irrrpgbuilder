#include "GUIManager.h"

#include "../LANGManager.h" //Need to access the strings for GUI (Can be translated in multiple languages)
#include "../objects/DynamicObjectsManager.h" //Access to objects managing functions
#include "../terrain/TerrainManager.h" //Access to the terrain informations
#include "../sound/SoundManager.h" //Access to sounds with GUI
#include <algorithm>    // std::sort

GUIManager::GUIManager()
{
	device = App::getInstance()->getDevice();
    guienv = device->getGUIEnvironment();
	driver = device->getVideoDriver();
	screensize = App::getInstance()->getScreenSize();

	// init those because they will move on the display.
	
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
	guiLoaderWindow=NULL;
	// Load the required font
	guiFontC12 = guienv->getFont("../media/fonts/char12.xml");

		for (s32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
    {
            video::SColor col = guienv->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
            col.setAlpha(230);
            guienv->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
    }

	textevent.clear();
	texteventcolor.clear();

	// Bigger Windows titlebar width
	guienv->getSkin()->setSize(EGDS_WINDOW_BUTTON_WIDTH,26);
	guienv->getSkin()->setColor(EGDC_ACTIVE_BORDER ,video::SColor(200,36,36,36));

	//New attemps as skinning into a darker theme
	//guienv->getSkin()->setColor(EGDC_3D_SHADOW,video::SColor(200,36,36,36));
	//guienv->getSkin()->setColor(EGDC_3D_FACE,video::SColor(200,164,164,164));
	//guienv->getSkin()->setColor(EGDC_WINDOW,video::SColor(255,96,96,96));
	//guienv->getSkin()->setColor(EGDC_EDITABLE,video::SColor(255,96,96,96));
	//guienv->getSkin()->setColor(EGDC_3D_HIGH_LIGHT,video::SColor(255,96,96,96));
	//guienv->getSkin()->setColor(EGDC_HIGH_LIGHT,video::SColor(255,135,135,135));
	//guienv->getSkin()->setColor(EGUI_LBC_TEXT,video::SColor(255,240,240,240));



	// Fake office style skin colors
	// We should allow creation of skins colors by the users or at least a choice of skins to use
	//guienv->getSkin()->setColor(EGDC_3D_SHADOW,video::SColor(200,140,178,226));
	//guienv->getSkin()->setColor(EGDC_3D_FACE,video::SColor(200,204,227,248));
	//guienv->getSkin()->setColor(EGDC_WINDOW,video::SColor(255,220,220,220));

}

// Clear all GUI when the class is deleted
GUIManager::~GUIManager()
{

	//dtor
}

IGUIFont* GUIManager::getFont(FONT_NAME fontName)
{
    switch(fontName)
    {
        case FONT_ARIAL:
            return guiFontC12;
            break;

		case FONT_LARGE:
			return guiFontLarge28;
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

#ifdef EDITOR
// Specific stuff related to the editor
void GUIManager::setupEditorGUI()
{
	GUIEditor::getInstance()->setupEditorGUI();
}
#endif

void GUIManager::setupGameplayGUI()
{
#ifndef EDITOR
	// Create the Configuration window (Need to be updated)
    configWindow = new GUIConfigWindow(App::getInstance()->getDevice());
#endif
	GUIGame::getInstance()->setupGameplayGUI();
}


rect<s32> GUIManager::myRect(s32 x, s32 y, s32 w, s32 h)
{
    return rect<s32>(x,y,x+w,y+h);
}

void GUIManager::drawHelpImage(GUI_HELP_IMAGE img)
{
	GUIEditor::getInstance()->drawHelpImage(img);
}

void GUIManager::drawPlayerStats()
{
	GUIGame::getInstance()->drawPlayerStats();
}

bool GUIManager::getCheckboxState(GUI_ID id)
{
	IGUICheckBox* checkbox = (IGUICheckBox*)getGUIElement(id);
	return checkbox->isChecked();
}

void GUIManager::setCheckboxState(GUI_ID id,bool value)
{
	IGUICheckBox* checkbox = (IGUICheckBox*)getGUIElement(id);
	checkbox->setChecked(value);
}

f32 GUIManager::getScrollBarValue(GUI_ID id)
{
	return GUIEditor::getInstance()->getScrollBarValue(id);
}

stringc GUIManager::getComboBoxItem(GUI_ID id)
{

	if (id==CO_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE)
	{
		IGUIComboBox* item = (IGUIComboBox*)getGUIElement(id);
		if (item)
			return stringc(item->getItem(item->getSelected()));
		else 
			return "frog";

	}
	IGUIListBox* item = (IGUIListBox*)getGUIElement(id);
	if (item)
	{
		return stringc(item->getListItem(item->getSelected()));
	}
	else
		return "frog";

}

IGUIListBox* GUIManager::getListBox(GUI_ID id)
{
	IGUIListBox* item = (IGUIListBox*)getGUIElement(id);
	if (item)
		return item;
	else
		return NULL;
}

core::stringw GUIManager::getEditCameraString(ISceneNode* node)
{
	return GUIEditor::getInstance()->getEditCameraString(node);
}

void GUIManager::updateEditCameraString(scene::ISceneNode * node)
{
	GUIEditor::getInstance()->updateEditCameraString(node);
}

// Update the Info panel GUI with the information contained in the template name
void GUIManager::getInfoAboutModel(LIST_TYPE type)
{
	GUIEditor::getInstance()->getInfoAboutModel(type);
}

//Check the children of this gui and return if the pointer is inside the childen
bool GUIManager::isGuiChildPresent(gui::IGUIElement* elem, vector2d<s32> mousepos)
{
#ifdef EDITOR
	//Update the content of the terrain tools
	IGUIWindow* guiTerrainToolbar=(IGUIWindow*)getGUIElement(GCW_TERRAIN_TOOLBAR);
	if (guiTerrainToolbar->isVisible() && guiTerrainToolbar->isPointInside(mousepos))
	{
		getScrollBarValue(SC_ID_TERRAIN_BRUSH_RADIUS);
        getScrollBarValue(SC_ID_TERRAIN_BRUSH_STRENGTH);
		getScrollBarValue(SC_ID_TERRAIN_BRUSH_PLATEAU);
	}
#endif

	const core::list<IGUIElement*>& children = elem->getChildren();

	for ( core::list<IGUIElement*>::ConstIterator it = children.begin(); it != children.end(); ++it )
	{
		IGUIElement* current = *it;
		if (current->isPointInside(mousepos) && current!=guienv->getRootGUIElement())
		{
			if (current->isVisible()==true)
			{
				return true;
			}
		}

	}
	return false;

}

// Reshesh the GUI informations inside a window
void GUIManager::UpdateGUIChooser(LIST_TYPE type)
{
	GUIEditor::getInstance()->UpdateGUIChooser(type);
}

// Refresh gui information inside a panel type
void GUIManager::updateCurrentCategory(LIST_TYPE type)
{
	GUIEditor::getInstance()->updateCurrentCategory(type);
}

void GUIManager::UpdateCollections(LIST_TYPE type)
{
	GUIEditor::getInstance()->UpdateCollections(type);
}

void GUIManager::buildSceneObjectList(DynamicObject::TYPE objtype)
{
	GUIEditor::getInstance()->buildSceneObjectList(objtype);
}


// Used to put a text description when loading a project
void GUIManager::setTextLoader(stringw text)
{
	IGUIStaticText* guiLoaderDescription = (IGUIStaticText*)getGUIElement(TXT_ID_LOADER);
	if (guiLoaderDescription)
	{
		guiLoaderDescription->setText(text.c_str());
		App::getInstance()->quickUpdate();
	}
}

// Console window GUI
void GUIManager::createConsole()
{

	core::dimension2d<u32> center = screensize/2;
	// consolewin = guienv->addWindow(rect<s32>(20,20,800,400),false,L"Console window",0,GCW_CONSOLE);
	CGUIExtWindow* consolewin = new CGUIExtWindow(LANGManager::getInstance()->getText("console_window_title").c_str(), guienv, guienv->getRootGUIElement(),GCW_CONSOLE,rect<s32>(center.Width-400,center.Height-200,center.Width+400,center.Height+200));
	consolewin->setDevice(App::getInstance()->getDevice());
	//consolewin->getCloseButton()->setVisible(false);
	consolewin->setCloseHide(true);
	consolewin->setStretchable(true);
	consolewin->setMinSize(core::dimension2du(140,70));

	// project TAB
	gui::IGUITabControl* control = guienv->addTabControl(myRect(10,30,780,360),consolewin,true,true);
	gui::IGUITab* tab=control->addTab(LANGManager::getInstance()->getText("tab_console_message").c_str());
	gui::IGUITab* tab2=control->addTab(LANGManager::getInstance()->getText("tab_console_log").c_str());
	tab->setBackgroundColor(video::SColor(255,220,220,220));
	tab->setDrawBackground(true);

	control->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

	//Message console
	IGUIListBox* console = guienv->addListBox(myRect(10,15,755,290),tab,LB_ID_LOGGER_MESSAGE,true);
	console->setAutoScrollEnabled(false);
	console->setItemHeight(20);
	console->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

	//logger console
	IGUIListBox* consolelog = guienv->addListBox(myRect(10,15,755,290),tab2,LB_ID_LOGGER_CONSOLE,true);
	consolelog->setAutoScrollEnabled(false);
	consolelog->setItemHeight(20);
	consolelog->setAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);

	consolewin->setVisible(false);
}

// Basic GUI Refresh loop
void GUIManager::update()
{
	// If the CONTEXT MENU WINDOW is visible and the cursor get outside of it, then close it after a delay
	if (isWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU))
	{

		IGUIWindow* guiDynamicObjects_Context_Menu_Window=(IGUIWindow*)getGUIElement(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU);
		if (!guiDynamicObjects_Context_Menu_Window->isPointInside(device->getCursorControl()->getPosition()))
		{
			if (device->getTimer()->getRealTime()-timer3>900) // 900 ms delay before closing
			{
			//The cursor is outside the window, close it then.
			setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU,false);
			}
		}
		 else
			timer3 = device->getTimer()->getRealTime();

	}
	// If the CONTEXT MENU WINDOW is visible and the cursor get outside of it, then close it after a delay
	if (isWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU1))
	{
		IGUIWindow* guiDynamicObjects_Context_Menu_Window1=(IGUIWindow*)getGUIElement(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU1);
		if (!guiDynamicObjects_Context_Menu_Window1->isPointInside(device->getCursorControl()->getPosition()))
		{
			if (device->getTimer()->getRealTime()-timer3>900) // 900 ms delay before closing
			{
				//The cursor is outside the window, close it then.
				setWindowVisible(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU1,false);
			}
		} else
		timer3 = device->getTimer()->getRealTime();

	}

	// Check for Windows that are "closed/hidden" and change the "app state" adequately
	CGUIExtWindow* guiDynamicObjectsWindowEditAction = (CGUIExtWindow*)getGUIElement(GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT);
	if (!guiDynamicObjectsWindowEditAction->isVisible() &&
		(App::getInstance()->getAppState()==App::APP_EDIT_DYNAMIC_OBJECTS_SCRIPT ||
		App::getInstance()->getAppState()==App::APP_EDIT_PLAYER_SCRIPT ||
		App::getInstance()->getAppState()==App::APP_EDIT_SCRIPT_GLOBAL))
		App::getInstance()->setAppState(App::APP_EDIT_DYNAMIC_OBJECTS_MODE);

}

//! Hides/Display a IRB window/Pane
// Could be simplified
void GUIManager::setWindowVisible(GUI_CUSTOM_WINDOW window, bool visible)
{
	bool retracted = false; //default status for panes
	core::dimension2d<u32> screen = App::getInstance()->getDevice()->getVideoDriver()->getScreenSize();
	IGUIElement* windowitems=NULL;
	IGUIWindow* guidialog=NULL;

	//Get back the windows pointers from the GUI Manager. Should be simplified. Too much stuff here!
	CGUIExtWindow* guiDynamicObjectsWindowChooser = (CGUIExtWindow*)getGUIElement(GCW_DYNAMIC_OBJECT_CHOOSER);
	CGUIExtWindow* guiDynamicPlayerWindowChooser = (CGUIExtWindow*)getGUIElement(GCW_DYNAMIC_PLAYER_EDIT);
	IGUIWindow* guiDynamicObjects_Context_Menu_Window=(IGUIWindow*)getGUIElement(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU);
	IGUIWindow* guiDynamicObjects_Context_Menu_Window1=(IGUIWindow*)getGUIElement(GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU1);
	CGUIExtWindow* guiDynamicObjectsWindowEditAction = (CGUIExtWindow*)getGUIElement(GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT);
	CGUIEditBoxIRB* guiDynamicObjects_Script = (CGUIEditBoxIRB*)getGUIElement(EB_ID_DYNAMIC_OBJECT_SCRIPT);
	IGUIWindow* guiTerrainToolbar = (IGUIWindow*)getGUIElement(GCW_TERRAIN_TOOLBAR);
	IGUIWindow* guiVegetationToolbar = (IGUIWindow*)getGUIElement(GCW_VEGE_TOOLBAR);
	IGUIWindow* guiAboutWindow = (IGUIWindow*)getGUIElement(GCW_ABOUT);
	
    switch(window)
    {
#ifdef EDITOR
		case GCW_DYNAMIC_OBJECT_INFO:
			retracted = guiDynamicObjectsWindowChooser->Status(CGUIExtWindow::PANE_LEFT);
			if (retracted)
				guiDynamicObjectsWindowChooser->Expand(guiDynamicObjectsWindowChooser->PANE_LEFT);
			else
				guiDynamicObjectsWindowChooser->Retract(guiDynamicObjectsWindowChooser->PANE_LEFT);
			if (visible)
			{
				guienv->setFocus(guiDynamicObjectsWindowChooser);
			}

			break;

        case GCW_DYNAMIC_OBJECT_CHOOSER:
			if(guiDynamicObjectsWindowChooser)
			{
				guiDynamicObjectsWindowChooser->setVisible(visible);
				if (visible)
					guienv->setFocus(guiDynamicPlayerWindowChooser);
			}
            break;

		case GCW_DYNAMIC_PLAYER_EDIT:
			if (guiDynamicPlayerWindowChooser)
			{
				guiDynamicPlayerWindowChooser->setVisible(visible);
				if (visible)
					guienv->setFocus(guiDynamicPlayerWindowChooser);
			}
			break;


        case GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU:

            mouseX = App::getInstance()->getDevice()->getCursorControl()->getPosition().X-100;
            mouseY = App::getInstance()->getDevice()->getCursorControl()->getPosition().Y-20;

			if (visible && screen.Height-200<mouseY+20) // Reposition the menu if it will be cropped by the screen clipping
				mouseY-=160;

			if (visible && mouseX<50) // Reposition the menu if it will be cropped by the screen clipping
				mouseX+=100;

			if (visible && (screen.Height-200<App::getInstance()->getDevice()->getCursorControl()->getPosition().Y || (App::getInstance()->getDevice()->getCursorControl()->getPosition().X-100)<50))
			{
				device->getCursorControl()->setPosition(mouseX+100,mouseY+20);
			}

			guiDynamicObjects_Context_Menu_Window->setRelativePosition(rect<s32>(mouseX,mouseY,mouseX+200,mouseY+220));
			if (visible)
			{
				guiDynamicObjects_Context_Menu_Window->setEnabled(true);
				guienv->setFocus(guiDynamicObjects_Context_Menu_Window);
			} else
			{
				guiDynamicObjects_Context_Menu_Window->setEnabled(false);
			}
            guiDynamicObjects_Context_Menu_Window->setVisible(visible);


            break;
		case GCW_ID_DYNAMIC_OBJECT_CONTEXT_MENU1:
			if (visible)
				guienv->setFocus(guiDynamicObjects_Context_Menu_Window1);
			mouseX = App::getInstance()->getDevice()->getCursorControl()->getPosition().X-100;
            mouseY = App::getInstance()->getDevice()->getCursorControl()->getPosition().Y-40;
			guiDynamicObjects_Context_Menu_Window1->setRelativePosition(rect<s32>(mouseX,mouseY,mouseX+200,mouseY+60));
            guiDynamicObjects_Context_Menu_Window1->setVisible(visible);

			break;


        case GCW_DYNAMIC_OBJECTS_EDIT_SCRIPT:
            guiDynamicObjectsWindowEditAction->setVisible(visible);
			if (visible)
			{
				guienv->setFocus(guiDynamicObjects_Script);
				guienv->getRootGUIElement()->bringToFront(guiDynamicObjects_Script);
			}
            break;
        case GCW_TERRAIN_TOOLBAR:
			if (visible)
				guienv->setFocus(guiTerrainToolbar);
            guiTerrainToolbar->setVisible(visible);
            break;
		case GCW_VEGE_TOOLBAR:
			if (visible)
				guienv->setFocus(guiVegetationToolbar);
			guiVegetationToolbar->setVisible(visible);
			break;
#endif
        case GCW_GAMEPLAY_ITEMS:
            this->updateItemsList();
			windowitems = getGUIElement(GCW_GAMEPLAY_ITEMS);
			if (windowitems)
				windowitems->setVisible(visible);
			
            break;
        case GCW_ABOUT:
			if (visible)
				guienv->setFocus(guiAboutWindow);
			guiAboutWindow->setVisible(visible);


            break;
        case GCW_TERRAIN_PAINT_VEGETATION:
            guiVegetationToolbar->setVisible(visible);
            break;
		case GCW_DIALOG:
			guidialog=(IGUIWindow*)this->getGUIElement(GCW_DIALOG);
			if (guidialog)
				guidialog->setVisible(visible);
			break;

        default:
            break;

    }
}

//! Check the visibility status of a IRB window
bool GUIManager::isWindowVisible(GUI_CUSTOM_WINDOW window)
{
	IGUIElement * elem = guienv->getRootGUIElement()->getElementFromId(window, true);
	if (elem)
	{
		return elem->isVisible();
	}

	else return false;
}



//! Update a object preview GUI
// Currently disabled
// Needed in the inventory GUI
void GUIManager::updateDynamicObjectPreview()
{
	// Temporary disabled until the new template system is in place.
	scene::ISceneNode* node = DynamicObjectsManager::getInstance()->findActiveObject();
	//if (node)
	//	guiDynamicObjects_NodePreview->setNode(node);
}

//! Return the text of the editboxes (currently only the script)
// Might be removed later, since we have a ID search that can
// give the same results
stringc GUIManager::getEditBoxText(GUI_ID id)
{
	CGUIEditBoxIRB* guiDynamicObjects_Script = (CGUIEditBoxIRB*)getGUIElement(EB_ID_DYNAMIC_OBJECT_SCRIPT);
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

//! This update for the new screen size... Needed by the images GUI
void GUIManager::updateGuiPositions(dimension2d<u32> screensize)
{
	this->screensize=screensize;
}
// Only in the editor
#ifdef EDITOR

//! Set the edit box text, can be the console or the script editor
void GUIManager::setEditBoxText(GUI_ID id, stringw text)
{

	 IGUIEditBox* guiDynamicObjects_Script_Console = (IGUIEditBox*)getGUIElement(EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE);
	 CGUIEditBoxIRB* guiDynamicObjects_Script = (CGUIEditBoxIRB*)getGUIElement(EB_ID_DYNAMIC_OBJECT_SCRIPT);
	//Clear the text before
	//guiDynamicObjects_Script_Console->setText(L"");
	//guiDynamicObjects_Script->setText(L"");

	//fix to get the extended characters not being removed inside.
	//Put the extended string to char, then encode it back
	core::stringc temptxt= text.c_str();
	char *mtext = (char *)temptxt.c_str(); // char buffer
    wchar_t buffer[131072]=L"";  //widestring buffer of 128k
    mbstowcs(buffer, mtext, strlen(mtext));
	core::stringw buf = (core::stringw)buffer;

	// ----------------------------------------------------
	// Shoulb be ok but a script should not be longer than 128k


	switch(id)
    {
        case EB_ID_DYNAMIC_OBJECT_SCRIPT_CONSOLE:
            guiDynamicObjects_Script_Console->setText(text.c_str());
            break;
        case EB_ID_DYNAMIC_OBJECT_SCRIPT:
			guiDynamicObjects_Script->setText(buf.c_str());
            break;
        default:
            break;
    }
}

void GUIManager::expandPanel(GUIManager::GUI_CUSTOM_WINDOW id)
{
	CGUIExtWindow* panel = (CGUIExtWindow*)getGUIElement(id);
	if (panel)
		panel->Expand(CGUIExtWindow::PANE_LEFT);
}

void GUIManager::contractPanel(GUIManager::GUI_ID id)
{
	CGUIExtWindow* panel = (CGUIExtWindow*)getGUIElement(id);
	//panel->Expand(CGUIExtWindow::PANE_LEFT);
}
#endif

//! Enable/Disable specific GUI buttons (Mostly IRB menu buttons)
// To be removed in the future and use this instead to get to the element:
// Ex: IGUIButton* button=(IGUIButton*)guienv->getRootGUIElement()->getElementFromId(GUIManager::BT_ID_TERRAIN_TRANSFORM,true);
void GUIManager::setElementEnabled(GUI_ID id, bool enable)
{
	IGUIButton* button=NULL;
	
	IGUIWindow* InnerChooser = (IGUIWindow*)getGUIElement(GCW_DYNAMIC_OBJECT_CHOOSER_INNER);
	IGUIWindow* InnerChooser1 = (IGUIWindow*)getGUIElement(GCW_DYNAMIC_OBJECT_CHOOSER_INNER1);
	IGUIWindow* InnerChooser2 = (IGUIWindow*)getGUIElement(GCW_DYNAMIC_OBJECT_CHOOSER_INNER2);
	IGUIWindow* InnerChooser3 = (IGUIWindow*)getGUIElement(GCW_DYNAMIC_OBJECT_CHOOSER_INNER3);

	IGUIButton* guiDOAddMode = (IGUIButton*)getGUIElement(BT_ID_DO_ADD_MODE);
	IGUIButton* guiDOSelMode = (IGUIButton*)getGUIElement(BT_ID_DO_SEL_MODE);
	IGUIButton* guiDOMovMode = (IGUIButton*)getGUIElement(BT_ID_DO_MOV_MODE);
	IGUIButton* guiDORotMode = (IGUIButton*)getGUIElement(BT_ID_DO_ROT_MODE);
	IGUIButton* guiDOScaMode = (IGUIButton*)getGUIElement(BT_ID_DO_SCA_MODE);
	
	///TODO: fazer metodo getElement by ID!!!
   switch(id)
    {
		case BT_ID_DO_ADD_MODE:
			InnerChooser->setVisible(enable);
			InnerChooser1->setVisible(!enable);
			InnerChooser2->setVisible(!enable);
			InnerChooser3->setVisible(!enable);

			guiDOAddMode->setPressed(enable);
			guiDOSelMode->setPressed(!enable);
			guiDOMovMode->setPressed(!enable);
			guiDORotMode->setPressed(!enable);
			guiDOScaMode->setPressed(!enable);
			break;
		case BT_ID_DO_SEL_MODE:
			InnerChooser->setVisible(!enable);
			InnerChooser1->setVisible(enable);
			InnerChooser2->setVisible(!enable);
			InnerChooser3->setVisible(enable);

			guiDOSelMode->setPressed(enable);
			guiDOAddMode->setPressed(!enable);
			guiDOMovMode->setPressed(!enable);
			guiDORotMode->setPressed(!enable);
			guiDOScaMode->setPressed(!enable);
			break;
		case BT_ID_DO_MOV_MODE:
			InnerChooser->setVisible(!enable);
			InnerChooser1->setVisible(!enable);
			InnerChooser2->setVisible(enable);
			InnerChooser3->setVisible(enable);

			guiDOAddMode->setPressed(!enable);
			guiDOSelMode->setPressed(!enable);
			guiDOMovMode->setPressed(enable);
			guiDORotMode->setPressed(!enable);
			guiDOScaMode->setPressed(!enable);
			break;
		case BT_ID_DO_ROT_MODE:
			InnerChooser->setVisible(!enable);
			InnerChooser1->setVisible(!enable);
			InnerChooser2->setVisible(enable);
			InnerChooser3->setVisible(enable);

			guiDOAddMode->setPressed(!enable);
			guiDOSelMode->setPressed(!enable);
			guiDOMovMode->setPressed(!enable);
			guiDORotMode->setPressed(enable);
			guiDOScaMode->setPressed(!enable);
			break;
		case BT_ID_DO_SCA_MODE:
			InnerChooser->setVisible(!enable);
			InnerChooser1->setVisible(!enable);
			InnerChooser2->setVisible(enable);

			guiDOAddMode->setPressed(!enable);
			guiDOSelMode->setPressed(!enable);
			guiDOMovMode->setPressed(!enable);
			guiDORotMode->setPressed(!enable);
			guiDOScaMode->setPressed(enable);
			break;
 
        default:
			button=(IGUIButton*)getGUIElement(id);
			{
				if (button)
				{
					button->setEnabled(enable);
					button->setPressed(!enable);
				}
			}
            break;

    }
}

// Set visibility of specific IRB gui (script editor, etc.)
void GUIManager::setElementVisible(GUI_ID id, bool visible)
{
	IGUIElement* playerlife=NULL;
	IGUIElement* barimage=NULL;
	IGUIElement* btviewitem=NULL;
	IGUIButton* guiPlayGame=NULL;
	IGUIButton* guiStopGame=NULL;

	IGUIButton* guiDOAddMode = NULL;
	IGUIButton* guiDOMovMode = NULL;
	IGUIButton* guiDORotMode = NULL;
	IGUIButton* guiDOScaMode = NULL;

	CGUIExtWindow* consolewin=NULL;

	IGUIWindow* guiMainWindow = (IGUIWindow*)getGUIElement(GCW_TOP_WINDOW);
	IGUIWindow* guiStatus = (IGUIWindow*)getGUIElement(GCW_STATUSBAR);
	 
	
    switch(id)
    {
        case BT_ID_PLAY_GAME:
			guiPlayGame=(IGUIButton*)getGUIElement(BT_ID_PLAY_GAME);
			if (guiPlayGame)
				guiPlayGame->setVisible(visible);
            break;

        case BT_ID_STOP_GAME:
			guiStopGame=(IGUIButton*)getGUIElement(BT_ID_STOP_GAME);
			if (guiStopGame)
				guiStopGame->setVisible(visible);
#ifdef EDITOR
			guiMainWindow->setVisible(!visible);
			guiStatus->setVisible(!visible);
#endif
            break;

        case ST_ID_PLAYER_LIFE:
			playerlife=getGUIElement(ST_ID_PLAYER_LIFE);
			if(playerlife)
				playerlife->setVisible(visible);

            //guiPlayerLife->setVisible(visible);
            //guiPlayerLife_Shadow->setVisible(visible);
            break;

        case BT_ID_PLAYER_EDIT_SCRIPT:
            break;

		case IMG_BAR:
			barimage = getGUIElement(IMG_BAR);
			if (barimage)
				barimage->setVisible(visible);
			
			//gameplay_bar_image->setVisible(visible);
			break;

		case CONSOLE:
			// Show hide the console. If it`s visible, focus on it
			consolewin=(CGUIExtWindow*)getGUIElement(GCW_CONSOLE);
			consolewin->setVisible(visible);
			if (visible)
				guienv->setFocus(consolewin);
			break;

		case BT_ID_VIEW_ITEMS:
		{
			btviewitem = getGUIElement(BT_ID_VIEW_ITEMS);
			if (btviewitem)
				btviewitem->setVisible(visible);
	      //  guiBtViewItems->setVisible(visible);
			// Update the gold items
			stringc playerMoney = LANGManager::getInstance()->getText("txt_player_money");
			playerMoney += DynamicObjectsManager::getInstance()->getPlayer()->getMoney();
			IGUIStaticText* text = (IGUIStaticText*)getGUIElement(ST_ID_PLAYER_MONEY);
			if (text)
				text->setText(((core::stringw)playerMoney).c_str());
		}
        break;

		case BT_ID_DO_SEL_MODE:
			guiDOMovMode = (IGUIButton*)getGUIElement(BT_ID_DO_MOV_MODE);
			guiDORotMode = (IGUIButton*)getGUIElement(BT_ID_DO_ROT_MODE);
			guiDOScaMode = (IGUIButton*)getGUIElement(BT_ID_DO_SCA_MODE);
			// This is used to unlock and display panels when in SEL mode and a object is selected.
			guiDOMovMode->setEnabled(visible); // Lock/Unlock the MOVE button
			guiDORotMode->setEnabled(visible); // Lock/Unlock the ROTATE button
			guiDOScaMode->setEnabled(visible); // Lock/Unlock the SCALE button
			break;

		case BT_ID_DO_ADD_MODE:
			guiDOAddMode = (IGUIButton*)getGUIElement(BT_ID_DO_ADD_MODE);
			guiDOAddMode->setPressed(visible); // Set the button press state (Needed when coming back in object edit mode as ADD is the default state.Game
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
	GUIGame::getInstance()->showBlackScreen(text);
}

void GUIManager::hideBlackScreen()
{
	GUIGame::getInstance()->hideBlackScreen();
}

void GUIManager::loadFonts()
{
	guiFontCourier12 = guienv->getFont("../media/fonts/courier12.xml");
	guiFontCourier11 = guienv->getFont("../media/fonts/courier11.xml");
	guiFontCourier10 = guienv->getFont("../media/fonts/dejavu12.xml");
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

void GUIManager::setConsoleText(stringw text, video::SColor color)
// Add text into the output console
// The function manage up to 5000 lines before clearing the buffer
// Using "forcedisplay" will toggle the display of the GUI
{
	//Temporary disable of this method to gain speed
	//Will have a toggle to use/not use this in the future

	if (!device)
		return;

	u32 maxitem = 5000;
	// If the GUI is not displayed, accumulate the info in a buffer
	if (textevent.size()<maxitem)
	{
		textevent.push_back(text);
		texteventcolor.push_back(color);
	}
	IGUIListBox* console = (IGUIListBox*)getGUIElement(LB_ID_LOGGER_MESSAGE);

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
	IGUIListBox* consolelog = (IGUIListBox*)getGUIElement(LB_ID_LOGGER_CONSOLE);
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

//stop sound when player cancel the dialog
void GUIManager::stopDialogSound()
{
	GUIGame::getInstance()->stopDialogSound();

}

void GUIManager::showDialogMessage(stringw text, std::string sound)
{

	GUIGame::getInstance()->showDialogMessage(text,sound);
}

bool GUIManager::showDialogQuestion(stringw text, std::string sound )
{
	return GUIGame::getInstance()->showDialogQuestion(text, sound); 
}

//Need to be reworked: BAD. Render loop is done here!
stringc GUIManager::showInputQuestion(stringw text)
{
	return GUIGame::getInstance()->showInputQuestion(text);
}

stringc GUIManager::getActivePlayerItem()
{
	return GUIGame::getInstance()->getActivePlayerItem();
}

DynamicObject* GUIManager::getActiveLootItem()
{
	return GUIGame::getInstance()->getActiveLootItem();
}

void GUIManager::updateItemsList()
{
	GUIGame::getInstance()->updateItemsList();
}

void GUIManager::updateNodeInfos(irr::scene::ISceneNode *node)
{
	vector3df pos = vector3df(0,0,0);
	vector3df rot = vector3df(0,0,0);
	vector3df sca = vector3df(0,0,0);

	if (node)
	{
		pos = node->getPosition();
		rot = node->getRotation();
		sca = node->getScale();
	}

	// Set the spinbox gui with the information

	IGUISpinBox* pos_x_text=(IGUISpinBox*)getGUIElement(TI_ID_POS_X);
	IGUISpinBox* pos_y_text=(IGUISpinBox*)getGUIElement(TI_ID_POS_Y);
	IGUISpinBox* pos_z_text=(IGUISpinBox*)getGUIElement(TI_ID_POS_Z);
	IGUISpinBox* rot_x_text=(IGUISpinBox*)getGUIElement(TI_ID_ROT_X);
	IGUISpinBox* rot_y_text=(IGUISpinBox*)getGUIElement(TI_ID_ROT_Y);
	IGUISpinBox* rot_z_text=(IGUISpinBox*)getGUIElement(TI_ID_ROT_Z);
	IGUISpinBox* sca_x_text=(IGUISpinBox*)getGUIElement(TI_ID_SCA_X);
	IGUISpinBox* sca_y_text=(IGUISpinBox*)getGUIElement(TI_ID_SCA_Y);
	IGUISpinBox* sca_z_text=(IGUISpinBox*)getGUIElement(TI_ID_SCA_Y);

	pos_x_text->setValue(pos.X);
	pos_y_text->setValue(pos.Y);
	pos_z_text->setValue(pos.Z);
	rot_x_text->setValue(rot.X);
	rot_y_text->setValue(rot.Y);
	rot_z_text->setValue(rot.Z);
	sca_x_text->setValue(sca.X);
	sca_y_text->setValue(sca.Y);
	sca_z_text->setValue(sca.Z);

}

// Flush all gui elements
void GUIManager::flush()
{
#ifdef EDITOR
	IGUIButton* guiMainLoadProject=(IGUIButton*)getGUIElement(BT_ID_LOAD_PROJECT);
	IGUIButton* guiMainSaveProject=(IGUIButton*)getGUIElement(BT_ID_SAVE_PROJECT);
	IGUIButton* guiMainNewProject=(IGUIButton*)getGUIElement(BT_ID_NEW_PROJECT);
	IGUIButton*  guiDynamicObjects_LoadScriptTemplateBT=(IGUIButton*)getGUIElement(BT_ID_DYNAMIC_OBJECT_LOAD_SCRIPT_TEMPLATE);
    guiMainLoadProject->setPressed(false);
    guiMainSaveProject->setPressed(false);
    guiMainNewProject->setPressed(false);
    guiDynamicObjects_LoadScriptTemplateBT->setPressed(false);
#endif
    //guiBtViewItems->setPressed(false);
}

// Display the configuration Window GUI
void GUIManager::showConfigWindow()
{
	App::APP_STATE old_State = App::getInstance()->getAppState();
	App::getInstance()->setAppState(App::APP_EDIT_WAIT_GUI);
	GUIConfigWindow* configWindow=GUIEditor::getInstance()->getConfigWindow();
	if (configWindow)
		configWindow->showWindow();

    App::getInstance()->setAppState(old_State);
}

void GUIManager::showCutsceneText(bool visible)
{
	GUIGame::getInstance()->showCutsceneText(visible);
}

void GUIManager::setCutsceneText(core::stringw text)
{
	GUIGame::getInstance()->setCutsceneText(text);
}



IGUIElement* GUIManager::getGUIElement(u32 id)
{
	IGUIElement* elem = guienv->getRootGUIElement()->getElementFromId(id, true);
	if (!elem)
		printf("failed to get this id: %d\n",(int)id); 
	return guienv->getRootGUIElement()->getElementFromId(id, true);
}