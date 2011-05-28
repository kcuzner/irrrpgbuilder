/***************************************************************
 * Name:      wxIrrHelloApp.h
 * Purpose:   Defines Application Class
 * Author:    Steve Bondy (stevebondy@shaw.ca)
 * Created:   2009-12-12
 * Copyright: Steve Bondy (http://stevebondy.ca)
 * License:
 **************************************************************/

#ifndef WXIRRHELLOAPP_H
#define WXIRRHELLOAPP_H

#include <wx/wx.h>

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/textctrl.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/gallery.h"
#include "wx/ribbon/toolbar.h"
#include "wx/sizer.h"
#include "wx/menu.h"
#include "wx/dcbuffer.h"
#include "wx/colordlg.h"
#include "wx/artprov.h"

#include "wxIrrlicht.h"
#include "wxIWindow.h"


#include "../app.h"

class CIrrApp;
class CIrrFrame;

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

		void OnQuit(wxRibbonButtonBarEvent& WXUNUSED(evt));
		void OnClose (wxCloseEvent& e);
		void OnAbout(wxCommandEvent &event);
		void OnStartUpdate(wxCommandEvent &event);
		void OnStopUpdate(wxCommandEvent &event);
		void OnTakeOver(wxCommandEvent &event);
		void OnNew(wxRibbonButtonBarEvent& WXUNUSED(evt));
		void OnSave(wxRibbonButtonBarEvent& WXUNUSED(evt));
		void OnLoad(wxRibbonButtonBarEvent& WXUNUSED(evt));
		void OnPlay(wxRibbonButtonBarEvent& WXUNUSED(evt));
		void OnStop(wxRibbonButtonBarEvent& WXUNUSED(evt));
		void OnObjectEditMode(wxRibbonButtonBarEvent& WXUNUSED(evt));
		void OnPlayerEdit(wxRibbonButtonBarEvent& WXUNUSED(evt));
		void OnScriptGlobal(wxRibbonButtonBarEvent& WXUNUSED(evt));
		void OnScriptPlayer(wxRibbonButtonBarEvent& WXUNUSED(evt));
		void OnTerrainSegment(wxRibbonButtonBarEvent& WXUNUSED(evt));
		void OnTerrainTransform(wxRibbonButtonBarEvent& WXUNUSED(evt));
		void OnTerrainTree(wxRibbonButtonBarEvent& WXUNUSED(evt));
		void OnDisplayConsole(wxRibbonButtonBarEvent& WXUNUSED(evt));

		int MessageBox(core::stringw message, core::stringw message2, int buttons);
		core::stringw FileOpen(core::stringw message);
		core::stringw FileSave();
		void UncheckAllButtons();


		DECLARE_EVENT_TABLE()

	protected:
		wxIDevice *device3D;
		wxCIWindow *window3D;

		wxRibbonBar* m_ribbon;
		wxRibbonBar* m_ribbon1;
		wxRibbonBar* m_ribbon2;

		wxRibbonButtonBar *test_button;
		wxRibbonButtonBar *project;
		wxRibbonButtonBar *terrain_button;
		wxRibbonButtonBar *terrain_buttonv;
		wxRibbonButtonBar *dynobject_button;
		wxRibbonButtonBar *script_button;
		wxRibbonButtonBar *tools_button;
		wxRibbonButtonBar *option_button;
};

#endif // WXIRRHELLOAPP_H
