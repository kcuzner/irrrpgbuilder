#include "EditorMain.h"
#include "../app.h"
#include "wxIWindow.h"
#include "wx/wx.h"


IMPLEMENT_APP(CIrrApp)

bool CIrrApp::OnInit()
{
	
	CIrrFrame *frame = new CIrrFrame( _T("IRR RPG Builder (SVN Release 0.2 Alpha) - May 2011"), wxPoint(0,0), wxSize(1024,768) );
	frame->Show(TRUE);
	
	frame->Centre();
	SetTopWindow(frame);
	if (App::getInstance()->getDevice()->run()==true)
	{
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
	EVT_RIBBONTOOLBAR_CLICKED(ID_Quit, CIrrFrame::OnQuit)
	EVT_RIBBONTOOLBAR_CLICKED(ID_New, CIrrFrame::OnNew)
	EVT_RIBBONTOOLBAR_CLICKED(ID_Save, CIrrFrame::OnSave)
	EVT_RIBBONTOOLBAR_CLICKED(ID_Load, CIrrFrame::OnLoad)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_Play, CIrrFrame::OnPlay)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_Stop, CIrrFrame::OnStop)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_ObjEditMode, CIrrFrame::OnObjectEditMode)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_PlayerEdit, CIrrFrame::OnPlayerEdit)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_PlayerScript, CIrrFrame::OnScriptPlayer)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_GlobalScript, CIrrFrame::OnScriptGlobal)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_TerrainSegment, CIrrFrame::OnTerrainSegment)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_TerrainTransform, CIrrFrame::OnTerrainTransform)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_TerrainTree, CIrrFrame::OnTerrainTree)
	EVT_RIBBONBUTTONBAR_CLICKED(403, CIrrFrame::OnDisplayConsole)
	
END_EVENT_TABLE()


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


CIrrFrame::CIrrFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame((wxFrame *)NULL, -1, title, pos, size, style)
{
	m_ribbon = new wxRibbonBar(this, wxID_ANY);
	//this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	SetIcon(wxICON(sample));
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	window3D = new wxCIWindow( this, wxID_ANY, style, irr::video::EDT_OPENGL, true );
	App::getInstance()->setupDevice(window3D->getDevice());
	window3D->AcceptsFocus();
	

	bSizer1->Add( m_ribbon, 0, wxEXPAND);
	bSizer1->Add( window3D, 1, wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();

	
	
	wxMenu *menuFile = new wxMenu;

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
	// Main toolbar
	wxRibbonPage* home = new wxRibbonPage(m_ribbon, wxID_ANY, App::getInstance()->getLangText("tab_project").c_str(), empty_xpm);
	if (home)
	{	
		wxRibbonPanel *toolbar_panel = new wxRibbonPanel(home, wxID_ANY,  App::getInstance()->getLangText("txt_tool_des0").c_str(), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
		wxRibbonPanel *test_panel = new wxRibbonPanel(home, wxID_ANY, App::getInstance()->getLangText("txt_tool_des4").c_str(), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
		/*wxRibbonButtonBar *main_button = new wxRibbonButtonBar(toolbar_panel, wxID_ANY);
		main_button->AddButton(wxID_ANY, wxT("Quit"), msw_style_xpm);
		main_button->Realize();*/
		
		wxRibbonToolBar *toolbar = new wxRibbonToolBar(toolbar_panel, ID_MAIN_TOOLBAR);
		toolbar->AddTool(ID_Quit, bt_close_program_xpm);
		toolbar->AddSeparator();
        toolbar->AddHybridTool(ID_New, bt_new_project_xpm);
        toolbar->AddTool(ID_Load, bt_load_project_xpm);
        toolbar->AddTool(ID_Save, bt_save_project_xpm);
        toolbar->AddTool(wxID_ANY, wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_OTHER));
        toolbar->AddSeparator();
        toolbar->AddDropdownTool(wxID_UNDO, wxArtProvider::GetBitmap(wxART_UNDO, wxART_OTHER));
        toolbar->AddDropdownTool(wxID_REDO, wxArtProvider::GetBitmap(wxART_REDO, wxART_OTHER));
        toolbar->AddSeparator();
        
        toolbar->SetRows(1, 2);
		wxRibbonButtonBar *test_button = new wxRibbonButtonBar(test_panel, wxID_ANY);
		test_button->AddButton(ID_Play, App::getInstance()->getLangText("txt_tool_edpl").c_str(), bt_play_game_xpm);
		test_button->AddButton(ID_Stop, App::getInstance()->getLangText("txt_tool_edit").c_str(), bt_stop_game_xpm);
		test_button->Realize();

	}
	// Terrain Toolbar
	wxRibbonPage* terrain = new wxRibbonPage(m_ribbon, wxID_ANY, App::getInstance()->getLangText("tab_environment").c_str(), empty_xpm);
	if (terrain)
	{
		wxRibbonPanel *terrain_panel = new wxRibbonPanel(terrain, wxID_ANY, App::getInstance()->getLangText("txt_tool_des5").c_str(), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
		wxRibbonPanel *terrain_panelv = new wxRibbonPanel(terrain, wxID_ANY,App::getInstance()->getLangText("txt_tool_des6").c_str(), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

		wxRibbonButtonBar *terrain_button = new wxRibbonButtonBar(terrain_panel, wxID_ANY);
		terrain_button->AddButton(ID_TerrainSegment, App::getInstance()->getLangText("bt_terrain_segments").c_str(), bt_terrain_add_segment_xpm);
		terrain_button->AddButton(ID_TerrainTransform, App::getInstance()->getLangText("bt_terrain_transform").c_str(), bt_terrain_up_xpm);
		terrain_button->Center();
		//terrain_button->SetInitialBestSize(wxSize(400,120));
		terrain_button->Realize();

		// Buttons
		wxRibbonButtonBar *terrain_buttonv = new wxRibbonButtonBar(terrain_panelv, wxID_ANY);
		terrain_buttonv->AddButton(ID_TerrainTree, App::getInstance()->getLangText("txt_tool_tree").c_str(), bt_terrain_paint_vegetation_xpm);
		terrain_buttonv->AddButton(wxID_ANY, App::getInstance()->getLangText("txt_tool_bush").c_str(), bt_terrain_paint_vegetation_xpm);
		terrain_buttonv->AddButton(wxID_ANY, App::getInstance()->getLangText("txt_tool_rock").c_str(), bt_terrain_paint_vegetation_xpm);
		terrain_buttonv->Realize();
	}
	//Dynamic Object toolbar
	wxRibbonPage* dynobject = new wxRibbonPage(m_ribbon, wxID_ANY, App::getInstance()->getLangText("tab_objects").c_str(), empty_xpm);
	if (dynobject)
	{
		wxRibbonPanel *dynobject_panel = new wxRibbonPanel(dynobject, wxID_ANY, App::getInstance()->getLangText("txt_tool_des1").c_str(), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
		wxRibbonButtonBar *dynobject_button = new wxRibbonButtonBar(dynobject_panel, wxID_ANY,wxDefaultPosition,wxDefaultSize);
		dynobject_button->AddButton(ID_ObjEditMode, App::getInstance()->getLangText("bt_dynamic_objects_mode").c_str(), bt_dynamic_objects_mode_xpm);
		dynobject_button->AddButton(ID_PlayerEdit,App::getInstance()->getLangText("bt_edit_character").c_str(), bt_edit_character_xpm, L"Aide é");
		dynobject_button->SetToolTip(App::getInstance()->getLangText("tab_objects").c_str());
		dynobject_button->Realize();
		
		
		wxRibbonPanel *script_panel = new wxRibbonPanel(dynobject, wxID_ANY, App::getInstance()->getLangText("txt_tool_des2").c_str(), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
		wxRibbonButtonBar *script_button = new wxRibbonButtonBar(script_panel, wxID_ANY,wxDefaultPosition,wxDefaultSize,1);
		script_button->AddButton(ID_PlayerScript, App::getInstance()->getLangText("bt_player_edit_script").c_str(), bt_player_edit_script_xpm);
		script_button->AddButton(ID_GlobalScript, App::getInstance()->getLangText("bt_edit_script_global").c_str(), bt_edit_script_global_xpm);
		
		script_panel->SetToolTip(App::getInstance()->getLangText("tab_objects").c_str());
		script_button->Realize();
	}
	//Options toolbar
	wxRibbonPage* option = new wxRibbonPage(m_ribbon, wxID_ANY, App::getInstance()->getLangText("tab_setup").c_str(), empty_xpm);
	if (option)
	{
		wxRibbonPanel* option_panel = new wxRibbonPanel(option, wxID_ANY, App::getInstance()->getLangText("txt_tool_des3").c_str(), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
		wxRibbonButtonBar* option_button = new wxRibbonButtonBar(option_panel, wxID_ANY,wxDefaultPosition,wxDefaultSize,1);
		option_button->AddButton(400, App::getInstance()->getLangText("bt_config").c_str(), bt_config_xpm, "Editor setup");
		
		option_button->AddButton(402, App::getInstance()->getLangText("txt_tool_setg").c_str(), bt_config_xpm );
		option_button->EnableButton(402,false);
		option_button->Realize();

		option_button->AddButton(403, App::getInstance()->getLangText("txt_tool_gc").c_str(), bt_config_xpm );
		option_button->EnableButton(403,true);
		option_button->Realize();
	}
	
//	SetMenuBar( menuBar );

	CreateStatusBar();
	SetStatusText(App::getInstance()->getLangText("msg_welcome").c_str());
	m_ribbon->SetActivePage(dynobject);
	Connect (-1, wxEVT_CLOSE_WINDOW,
             wxCloseEventHandler (CIrrFrame::OnClose),
             NULL, this);

	//App::getInstance()->run();
	//window3D->StartUpdate(0);

}

CIrrFrame::~CIrrFrame()
{
}

void CIrrFrame::OnQuit(wxRibbonToolBarEvent& WXUNUSED(evt))
{
	
	Close(true);
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

void CIrrFrame::OnNew(wxRibbonToolBarEvent& WXUNUSED(evt))
{
	if (MessageBox(App::getInstance()->getLangText("msg_prj_np0").c_str(),App::getInstance()->getLangText("msg_prj_np1").c_str(),2)==1)
	{
		App::getInstance()->createNewProject();
		wxLogStatus(App::getInstance()->getLangText("msg_prj_np2").c_str());
	}

}

void CIrrFrame::OnSave(wxRibbonToolBarEvent& WXUNUSED(evt))
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

void CIrrFrame::OnLoad(wxRibbonToolBarEvent& WXUNUSED(evt))
{
	APP_STATE old_state = App::getInstance()->getAppState();
	App::getInstance()->setAppState(APP_EDIT_WAIT_GUI);

	int boxres = MessageBox(App::getInstance()->getLangText("msg_prj_lp0").c_str(),App::getInstance()->getLangText("msg_prj_lp1").c_str(),3);
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

void CIrrFrame::OnPlay(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	App::getInstance()->playGame();
}

void CIrrFrame::OnStop(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	App::getInstance()->stopGame();
}

void CIrrFrame::OnObjectEditMode(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	App::getInstance()->setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
}

void CIrrFrame::OnPlayerEdit(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	App::getInstance()->setAppState(APP_EDIT_CHARACTER);
}

void CIrrFrame::OnScriptGlobal(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	App::getInstance()->setAppState(APP_EDIT_SCRIPT_GLOBAL);
}

void CIrrFrame::OnScriptPlayer(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	App::getInstance()->setAppState(APP_EDIT_PLAYER_SCRIPT);
}

void CIrrFrame::OnTerrainSegment(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	App::getInstance()->setAppState(APP_EDIT_TERRAIN_SEGMENTS);
}

void CIrrFrame::OnTerrainTransform(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	App::getInstance()->setAppState(APP_EDIT_TERRAIN_TRANSFORM);
}

void CIrrFrame::OnTerrainTree(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	App::getInstance()->setAppState(APP_EDIT_TERRAIN_PAINT_VEGETATION);
}

void CIrrFrame::OnDisplayConsole(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	App::getInstance()->displayGuiConsole();
}

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
		wxString extmsg;
		if ( dialog.SetYesNoCancelLabels
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
		dialog.SetExtendedMessage(extmsg);

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
		if ( dialog.SetOKCancelLabels
			(
			App::getInstance()->getLangText("msg_dlg_yes").c_str(),
			App::getInstance()->getLangText("msg_dlg_no").c_str()) )
		{
			extmsg = message2.c_str();
		}
		else
		{
			extmsg = message2.c_str();
		}
		dialog.SetExtendedMessage(extmsg);

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

    return (core::stringw)path.c_str().AsString().ToStdWstring().c_str();
}

core::stringw CIrrFrame::FileSave()
{
    wxFileDialog dialog(this,
                        wxT("Testing save file dialog"),
                        wxT("../projects"),
						App::getInstance()->getProjectName().c_str(),
                        //wxT("irb_temp_project.XML"),
                        wxT("Project files (*.XML)|*.XML"),
                        wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    dialog.SetFilterIndex(1);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxLogStatus(wxT("Saving %s ..."),dialog.GetPath().c_str());
		return dialog.GetPath().c_str().AsString().ToStdWstring().c_str();
    }
	return L"";
}

int winmain(int argc, char** argv)
{
    ///TODO: Create a Window to choose resolution

	CIrrApp::CIrrApp();
    return 0;
}