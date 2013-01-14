#include "CGUIStretchWindow.h"


const s32 FOD_WIDTH = 640;
const s32 FOD_HEIGHT = 400;
const s32 yoffset = 10;

//! constructor
CGUIStretchWindow::CGUIStretchWindow(const wchar_t* title, IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
: IGUIElement(EGUIET_WINDOW, environment, parent, id, rectangle), Dragging(false), IsDraggable(true), DrawBackground(true), DrawTitlebar(true), IsActive(false)
{
#ifdef _DEBUG
	IGUIElement::setDebugName("CGUIStretchWindow");
#endif

	Text = title;
	device=NULL;
	
	IGUISkin* skin = Environment->getSkin();
	IGUISpriteBank* sprites = 0;
	video::SColor color(255,255,255,255);
	if (skin)
	{
		sprites = skin->getSpriteBank();
		color = skin->getColor(EGDC_WINDOW_SYMBOL);
	}

	s32 buttonw = Environment->getSkin()->getSize(EGDS_WINDOW_BUTTON_WIDTH);

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

}



//! destructor
CGUIStretchWindow::~CGUIStretchWindow()
{
	if (CloseButton)
		CloseButton->drop();

}


//! Returns pointer to the close button
IGUIButton* CGUIStretchWindow::getCloseButton() const
{
	return CloseButton;
}

//! called if an event happened.
bool CGUIStretchWindow::OnEvent(const SEvent& event)
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

				move(core::position2d<s32>(event.MouseInput.X - DragStart.X, event.MouseInput.Y - DragStart.Y));
				DragStart.X = event.MouseInput.X;
				DragStart.Y = event.MouseInput.Y;
				return true;
			}
			break;

		}
	}

	return Parent ? Parent->OnEvent(event) : false;
}


//! draws the element and its children
void CGUIStretchWindow::draw()
{
	if (!IsVisible)
		return;

	// Update the size of the client rectangle if changed
	ClientRect = core::rect<s32>(0,0, AbsoluteRect.getWidth(), AbsoluteRect.getHeight());

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

	if (!Dragging)
	{
		//Hinting and detection for stretch on the right side
		if ((mousepos.X>=AbsoluteRect.LowerRightCorner.X-5) && (mousepos.X<=AbsoluteRect.LowerRightCorner.X) &&
			(mousepos.Y<=AbsoluteRect.LowerRightCorner.Y) && (mousepos.Y>=AbsoluteRect.UpperLeftCorner.Y))
		{   
			//driver->draw2DLine(core::vector2d<s32>(AbsoluteRect.LowerRightCorner.X-1,AbsoluteRect.UpperLeftCorner.Y+1),core::vector2d<s32>(AbsoluteRect.LowerRightCorner.X-1,AbsoluteRect.LowerRightCorner.Y-1),video::SColor(255,255,255,0));
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
			this->stretchbottom=true;
		}
		else
			this->stretchbottom=false;

		//Hinting and detection for stretch on the top
		if ((mousepos.Y<=AbsoluteRect.UpperLeftCorner.Y+5) && (mousepos.Y>=AbsoluteRect.UpperLeftCorner.Y) &&
			(mousepos.X>=AbsoluteRect.UpperLeftCorner.X) && (mousepos.X<=AbsoluteRect.LowerRightCorner.X))
		{
			//driver->draw2DLine(core::vector2d<s32>(AbsoluteRect.UpperLeftCorner.X+1,AbsoluteRect.UpperLeftCorner.Y+1),core::vector2d<s32>(AbsoluteRect.LowerRightCorner.X-1,AbsoluteRect.UpperLeftCorner.Y+1),video::SColor(255,255,255,0));
			this->stretchtop=true;
		}
		else
			this->stretchtop=false;

		if (stretchright || stretchleft)
		{
			if (!stretchbottom && !stretchtop && Environment->getFocus()==this)
				device->getCursorControl()->setActiveIcon( ECURSOR_ICON(11) );
		}
		if (stretchbottom || stretchtop)
		{
			if (!stretchright && !stretchleft && Environment->getFocus()==this)
				device->getCursorControl()->setActiveIcon( ECURSOR_ICON(10) );
		}

		if (stretchbottom && stretchleft && Environment->getFocus()==this)
		{
			device->getCursorControl()->setActiveIcon( ECURSOR_ICON(8) );
		}

		if (stretchbottom && stretchright && Environment->getFocus()==this)
		{
			device->getCursorControl()->setActiveIcon( ECURSOR_ICON(9) );
		}

		if (stretchtop && stretchleft && Environment->getFocus()==this)
		{
			device->getCursorControl()->setActiveIcon( ECURSOR_ICON(9) );
		}

		if (stretchtop && stretchright && Environment->getFocus()==this)
		{
			device->getCursorControl()->setActiveIcon( ECURSOR_ICON(8) );
		}

		if (!stretchright && !stretchleft && !stretchbottom && !stretchtop && Environment->getFocus()==this)
			device->getCursorControl()->setActiveIcon( ECURSOR_ICON(0) );

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


			move(core::position2d<s32>(mousepos.X - DragStart.X, mousepos.Y - DragStart.Y));
			updateAbsolutePosition();
			DragStart.X = mousepos.X;
			DragStart.Y = mousepos.Y;
			IGUIElement::draw();
			return;
		}

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

			if ((AbsoluteRect.LowerRightCorner.X-mousepos.X)>200)
			{
				AbsoluteRect.UpperLeftCorner.X=mousepos.X-5;
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

	IGUIElement::draw();
}

//! Returns the rectangle of the drawable area (without border, without titlebar and without scrollbars)
core::rect<s32> CGUIStretchWindow::getClientRect()
{
	return ClientRect;
}
