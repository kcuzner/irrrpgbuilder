#include "EditorMain.h"
#include "../App.h"
#include "wxIWindow.h"
#include "wx/wx.h"
#include "wx/aboutdlg.h"


IMPLEMENT_APP(CIrrApp)

bool CIrrApp::OnInit()
{
	
	CIrrFrame *frame = new CIrrFrame( _T("IRR RPG Builder (SVN Release 0.2 Alpha) - January 2012"), wxPoint(0,0), wxSize(10,10) );
	frame->Show(TRUE);
	core::dimension2d<u32> deskres=core::dimension2d<u32>(0,0);
	if (App::getInstance()->fullScreen)
	{
		IrrlichtDevice *nulldevice = createDevice(video::EDT_NULL);
		deskres = nulldevice->getVideoModeList()->getDesktopResolution();
		nulldevice -> drop();
		deskres.Height-=30;
		deskres.Width-=8;
		frame->SetSize(deskres.Width,deskres.Height);
	}
	else
	{
		frame->SetSize(App::getInstance()->getScreenSize().Width, App::getInstance()->getScreenSize().Height);
		deskres.Height=(u32)frame->GetScreenRect().GetHeight()-60;
		deskres.Width=(u32)frame->GetScreenRect().GetWidth()-16;
	}


	frame->Centre();
	SetTopWindow(frame);
	frame->Show(TRUE);
	
	if (App::getInstance()->getDevice()->run()==true)
	{
		App::getInstance()->setScreenSize(deskres);
		App::getInstance()->hideEditGui();
		App::getInstance()->initialize();
		App::getInstance()->run();	
	}	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CIrrFrame, wxFrame)
	EVT_MENU(ID_About, CIrrFrame::OnAbout)
	EVT_MENU(ID_StartUpdate, CIrrFrame::OnStartUpdate)
	EVT_MENU(ID_StopUpdate, CIrrFrame::OnStopUpdate)
	EVT_MENU(ID_Irrlicht,CIrrFrame::OnTakeOver)
	EVT_TIMER(ID_UpdateTimer, OnProgressTimer)
	
END_EVENT_TABLE()

	BEGIN_EVENT_TABLE(ConsoleDialog, wxDialog)
        EVT_CLOSE(ConsoleDialog::OnClose)
	END_EVENT_TABLE()

/*  Perhaps will be removed as we will not use wxWiget for drawing the main gui
#include "icons/empty.xpm"
#include "icons/bt_close_program.xpm"
#include "icons/bt_dynamic_objects_mode.xpm"
#include "icons/bt_edit_character.xpm"
#include "icons/bt_player_edit_script.xpm"
#include "icons/bt_edit_script_global.xpm"
#include "icons/bt_config.xpm"
#include "icons/bt_play_game.xpm"
#include "icons/bt_stop_game.xpm"
#include "icons/bt_terrain_add_segment.xpm"
#include "icons/bt_terrain_paint_vegetation.xpm"
#include "icons/bt_terrain_up.xpm"
#include "icons/bt_load_project.xpm"
#include "icons/bt_new_project.xpm"
#include "icons/bt_save_project.xpm"
#include "icons/bt_about.xpm"
#include "icons/bt_help.xpm"
#include "icons/logo1.xpm"
*/

CIrrFrame::CIrrFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame((wxFrame *)NULL, -1, title, pos, size, style)
{
	SetIcon(wxICON(sample));
	wxBoxSizer* bSizer1;
	wxBoxSizer* bSizer2;

	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	bSizer1 = new wxBoxSizer( wxVERTICAL );


	window3D = new wxCIWindow( this, wxID_ANY, style, irr::video::EDT_OPENGL, true );
	App::getInstance()->setupDevice(window3D->getDevice());
	window3D->AcceptsFocus();
	bSizer1->Add( window3D, 1, wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();

	//wxMenu *menuFile = new wxMenu;

	// Timer set there to check for the engine event (IRRlicht), need to pull updates
	wxTimer * m_timer = new wxTimer(this, ID_UpdateTimer);
	m_timer->Start(25);

	 
	console_dialog = (ConsoleDialog *)NULL;

/*	menuFile->Append( ID_About, _T("&About...") );
	menuFile->AppendSeparator();
	menuFile->Append( ID_Quit, _T("E&xit") );

	wxMenu *menuCommands = new wxMenu;
	menuCommands->Append( ID_StartUpdate, _T("&Start Update") );
	menuCommands->Append( ID_StopUpdate, _T("St&op Update") );
	menuCommands->Append( ID_Irrlicht, _T("Irrlicht takeover") );

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, _T("&File") );
	menuBar->Append( menuCommands, _T("&Command") );
*/


	CreateStatusBar();
	SetStatusText(App::getInstance()->getLangText("msg_welcome").c_str());
	// m_ribbon->SetActivePage(dynobject);
	Connect (-1, wxEVT_CLOSE_WINDOW,
             wxCloseEventHandler (CIrrFrame::OnClose),
             NULL, this);

	//App::getInstance()->run();
	//window3D->StartUpdate(0);

	// Temporary
	if (!console_dialog)
		console_dialog = new ConsoleDialog(this);
	if (console_dialog)
	{
		if (console_dialog->IsVisible())
			console_dialog->Hide();
		//else
		//	console_dialog->Show();
	}

	App::getInstance()->setFramePointer(this);

}

CIrrFrame::~CIrrFrame()
{
}


void CIrrFrame::OnClose (wxCloseEvent& e)
{
	if (window3D)
	{	
		
		App::getInstance()->shutdown();
		window3D->ClearEventHashTable();
		
		window3D->Close(true);
		//window3D->DissociateHandle();
		
	}
	exit(0);
}

void CIrrFrame::OnProgressTimer(wxTimerEvent& event)
{
	// Check for console events on the APP class (from timer event here (250ms)
	if (console_dialog && console_dialog->IsVisible())
	{
		for (int a=0; a<(int)App::getInstance()->getConsoleText().size(); a++)
		{
			console_dialog->AddMessage(App::getInstance()->getConsoleText()[a].c_str(),App::getInstance()->getConsoleColor()[a]);
		}
		App::getInstance()->clearConsole();
		//console_dialog->AddMessage(L"Timed test message",SColor(255,0,0,64));
	}
}

void CIrrFrame::OnNew()
{
	if (MessageBox(App::getInstance()->getLangText("msg_prj_np0").c_str(),App::getInstance()->getLangText("msg_prj_np1").c_str(),2)==1)
	{
		App::getInstance()->createNewProject();
		wxLogStatus(App::getInstance()->getLangText("msg_prj_np2").c_str());
	}

}

void CIrrFrame::OnSave()
{

	APP_STATE old_state = App::getInstance()->getAppState();
	App::getInstance()->setAppState(APP_EDIT_WAIT_GUI);

	if (MessageBox(App::getInstance()->getLangText("msg_prj_sp0").c_str(),App::getInstance()->getLangText("msg_prj_sp1").c_str(),2)==1)
	{
		stringw result = FileSave();
		if (result.size()>2)
		{
			App::getInstance()->saveProjectToXML(result.c_str());
			wxLogStatus(App::getInstance()->getLangText("msg_prj_sp2").c_str(),result.c_str());
		}
	}
	App::getInstance()->setAppState(old_state);
}

void CIrrFrame::OnLoad()
{
	APP_STATE old_state = App::getInstance()->getAppState();
	App::getInstance()->setAppState(APP_EDIT_WAIT_GUI);

	// Change (Release 2.8 don't seem to work as in 2.9) Seem fixed now. (1/26/12)
	int boxres = MessageBox(App::getInstance()->getLangText("msg_prj_lp1").c_str(),App::getInstance()->getLangText("msg_prj_lp1").c_str(),3);
	if (boxres==1 || boxres==0)
	{
		if (boxres==1)
		{
			App::getInstance()->saveProjectToXML(App::getInstance()->getProjectName().c_str());
			wxLogStatus(App::getInstance()->getLangText("msg_prj_lp2").c_str(),App::getInstance()->getProjectName().c_str());
		}

		core::stringw result=FileOpen(App::getInstance()->getLangText("msg_prj_lp3").c_str());
		if (result.size()>2)
		{
			App::getInstance()->cleanWorkspace();
			App::getInstance()->loadProjectFromXML(result.c_str());
			wxLogStatus(App::getInstance()->getLangText("msg_prj_lp4").c_str(),result.c_str());
		}
	}
	App::getInstance()->setAppState(old_state);
}

void CIrrFrame::MessageStatus(core::stringw message)
{
	wxLogStatus(message.c_str());
};

void CIrrFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox( _T("This is IRR RPG Builder"),	_T("About IRR RPG Builder"), wxOK|wxICON_INFORMATION, this );
}

void CIrrFrame::OnStartUpdate(wxCommandEvent &event)
{
	window3D->StartUpdate(1);
}

void CIrrFrame::OnTakeOver(wxCommandEvent &event)
{
	App::getInstance()->run();
}

void CIrrFrame::OnStopUpdate(wxCommandEvent &event)
{
	window3D->StopUpdate();
}

int CIrrFrame::MessageBox(core::stringw message, core::stringw message2, int buttons)
{
	if (buttons==3)
	{
		wxMessageDialog dialog(this,
							message.c_str(),
                           App::getInstance()->getLangText("msg_dlg_warn").c_str(),
                           wxCENTER | 
						   wxYES_NO | wxYES_DEFAULT | wxCANCEL |
						   wxICON_WARNING);
		//wxString extmsg;
		/*if ( dialog.SetYesNoCancelLabels
			(
			App::getInstance()->getLangText("msg_dlg_yes").c_str(),
			App::getInstance()->getLangText("msg_dlg_no").c_str(),
			App::getInstance()->getLangText("msg_dlg_cancel").c_str()) )
		{
			extmsg = message2.c_str();
		}
		else
		{
			extmsg = message2.c_str();
		}
		dialog.SetExtendedMessage(extmsg);*/

		switch ( dialog.ShowModal() )
		{
			case wxID_YES:
				return 1;
				break;

			case wxID_NO:
				return 0;
				break;

			case wxID_CANCEL:
				return 2;
				break;

			default:
				wxLogError(wxT("Unexpected wxMessageDialog return code!"));
				return -1;
		}
	}

	if (buttons==2)
	{

		wxMessageDialog dialog(this,
							message.c_str(),
                           App::getInstance()->getLangText("msg_dlg_warn").c_str(),
                           wxCENTER | 
						   wxOK | wxCANCEL |
						   wxICON_WARNING);
		wxString extmsg;
		/*if ( dialog.SetOKCancelLabels
			(
			App::getInstance()->getLangText("msg_dlg_yes").c_str(),
			App::getInstance()->getLangText("msg_dlg_no").c_str()) )
		{
			extmsg = message2.c_str();
		}
		else
		{
			extmsg = message2.c_str();
		}*/

		//dialog.SetExtendedMessage(extmsg);
        
		switch ( dialog.ShowModal() )
		{
			case wxID_OK:
				return 1;
				break;

			case wxID_CANCEL:
				return 0;
				break;

			default:
				wxLogError(wxT("Unexpected wxMessageDialog return code!"));
				return -1;
		}
	}
    
	return -1;
}

core::stringw CIrrFrame::FileOpen(core::stringw message)
{
	static wxString s_extDef;
	wxString path = wxFileSelector(
									message.c_str(),
                                    wxT("../projects"), wxEmptyString,
                                    s_extDef,
                                    wxString::Format
                                    (
                                        wxT("Irr Rpg Builder (*.xml)|*.xml|All files (%s)|%s"),
                                        wxFileSelectorDefaultWildcardStr,
                                        wxFileSelectorDefaultWildcardStr
                                    ),
                                    wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                                    this
                                   );

    if ( !path )
        return L"";

    // it is just a sample, would use wxSplitPath in real program
    s_extDef = path.AfterLast(wxT('.'));
	//wxLogStatus(wxT("You selected the file '%s', remembered extension '%s'"),
    //             path, s_extDef);

	return (core::stringw)path.c_str();
		//.c_str().AsString().ToStdWstring().c_str();

	//return (core::stringw)L"";
}

core::stringw CIrrFrame::FileSave()
{
	//wxString filetext = (wxString)App::getInstance()->getProjectName().c_str();
    wxFileDialog dialog(this,
                        wxT("Testing save file dialog"),
                        wxT("../projects"),
						wxT("file.xml"),//App::getInstance()->getProjectName().c_str(),
                        //wxT("irb_temp_project.XML"),
                        wxT("Project files (*.XML)|*.XML"),
                        wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    dialog.SetFilterIndex(1);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxLogStatus(wxT("Saving %s ..."),dialog.GetPath().c_str());
		return dialog.GetPath().c_str();
			//.AsString().ToStdWstring().c_str();
    }
	return L"";
}

// Log console dialog (Might be removed for a Irrlicht variant, using a separate window)
ConsoleDialog::ConsoleDialog(wxWindow *parent)
                : wxDialog(parent, wxID_ANY, wxString(wxT("Console")),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
	wxBoxSizer *sizerH = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	m_textMsg = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(600,200), wxLC_REPORT);
	
   	sizerH->Add(m_textMsg,5,wxEXPAND,10);
	sizer->Add(sizerH,5,wxEXPAND);
	sizer->SetSizeHints(this);
	SetSizer(sizer);

	m_textMsg->InsertColumn(0, wxT("Item"), wxLIST_FORMAT_LEFT, 1600); 
}

void ConsoleDialog::AddMessage(core::stringw text, video::SColor color)
{
	wxListItem NewItem;
	
	NewItem.SetText(text.c_str());
	NewItem.SetFont(wxFont(10,wxFONTFAMILY_MODERN,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false));
	NewItem.SetTextColour(wxColour(color.getRed(),color.getGreen(),color.getBlue()));
	if (m_textMsg && m_textMsg->GetColumnCount()>0)
	{
		if (m_textMsg->GetItemCount()>1999)
			m_textMsg->DeleteAllItems();

		m_textMsg->InsertItem(NewItem);

	// Temporary just to have too much memory used. After 2000 lines, it clear itself
	// Will need to remove the first item entered in the list and remove the first item
	}	
}


void ConsoleDialog::OnClose(wxCloseEvent& event)
{
    if ( event.CanVeto() )
    {
        //wxMessageBox(wxT("Use the menu item to close this dialog"),
        //             wxT("Modeless dialog"),
        //             wxOK | wxICON_INFORMATION, this);
		this->Hide();

        event.Veto();
    }
}

int winmain(int argc, char** argv)
{
    ///TODO: Create a Window to choose resolution

	CIrrApp::CIrrApp();
    return 0;
}