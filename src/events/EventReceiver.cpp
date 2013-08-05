#include "EventReceiver.h"
#include "../gui/GUIManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

EventReceiver::EventReceiver()
{
	memset(mouse, 0x00, sizeof mouse);
	memset(keys, 0x00, sizeof keys);

	//mouse = {0};
	//keys = {0};
}

EventReceiver::~EventReceiver()
{
    //dtor
}

EventReceiver* EventReceiver::getInstance()
{
    static EventReceiver *instance = 0;
    if (!instance) instance = new EventReceiver();
    return instance;
}


bool EventReceiver::isMousePressed(int mb)
{
    return mouse[mb];
}

bool EventReceiver::isKeyPressed(int key)
{
    return keys[key];
}


bool EventReceiver::OnEvent(const SEvent& event)
{
	s32 id = 0;
	stringw text = "";

	switch (event.EventType)
	{
	// Keyboard input events
	case irr::EET_KEY_INPUT_EVENT:
		if(event.KeyInput.PressedDown == true)
        {
            keys[event.KeyInput.Key] = 1;
        }
        else
        {
            keys[event.KeyInput.Key] = 0;
        }
		App::getInstance()->eventKeyPressed(event.KeyInput.Key);
		break;

	// Mouse events
	case irr::EET_MOUSE_INPUT_EVENT:
        App::getInstance()->eventMousePressed(event.MouseInput.Event);

		mouse[event.MouseInput.Event] = 1;

		if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
		{
			mouse[EMIE_LMOUSE_PRESSED_DOWN] = 1;
			//mouse[EMIE_RMOUSE_PRESSED_DOWN] = 1;
		}
		else if(event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
        {
			mouse[EMIE_LMOUSE_PRESSED_DOWN] = 0;
        }
		else if(event.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN)
		{
			mouse[EMIE_RMOUSE_PRESSED_DOWN] = 1;
		}
        else if(event.MouseInput.Event == EMIE_RMOUSE_LEFT_UP)
        {
			mouse[EMIE_RMOUSE_PRESSED_DOWN] = 0;
        }
        else if(event.MouseInput.Event == EMIE_MOUSE_WHEEL)
        {
            App::getInstance()->eventMouseWheel(event.MouseInput.Wheel);
        }
		break;

	// GUIevents
	case EET_GUI_EVENT:
		id = event.GUIEvent.Caller->getID();
		// GUI Events per types
        switch(event.GUIEvent.EventType)
        {
            case EGET_BUTTON_CLICKED:
                App::getInstance()->eventGuiButton(id);
                break;
#ifdef EDITOR
            case EGET_CHECKBOX_CHANGED:
                App::getInstance()->eventGuiCheckbox(id);
                break;

            case EGET_COMBO_BOX_CHANGED:
                App::getInstance()->eventGuiCombobox(id);
                break;

			case EGET_MESSAGEBOX_YES:
				App::getInstance()->eventMessagebox(EGET_MESSAGEBOX_YES);
				break;

			case EGET_FILE_SELECTED:
				App::getInstance()->loadProjectFile(true);
				break;

			case EGET_FILE_CHOOSE_DIALOG_CANCELLED:
				App::getInstance()->loadProjectFile(false);
				break;

			case EGET_EDITBOX_ENTER:
				App::getInstance()->eventGuiEditBox(id);
				break;

			case EGET_EDITBOX_CHANGED:
				App::getInstance()->eventGuiEditBox(id);
				break;

			case EGET_LISTBOX_CHANGED:
				App::getInstance()->eventGuiCombobox(id);
				break;

			// Perhaps create a real function for this
			// At the moment it share the id with the Combo Box
			case  EGET_LISTBOX_SELECTED_AGAIN:
				App::getInstance()->eventGuiCombobox(id);
				break;
#endif
            default: break;
		}
        break;

	// Irrlicht logger events
	case EET_LOG_TEXT_EVENT:
		text += stringw(event.LogEvent.Text).c_str();
		if (text.size()>0)
		   App::getInstance()->textevent.push_back(text.c_str());
		break;

	default: break;
	}


    return false;
}

void EventReceiver::flushKeys()
{
    //keys = {0};
	memset(keys, 0x00, sizeof keys);
}

void EventReceiver::flushMouse()
{
    //mouse = {0};
	memset(mouse, 0x00, sizeof mouse);
}
