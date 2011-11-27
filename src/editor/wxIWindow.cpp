#include "wxIWindow.h"

wxCIDevice::wxCIDevice()
{
	irrDevice = NULL;
}

wxCIDevice::wxCIDevice(HWND hwnd, wxIDriverType type, bool bResizeable)
{
    Create(hwnd, type, bResizeable);
}

wxCIDevice::~wxCIDevice()
{
	if ( irrDevice )
	{
		//App::getInstance()->shutdown();
		//irrDevice->closeDevice();
		irrDevice->drop();
	}
}

wxIDevice *wxCIDevice::Create(HWND hwnd, wxIDriverType type, bool bResizeable)
{
	if (irrDevice)
		return irrDevice;

	wxICreateParam param;

	//param.WindowSize =   irr::core::dimension2d<s32>(1008,596); 

	param.WindowId = reinterpret_cast<void *>( hwnd );
	param.DriverType = type;

	irrDevice = irr::createDeviceEx(param);

	if ( irrDevice )
		irrDevice->setResizable( bResizeable );

	return irrDevice;
}

////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxCIWindow, wxWindow)
	EVT_PAINT(wxCIWindow::OnPaint)
	EVT_SIZE(wxCIWindow::OnSize)
	EVT_TIMER( wxID_ANY, wxCIWindow::OnTimer)
	EVT_MOUSE_EVENTS(wxCIWindow::OnMouse)
	//EVT_KEY_DOWN(wxCIWindow::OnKey)
    //EVT_KEY_UP(wxCIWindow::OnKey)
END_EVENT_TABLE()

wxCIWindow::wxCIWindow(wxWindow *parent, wxWindowID id, long style, wxIDriverType type, bool bResizeable)
:	wxWindow( parent, id, wxDefaultPosition, wxDefaultSize, style )
{
	wxCIDevice::Create( (HWND) wxWindow::GetHandle(), type, bResizeable );
    m_timer.SetOwner(this, wxID_ANY);
	font = irrDevice->getGUIEnvironment()->getBuiltInFont();
}

wxCIWindow::~wxCIWindow()
{
}

void wxCIWindow::OnPaint(wxPaintEvent &event)
{
	wxPaintDC dc(this);
    //doUpdate();
	event.Skip();
}
void wxCIWindow::doUpdate() {
	if ( irrDevice )
	{
		wxIVideoDriver *driver=GetVideoDriver();
		wxISceneManager *scenemgr=GetSceneManager();

		if ( driver && scenemgr )
		{
			//App::getInstance()->update();
		}
		if ( !m_timer.IsRunning() )
				Update();
	}
}
void wxCIWindow::OnSize(wxSizeEvent &event)
{
	if ( irrDevice )
	{
		wxIVideoDriver *driver=GetVideoDriver();

		if ( driver )
		{
			driver->OnResize( wxIDimension2DInteger(GetClientSize().GetWidth(), GetClientSize().GetHeight()) );
			wxISceneManager *scene=GetSceneManager();
			wxICamera * camera=scene->getActiveCamera();
			
			// Set back the proper aspect ratio of the camera if there is a camera
			int w, h;
			this->GetSize(&w, &h);
			// Tell the new size to Irrlicht
			//App::getInstance()->setScreenSize(dimension2d<u32>(w,h));
			if (camera)
				camera->setAspectRatio(float(w)/h);
			
			if ( !m_timer.IsRunning() )
				Update();

		}
	}
	event.Skip();
}

void wxCIWindow::OnTimer(wxTimerEvent &event)
{
	Refresh( false );
	// Update the IRRlicht timer.
	App::getInstance()->getDevice()->getTimer()->tick();
	event.Skip();
}

void wxCIWindow::OnMouse(wxMouseEvent& event) 
{
    irr::SEvent sevt;

    sevt.EventType = irr::EET_MOUSE_INPUT_EVENT;

    if (event.IsButton()) 
	{
        if (event.LeftDown())
            sevt.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
        else if (event.LeftUp())
            sevt.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
        else if (event.MiddleDown())
            sevt.MouseInput.Event = irr::EMIE_MMOUSE_PRESSED_DOWN;
        else if (event.MiddleUp())
            sevt.MouseInput.Event = irr::EMIE_MMOUSE_LEFT_UP;
        else if (event.RightDown())
            sevt.MouseInput.Event = irr::EMIE_RMOUSE_PRESSED_DOWN;
        else if (event.RightUp())
            sevt.MouseInput.Event = irr::EMIE_RMOUSE_LEFT_UP;
		else 
			sevt.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
    }
    else if (event.GetWheelRotation() != 0) 
	{
		// This seem relatively the same as IRRlicht
        sevt.MouseInput.Wheel = (float)event.GetWheelRotation()/50;
        sevt.MouseInput.Event = irr::EMIE_MOUSE_WHEEL;
    }    
    else //if (event.Moving() || event.Dragging()) 
	{
		// We need to send at least a compatible event to IRRlicht
		sevt.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
	}

    sevt.MouseInput.X = event.GetX();
    sevt.MouseInput.Y = event.GetY();

	irrDevice->postEventFromUser(sevt);

    if (!m_timer.IsRunning()) {
        Update();
	}
    event.Skip();
}

void wxCIWindow::OnKey(wxKeyEvent& event) 
{
    irr::SEvent sevt;

    sevt.EventType = irr::EET_KEY_INPUT_EVENT;

    sevt.KeyInput.Key = (irr::EKEY_CODE)event.GetUnicodeKey();
		//GetRawKeyCode();
    sevt.KeyInput.PressedDown = event.GetEventType() == wxEVT_KEY_DOWN;

    sevt.KeyInput.Shift = event.ShiftDown();
	sevt.KeyInput.Control = event.CmdDown();
	
#if wxUSE_UNICODE
    sevt.KeyInput.Char = event.GetUnicodeKey();
	
#else
    sevt.KeyInput.Char = event.GetKeyCode();
#endif

    //irrDevice->postEventFromUser(sevt);
	irrDevice->run();

    if (!m_timer.IsRunning()) {
		Update();
	}
	
    event.Skip();
}
