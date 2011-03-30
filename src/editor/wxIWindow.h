#ifndef WX_IRRLICHT_DEVICE
#define WX_IRRLICHT_DEVICE

class wxCIDevice;
class wxCIWindow;

#include "wxIrrlicht.h"

#include "../app.h"

class wxCIDevice
{
	public:
		wxCIDevice();
		wxCIDevice(HWND hwnd, wxIDriverType type=irr::video::EDT_OPENGL, bool bResizeable=true);
		~wxCIDevice();

		wxIDevice *Create(HWND hwnd, wxIDriverType type=irr::video::EDT_OPENGL, bool bResizeable=true);

		inline wxIDevice *GetHandle(void) { return irrDevice; }
		inline wxISceneManager *GetSceneManager(void) { return irrDevice ? irrDevice->getSceneManager() : NULL; }
		inline wxIVideoDriver *GetVideoDriver(void) { return irrDevice ? irrDevice->getVideoDriver() : NULL; }

	protected:
		wxIDevice *irrDevice;
};

class wxCIWindow : public wxWindow, public wxCIDevice
{
	public:
		wxCIWindow(wxWindow *parent, wxWindowID id, long style=wxTAB_TRAVERSAL, wxIDriverType type=irr::video::EDT_OPENGL, bool bResizeable=true);
		~wxCIWindow();

		void OnPaint(wxPaintEvent &event);
		void doUpdate();
		void OnSize(wxSizeEvent &event);
		void OnTimer(wxTimerEvent &event);
		void OnMouse(wxMouseEvent& event); 
		void OnKey(wxKeyEvent& event);
		inline IrrlichtDevice * getDevice() { return wxCIDevice::GetHandle();}

		inline void StartUpdate(int millisecs) { (!m_timer.IsRunning()) ? m_timer.Start(millisecs) : NULL; }
		inline void StopUpdate(void) { if ( m_timer.IsRunning() ) { m_timer.Stop();} }
		

	DECLARE_EVENT_TABLE()

	protected:
		
	private:
		wxTimer m_timer;
		wxIGuiFont *font;
};

#endif
