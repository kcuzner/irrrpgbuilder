#include "EditorMain.h"
#include "../app.h"
#include "wxIWindow.h"
#include "wx/wx.h"
#include "wx/aboutdlg.h"


IMPLEMENT_APP(CIrrApp)

bool CIrrApp::OnInit()
{
	
	CIrrFrame *frame = new CIrrFrame( _T("IRR RPG Builder (SVN Release 0.2 Alpha) - January 2012"), wxPoint(0,0), wxSize(10,10) );
	frame->Show(TRUE);
	
	//App::getInstance()->loadConfig();

	// create a NULL device to detect screen resolution
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
/*
	EVT_RIBBONTOOLBAR_CLICKED(ID_Quit, CIrrFrame::OnQuit)
	EVT_RIBBONTOOLBAR_CLICKED(ID_New, CIrrFrame::OnNew)
	EVT_RIBBONTOOLBAR_CLICKED(ID_Save, CIrrFrame::OnSave)
	EVT_RIBBONTOOLBAR_CLICKED(ID_Load, CIrrFrame::OnLoad)

	EVT_RIBBONBUTTONBAR_CLICKED(ID_Quit, CIrrFrame::OnQuit)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_New, CIrrFrame::OnNew)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_Save, CIrrFrame::OnSave)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_Load, CIrrFrame::OnLoad)

	EVT_RIBBONBUTTONBAR_CLICKED(ID_Play, CIrrFrame::OnPlay)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_Stop, CIrrFrame::OnStop)

	EVT_RIBBONBUTTONBAR_CLICKED(ID_About, CIrrFrame::OnAbout)

	EVT_RIBBONBUTTONBAR_CLICKED(ID_ObjEditMode, CIrrFrame::OnObjectEditMode)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_PlayerEdit, CIrrFrame::OnPlayerEdit)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_PlayerScript, CIrrFrame::OnScriptPlayer)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_GlobalScript, CIrrFrame::OnScriptGlobal)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_TerrainSegment, CIrrFrame::OnTerrainSegment)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_TerrainTransform, CIrrFrame::OnTerrainTransform)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_TerrainTree, CIrrFrame::OnTerrainTree)
	//EVT_RIBBONBUTTONBAR_CLICKED(ID_Console, CIrrFrame::OnDisplayConsole)
	EVT_RIBBONBUTTONBAR_CLICKED(ID_Console, CIrrFrame::OnLog) */

	EVT_TIMER(ID_UpdateTimer, OnProgressTimer)
	
END_EVENT_TABLE()

	BEGIN_EVENT_TABLE(ConsoleDialog, wxDialog)
        EVT_CLOSE(ConsoleDialog::OnClose)
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
#include "icons/bt_about.xpm"
#include "icons/bt_help.xpm"
#include "icons/logo1.xpm"


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
	/* bSizer2->Add( m_ribbon1, 1, wxLEFT, 5);
	bSizer2->Add( m_ribbon, 8, wxEXPAND, 5);
	bSizer2->Add( m_ribbon2, 1, wxRIGHT, 5);
	

	bSizer1->Add( bSizer2, 0, wxEXPAND); */
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
	// "Playtest" toolbar
/*	wxRibbonPage* home2 = new wxRibbonPage(m_ribbon2, wxID_ANY, App::getInstance()->getLangText("txt_tool_des4").c_str(), empty_xpm);
	if (home2)
	{
		wxRibbonPanel *test_panel = new wxRibbonPanel(home2, wxID_ANY, App::getInstance()->getLangText("txt_tool_des4").c_str(), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE | wxRIBBON_PANEL_MINIMISE_BUTTON);
			
		test_button = new wxRibbonButtonBar(test_panel, wxID_ANY);
		test_button->AddToggleButton(ID_Play, App::getInstance()->getLangText("txt_tool_edpl").c_str(), bt_play_game_xpm);
		test_button->AddToggleButton(ID_Stop, App::getInstance()->getLangText("txt_tool_edit").c_str(), bt_stop_game_xpm);
		test_button->ToggleButton(ID_Stop,true);
		test_button->AddButton(ID_Console, App::getInstance()->getLangText("txt_tool_gc").c_str(), bt_config_xpm );
		test_button->EnableButton(ID_Console,true);
	
		test_button->Realize();
	}
    // Main toolbar
	wxRibbonPage* home1 = new wxRibbonPage(m_ribbon1, wxID_ANY, App::getInstance()->getLangText("tab_project").c_str(), empty_xpm);
	if (home1)
	{	
		wxRibbonPanel *toolbar_panel = new wxRibbonPanel(home1, wxID_ANY,  App::getInstance()->getLangText("txt_tool_des0").c_str(), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
			
		// Used a button bar instead of a toolbar.
		//wxRibbonToolBar *toolbar = new wxRibbonToolBar(toolbar_panel, ID_MAIN_TOOLBAR);
		
		//toolbar->AddTool(ID_Quit, bt_close_program_xpm);
		//toolbar->AddSeparator();
        //toolbar->AddHybridTool(ID_New, bt_new_project_xpm);
        //toolbar->AddTool(ID_Load, bt_load_project_xpm);
        //toolbar->AddTool(ID_Save, bt_save_project_xpm);

		// Will show this when it's implemented (if it's ever implemented (undo/redo/save as)
		//toolbar->AddTool(wxID_ANY, wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_OTHER));
		//toolbar->AddSeparator();
		//toolbar->AddDropdownTool(wxID_UNDO, wxArtProvider::GetBitmap(wxART_UNDO, wxART_OTHER));
		//toolbar->AddDropdownTool(wxID_REDO, wxArtProvider::GetBitmap(wxART_REDO, wxART_OTHER));
		//toolbar->AddSeparator();

		//toolbar->SetRows(1, 2);

		project = new wxRibbonButtonBar(toolbar_panel, wxID_ANY);
		project->AddButton(ID_Quit,App::getInstance()->getLangText("bt_close_program").c_str(),bt_close_program_xpm);
		project->AddButton(ID_New,App::getInstance()->getLangText("bt_new_project").c_str(),bt_new_project_xpm);
		project->AddButton(ID_Load,App::getInstance()->getLangText("bt_load_project").c_str(),bt_load_project_xpm);
		project->AddButton(ID_Save,App::getInstance()->getLangText("bt_save_project").c_str(),bt_save_project_xpm);
		
		project->Realize();
		
	
	}
	// Terrain Toolbar
	wxRibbonPage* terrain = new wxRibbonPage(m_ribbon, wxID_ANY, App::getInstance()->getLangText("tab_environment").c_str(), empty_xpm);
	if (terrain)
	{
		wxRibbonPanel *terrain_panel = new wxRibbonPanel(terrain, wxID_ANY, App::getInstance()->getLangText("txt_tool_des5").c_str(), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
		wxRibbonPanel *terrain_panelv = new wxRibbonPanel(terrain, wxID_ANY,App::getInstance()->getLangText("txt_tool_des6").c_str(), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

		terrain_button = new wxRibbonButtonBar(terrain_panel, wxID_ANY);
		terrain_button->AddToggleButton(ID_TerrainSegment, App::getInstance()->getLangText("bt_terrain_segments").c_str(), bt_terrain_add_segment_xpm);
		terrain_button->AddToggleButton(ID_TerrainTransform, App::getInstance()->getLangText("bt_terrain_transform").c_str(), bt_terrain_up_xpm);
		terrain_button->Center();
		//terrain_button->SetInitialBestSize(wxSize(400,120));
		terrain_button->Realize();

		// Buttons
		terrain_buttonv = new wxRibbonButtonBar(terrain_panelv, wxID_ANY);
		terrain_buttonv->AddToggleButton(ID_TerrainTree, App::getInstance()->getLangText("txt_tool_tree").c_str(), bt_terrain_paint_vegetation_xpm);
		terrain_buttonv->AddButton(wxID_ANY, App::getInstance()->getLangText("txt_tool_bush").c_str(), bt_terrain_paint_vegetation_xpm);
		terrain_buttonv->AddButton(wxID_ANY, App::getInstance()->getLangText("txt_tool_rock").c_str(), bt_terrain_paint_vegetation_xpm);
		terrain_buttonv->Realize();
	}
	//Dynamic Object toolbar
	wxRibbonPage* dynobject = new wxRibbonPage(m_ribbon, wxID_ANY, App::getInstance()->getLangText("tab_objects").c_str(), empty_xpm);
	if (dynobject)
	{
		wxRibbonPanel *dynobject_panel = new wxRibbonPanel(dynobject, wxID_ANY, App::getInstance()->getLangText("txt_tool_des1").c_str(), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
		dynobject_button = new wxRibbonButtonBar(dynobject_panel, wxID_ANY,wxDefaultPosition,wxDefaultSize);
		dynobject_button->AddToggleButton(ID_ObjEditMode, App::getInstance()->getLangText("bt_dynamic_objects_mode").c_str(), bt_dynamic_objects_mode_xpm);
		dynobject_button->AddToggleButton(ID_PlayerEdit,App::getInstance()->getLangText("bt_edit_character").c_str(), bt_edit_character_xpm, L"Aide é");
		dynobject_button->SetToolTip(App::getInstance()->getLangText("tab_objects").c_str());
		dynobject_button->ToggleButton(ID_ObjEditMode,true);
		dynobject_button->Realize();
		
		
		wxRibbonPanel *script_panel = new wxRibbonPanel(dynobject, wxID_ANY, App::getInstance()->getLangText("txt_tool_des2").c_str(), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
		script_button = new wxRibbonButtonBar(script_panel, wxID_ANY,wxDefaultPosition,wxDefaultSize,1);
		script_button->AddToggleButton(ID_PlayerScript, App::getInstance()->getLangText("bt_player_edit_script").c_str(), bt_player_edit_script_xpm);
		script_button->AddToggleButton(ID_GlobalScript, App::getInstance()->getLangText("bt_edit_script_global").c_str(), bt_edit_script_global_xpm);
		
		script_panel->SetToolTip(App::getInstance()->getLangText("tab_objects").c_str());
		script_button->Realize();
	}
	// Tools toolbar
	wxRibbonPage* tools = new wxRibbonPage(m_ribbon, wxID_ANY, App::getInstance()->getLangText("tab_tools").c_str(), empty_xpm);
	if (tools)
	{
		wxRibbonPanel* tools_panel = new wxRibbonPanel(tools, wxID_ANY, App::getInstance()->getLangText("tab_tools").c_str(), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
		tools_button = new wxRibbonButtonBar(tools_panel, wxID_ANY,wxDefaultPosition,wxDefaultSize,1);
		//tools_button->AddToggleButton(ID_Console, App::getInstance()->getLangText("txt_tool_gc").c_str(), bt_config_xpm );
		//tools_button->EnableButton(ID_Console,true);
		tools_button->AddButton(ID_About, App::getInstance()->getLangText("bt_about").c_str(), bt_about_xpm );
		tools_button->EnableButton(ID_About,true);
		tools_button->Realize();
	}
	//Options toolbar
	wxRibbonPage* option = new wxRibbonPage(m_ribbon, wxID_ANY, App::getInstance()->getLangText("tab_setup").c_str(), empty_xpm);
	if (option)
	{
		wxRibbonPanel* option_panel = new wxRibbonPanel(option, wxID_ANY, App::getInstance()->getLangText("txt_tool_des3").c_str(), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
		option_button = new wxRibbonButtonBar(option_panel, wxID_ANY,wxDefaultPosition,wxDefaultSize,1);
		option_button->AddButton(ID_EditConfig, App::getInstance()->getLangText("bt_config").c_str(), bt_config_xpm, "Editor setup");
		
		option_button->AddButton(ID_EditGameConfig, App::getInstance()->getLangText("txt_tool_setg").c_str(), bt_config_xpm );
		option_button->EnableButton(ID_EditGameConfig,false);
		option_button->Realize();
		
	}
*/
	
	
	
//	SetMenuBar( menuBar );

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


/*void CIrrFrame::OnQuit(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	
	Close(true);
}*/

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

//void CIrrFrame::OnNew(wxRibbonButtonBarEvent& WXUNUSED(evt))
void CIrrFrame::OnNew()
{
	if (MessageBox(App::getInstance()->getLangText("msg_prj_np0").c_str(),App::getInstance()->getLangText("msg_prj_np1").c_str(),2)==1)
	{
		App::getInstance()->createNewProject();
		wxLogStatus(App::getInstance()->getLangText("msg_prj_np2").c_str());
	}

}

//void CIrrFrame::OnSave(wxRibbonButtonBarEvent& WXUNUSED(evt))
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

//void CIrrFrame::OnLoad(wxRibbonButtonBarEvent& WXUNUSED(evt))
void CIrrFrame::OnLoad()
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

void CIrrFrame::MessageStatus(core::stringw message)
{
	wxLogStatus(message.c_str());
};

/*
void CIrrFrame::OnPlay(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	UncheckAllButtons();
	m_ribbon->Disable();
	test_button->ToggleButton(ID_Play,true);
	test_button->ToggleButton(ID_Stop,false);
	this->Layout();
	App::getInstance()->playGame();
}

void CIrrFrame::OnStop(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	UncheckAllButtons();
	//this->m_ribbon->Show();
	m_ribbon->Enable();
	test_button->ToggleButton(ID_Play,false);
	test_button->ToggleButton(ID_Stop,true);
	this->Layout();
	App::getInstance()->stopGame();
}

void CIrrFrame::OnAbout(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	wxAboutDialogInfo info;
	info.SetName(wxT("IrrRPG Builder "));
	info.SetIcon(wxIcon(logo1_xpm));
	info.SetDescription(wxString::Format
        (
			"Libraries:\nwxWidget %s version %s\nIrrlicht version %s",
			wxMINOR_VERSION % 2 ? "Development" : "Stable",
			wxVERSION_NUM_DOT_STRING,
			App::getInstance()->getDevice()->getVersion()
		));
	info.SetVersion(wxT("0.2 Alpha"),wxT("0.2 Alpha"));
	std::vector<core::stringw> abouttxt = App::getInstance()->getAbout();
	stringw text = L"\n";
	for (int a=0; a<(int)abouttxt.size(); a++)
	{
		text+=abouttxt[a].c_str();
		text+=L"\n";
		//info.AddDeveloper(text.c_str());
	}
	
	//info.SetDescription(text.c_str());
	info.SetDevelopers(wxArrayString(1,text.c_str()));
    info.SetCopyright(wxT("(C) 2011  IrrRPG BUILDER dev team"));
	info.SetWebSite(wxT("http://irrrpgbuilder.sourceforge.net/"),wxT("IRR RPG BUILDER official web site"));
	info.SetLicence(wxString::FromAscii(
	"Copyright (c) 2011\n"
	"Permission is hereby granted,\n"
	"free of charge, to any person\n"
	"obtaining a copy of this software\n"
	"and associated documentation files\n"
	"(the 'Software'), to deal in the\n"
	"Software without restriction,\n"
	"including without limitation\n"
	"the rights to use, copy, modify,\n"
	"merge, publish, distribute,\n"
	"sublicense, and/or sell copies\n"
	"of the Software, and to permit\n"
	"persons to whom the Software is\n"
	"furnished to do so, subject to\n"
	"the following conditions:\n"
	"\n"
	"The above copyright notice\n"
	"and this permission notice\n"
	"shall be included in all copies\n"
	"or substantial portions of the\n"
	"Software.\n"
	"\n"
	"THE SOFTWARE IS PROVIDED 'AS IS',\n"
	"WITHOUT WARRANTY OF ANY KIND,\n"
	"EXPRESS OR IMPLIED, INCLUDING BUT\n"
	"NOT LIMITED TO THE WARRANTIES OF\n"
	"MERCHANTABILITY, FITNESS FOR A\n"
	"PARTICULAR PURPOSE AND\n"
	"NONINFRINGEMENT. IN NO EVENT SHALL\n"
	"THE AUTHORS OR COPYRIGHT HOLDERS\n"
	"BE LIABLE FOR ANY CLAIM, DAMAGES\n"
	"OR OTHER LIABILITY, WHETHER IN AN\n"
	"ACTION OF CONTRACT, TORT OR\n"
	"OTHERWISE, ARISING FROM, OUT OF OR\n"
	"IN CONNECTION WITH THE SOFTWARE OR\n"
	"THE USE OR OTHER DEALINGS IN THE\n"
	"SOFTWARE.\n\n"
    ));
    
	wxAboutBox(info, this);
}

void CIrrFrame::OnLog(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	if (!console_dialog)
		console_dialog = new ConsoleDialog(this);
	if (console_dialog)
	{
		if (console_dialog->IsVisible())
			console_dialog->Hide();
		else
			console_dialog->Show();
	}
}

void CIrrFrame::OnObjectEditMode(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	UncheckAllButtons();
	dynobject_button->ToggleButton(ID_ObjEditMode,true);
	App::getInstance()->setAppState(APP_EDIT_DYNAMIC_OBJECTS_MODE);
}

void CIrrFrame::OnPlayerEdit(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	UncheckAllButtons();
	dynobject_button->ToggleButton(ID_PlayerEdit,true);
	App::getInstance()->setAppState(APP_EDIT_CHARACTER);
}

void CIrrFrame::OnScriptGlobal(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	UncheckAllButtons();
	script_button->ToggleButton(ID_GlobalScript,true);
	App::getInstance()->setAppState(APP_EDIT_SCRIPT_GLOBAL);
}

void CIrrFrame::OnScriptPlayer(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	UncheckAllButtons();
	script_button->ToggleButton(ID_PlayerScript,true);
	App::getInstance()->setAppState(APP_EDIT_PLAYER_SCRIPT);
}

void CIrrFrame::OnTerrainSegment(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	UncheckAllButtons();
	terrain_button->ToggleButton(ID_TerrainSegment,true);
	App::getInstance()->setAppState(APP_EDIT_TERRAIN_SEGMENTS);
}

void CIrrFrame::OnTerrainTransform(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	UncheckAllButtons();
	terrain_button->ToggleButton(ID_TerrainTransform,true);
	App::getInstance()->setAppState(APP_EDIT_TERRAIN_TRANSFORM);
}

void CIrrFrame::OnTerrainTree(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	UncheckAllButtons();
	terrain_buttonv->ToggleButton(ID_TerrainTree,true);
	App::getInstance()->setAppState(APP_EDIT_TERRAIN_PAINT_VEGETATION);
}

void CIrrFrame::OnDisplayConsole(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	//UncheckAllButtons();
	//option_button->ToggleButton(ID_Console,true);
	App::getInstance()->displayGuiConsole();
}
*/


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

void CIrrFrame::UncheckAllButtons()
{
	/*terrain_button->ToggleButton(ID_TerrainSegment,false);
	terrain_button->ToggleButton(ID_TerrainTransform,false);
	terrain_buttonv->ToggleButton(ID_TerrainTree,false);
	dynobject_button->ToggleButton(ID_ObjEditMode,false);
	dynobject_button->ToggleButton(ID_PlayerEdit,false);
	script_button->ToggleButton(ID_GlobalScript,false);
	script_button->ToggleButton(ID_PlayerScript,false);
	option_button->ToggleButton(ID_EditConfig,false);
	option_button->ToggleButton(ID_EditGameConfig,false);
	//option_button->ToggleButton(ID_Console,false);
   */

		/*ID_New,
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
		ID_TerrainTree,*/
}

// Log console dialog
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