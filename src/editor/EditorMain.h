/***************************************************************
 * Name:      wxIrrHelloApp.h
 * Purpose:   Defines Application Class
 * Author:    Steve Bondy (stevebondy@shaw.ca)
 * Created:   2009-12-12
 * Copyright: Steve Bondy (http://stevebondy.ca)
 * License:
 * Partly used in IRR RPG Builder
 * Updated by: Christian Clavet (http://www.clavet.org)
 **************************************************************/

#ifndef WXIRRHELLOAPP_H
#define WXIRRHELLOAPP_H

#include <wx/wx.h>

#include "wx/app.h"
#include "wx/frame.h"
//#include "wx/textctrl.h"
#include <wx/listctrl.h> 
#include "wx/sizer.h"
#include "wx/menu.h"
#include "wx/dcbuffer.h"
#include "wx/colordlg.h"
#include "wx/artprov.h"

#include "wxIrrlicht.h"
#include "wxIWindow.h"


#include "../App.h"

//class CIrrApp;
//class CIrrFrame;

enum
{
	ID_Quit = 1,
	ID_About,
	ID_StartUpdate,
	ID_StopUpdate,
	ID_Irrlicht,
};

class CIrrApp : public wxApp
{
	public:
		virtual bool OnInit(void);
};

class ConsoleDialog : public wxDialog
{
public:
    ConsoleDialog(wxWindow *parent);
	void AddMessage(core::stringw text, video::SColor color);
    void OnClose(wxCloseEvent& event);
	
private:
    DECLARE_EVENT_TABLE()
	wxListCtrl * m_textMsg;
};

class CIrrFrame : public wxFrame
{
	public:

	enum
    {
        ID_PRIMARY_COLOUR,
        ID_SECONDARY_COLOUR,
        ID_DEFAULT_PROVIDER,
        ID_AUI_PROVIDER,
        ID_MSW_PROVIDER,
		ID_New,
		ID_Save,
		ID_Load,
		ID_Quit,
		ID_Play,
		ID_Stop,
		ID_ObjEditMode,
		ID_PlayerEdit,
		ID_GlobalScript,
		ID_PlayerScript,
		ID_TerrainSegment,
		ID_TerrainTransform,
		ID_TerrainTree,
		ID_EditConfig,
		ID_EditGameConfig,
		ID_Console,
		ID_About,
		ID_UpdateTimer,
        ID_MAIN_TOOLBAR,
		ID_TOOLBAR_TERRAIN,
		ID_TOOLBAR_TERRAIN2,
		ID_TOOLBAR_DYNOBJ1,
		ID_TOOLBAR_DYNOBJ2,
        ID_POSITION_TOP,
        ID_POSITION_TOP_ICONS,
        ID_POSITION_TOP_BOTH,
        ID_POSITION_LEFT,
        ID_POSITION_LEFT_LABELS,
        ID_POSITION_LEFT_BOTH,
    };

	  
		CIrrFrame(const wxString &title, const wxPoint &pos, const wxSize &size, long style=wxDEFAULT_FRAME_STYLE);
		~CIrrFrame();

		CIrrFrame getInstance();
		void OnProgressTimer(wxTimerEvent& event);
		void OnClose (wxCloseEvent& e);
		void OnAbout(wxCommandEvent &event);
		void OnStartUpdate(wxCommandEvent &event);
		void OnStopUpdate(wxCommandEvent &event);
		void OnTakeOver(wxCommandEvent &event);
		
		// Will call the file request feature of wxWidget from outside
		void OnNew();
		void OnSave();
		void OnLoad();

		void MessageStatus(core::stringw message);

		int MessageBox(core::stringw message, core::stringw message2, int buttons);
		core::stringw FileOpen(core::stringw message);
		core::stringw FileSave();
		ConsoleDialog * console_dialog;

		DECLARE_EVENT_TABLE()

	protected:
		wxIDevice *device3D;
		wxCIWindow *window3D;


	private:
};

#endif // WXIRRHELLOAPP_H
