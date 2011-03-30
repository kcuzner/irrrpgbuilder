#include "wxIViewport.h"
#include <wx/arrimpl.cpp>

HCURSOR cwe=::LoadCursor(NULL, IDC_SIZEWE);
HCURSOR cns=::LoadCursor(NULL, IDC_SIZENS);

wxCIViewportSettings::wxCIViewportSettings()
{
	SetDefaults();
}

wxCIViewportSettings::wxCIViewportSettings(const wxCIViewportSettings &src)
{
	b_active = src.b_active;
	b_selected = src.b_selected;
	color_background = src.color_background;
	color_selected = src.color_selected;
	color_border = src.color_border;
	m_border = src.m_border;
	font = src.font;
	color_text = src.color_text;
	color_selectedtext = src.color_selectedtext;
	m_namepos = src.m_namepos;
	sz_name = src.sz_name;
}

wxCIViewportSettings::~wxCIViewportSettings()
{
}

wxCIViewportSettings &wxCIViewportSettings::operator =(const wxCIViewportSettings &src)
{
	b_active = src.b_active;
	b_selected = src.b_selected;
	color_background = src.color_background;
	color_selected = src.color_selected;
	color_border = src.color_border;
	m_border = src.m_border;
	font = src.font;
	color_text = src.color_text;
	color_selectedtext = src.color_selectedtext;
	m_namepos = src.m_namepos;
	sz_name = src.sz_name;
	return *this;
}

void wxCIViewportSettings::SetDefaults(void)
{
	b_active = false;
	b_selected = false;
	color_background = GetColorBackground(true);
	color_selected = GetColorSelected(true);
	color_border = GetColorBorder(true);
	m_border = GetBorder(true);
	font = GetFont(true);
	color_text = GetColorText(true);
	color_selectedtext = GetColorSelectedText(true);
	m_namepos = GetNamePosition(true);
	sz_name = GetName(true);
}

///////////////////////////////////////////////////////////////////////////////

wxCIViewport::wxCIViewport() : m_pos(), m_size(), m_settings()
{
	n_split = WXIRR_DIRNONE;
	p_split1 = NULL;
	p_split2 = NULL;
	f_percentw = 1.0;
	f_percenth = 1.0;
	p_cam = NULL;
}

wxCIViewport::wxCIViewport(const wxCIViewport &src)
{
	n_split = src.n_split;
	p_split1 = src.p_split1;
	p_split2 = src.p_split2;
	m_pos = src.m_pos;
	m_size = src.m_size;
	f_percentw = src.f_percentw;
	f_percenth = src.f_percenth;
	m_settings = src.m_settings;
	p_cam = src.p_cam;
}

wxCIViewport::~wxCIViewport()
{
}

wxCIViewport &wxCIViewport::operator =(const wxCIViewport &src)
{
	n_split = src.n_split;
	p_split1 = src.p_split1;
	p_split2 = src.p_split2;
	m_pos = src.m_pos;
	m_size = src.m_size;
	f_percentw = src.f_percentw;
	f_percenth = src.f_percenth;
	m_settings = src.m_settings;
	p_cam = src.p_cam;
	return *this;
}

void wxCIViewport::Draw(wxIVideoDriver *driver)
{
	wxIRectInteger pos=GetArea(), textpos=pos;

	driver->draw2DRectangle( GetBorderColor(), pos );
	pos.UpperLeftCorner.X += ( m_settings.GetBorder().GetWidth()-1 );
	pos.UpperLeftCorner.Y += ( m_settings.GetBorder().GetHeight()-1 );
	pos.LowerRightCorner.X -= ( m_settings.GetBorder().GetWidth()-1 );
	pos.LowerRightCorner.Y -= ( m_settings.GetBorder().GetHeight()-1 );
	driver->draw2DRectangle( GetBackgroundlColor(), pos);
	if (m_settings.b_selected && (m_settings.m_border.GetWidth()<m_size.GetWidth()) && (m_settings.m_border.GetHeight()<m_size.GetHeight()))
		driver->draw2DRectangleOutline( pos, GetSelectedColor() );
	if (m_settings.font)
	{
		wxIColor color=(m_settings.b_selected) ? m_settings.color_selectedtext : m_settings.color_text;

		textpos.UpperLeftCorner.X += m_settings.m_namepos.x;
		textpos.UpperLeftCorner.Y += m_settings.m_namepos.y;
		//m_settings.font->draw(m_settings.sz_name.c_str(), pos, color);
	}
}

bool wxCIViewport::Contains(wxPoint pos)
{
	wxRect rect(m_pos, m_size);

	return rect.Contains(pos);
}

wxIrrOrientation wxCIViewport::BorderContains(wxPoint pos)
{
	wxRect r1(m_pos, wxSize(m_size.GetWidth(), m_settings.m_border.GetHeight()) );	// top border
	wxRect r2(m_pos, wxSize(m_settings.m_border.GetWidth(), m_size.GetHeight()) );	// left border

	if ( r1.Contains(pos) )
		return WXIRR_DIRNORTH;
	if ( r2.Contains(pos) )
		return WXIRR_DIRWEST;
		
	r1.y += m_size.GetHeight() - m_settings.m_border.GetHeight() + 1 ;				// bottom border
	r2.x += m_size.GetWidth() - m_settings.m_border.GetWidth() + 1;					// right border

	if ( r1.Contains(pos) )
		return WXIRR_DIRSOUTH;
	if ( r2.Contains(pos) )
		return WXIRR_DIREAST;

	return WXIRR_DIRNONE;
}

bool wxCIViewport::CanBeShown(void)
{
	return (GetWidth()>m_settings.m_border.GetWidth()*2) & (GetHeight()>m_settings.m_border.GetHeight()*2);
}

wxIRectInteger wxCIViewport::GetArea(bool bmargin)
{
	wxIRectInteger area;

	if (bmargin)
	{
		area.UpperLeftCorner.X = m_pos.x + m_settings.m_border.GetX();
		area.UpperLeftCorner.Y = m_pos.y + m_settings.m_border.GetY();
		area.LowerRightCorner.X = m_pos.x+m_size.GetWidth() - m_settings.m_border.GetX();
		area.LowerRightCorner.Y = m_pos.y+m_size.GetHeight() - m_settings.m_border.GetY();
	}
	else
	{
		area.UpperLeftCorner.X = m_pos.x;
		area.UpperLeftCorner.Y = m_pos.y;
		area.LowerRightCorner.X = m_pos.x + m_size.GetWidth();
		area.LowerRightCorner.Y = m_pos.y + m_size.GetHeight();
	}
	return area;
}

void wxCIViewport::Resize(wxSize size)
{
	if (n_split!=WXIRR_DIRNONE)
	{
		p_split1->m_size.SetWidth( size.GetWidth() * p_split1->f_percentw );
		p_split1->m_size.SetHeight( size.GetHeight() * p_split1->f_percenth );
		switch (n_split)
		{
			case WXIRR_DIRHORIZ:
				p_split2->m_size.SetWidth( p_split1->m_size.GetWidth() );
				p_split2->m_size.SetHeight( size.GetHeight() - p_split1->m_size.GetHeight() + 1 );
				break;
			case WXIRR_DIRVERT:
				p_split2->m_size.SetWidth( size.GetWidth() - p_split1->m_size.GetWidth() + 1  );
				p_split2->m_size.SetHeight( p_split1->m_size.GetHeight() );
				break;
		}
	}
	m_size = size;
}

void wxCIViewport::Layout(void)
{
	if (n_split)
	{
		p_split1->m_pos.x = m_pos.x;
		p_split1->m_pos.y = m_pos.y;
		p_split2->m_pos.x = (n_split==WXIRR_DIRHORIZ) ? m_pos.x : m_pos.x+p_split1->m_size.GetWidth() - 1;
		p_split2->m_pos.y = (n_split==WXIRR_DIRHORIZ) ? m_pos.y+p_split1->m_size.GetHeight() - 1 : m_pos.y;
	}
}

void wxCIViewport::RemoveChildsSafe(void)
{
	// remove childs only if they are not splited
	if (p_split1 && p_split1->n_split==WXIRR_DIRNONE)
	{
		delete p_split1;
		p_split1 = NULL;
	}
	if (p_split2 && p_split2->n_split==WXIRR_DIRNONE)
	{
		delete p_split2;
		p_split2 = NULL;
	}
	if ( (p_split1==NULL) && (p_split2==NULL) )
		n_split = WXIRR_DIRNONE;
}

///////////////////////////////////////////////////////////////////////////////

WX_DEFINE_OBJARRAY(wxIrrArrayOfViewportsPointers);

wxCIViewportManager::wxCIViewportManager() : m_vstack(), m_point(-1, -1)
{
	// create the root viewport. Can't be deleted
	m_root = new wxCIViewport();
	m_active = NULL;
	m_selected = NULL;
	m_grabed = NULL;
	SetActiveViewport(m_root);
	SelectViewport(m_root);
	b_refresh = false;
	b_allowroodel = false;
	grabdir = WXIRR_DIRNONE;
	font = NULL;
}

wxCIViewportManager::~wxCIViewportManager()
{
	m_vstack.Clear();
	b_allowroodel = true;
	RemoveViewport( m_root );
	if (cwe)
		::DestroyCursor(cwe);
	if (cns)
		::DestroyCursor(cns);
}

void wxCIViewportManager::OnSize(wxSizeEvent &event)
{
	Resize( event.GetSize() );
	Layout( );
	b_refresh = true;
}

void wxCIViewportManager::OnMouseMove(wxMouseEvent &event)
{
	b_refresh = false;
	if ( m_point != wxPoint(-1, -1) )
	{
		if (m_grabed)
		{
			float percent;
			int d;

			b_refresh = true;
			switch (m_grabed->n_split)
			{
				case WXIRR_DIRHORIZ:
					d = event.GetY() - m_point.y;
					if ( m_size.GetHeight() + d > 0 )
					{
						if (m_size.GetHeight() + d < m_grabed->m_size.GetHeight() )
						{
							percent = (float)(m_size.GetHeight()+d)/(float)(m_grabed->m_size.GetHeight());
							m_grabed->p_split1->f_percenth = percent;
						}
					}
					break;
				case WXIRR_DIRVERT:
					d = event.GetX() - m_point.x;
					if ( m_size.GetWidth() + d > 0 )
					{
						if (m_size.GetWidth() + d < m_grabed->m_size.GetWidth() )
						{
							percent = (float)(m_size.GetWidth()+d)/(float)(m_grabed->m_size.GetWidth());
							m_grabed->p_split1->f_percentw = percent;
						}
					}
					break;
			}
			SetCursor();
			Resize( m_root->GetSize() );
			Layout();
		}
	}
	else
	{
		bool cleargrab=true;
		wxCIViewport *current=m_root;

		ClearStack();
		Push(current);
		while ( !IsStackEmpty() )
		{
			current = Pop();
			if ( current && current->n_split!=WXIRR_DIRNONE )
			{
				grabdir = current->p_split1->BorderContains(event.GetPosition());
				if (grabdir!=WXIRR_DIRNONE)
				{
					m_grabed = current;
					cleargrab = false;
					break;
				}
			}
			while (current && current->n_split!=WXIRR_DIRNONE)
			{
				Push(current->p_split2);
				current = current->p_split1;
				if ( current->n_split!=WXIRR_DIRNONE )
				{
					grabdir = current->p_split1->BorderContains(event.GetPosition());
					if (grabdir!=WXIRR_DIRNONE)
					{
						m_grabed = current;
						cleargrab = false;
						ClearStack();
						break;
					}
				}
			}
			if (cleargrab)
				m_grabed = NULL;
		}
	}
	SetCursor();
	SetActiveViewport( Contains(event.GetPosition()) );
}

void wxCIViewportManager::OnMouseLeftDown(wxMouseEvent &event)
{
	SelectViewport( Contains(event.GetPosition()) );
	if ( m_point==wxPoint(-1, -1)  )
	{
		m_point = event.GetPosition();
		if ( m_grabed && m_grabed->p_split1 )
		{
			m_size = m_grabed->p_split1->m_size;
			SetCursor();
			if (m_grabed!=m_root)
			{
				if ( m_grabed->p_split1->BorderContains( event.GetPosition() )==WXIRR_DIRWEST)
				{
					m_grabed = GetParent(m_grabed);
					m_size = m_grabed->p_split1->m_size;
				}
				if ( m_grabed->p_split1->BorderContains( event.GetPosition() )==WXIRR_DIRNORTH)
				{
					m_grabed = GetParent(m_grabed);
					m_size = m_grabed->p_split1->m_size;
				}
				while ( m_grabed && (grabdir==m_grabed->n_split) )
					m_grabed = GetParent(m_grabed);
			}
		}
	}
}

void wxCIViewportManager::OnMouseLeftUp(wxMouseEvent &event)
{
	m_point = wxPoint(-1, -1);
	m_grabed = NULL;
}

void wxCIViewportManager::OnPaint(wxIVideoDriver *driver, wxISceneManager *mgr, wxICamera *cam, wxCIViewport *viewport)
{
	wxICamera *currentcam;
	wxCIViewport *current=(viewport!=NULL) ? viewport : m_root;

	ClearStack();
	Push(current);
	while ( !IsStackEmpty() )
	{
		current = Pop();
		if ( current && (current->n_split==WXIRR_DIRNONE) )
		{
			currentcam = current->p_cam ? current->p_cam : cam;
			driver->setViewPort(m_root->GetArea());
			current->Draw(driver);
			if ( current->CanBeShown() && currentcam )
			{
				currentcam->setAspectRatio( current->GetRatio() );
				mgr->setActiveCamera(currentcam);
				driver->setViewPort( current->GetArea(true) );
				mgr->drawAll();
			}
		}
		else
		{
			while (current && current->n_split!=WXIRR_DIRNONE)
			{
				Push(current->p_split2);
				current = current->p_split1;
				if (current && current->n_split==WXIRR_DIRNONE )
				{
					currentcam = current->p_cam ? current->p_cam : cam;
					driver->setViewPort(m_root->GetArea());
					current->Draw(driver);
					if ( current->CanBeShown() && currentcam )
					{
						currentcam->setAspectRatio( current->GetRatio() );
						mgr->setActiveCamera(currentcam);
						driver->setViewPort( current->GetArea(true) );
						mgr->drawAll();
					}
				}
			}
		}
	}
}

wxISplitInfo wxCIViewportManager::SplitViewport(wxCIViewport *viewport, wxIrrOrientation split, float percent)
{
	wxISplitInfo m_splitinfo={ NULL, NULL };

	if (!viewport)
		return m_splitinfo;

	wxCIViewport *split1, *split2;

	if (viewport->n_split==WXIRR_DIRNONE)
	{
		split1 = new wxCIViewport();
		split2 = new wxCIViewport();

		if (split1)
		{
			split1->f_percentw = (split==WXIRR_DIRHORIZ) ? 1.0f : percent;
			split1->f_percenth = (split==WXIRR_DIRHORIZ) ? percent : 1.0f;
			split1->SetFont(font);
		}
		if (split2)
		{
			split2->f_percentw = (split==WXIRR_DIRHORIZ) ? 1.0f : percent;
			split2->f_percenth = (split==WXIRR_DIRHORIZ) ? percent : 1.0f;
			split2->SetFont(font);
		}

		viewport->p_split1 = split1;
		viewport->p_split2 = split2;
		viewport->n_split = split;
		viewport->Resize(viewport->m_size);
		viewport->Layout();

		m_splitinfo.split1 = split1;
		m_splitinfo.split2 = split2;

		SelectViewport(split1);
	}
	return m_splitinfo;
}

void wxCIViewportManager::RemoveViewport(wxCIViewport *viewport)
{
	// do not remove the root viewport
	if ( (!viewport) )
		return;

	if (!b_allowroodel && (viewport==m_root) )
		return;

	wxCIViewport *current=viewport;
	bool activeremoved, selectedremoved;

	ClearStack();
	Push(current);
	while ( !IsStackEmpty() )
	{
		current = Pop();
		activeremoved=(m_active==current);
		selectedremoved=(m_selected==current);
		while (current && current->n_split!=WXIRR_DIRNONE)
		{
			current->RemoveChildsSafe();
			Push(current->p_split2);
			current = current->p_split1;
			activeremoved=(m_active==current);
			selectedremoved=(m_selected==current);
		}
	}
	viewport->RemoveChildsSafe();

	wxCIViewport *parent=GetParent(viewport);

	if ( parent )
	{
		wxCIViewport *brother, *split1, *split2;
		wxIrrOrientation n_split;

		brother = ( viewport == parent->p_split2 ) ? parent->p_split1 : parent->p_split2;
		split1 = ( viewport == parent->p_split2 ) ? parent->p_split1->p_split1 : split1 = parent->p_split2->p_split1;
		split2 = ( viewport == parent->p_split2 ) ? parent->p_split1->p_split2 : split2 = parent->p_split2->p_split2;
		n_split = brother->n_split;
		parent->p_split1 = split1;
		parent->p_split2 = split2;
		parent->m_settings = brother->m_settings;
		parent->p_cam = brother->p_cam;
		delete brother;
		if ( (parent->p_split1==NULL) && (parent->p_split2==NULL) )
			parent->n_split = WXIRR_DIRNONE;
		else
			parent->n_split = n_split;
		Resize(parent->m_size, parent);
		Layout(parent);
		if (activeremoved)
		{
			if (parent->n_split==WXIRR_DIRNONE)
				SetActiveViewport(parent);
			else
				SetActiveViewport( ( viewport == parent->p_split1 ) ? split1 : split2 );
		}
		if (selectedremoved)
		{
			if (parent->n_split==WXIRR_DIRNONE)
				SelectViewport(parent);
			else
				SelectViewport( ( viewport == parent->p_split1 ) ? split1 : split2 );
		}
	}
	delete viewport;
}

wxCIViewport *wxCIViewportManager::Contains(wxPoint pos)
{
	wxCIViewport *current=m_root;

	ClearStack();
	Push(current);
	while ( !IsStackEmpty() )
	{
		current = Pop();
		if ( current->n_split==WXIRR_DIRNONE)
			if ( current->Contains(pos) )
				return current;
		while (current && current->n_split!=WXIRR_DIRNONE)
		{
			Push(current->p_split2);
			current = current->p_split1;
			if ( current->n_split==WXIRR_DIRNONE)
				if ( current->Contains(pos) )
					return current;
		}
	}
	return NULL;
}

void wxCIViewportManager::SelectViewport(wxCIViewport *selected)
{
	if ( (m_selected==selected) || (!selected) )
		return;
	b_refresh = true;
	if (m_selected)
		m_selected->m_settings.b_selected = false;
	m_selected = selected;
	m_selected->m_settings.b_selected = true;
}

unsigned int wxCIViewportManager::GetViewportCount(bool bsplited)
{
	int n=0;
	wxCIViewport *current=m_root;

	ClearStack();
	Push(current);
	while ( !IsStackEmpty() )
	{
		current = Pop();
		if (bsplited)
			n++;
		else
		{
			if ( current->n_split==WXIRR_DIRNONE)
				n++;
		}
		while (current && current->n_split!=WXIRR_DIRNONE)
		{
			Push(current->p_split2);
			current = current->p_split1;
			if (bsplited)
				n++;
			else
			{
				if ( current->n_split==WXIRR_DIRNONE)
					n++;
			}
		}
	}
	return n;
}

wxCIViewport *wxCIViewportManager::GetViewport(int index, wxCIViewport *viewport)
{
	int n=0;
	wxCIViewport *current=(viewport==NULL) ? m_root : viewport;

	ClearStack();
	Push(current);
	while ( !IsStackEmpty() )
	{
		current = Pop();
		if ( current->n_split==WXIRR_DIRNONE)
		{
			if (n==index)
				return current;
			n++;
		}
		while (current && current->n_split!=WXIRR_DIRNONE)
		{
			Push(current->p_split2);
			current = current->p_split1;
			if ( current->n_split==WXIRR_DIRNONE)
			{
				if (n==index)
					return current;
				n++;
			}
		}
	}
	return NULL;
}

wxIColor wxCIViewportManager::GetViewportBackgroundColor(wxCIViewport *viewport)
{
	if (!viewport)
		return wxIColor();
	return viewport->GetBackgroundlColor();
}

wxIColor wxCIViewportManager::GetViewportSelectedColor(wxCIViewport *viewport)
{
	if (!viewport)
		return wxIColor();
	return viewport->GetSelectedColor();
}

wxIColor wxCIViewportManager::GetViewportBorderColor(wxCIViewport *viewport)
{
	if (!viewport)
		return wxIColor();
	return viewport->GetBorderColor();
}

wxSize wxCIViewportManager::GetViewportBorderWidth(wxCIViewport *viewport)
{
	if (!viewport)
		return wxSize();
	return viewport->GetBorderWidth();
}

wxString wxCIViewportManager::GetViewportName(wxCIViewport *viewport)
{
	if (!viewport)
		return wxEmptyString;
	return viewport->GetName();
}

wxIGuiFont *wxCIViewportManager::GetViewportFont(wxCIViewport *viewport)
{
	if (!viewport)
		return NULL;
	return viewport->GetFont();
}

wxIColor wxCIViewportManager::GetViewportTextColor(wxCIViewport *viewport)
{
	if (!viewport)
		return wxIColor();
	return viewport->GetTextColor();
}

wxIColor wxCIViewportManager::GetViewportSelectedTextColor(wxCIViewport *viewport)
{
	if (!viewport)
		return wxIColor();
	return viewport->GetSelectedTextColor();
}

void wxCIViewportManager::SetViewportBackgroundColor(wxCIViewport *viewport, wxIColor color)
{
	if (!viewport)
		return;
	viewport->SetBackgroundColor(color);
}

void wxCIViewportManager::SetViewportSelectedColor(wxCIViewport *viewport, wxIColor color)
{
	if (!viewport)
		return;
	viewport->SetSelectedColor(color);
}

void wxCIViewportManager::SetViewportBorderColor(wxCIViewport *viewport, wxIColor color)
{
	if (!viewport)
		return;
	viewport->SetBorderColor(color);
}

void wxCIViewportManager::SetViewportBorderWidth(wxCIViewport *viewport, wxSize borderwidth)
{
	if (!viewport)
		return;
	viewport->SetBorderWidth(borderwidth);
}

void wxCIViewportManager::SetViewportName(wxCIViewport *viewport, wxString name)
{
	if (!viewport)
		return;
	viewport->SetName(name);
}

void wxCIViewportManager::SetViewportFont(wxCIViewport *viewport, wxIGuiFont *fnt)
{
	if (!viewport)
		return;
	viewport->SetFont(fnt);
}

void wxCIViewportManager::SetViewportTextColor(wxCIViewport *viewport, wxIColor color)
{
	if (!viewport)
		return;
	viewport->SetTextColor(color);
}

void wxCIViewportManager::SetViewportSelectedTextColor(wxCIViewport *viewport, wxIColor color)
{
	if (!viewport)
		return;
	viewport->SetSelectedTextColor(color);
}

void wxCIViewportManager::SetFont(wxIGuiFont *fnt)
{
	wxCIViewport *current=m_root;

	font = fnt;
	ClearStack();
	Push(current);
	while ( !IsStackEmpty() )
	{
		current = Pop();
		if ( current )
			current->SetFont(font);
		while (current && current->n_split!=WXIRR_DIRNONE)
		{
			Push(current->p_split2);
			current = current->p_split1;
			if ( current )
				current->SetFont(font);
		}
	}
}

void wxCIViewportManager::Resize(wxSize size, wxCIViewport *viewport)
{
	wxCIViewport *current=(viewport!=NULL) ? viewport : m_root;
	wxCIViewport *D=current->p_split2;

	if (current->n_split==WXIRR_DIRNONE)
		current->Resize(size);
	else
	{
		wxSize m_size=size;

		current->Resize( m_size );
		ClearStack();
		Push(current);
		while ( !IsStackEmpty() )
		{
			current = Pop();
			m_size = current->m_size;
			if ( current && (current->n_split!=WXIRR_DIRNONE) )
				current->Resize( m_size );
			while ( current && (current->n_split!=WXIRR_DIRNONE) )
			{
				Push(current->p_split2);
				current = current->p_split1;
				m_size = current->m_size;
				current->Resize( m_size );
			}
		}
	}
}

void wxCIViewportManager::Layout(wxCIViewport *viewport)
{
	wxCIViewport *current=(viewport!=NULL) ? viewport : m_root;

	ClearStack();
	Push(current);
	while ( !IsStackEmpty() )
	{
		current = Pop();
		if ( current && (current->n_split!=WXIRR_DIRNONE) )
			current->Layout();
		while (current && current->n_split!=WXIRR_DIRNONE)
		{
			if ( current )
				current->Layout();
			Push(current->p_split2);
			current = current->p_split1;
		}
	}
}

void wxCIViewportManager::SetActiveViewport(wxCIViewport *active)
{
	if ( (m_active==active) || (!active) )
		return;
	b_refresh = true;
	if (m_active)
		m_active->m_settings.b_active = false;
	m_active = active;
	m_active->m_settings.b_active = true;
}

void wxCIViewportManager::Push(wxCIViewport *viewport)
{
	m_vstack.Add(viewport);
}

wxCIViewport *wxCIViewportManager::Pop(void)
{
	if ( m_vstack.GetCount()==0 )
		return NULL;

	wxCIViewport **v=m_vstack.Detach(m_vstack.GetCount()-1);

	return *v;
}

wxCIViewport *wxCIViewportManager::GetParent(wxCIViewport *viewport)
{
	if ( (!viewport) || (viewport==m_root) )
		return NULL;

	wxCIViewport *current=m_root;

	ClearStack();
	Push(current);
	while ( !IsStackEmpty() )
	{
		current = Pop();
		while (current && current->n_split!=WXIRR_DIRNONE)
		{
			if ( (current->p_split1==viewport) || (current->p_split2==viewport) )
				return current;
			Push(current->p_split2);
			current = current->p_split1;
		}
	}
	return current;
}

void wxCIViewportManager::SetCursor(void)
{
	switch (grabdir)
	{
		case WXIRR_DIRWEST:
		case WXIRR_DIREAST:
			::SetCursor(cwe);
			break;
		case WXIRR_DIRNORTH:
		case WXIRR_DIRSOUTH:
			::SetCursor(cns);
			break;
	}
}