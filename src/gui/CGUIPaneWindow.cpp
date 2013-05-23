#include "CGUIPaneWindow.h"


const s32 FOD_WIDTH = 640;
const s32 FOD_HEIGHT = 400;
const s32 yoffset = 10;

//! constructor
CGUIPaneWindow::CGUIPaneWindow(const wchar_t* title, IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle, bool insideborder)
: IGUIElement(EGUIET_WINDOW, environment, parent, id, rectangle), Dragging(false), IsDraggable(true), DrawBackground(true), DrawTitlebar(true), IsActive(false),
DrawInsideBorder(false)
{
#ifdef _DEBUG
	IGUIElement::setDebugName("CGUIPaneWindow");
#endif

	if (insideborder)
		DrawInsideBorder=true;

	Text = title;
	device=NULL;
	scrollpos = 0;
	borderwidth=5;
	
	//Keep the old rectangle in memory.
	oldrectangle=rectangle;

	IGUISkin* skin = Environment->getSkin();
	IGUISpriteBank* sprites = 0;
	video::SColor color(255,255,255,255);
	if (skin)
	{
		sprites = skin->getSpriteBank();
		color = skin->getColor(EGDC_WINDOW_SYMBOL);
	}

	s32 buttonw = Environment->getSkin()->getSize(EGDS_WINDOW_BUTTON_WIDTH);
	
	enableright=false;
	enableleft=false;
	enablebottom=false;
	enabletop=false;
	timer1=0;
	timer2=0;
	expand = false;
	retract = false;

	// Position the close button Windows or Linux style
#ifdef WIN32
	s32 posx = RelativeRect.getWidth() - buttonw - 4;
#else
	s32 posx = 4;
#endif
	CloseButton = Environment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3 + buttonw), this, -1,
		L"", L"Close");
	CloseButton->setSubElement(true);
	if (sprites) {
		CloseButton->setSpriteBank(sprites);
		CloseButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_WINDOW_CLOSE), color);
		CloseButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_WINDOW_CLOSE), color);
	}
#ifdef WIN32
	CloseButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
#else
	CloseButton->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
#endif
	CloseButton->grab();

	//Define the scroolbar
	scroll = Environment->addScrollBar(false, core::rect<s32>(AbsoluteRect.getWidth()-15,0,AbsoluteRect.getWidth(),AbsoluteRect.getHeight()),this,-1);
	scroll->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	scroll->setVisible(false);

}



//! destructor
CGUIPaneWindow::~CGUIPaneWindow()
{
	if (CloseButton)
		CloseButton->drop();

}


//! Returns pointer to the close button
IGUIButton* CGUIPaneWindow::getCloseButton() const
{
	return CloseButton;
}

//! called if an event happened.
bool CGUIPaneWindow::OnEvent(const SEvent& event)
{
	switch(event.EventType)
	{
	
	case EET_GUI_EVENT:
		if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
		{
			Dragging = false;
			IsActive = false;
		}
		else
		if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUSED)
		{
			if (Parent && ((event.GUIEvent.Caller == this) || isMyChild(event.GUIEvent.Caller)))
			{
				Parent->bringToFront(this);
				IsActive = true;
			}
			else
			{
				IsActive = false;
			}
		}
		else
		if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
		{
			if (event.GUIEvent.Caller == CloseButton)
			{
				remove();
				return true;
			}
	
		}
		break;

	case EET_MOUSE_INPUT_EVENT:
		switch(event.MouseInput.Event)
		{
		case EMIE_LMOUSE_PRESSED_DOWN:
			DragStart.X = event.MouseInput.X;
			DragStart.Y = event.MouseInput.Y;
			//if (IsDraggable)
				Dragging = true;			
			return true;

		case EMIE_LMOUSE_LEFT_UP:
			Dragging = false;
			return true;

		case EMIE_MOUSE_MOVED:
			if (Dragging && !stretchbottom && !stretchtop  && !stretchright && !stretchleft)
			{
				// gui window should not be dragged outside its parent
				if (Parent)
					if (event.MouseInput.X < Parent->getAbsolutePosition().UpperLeftCorner.X +1 ||
						event.MouseInput.Y < Parent->getAbsolutePosition().UpperLeftCorner.Y +1 ||
						event.MouseInput.X > Parent->getAbsolutePosition().LowerRightCorner.X -1 ||
						event.MouseInput.Y > Parent->getAbsolutePosition().LowerRightCorner.Y -1)

						return true;

				if (!IsDraggable)
					return true;

				move(core::position2d<s32>(event.MouseInput.X - DragStart.X, event.MouseInput.Y - DragStart.Y));
				DragStart.X = event.MouseInput.X;
				DragStart.Y = event.MouseInput.Y;
				return true;
			}
			break;
		}
	case EGET_SCROLL_BAR_CHANGED:
		//if (event.GUIEvent.Caller == scroll)
		{
			s32 VScrollPos = (s32)scroll->getPos();
			printf ("Scrollpos: %d\n",VScrollPos);
			scrollpos=VScrollPos;
		}
		return true;
		break;
	}
	return Parent ? Parent->OnEvent(event) : false;
}


//! draws the element and its children
void CGUIPaneWindow::draw()
{
	if (!IsVisible)
		return;

	// Update the size of the client rectangle if changed
	ClientRect = core::rect<s32>(0,0, AbsoluteRect.getWidth(), AbsoluteRect.getHeight());
	
	if (scroll->isVisible())
		this->bringToFront(scroll);

	core::vector2d<s32> mousepos = core::vector2d<s32>(0,0);
	IGUISkin* skin = Environment->getSkin();
	irr::video::IVideoDriver* driver = Environment->getVideoDriver();
	
	// Get the cursor position
	// For this, the device need to be set
	if (device)
		mousepos=device->getCursorControl()->getPosition();

	core::rect<s32> rect = AbsoluteRect;

	rect = skin->draw3DWindowBackground(this, true, skin->getColor(IsActive ? EGDC_ACTIVE_BORDER : EGDC_INACTIVE_BORDER),
		rect, &AbsoluteClippingRect);

	if (Text.size())
	{
#ifdef WIN32
		rect.UpperLeftCorner.X += 2;
#else
		rect.UpperLeftCorner.X += CloseButton->getAbsoluteClippingRect().getWidth()+10;
#endif
		rect.LowerRightCorner.X -= skin->getSize(EGDS_WINDOW_BUTTON_WIDTH) + 5;

		IGUIFont* font = skin->getFont(EGDF_WINDOW);
		if (font)
			font->draw(Text.c_str(), rect, skin->getColor(EGDC_ACTIVE_CAPTION), false, true,
			&AbsoluteClippingRect);
	}

	//Draw inside borders
	if (DrawInsideBorder)
	{
		// Inner border as double border lines (better looking, could use an activator so the user could decide to have it or not
		rect.UpperLeftCorner.X=AbsoluteRect.UpperLeftCorner.X+borderwidth;
		rect.UpperLeftCorner.Y=AbsoluteRect.UpperLeftCorner.Y+(20+borderwidth);
		if (scroll->isVisible())
			rect.LowerRightCorner.X=AbsoluteRect.LowerRightCorner.X-(15+borderwidth);
		else
			rect.LowerRightCorner.X=AbsoluteRect.LowerRightCorner.X-borderwidth;

		rect.LowerRightCorner.Y=AbsoluteRect.LowerRightCorner.Y-borderwidth;
		skin->draw3DSunkenPane(this,skin->getColor(EGDC_INACTIVE_BORDER),true,true,rect,&AbsoluteClippingRect);
	}

	// Draw cursor changes
	if (!Dragging)
	{
		drawRef(mousepos);
	}
	else
	{
		if (!stretchtop && !stretchbottom && !stretchright && !stretchleft)
		{
			// gui window should not be dragged outside its parent
			if (Parent)
			{
				if (mousepos.X < Parent->getAbsolutePosition().UpperLeftCorner.X +30 ||
					mousepos.Y < Parent->getAbsolutePosition().UpperLeftCorner.Y +30 ||
					mousepos.X > Parent->getAbsolutePosition().LowerRightCorner.X -30 ||
					mousepos.Y > Parent->getAbsolutePosition().LowerRightCorner.Y -30)
				{
					IGUIElement::draw();
					return;
				}
			}

			if (IsDraggable)
				move(core::position2d<s32>(mousepos.X - DragStart.X, mousepos.Y - DragStart.Y));

			updateAbsolutePosition();
			DragStart.X = mousepos.X;
			DragStart.Y = mousepos.Y;
			IGUIElement::draw();
			return;
		}
		//Stretch the window defined on the desired direction
		drawStretch(mousepos);
	}

	if (expand && device)
	{
		u32 time = device->getTimer()->getRealTime();
		//2 ms interval
		if ((time-timer1) > 1)
		{
			timer1=time;
			AbsoluteRect.UpperLeftCorner.X-=5;
			if (AbsoluteRect.getWidth()>(irr::s32)this->MaxSize.Width)
			{
				expand=false;
				AbsoluteRect.UpperLeftCorner.X+=5;
			}
			this->AbsoluteClippingRect=this->AbsoluteRect;
			this->DesiredRect=AbsoluteRect;
			this->updateAbsolutePosition();

		}

	}
	if (retract && device)
	{
		
		u32 time = device->getTimer()->getRealTime();
		//2 ms interval
		if ((time-timer1) > 1)
		{
			timer1=time;
			AbsoluteRect.UpperLeftCorner.X+=5;
			if (AbsoluteRect.getWidth()<(irr::s32)this->MinSize.Width)
			{
				retract=false;
				AbsoluteRect.UpperLeftCorner.X-=5;
			}
			this->AbsoluteClippingRect=this->AbsoluteRect;
			this->DesiredRect=AbsoluteRect;
			this->updateAbsolutePosition();

		}

	}
	
	IGUIElement::draw();
	
	// If not streched from any corner then update the reference rectangle
	if (!stretchright && !stretchleft && !stretchbottom && !stretchtop)
		oldrectangle=AbsoluteRect;
}

//Draw the cursor changes or hinting reference for a window stretch
void CGUIPaneWindow::drawRef(core::vector2d<s32> mousepos)
{
	//Hinting and detection for stretch on the right side
		if ((mousepos.X>=AbsoluteRect.LowerRightCorner.X-5) && (mousepos.X<=AbsoluteRect.LowerRightCorner.X) &&
			(mousepos.Y<=AbsoluteRect.LowerRightCorner.Y) && (mousepos.Y>=AbsoluteRect.UpperLeftCorner.Y))
		{   
			//driver->draw2DLine(core::vector2d<s32>(AbsoluteRect.LowerRightCorner.X-1,AbsoluteRect.UpperLeftCorner.Y+1),core::vector2d<s32>(AbsoluteRect.LowerRightCorner.X-1,AbsoluteRect.LowerRightCorner.Y-1),video::SColor(255,255,255,0));
			if (enableright) 
				this->stretchright=true;
		} else
		{
			this->stretchright=false;
		}

		//Hinting and detection for stretch on the left side
		if ((mousepos.X>=AbsoluteRect.UpperLeftCorner.X) && (mousepos.X<=AbsoluteRect.UpperLeftCorner.X+5) &&
			(mousepos.Y<=AbsoluteRect.LowerRightCorner.Y) && (mousepos.Y>=AbsoluteRect.UpperLeftCorner.Y))
		{
			//driver->draw2DLine(core::vector2d<s32>(AbsoluteRect.UpperLeftCorner.X+1,AbsoluteRect.LowerRightCorner.Y-1),core::vector2d<s32>(AbsoluteRect.UpperLeftCorner.X+1,AbsoluteRect.UpperLeftCorner.Y-1),video::SColor(255,255,255,0));
			if (enableleft)
				this->stretchleft=true;
		} else
		{
			this->stretchleft=false;
		}

		//Hinting and detection for stretch on the bottom
		if ((mousepos.Y>=AbsoluteRect.LowerRightCorner.Y-5) && (mousepos.Y<=AbsoluteRect.LowerRightCorner.Y) &&
			(mousepos.X>=AbsoluteRect.UpperLeftCorner.X) && (mousepos.X<=AbsoluteRect.LowerRightCorner.X))
		{
			//driver->draw2DLine(core::vector2d<s32>(AbsoluteRect.UpperLeftCorner.X+1,AbsoluteRect.LowerRightCorner.Y-1),core::vector2d<s32>(AbsoluteRect.LowerRightCorner.X-1,AbsoluteRect.LowerRightCorner.Y-1),video::SColor(255,255,255,0));
			if (enablebottom)
				this->stretchbottom=true;
		}
		else
			this->stretchbottom=false;

		//Hinting and detection for stretch on the top
		if ((mousepos.Y<=AbsoluteRect.UpperLeftCorner.Y+5) && (mousepos.Y>=AbsoluteRect.UpperLeftCorner.Y) &&
			(mousepos.X>=AbsoluteRect.UpperLeftCorner.X) && (mousepos.X<=AbsoluteRect.LowerRightCorner.X))
		{
			//driver->draw2DLine(core::vector2d<s32>(AbsoluteRect.UpperLeftCorner.X+1,AbsoluteRect.UpperLeftCorner.Y+1),core::vector2d<s32>(AbsoluteRect.LowerRightCorner.X-1,AbsoluteRect.UpperLeftCorner.Y+1),video::SColor(255,255,255,0));
			if (enabletop)
				this->stretchtop=true;
		}
		else
			this->stretchtop=false;

		//Determine if the cursor is over the GUI because the focus is often lost over this gui
		bool focus = false;
		if (this->isVisible() && this->isPointInside(mousepos))
		{
			focus=true;
			//Environment->setFocus(this);
		}
			

		if (stretchright || stretchleft)
		{
			if (!stretchbottom && !stretchtop && focus)
				device->getCursorControl()->setActiveIcon( ECURSOR_ICON(11) );
		}
		if (stretchbottom || stretchtop)
		{
			if (!stretchright && !stretchleft && focus)
				device->getCursorControl()->setActiveIcon( ECURSOR_ICON(10) );
		}

		if (stretchbottom && stretchleft && focus)
		{
			device->getCursorControl()->setActiveIcon( ECURSOR_ICON(8) );
		}

		if (stretchbottom && stretchright && focus)
		{
			device->getCursorControl()->setActiveIcon( ECURSOR_ICON(9) );
		}

		if (stretchtop && stretchleft && focus)
		{
			device->getCursorControl()->setActiveIcon( ECURSOR_ICON(9) );
		}

		if (stretchtop && stretchright && focus)
		{
			device->getCursorControl()->setActiveIcon( ECURSOR_ICON(8) );
		}

		if (!stretchright && !stretchleft && !stretchbottom && !stretchtop && !focus)
			device->getCursorControl()->setActiveIcon( ECURSOR_ICON(0) );

		if (!stretchright && !stretchleft && !stretchbottom && !stretchtop && focus)
			device->getCursorControl()->setActiveIcon( ECURSOR_ICON(0) );

}

//Stretch the windows defined on the desired direction
void CGUIPaneWindow::drawStretch(core::vector2d<s32> mousepos)
{
	if (stretchright)
	{
		if (mousepos.X<10 || mousepos.X>this->getParent()->getAbsoluteClippingRect().getWidth()-10)
		{
			IGUIElement::draw();
			return;
		}
		if ((mousepos.X-AbsoluteRect.UpperLeftCorner.X)>200)
		{
			AbsoluteRect.LowerRightCorner.X=mousepos.X+5;
		}
		this->AbsoluteClippingRect=this->AbsoluteRect;
		this->DesiredRect=AbsoluteRect;
		this->updateAbsolutePosition();
	}
	if (stretchleft)
	{
		if (mousepos.X<10 || mousepos.X>this->getParent()->getAbsoluteClippingRect().getWidth()-10)
		{
			IGUIElement::draw();
			return;
		}

		if (AbsoluteRect.LowerRightCorner.X-mousepos.X>(irr::s32)this->MinSize.Width-5)
		{
			AbsoluteRect.UpperLeftCorner.X=mousepos.X-5;
			// If the rectangle is moved then cancel the movement.
			if (oldrectangle.LowerRightCorner.X > AbsoluteRect.LowerRightCorner.X)
			{
				AbsoluteRect.LowerRightCorner.X = oldrectangle.LowerRightCorner.X;
				AbsoluteRect.UpperLeftCorner.X=mousepos.X+5;
				stretchleft = false;
			}
		}
		this->AbsoluteClippingRect=this->AbsoluteRect;
		this->DesiredRect=AbsoluteRect;
		this->updateAbsolutePosition();
	}
	if (stretchbottom)
	{
		if (mousepos.Y<10 || mousepos.Y>this->getParent()->getAbsoluteClippingRect().getHeight()-10)
		{
			IGUIElement::draw();
			return;
		}

		if ((mousepos.Y-AbsoluteRect.UpperLeftCorner.Y)>200)
		{
			AbsoluteRect.LowerRightCorner.Y=mousepos.Y+5;
		}
		this->AbsoluteClippingRect=this->AbsoluteRect;
		this->DesiredRect=AbsoluteRect;
		this->updateAbsolutePosition();
	}
	if (stretchtop)
	{
		if (mousepos.Y<10 || mousepos.Y>this->getParent()->getAbsoluteClippingRect().getHeight()-10)
		{
			IGUIElement::draw();
			return;
		}

		if ((AbsoluteRect.LowerRightCorner.Y-mousepos.Y)>200)
		{
			AbsoluteRect.UpperLeftCorner.Y=mousepos.Y-5;
		}
		this->AbsoluteClippingRect=this->AbsoluteRect;
		this->DesiredRect=AbsoluteRect;
		this->updateAbsolutePosition();
	}

}

//! Returns the rectangle of the drawable area (without border, without titlebar and without scrollbars)
core::rect<s32> CGUIPaneWindow::getClientRect()
{
	return ClientRect;
}

// Initialize a expand of the left side of the window
// Will have to be expanded to allow the four direction
// Enums will be needed to define the direction
void CGUIPaneWindow::expandLeft()
{
	timer1 = device->getTimer()->getRealTime();
	expand=true;
	printf("Asked to expand the pane\n");

}

// Initialize a retract of the left side of the window
// Will have to be expanded to allow the four direction
// Enums will be needed to define the direction
void CGUIPaneWindow::retractLeft()
{
	timer1 = device->getTimer()->getRealTime();
	retract=true;
	printf("Asked to retract the pane\n");

}

// Give a status of the window (to determine to expand or retract)
bool CGUIPaneWindow::leftStatus()
{
	printf("Asked the pane status\n");
	if ((AbsoluteRect.getWidth()/2)>(irr::s32)MinSize.Width)
		return true; // Should call expand
	else
		return false; // Should call retract
}