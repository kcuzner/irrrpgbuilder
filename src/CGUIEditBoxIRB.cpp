// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

// This component was modified to support LUA Script editing in IRB

#include "CGUIEditBoxIRB.h"
#include "IGUIScrollBar.h"

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IGUIFont.h"
#include "IVideoDriver.h"
#include "IrrlichtDevice.h"
#include "rect.h"
//#include "os.h"
#include "Keycodes.h"

#include <iostream>
#include <vector>
#include <sstream>

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

/*
	todo:
	optional scrollbars
	ctrl+left/right to select word
	double click/ctrl click: word select + drag to select whole words, triple click to select line
	optional? dragging selected text
	numerical
*/

namespace irr
{
namespace gui
{

//! constructor
CGUIEditBoxIRB::CGUIEditBoxIRB(const wchar_t* text, bool border, bool lines,
		IGUIEnvironment* environment, IGUIElement* parent, s32 id,
		const core::rect<s32>& rectangle, IrrlichtDevice* device)
	: IGUIEditBox(environment, parent, id, rectangle), MouseMarking(false),
	Border(border), LineNumbering(lines), OverrideColorEnabled(false), MarkBegin(0), MarkEnd(0),
	OverrideColor(video::SColor(101,255,255,255)), OverrideFont(0), LastBreakFont(0),
	Operator(0), BlinkStartTime(0), CursorPos(0), HScrollPos(0), VScrollPos(0), Max(0),
	WordWrap(false), MultiLine(false), AutoScroll(true), PasswordBox(false),
	PasswordChar(L'*'), HAlign(EGUIA_UPPERLEFT), VAlign(EGUIA_CENTER),
	CurrentTextRect(0,0,1,1), FrameRect(rectangle)
{
	#ifdef _DEBUG
	setDebugName("CGUIEditBoxIRB");
	#endif

	linecount_text="";
	Text = text;
	IRRdevice = device;

	if (Environment)
		Operator = Environment->getOSOperator();

	if (Operator)
		Operator->grab();

	// this element can be tabbed to
	setTabStop(true);
	setTabOrder(-1);

	IGUISkin *skin = 0;
	if (Environment)
		skin = Environment->getSkin();

	if (Border && skin)
	{
		FrameRect.UpperLeftCorner.X += skin->getSize(EGDS_TEXT_DISTANCE_X)+1;
		FrameRect.UpperLeftCorner.Y += skin->getSize(EGDS_TEXT_DISTANCE_Y)+1;
		FrameRect.LowerRightCorner.X -= skin->getSize(EGDS_TEXT_DISTANCE_X)+1;
		FrameRect.LowerRightCorner.Y -= skin->getSize(EGDS_TEXT_DISTANCE_Y)+1;
	}
	irr::core::rect<s32> myRect(s32 x, s32 y, s32 w, s32 h);
	
	
	ScrollbarH = Environment->addScrollBar(true,myRect(2,FrameRect.getHeight()-20,FrameRect.getWidth()-18,20),this,-1);
	Scrollbar = Environment->addScrollBar(false,myRect(FrameRect.getWidth()-15,2,20,FrameRect.getHeight()-22),this,-1);
	LineToggle = Environment->addButton(myRect(FrameRect.getWidth()-15,FrameRect.getHeight()-20,20,20),this,-1,L"#",linecount_text.c_str());
	LineToggle->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT);


	Scrollbar->setSubElement(true);
	Scrollbar->setTabStop(false);
	Scrollbar->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	Scrollbar->setPos(0);
	Scrollbar->setSmallStep(3);

	ScrollbarH->setSubElement(true);
	ScrollbarH->setTabStop(false);
	ScrollbarH->setSmallStep(3);
	ScrollbarH->setMax(0);
	ScrollbarH->setPos(0);
	ScrollbarH->setVisible(false);
	
	if (MultiLine)
	{
		LineToggle->setVisible(false);
		Scrollbar->setVisible(false);
		ScrollbarH->setVisible(false);
		LineNumbering = false;
	}

	// Init the text spacing for adding GUI elements
	if (LineNumbering)
		LeftSpace = 60; // for the line counter
	else
		LeftSpace = 0;

	RightSpace = 25; // scrollbar
	UpperSpace = 0;
	LowerSpace = 0; // scrollbar

	breakText();
	calculateScrollPos();
}


//! destructor
CGUIEditBoxIRB::~CGUIEditBoxIRB()
{
	if (OverrideFont)
		OverrideFont->drop();

	if (Operator)
		Operator->drop();
}

//! Sets another skin independent font.
void CGUIEditBoxIRB::setOverrideFont(IGUIFont* font)
{
	if (OverrideFont == font)
		return;

	if (OverrideFont)
		OverrideFont->drop();

	OverrideFont = font;

	if (OverrideFont)
		OverrideFont->grab();

	breakText();
}


//! Sets another color for the text.
void CGUIEditBoxIRB::setOverrideColor(video::SColor color)
{
	OverrideColor = color;
	OverrideColorEnabled = true;
}


//! Turns the border on or off
void CGUIEditBoxIRB::setDrawBorder(bool border)
{
	Border = border;
}


//! Sets if the text should use the overide color or the color in the gui skin.
void CGUIEditBoxIRB::enableOverrideColor(bool enable)
{
	OverrideColorEnabled = enable;
}


//! Enables or disables word wrap
void CGUIEditBoxIRB::setWordWrap(bool enable)
{
	WordWrap = enable;
	breakText();
}


void CGUIEditBoxIRB::updateAbsolutePosition()
{
    core::rect<s32> oldAbsoluteRect(AbsoluteRect);
	IGUIElement::updateAbsolutePosition();
	if ( oldAbsoluteRect != AbsoluteRect )
	{
        breakText();
	}
}


//! Checks if word wrap is enabled
bool CGUIEditBoxIRB::isWordWrapEnabled() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return WordWrap;
}


//! Enables or disables newlines.
void CGUIEditBoxIRB::setMultiLine(bool enable)
{
	MultiLine = enable;
	LineToggle->setVisible(enable);
	Scrollbar->setVisible(enable);
	//ScrollbarH->setVisible(enable);
	LineNumbering = enable;
}


//! Checks if multi line editing is enabled
bool CGUIEditBoxIRB::isMultiLineEnabled() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return MultiLine;
}


void CGUIEditBoxIRB::setPasswordBox(bool passwordBox, wchar_t passwordChar)
{
	PasswordBox = passwordBox;
	if (PasswordBox)
	{
		PasswordChar = passwordChar;
		setMultiLine(false);
		setWordWrap(false);
		BrokenText.clear();
	}
}


bool CGUIEditBoxIRB::isPasswordBox() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return PasswordBox;
}


//! Sets text justification
void CGUIEditBoxIRB::setTextAlignment(EGUI_ALIGNMENT horizontal, EGUI_ALIGNMENT vertical)
{
	HAlign = horizontal;
	VAlign = vertical;
}

void CGUIEditBoxIRB::setLineCountButtonText(irr::core::stringw text)
{
	linecount_text=text;
	LineToggle->setToolTipText(text.c_str());
}

//! called if an event happened.
bool CGUIEditBoxIRB::OnEvent(const SEvent& event)
{
	if (IsEnabled)
	{

		switch(event.EventType)
		{
		case EET_GUI_EVENT:
			if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
			{
				if (event.GUIEvent.Caller == this)
				{
					MouseMarking = false;
					setTextMarkers(0,0);
					Environment->setFocus(this);
				}
				break;
			}
			if (event.GUIEvent.EventType == EGET_EDITBOX_ENTER)
			{
				if (event.GUIEvent.Caller == this)
				{
				
				}
				break;
			}
			
			if (event.GUIEvent.EventType == EGET_SCROLL_BAR_CHANGED)
			{
				
				if (event.GUIEvent.Caller == Scrollbar)
				{
					if (MultiLine || (WordWrap && BrokenText.size() > 1) )
					{
						IGUISkin* skin = Environment->getSkin();
						IGUIFont* font = OverrideFont;
						if (!OverrideFont)
							font = skin->getFont();
						s32 scrollMove = font->getDimension(L"000000").Height;
						VScrollPos = (s32)(scrollMove/1.7f) * Scrollbar->getPos();
			
					}
				}
				break;
			}

			if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
			{
				if (event.GUIEvent.Caller == LineToggle)
				{
					if (LineNumbering)
					{
						LineNumbering=false;
						LeftSpace=0;
						irr::core::rect<s32> myRect(s32 x, s32 y, s32 w, s32 h);
						core::rect<s32> localClipRect = myRect(FrameRect.UpperLeftCorner.X+LeftSpace,FrameRect.UpperLeftCorner.Y,FrameRect.getWidth()-(LeftSpace/2),FrameRect.getHeight());
	                	localClipRect.clipAgainst(AbsoluteClippingRect);
					}

					else
					{
						LineNumbering=true;
						LeftSpace=60;
						irr::core::rect<s32> myRect(s32 x, s32 y, s32 w, s32 h);
						core::rect<s32> localClipRect = myRect(FrameRect.UpperLeftCorner.X+LeftSpace,FrameRect.UpperLeftCorner.Y,FrameRect.getWidth()-(LeftSpace/2),FrameRect.getHeight());
						localClipRect.clipAgainst(AbsoluteClippingRect);
					}
				}

			}
			break;

		case EET_KEY_INPUT_EVENT:
			if (processKey(event))
				return true;
			break;
		
		case EET_MOUSE_INPUT_EVENT:
			if (processMouse(event))
				return true;
			break;
		
		default:
			break;
		}
	}

	return IGUIElement::OnEvent(event);
}


bool CGUIEditBoxIRB::processKey(const SEvent& event)
{
	if (!event.KeyInput.PressedDown)
		return false;

	bool textChanged = false;
	s32 newMarkBegin = MarkBegin;
	s32 newMarkEnd = MarkEnd;

	// control shortcut handling

	if (event.KeyInput.Control)
	{
		// german backlash '\' entered with control + '?'
		if ( event.KeyInput.Char == '\\' )
		{
			inputChar(event.KeyInput.Char);
			return true;
		}

		switch(event.KeyInput.Key)
		{
		case KEY_KEY_A:
			// select all
			newMarkBegin = 0;
			newMarkEnd = Text.size();
			break;
		case KEY_KEY_C:
			// copy to clipboard
			if (!PasswordBox && Operator && MarkBegin != MarkEnd)
			{
				const s32 realmbgn = MarkBegin < MarkEnd ? MarkBegin : MarkEnd;
				const s32 realmend = MarkBegin < MarkEnd ? MarkEnd : MarkBegin;

				core::stringc s;
				s = Text.subString(realmbgn, realmend - realmbgn).c_str();
				Operator->copyToClipboard(s.c_str());
			}
			break;
		case KEY_KEY_X:
			// cut to the clipboard
			if (!PasswordBox && Operator && MarkBegin != MarkEnd)
			{
				const s32 realmbgn = MarkBegin < MarkEnd ? MarkBegin : MarkEnd;
				const s32 realmend = MarkBegin < MarkEnd ? MarkEnd : MarkBegin;

				// copy
				core::stringc sc;
				sc = Text.subString(realmbgn, realmend - realmbgn).c_str();
				Operator->copyToClipboard(sc.c_str());

				if (IsEnabled)
				{
					// delete
					core::stringw s;
					s = Text.subString(0, realmbgn);
					s.append( Text.subString(realmend, Text.size()-realmend) );
					Text = s;

					CursorPos = realmbgn;
					newMarkBegin = 0;
					newMarkEnd = 0;
					textChanged = true;
				}
			}
			break;
		case KEY_KEY_V:
			if ( !IsEnabled )
				break;

			// paste from the clipboard
			if (Operator)
			{
				const s32 realmbgn = MarkBegin < MarkEnd ? MarkBegin : MarkEnd;
				const s32 realmend = MarkBegin < MarkEnd ? MarkEnd : MarkBegin;

				// add new character
				const c8* p = Operator->getTextFromClipboard();
				if (p)
				{
					if (MarkBegin == MarkEnd)
					{
						// insert text
						core::stringw s = Text.subString(0, CursorPos);
						s.append(p);
						s.append( Text.subString(CursorPos, Text.size()-CursorPos) );

						if (!Max || s.size()<=Max) // thx to Fish FH for fix
						{
							Text = s;
							s = p;
							CursorPos += s.size();
						}
					}
					else
					{
						// replace text

						core::stringw s = Text.subString(0, realmbgn);
						s.append(p);
						s.append( Text.subString(realmend, Text.size()-realmend) );

						if (!Max || s.size()<=Max)  // thx to Fish FH for fix
						{
							Text = s;
							s = p;
							CursorPos = realmbgn + s.size();
						}
					}
				}

				newMarkBegin = 0;
				newMarkEnd = 0;
				textChanged = true;
			}
			break;
		case KEY_HOME:
			// move/highlight to start of text
			if (event.KeyInput.Shift)
			{
				newMarkEnd = CursorPos;
				newMarkBegin = 0;
				CursorPos = 0;
			}
			else
			{
				CursorPos = 0;
				newMarkBegin = 0;
				newMarkEnd = 0;
			}
			break;
		case KEY_END:
			// move/highlight to end of text
			if (event.KeyInput.Shift)
			{
				newMarkBegin = CursorPos;
				newMarkEnd = Text.size();
				CursorPos = 0;
			}
			else
			{
				CursorPos = Text.size();
				newMarkBegin = 0;
				newMarkEnd = 0;
			}
			break;
		default:
			return false;
		}
	}
	// default keyboard handling
	else
	switch(event.KeyInput.Key)
	{
	case KEY_END:
		{
			s32 p = Text.size();
			if (WordWrap || MultiLine)
			{
				p = getLineFromPos(CursorPos);
				p = BrokenTextPositions[p] + (s32)BrokenText[p].size();
				if (p > 0 && (Text[p-1] == L'\r' || Text[p-1] == L'\n' ))
					p-=1;
			}

			if (event.KeyInput.Shift)
			{
				if (MarkBegin == MarkEnd)
					newMarkBegin = CursorPos;

				newMarkEnd = p;
			}
			else
			{
				newMarkBegin = 0;
				newMarkEnd = 0;
			}
			CursorPos = p;
			BlinkStartTime = IRRdevice->getTimer()->getRealTime();
		}
		break;
	case KEY_HOME:
		{

			s32 p = 0;
			if (WordWrap || MultiLine)
			{
				p = getLineFromPos(CursorPos);
				p = BrokenTextPositions[p];
			}

			if (event.KeyInput.Shift)
			{
				if (MarkBegin == MarkEnd)
					newMarkBegin = CursorPos;
				newMarkEnd = p;
			}
			else
			{
				newMarkBegin = 0;
				newMarkEnd = 0;
			}
			CursorPos = p;
			BlinkStartTime = IRRdevice->getTimer()->getRealTime();
		
		}
		break;
	case KEY_RETURN:
		if (MultiLine)
		{
			sendGuiEvent( EGET_EDITBOX_ENTER );
            s32 lineStart = getLineFromPos(CursorPos);
			lineStart = BrokenTextPositions[lineStart];

            s32 lineEnd = CursorPos;

            inputChar(L'\n');

            for (int i=lineStart ; i<=lineEnd ; i++)
            {
                if( Text.subString(i,1) == irr::core::stringw(" ") )
                {
                    inputChar(L' ');
                }
                else
                    break;
            }
			
			return true;
		}
		else
		{
		    sendGuiEvent( EGET_EDITBOX_ENTER );
		}
		break;
	case KEY_LEFT:

		if (event.KeyInput.Shift)
		{
			if (CursorPos > 0)
			{
				if (MarkBegin == MarkEnd)
					newMarkBegin = CursorPos;

				newMarkEnd = CursorPos-1;
			}
		}
		else
		{
			newMarkBegin = 0;
			newMarkEnd = 0;
		}

		if (CursorPos > 0) 
			CursorPos--;
		BlinkStartTime = IRRdevice->getTimer()->getRealTime();
		break;

	case KEY_RIGHT:
		if (event.KeyInput.Shift)
		{
			if (Text.size() > (u32)CursorPos)
			{
				if (MarkBegin == MarkEnd)
					newMarkBegin = CursorPos;

				newMarkEnd = CursorPos+1;
			}
		}
		else
		{
			newMarkBegin = 0;
			newMarkEnd = 0;
		}

		if (Text.size() > (u32)CursorPos) 
			CursorPos++;
		BlinkStartTime = IRRdevice->getTimer()->getRealTime();
		break;

	case KEY_UP:
		if (MultiLine || (WordWrap && BrokenText.size() > 1) )
		{
			s32 lineNo = getLineFromPos(CursorPos);
			s32 mb = (MarkBegin == MarkEnd) ? CursorPos : (MarkBegin > MarkEnd ? MarkBegin : MarkEnd);
			if (lineNo > 0)
			{
				s32 cp = CursorPos - BrokenTextPositions[lineNo];
				if ((s32)BrokenText[lineNo-1].size() < cp)
					CursorPos = BrokenTextPositions[lineNo-1] + (s32)BrokenText[lineNo-1].size()-1;
				else
					CursorPos = BrokenTextPositions[lineNo-1] + cp;
			}

			if (event.KeyInput.Shift)
			{
				newMarkBegin = mb;
				newMarkEnd = CursorPos;
			}
			else
			{
				newMarkBegin = 0;
				newMarkEnd = 0;
			}
			Scrollbar->setPos(getLineFromPos(CursorPos));

		}
		else
		{
			return false;
		}
		break;

	case KEY_DOWN:
		if (MultiLine || (WordWrap && BrokenText.size() > 1) )
		{
			s32 lineNo = getLineFromPos(CursorPos);
			s32 mb = (MarkBegin == MarkEnd) ? CursorPos : (MarkBegin < MarkEnd ? MarkBegin : MarkEnd);
			if (lineNo < (s32)BrokenText.size()-1)
			{
				s32 cp = CursorPos - BrokenTextPositions[lineNo];
				if ((s32)BrokenText[lineNo+1].size() < cp)
					CursorPos = BrokenTextPositions[lineNo+1] + BrokenText[lineNo+1].size()-1;
				else
					CursorPos = BrokenTextPositions[lineNo+1] + cp;
			}

			if (event.KeyInput.Shift)
			{
				newMarkBegin = mb;
				newMarkEnd = CursorPos;
			}
			else
			{
				newMarkBegin = 0;
				newMarkEnd = 0;
			}
			Scrollbar->setPos(getLineFromPos(CursorPos));

		}
		else
		{
			return false;
		}
		break;

	case KEY_BACK:
		if ( !this->IsEnabled )
			break;

		if (Text.size())
		{
			core::stringw s;

			if (MarkBegin != MarkEnd)
			{
				// delete marked text
				const s32 realmbgn = MarkBegin < MarkEnd ? MarkBegin : MarkEnd;
				const s32 realmend = MarkBegin < MarkEnd ? MarkEnd : MarkBegin;

				s = Text.subString(0, realmbgn);
				s.append( Text.subString(realmend, Text.size()-realmend) );
				Text = s;

				CursorPos = realmbgn;
			}
			else
			{
				// delete text behind cursor
				if (CursorPos>0)
					s = Text.subString(0, CursorPos-1);
				else
					s = L"";
				s.append( Text.subString(CursorPos, Text.size()-CursorPos) );
				Text = s;
				--CursorPos;
			}

			if (CursorPos < 0)
				CursorPos = 0;
			
			BlinkStartTime = IRRdevice->getTimer()->getRealTime();
			newMarkBegin = 0;
			newMarkEnd = 0;
			textChanged = true;
		}
		break;

	case KEY_DELETE:
		if ( !this->IsEnabled )
			break;

		if (Text.size() != 0)
		{
			core::stringw s;

			if (MarkBegin != MarkEnd)
			{
				// delete marked text
				const s32 realmbgn = MarkBegin < MarkEnd ? MarkBegin : MarkEnd;
				const s32 realmend = MarkBegin < MarkEnd ? MarkEnd : MarkBegin;

				s = Text.subString(0, realmbgn);
				s.append( Text.subString(realmend, Text.size()-realmend) );
				Text = s;

				CursorPos = realmbgn;
			}
			else
			{
				// delete text before cursor
				s = Text.subString(0, CursorPos);
				s.append( Text.subString(CursorPos+1, Text.size()-CursorPos-1) );
				Text = s;
			}

			if (CursorPos > (s32)Text.size())
				CursorPos = (s32)Text.size();

			BlinkStartTime = IRRdevice->getTimer()->getRealTime();
			newMarkBegin = 0;
			newMarkEnd = 0;
			textChanged = true;
		}
		break;

	case KEY_TAB:
		Environment->setFocus(this);
        inputChar(L' ');
        //inputChar(L' ');
        break;
	case KEY_ESCAPE:
	case KEY_SHIFT:
	case KEY_F1:
	case KEY_F2:
	case KEY_F3:
	case KEY_F4:
	case KEY_F5:
	case KEY_F6:
	case KEY_F7:
	case KEY_F8:
	case KEY_F9:
	case KEY_F10:
	case KEY_F11:
	case KEY_F12:
	case KEY_F13:
	case KEY_F14:
	case KEY_F15:
	case KEY_F16:
	case KEY_F17:
	case KEY_F18:
	case KEY_F19:
	case KEY_F20:
	case KEY_F21:
	case KEY_F22:
	case KEY_F23:
	case KEY_F24:
		// ignore these keys
		return false;

	default:
        // ` ' and " must be replaced to ' (lua interpreter does not recognize wrong chars)
        if(event.KeyInput.Char == 96 || event.KeyInput.Char == 180 || event.KeyInput.Char == 168)
            inputChar(L'"');
        else
            inputChar(event.KeyInput.Char);
		printf("%s",(core::stringc)event.KeyInput.Char);
		return true;
	}

    // Set new text markers
    setTextMarkers( newMarkBegin, newMarkEnd );

	// break the text if it has changed
	if (textChanged)
	{
		breakText();
		sendGuiEvent(EGET_EDITBOX_CHANGED);
	}

	calculateScrollPos();

	return true;
}


//! draws the element and its children
void CGUIEditBoxIRB::draw()
{
	if (!IsVisible)
		return;

	const bool focus = Environment->hasFocus(this);

	IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;
	
	irr::core::rect<s32> myRect(s32 x, s32 y, s32 w, s32 h);

	FrameRect = AbsoluteRect;
	// draw the border

	if (Border)
	{
		skin->draw3DSunkenPane(this, skin->getColor(EGDC_WINDOW),
			false, true, myRect(FrameRect.UpperLeftCorner.X-1,FrameRect.UpperLeftCorner.Y-2,FrameRect.getWidth(),FrameRect.getHeight()), &AbsoluteClippingRect);

		FrameRect.UpperLeftCorner.X += skin->getSize(EGDS_TEXT_DISTANCE_X)+1;
		FrameRect.UpperLeftCorner.Y += skin->getSize(EGDS_TEXT_DISTANCE_Y)+1;
		FrameRect.LowerRightCorner.X -= skin->getSize(EGDS_TEXT_DISTANCE_X)+1;
		FrameRect.LowerRightCorner.Y -= skin->getSize(EGDS_TEXT_DISTANCE_Y)+1;
	
	}

	if (LineNumbering)
	{
		// Line count background
		skin->draw2DRectangle(this,video::SColor(255,220,220,220),myRect(FrameRect.UpperLeftCorner.X,FrameRect.UpperLeftCorner.Y,50,FrameRect.getHeight()-4),&AbsoluteClippingRect);
		skin->draw2DRectangle(this,video::SColor(255,192,192,192),myRect(FrameRect.UpperLeftCorner.X+45,FrameRect.UpperLeftCorner.Y,5,FrameRect.getHeight()-4),&AbsoluteClippingRect);
	}

	core::rect<s32> localClipRect = myRect(FrameRect.UpperLeftCorner.X+LeftSpace,FrameRect.UpperLeftCorner.Y,FrameRect.getWidth()-(LeftSpace/2),FrameRect.getHeight());
	
	localClipRect.clipAgainst(AbsoluteClippingRect);

	// draw the text

	IGUIFont* font = OverrideFont;
	if (!OverrideFont)
		font = skin->getFont();

	s32 cursorLine = 0;
	s32 charcursorpos = 0;

	if (font)
	{
		if (LastBreakFont != font)
		{
			breakText();
		}

		// calculate cursor pos

		core::stringw *txtLine = &Text;
		s32 startPos = 0;

		core::stringw s, s2;

		// get mark position
		const bool ml = (!PasswordBox && (WordWrap || MultiLine));
		const s32 realmbgn = MarkBegin < MarkEnd ? MarkBegin : MarkEnd;
		const s32 realmend = MarkBegin < MarkEnd ? MarkEnd : MarkBegin;
		const s32 hlineStart = ml ? getLineFromPos(realmbgn) : 0;
		const s32 hlineCount = ml ? getLineFromPos(realmend) - hlineStart + 1 : 1;
		//const s32 lineCount = ml ? BrokenText.size() : 1;
		lineCount = ml ? BrokenText.size() : 1;

		// Save the override color information.
		// Then, alter it if the edit box is disabled.
		const bool prevOver = OverrideColorEnabled;
		const video::SColor prevColor = OverrideColor;

		if (Text.size())
		{
			if (!IsEnabled && !OverrideColorEnabled)
			{
				OverrideColorEnabled = true;
				OverrideColor = skin->getColor(EGDC_GRAY_TEXT);
			}

			for (s32 i=0; i < lineCount; ++i)
			{
				setTextRect(i);

				// clipping test - don't draw anything outside the visible area
				core::rect<s32> c = localClipRect;
				c.clipAgainst(CurrentTextRect);
				if (!c.isValid())
					continue;

				// get current line
				if (PasswordBox)
				{
					if (BrokenText.size() != 1)
					{
						BrokenText.clear();
						BrokenText.push_back(core::stringw());
					}
					if (BrokenText[0].size() != Text.size())
					{
						BrokenText[0] = Text;
						for (u32 q = 0; q < Text.size(); ++q)
						{
							BrokenText[0] [q] = PasswordChar;
						}
					}
					txtLine = &BrokenText[0];
					startPos = 0;
				}
				else
				{
					txtLine = ml ? &BrokenText[i] : &Text;
					startPos = ml ? BrokenTextPositions[i] : 0;
				}


				// Parse the TAB character
				std::string txt_main = core::stringc(txtLine->c_str()).c_str();

				for(int chart = 0; chart < (int)txt_main.size(); chart++)
                {
					
                    if(txt_main[chart] == static_cast<char>( 9 ))
					{
						txt_main.erase(chart,1);
						txt_main.insert(chart," ");
					}
                }

				

                //separate and draw keywords in blue
                // sample:
                // txt         = if a>10 then print "hello" end
                // txtKeywords = if      then               end
                // just draw txtKeywords in blue over txt :)

                std::vector<std::string> tokens;
                //std::string txt = core::stringc(txtLine->c_str()).c_str();
				std::string txt = txt_main;
                std::string txtKeywods = "";

                tokens = split(txt,' ');

                for(int tokenCount = 0; tokenCount < (int)tokens.size(); tokenCount++)
                {
                    if(tokens[tokenCount] == "if" ||
                       tokens[tokenCount] == "and"||
                       tokens[tokenCount] == "break"||
                       tokens[tokenCount] == "do"||
                       tokens[tokenCount] == "else"||
                       tokens[tokenCount] == "elseif"||
                       tokens[tokenCount] == "end"||
                       tokens[tokenCount] == "for"||
                       tokens[tokenCount] == "function"||
                       tokens[tokenCount] == "if"||
                       tokens[tokenCount] == "local"||
                       tokens[tokenCount] == "nil"||
                       tokens[tokenCount] == "not"||
                       tokens[tokenCount] == "or"||
                       tokens[tokenCount] == "repeat"||
                       tokens[tokenCount] == "return"||
                       tokens[tokenCount] == "then"||
                       tokens[tokenCount] == "until"||
                       tokens[tokenCount] == "while")
                    {
                        txtKeywods.append(tokens[tokenCount]);
                    }
                    else
                    {
                        txtKeywods.append(std::string(tokens[tokenCount].size(),' '));
                    }
                }

                tokens.clear();

				// Add specific IRB token in another tint of blue
				
				std::string txt2 = txt_main;
				tokens = split(txt2,' ');

				std::string txtKeywodsIRB = "";

				for(int tokenCount = 0; tokenCount < (int)tokens.size(); tokenCount++)
                {
					std::string tokenIRB = tokens[tokenCount];
					int end = tokenIRB.find("(");
					std::string tIRB = tokenIRB.substr(0,end);
					int begin = tokenIRB.find(")..");
					tokenIRB = tIRB;
					if	(tokenIRB == "setObjectName" ||
						tokenIRB == "chaseObject" ||
						tokenIRB == "walkRandomly" ||
						tokenIRB == "walkToObject" ||
						tokenIRB == "CustomDynamicObjectUpdate" ||
						tokenIRB == "programAction" ||
						tokenIRB == "CustomDynamicObjectUpdateProgrammedAction" ||
						tokenIRB == "hasActionProgrammed" ||
						tokenIRB == "enableObject" ||
						tokenIRB == "disableObject" ||
						tokenIRB == "increasePlayerLife" ||
						tokenIRB == "decreasePlayerLife" ||
						tokenIRB == "decreasePLayerMoney" ||
						tokenIRB == "increasePlayerMoney" ||
						tokenIRB == "playSound" ||
						tokenIRB == "emitSound" ||
						tokenIRB == "sleep" ||
						tokenIRB == "setGlobal" ||
						tokenIRB == "getGlobal" ||
						tokenIRB == "setGlobal" ||
						tokenIRB == "deleteGlobal" ||
						tokenIRB == "setTimeOfDay" ||
						tokenIRB == "setAmbientLight" ||
						tokenIRB == "getAmbientColor" ||
						tokenIRB == "setFogColor" ||
						tokenIRB == "getFogColor" ||
						tokenIRB == "setFogRange" ||
						tokenIRB == "getFogRange" ||
						tokenIRB == "setCameraTarget" ||
						tokenIRB == "getCameraTarget" ||
						tokenIRB == "getObjectPosition" ||
						tokenIRB == "playSound2D" ||
						tokenIRB == "playSound3D" ||
						tokenIRB == "setSoundListenerPosition" ||
						tokenIRB == "setSoundVolume" ||
						tokenIRB == "stopSounds" ||
						tokenIRB == "setPlayerLife" ||
						tokenIRB == "getPlayerLife" ||
						tokenIRB == "setPlayerMoney" ||
						tokenIRB == "getPlayerMoney" ||
						tokenIRB == "addPlayerItem" ||
						tokenIRB == "removePlayerItem" ||
						tokenIRB == "usePlayerItem" ||
						tokenIRB == "getItemCount" ||
						tokenIRB == "showBlackScreen" ||
						tokenIRB == "hideBlackScreen" ||
						tokenIRB == "showDialogMessage" ||
						tokenIRB == "showDialogQuestion" ||
						tokenIRB == "saveGame" ||
						tokenIRB == "loadGame" ||
						tokenIRB == "showObjectLabel" ||
						tokenIRB == "hideObjectLabel" ||
						tokenIRB == "setObjectLabel" ||
						tokenIRB == "setPosition" ||
						tokenIRB == "getPosition" ||
						tokenIRB == "setRotation" ||
						tokenIRB == "getRotation" ||
						tokenIRB == "turn" ||
						tokenIRB == "move" ||
						tokenIRB == "walkTo" ||
						tokenIRB == "lookAt" ||
						tokenIRB == "lookToObject" ||
						tokenIRB == "getName" ||
						tokenIRB == "distanceFrom" ||
						tokenIRB == "setEnabled" ||
						tokenIRB == "setFrameLoop" ||
						tokenIRB == "setAnimationSpeed" ||
						tokenIRB == "setAnimation" ||
						tokenIRB == "setEnemy" ||
						tokenIRB == "setObject" ||
						tokenIRB == "setPropertie" ||
						tokenIRB == "getPropertie" ||
						tokenIRB == "attack" ||
						tokenIRB == "onLoad" ||
						tokenIRB == "onUpdate" ||
						tokenIRB == "step" ||
						tokenIRB == "onClicked" ||
						tokenIRB == "onCollision")
                    {
                        txtKeywodsIRB.append(tokens[tokenCount]);
                    }
                    else
                    {
                        txtKeywodsIRB.append(std::string(tokens[tokenCount].size(),' '));
                    }
                }
				tokens.clear();


				//re-insert original spaces
                for(int chart = 0; chart < (int)txt.size(); chart++)
                {
                    if(txt_main[chart] == ' ') txtKeywods.insert(chart," ");
                }

				//re-insert original spaces
                for(int chart = 0; chart < (int)txt.size(); chart++)
                {
                    if(txt_main[chart] == ' ') txtKeywodsIRB.insert(chart," ");
                }
				// Draw line numbering
				if (LineNumbering)
				{
					s32 right = FrameRect.UpperLeftCorner.X+20 - font->getDimension(linenumber.c_str()).Width;
					s32 base = 2+(font->getDimension(L"000000").Height * lineCount)/2;
					irr::core::rect<s32> LineRect = myRect(right,
						base+FrameRect.UpperLeftCorner.Y-VScrollPos,
						50,
						CurrentTextRect.getHeight());

					irr::core::rect<s32> LineClipRect = myRect(AbsoluteClippingRect.UpperLeftCorner.X,
						AbsoluteClippingRect.UpperLeftCorner.Y+2,50,AbsoluteClippingRect.getHeight()-10);
					font->draw(linenumber.c_str(), 					
						LineRect,
						video::SColor(255,0,128,192),
						false, true, &LineClipRect); //&AbsoluteClippingRect
					
				}
				// draw normal text
				//font->draw(core::stringw(txt_main.c_str()), CurrentTextRect,
				//	OverrideColorEnabled ? OverrideColor : skin->getColor(EGDC_BUTTON_TEXT),
                    //video::SColor(255,255,0,0),
				//	false, true, &localClipRect);
				font->draw(txtLine->c_str(), CurrentTextRect,
							OverrideColorEnabled ? OverrideColor : skin->getColor(EGDC_BUTTON_TEXT),
							false, true, &localClipRect);


                //draw keywords
                font->draw(txtKeywods.c_str(), CurrentTextRect,
					//OverrideColorEnabled ? OverrideColor : skin->getColor(EGDC_BUTTON_TEXT),
                    video::SColor(200,0,0,255),
					false, true, &localClipRect);

				//draw IRB keywords
                font->draw(txtKeywodsIRB.c_str(), CurrentTextRect,
					//OverrideColorEnabled ? OverrideColor : skin->getColor(EGDC_BUTTON_TEXT),
					video::SColor(255,128,0,255),
					false, true, &localClipRect);

				// draw mark and marked text
				if (focus && MarkBegin != MarkEnd && i >= hlineStart && i < hlineStart + hlineCount)
				{

					s32 mbegin = 0, mend = 0;
					s32 lineStartPos = 0, lineEndPos = txtLine->size();

					if (i == hlineStart)
					{
						// highlight start is on this line
						s = txtLine->subString(0, realmbgn - startPos);
						mbegin = font->getDimension(s.c_str()).Width;

						// deal with kerning
						mbegin += font->getKerningWidth(
							&((*txtLine)[realmbgn - startPos]),
							realmbgn - startPos > 0 ? &((*txtLine)[realmbgn - startPos - 1]) : 0);

						lineStartPos = realmbgn - startPos;
					}
					if (i == hlineStart + hlineCount - 1)
					{
						// highlight end is on this line
						s2 = txtLine->subString(0, realmend - startPos);
						mend = font->getDimension(s2.c_str()).Width;
						lineEndPos = (s32)s2.size();
					}
					else
						mend = font->getDimension(txtLine->c_str()).Width;

					CurrentTextRect.UpperLeftCorner.X += mbegin;
					CurrentTextRect.LowerRightCorner.X = CurrentTextRect.UpperLeftCorner.X + mend - mbegin;

					// draw mark
					skin->draw2DRectangle(this, video::SColor(255,100,100,255), CurrentTextRect, &localClipRect);

					// draw marked text
					s = txtLine->subString(lineStartPos, lineEndPos - lineStartPos);

					if (s.size())
						font->draw(s.c_str(), CurrentTextRect,
							OverrideColorEnabled ? OverrideColor : skin->getColor(EGDC_HIGH_LIGHT_TEXT),
							false, true, &localClipRect);

				}
			}

			// Return the override color information to its previous settings.
			OverrideColorEnabled = prevOver;
			OverrideColor = prevColor;
		}

		// draw cursor

		if (WordWrap || MultiLine)
		{
			cursorLine = getLineFromPos(CursorPos);
			txtLine = &BrokenText[cursorLine];
			startPos = BrokenTextPositions[cursorLine];
		}
		s = txtLine->subString(0,CursorPos-startPos);
		// Ugly fix to a bug when the mouse was going outside the limit of the text in selections.
		// Checking that the "txtline" size is bigger than 5 to redefine the position of the cursor
		// Was crashing for a position NOT allocated in the line.
		if (txtLine->size()>0)
			charcursorpos = font->getDimension(s.c_str()).Width +
				font->getKerningWidth(L"_", CursorPos-startPos > 0 ? &((*txtLine)[CursorPos-startPos-1]) : 0);

		//if (focus && (os::Timer::getTime() - BlinkStartTime) % 700 < 350)
		if (focus && (IRRdevice->getTimer()->getRealTime() - BlinkStartTime) % 700 < 350)
		{
			setTextRect(cursorLine);
			CurrentTextRect.UpperLeftCorner.X += charcursorpos;

			font->draw(L"_", CurrentTextRect,
				OverrideColorEnabled ? OverrideColor : skin->getColor(EGDC_BUTTON_TEXT),
				false, true, &localClipRect);
		}  // To be fixed 
	}

	// draw children
	IGUIElement::draw();
}


//! Sets the new caption of this element.
void CGUIEditBoxIRB::setText(const wchar_t* text)
{
	Text = text;
	if (u32(CursorPos) > Text.size())
		CursorPos = Text.size();
	HScrollPos = 0;
	breakText();
}


//! Enables or disables automatic scrolling with cursor position
//! \param enable: If set to true, the text will move around with the cursor position
void CGUIEditBoxIRB::setAutoScroll(bool enable)
{
	AutoScroll = enable;
}


//! Checks to see if automatic scrolling is enabled
//! \return true if automatic scrolling is enabled, false if not
bool CGUIEditBoxIRB::isAutoScrollEnabled() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return AutoScroll;
}


//! Gets the area of the text in the edit box
//! \return Returns the size in pixels of the text
core::dimension2du CGUIEditBoxIRB::getTextDimension()
{
	core::rect<s32> ret;

	setTextRect(0);
	ret = CurrentTextRect;

	for (u32 i=1; i < BrokenText.size(); ++i)
	{
		setTextRect(i);
		ret.addInternalPoint(CurrentTextRect.UpperLeftCorner);
		ret.addInternalPoint(CurrentTextRect.LowerRightCorner);
	}

	return core::dimension2du(ret.getSize());
}


//! Sets the maximum amount of characters which may be entered in the box.
//! \param max: Maximum amount of characters. If 0, the character amount is
//! infinity.
void CGUIEditBoxIRB::setMax(u32 max)
{
	Max = max;

	if (Text.size() > Max && Max != 0)
		Text = Text.subString(0, Max);
}


//! Returns maximum amount of characters, previously set by setMax();
u32 CGUIEditBoxIRB::getMax() const
{
	return Max;
}


bool CGUIEditBoxIRB::processMouse(const SEvent& event)
{
	s32 value = 0;
	switch(event.MouseInput.Event)
	{
	case irr::EMIE_MOUSE_WHEEL:
		value = (s32)-event.MouseInput.Wheel;
		
		if (MultiLine || (WordWrap && BrokenText.size() > 1) )
		{
			s32 lineNo = getLineFromPos(CursorPos);
			if (lineNo > 0)
			{
				s32 cp = CursorPos - BrokenTextPositions[lineNo];
				
				if ((lineNo+value)>=lineCount-1)
					value=0;
				
				if ((lineNo+value)<0)
					value=0;
				
				if (value!=0)
				{
					if ((s32)BrokenText[lineNo-1].size() < cp)
						CursorPos = BrokenTextPositions[lineNo+value] + (s32)BrokenText[lineNo+value].size()+value;
					else
						CursorPos = BrokenTextPositions[lineNo+value] + cp;
				}
			}
			if (value!=0)
				// Update the scrollbar
				Scrollbar->setPos(getLineFromPos(CursorPos));
				calculateScrollPos();
			return true;

		}
		

		break;
	case irr::EMIE_LMOUSE_LEFT_UP:
		if (Environment->hasFocus(this))
		{
			CursorPos = getCursorPos(event.MouseInput.X, event.MouseInput.Y);
			if (MouseMarking)
			{
			    setTextMarkers( MarkBegin, CursorPos );
			}
			MouseMarking = false;
			Scrollbar->setPos(getLineFromPos(CursorPos));
			//calculateScrollPos();
			return true;
		}
		break;
	case irr::EMIE_MOUSE_MOVED:
		{
			if (MouseMarking)
			{
				CursorPos = getCursorPos(event.MouseInput.X, event.MouseInput.Y);
				setTextMarkers( MarkBegin, CursorPos );
				//TODO: calculateScrollPos seem to have bugs
				//if (MarkBegin!=CursorPos)
				//	calculateScrollPos();
				return true;
			}
		}
		break;
	case EMIE_LMOUSE_PRESSED_DOWN:
		if (!Environment->hasFocus(this))
		{
			BlinkStartTime = IRRdevice->getTimer()->getRealTime();
			MouseMarking = true;
			CursorPos = getCursorPos(event.MouseInput.X, event.MouseInput.Y);
			setTextMarkers(CursorPos, CursorPos );
			calculateScrollPos();
			return true;
		}
		else
		{
			if (!AbsoluteClippingRect.isPointInside(
				core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y)))
			{
				return false;
			}
			else
			{
				// move cursor
				CursorPos = getCursorPos(event.MouseInput.X, event.MouseInput.Y);

                s32 newMarkBegin = MarkBegin;
				if (!MouseMarking)
					newMarkBegin = CursorPos;

				MouseMarking = true;
				setTextMarkers( newMarkBegin, CursorPos);
				//calculateScrollPos();
				return true;
			}
		}
	default:
		break;
	}

	return false;
}


s32 CGUIEditBoxIRB::getCursorPos(s32 x, s32 y)
{
	IGUIFont* font = OverrideFont;
	IGUISkin* skin = Environment->getSkin();
	if (!OverrideFont)
		font = skin->getFont();

	const u32 lineCount = (WordWrap || MultiLine) ? BrokenText.size() : 1;

	core::stringw *txtLine=0;
	s32 startPos=0;
	x+=3;

	for (u32 i=0; i < lineCount; ++i)
	{
		setTextRect(i);
		if (i == 0 && y < CurrentTextRect.UpperLeftCorner.Y)
			y = CurrentTextRect.UpperLeftCorner.Y;
		if (i == lineCount - 1 && y > CurrentTextRect.LowerRightCorner.Y )
			y = CurrentTextRect.LowerRightCorner.Y;

		// is it inside this region?
		if (y >= CurrentTextRect.UpperLeftCorner.Y && y <= CurrentTextRect.LowerRightCorner.Y)
		{
			// we've found the clicked line
			txtLine = (WordWrap || MultiLine) ? &BrokenText[i] : &Text;
			startPos = (WordWrap || MultiLine) ? BrokenTextPositions[i] : 0;
			break;
		}
	}

	if (x < CurrentTextRect.UpperLeftCorner.X)
		x = CurrentTextRect.UpperLeftCorner.X;

	//s32 idx = font->getCharacterFromPos(Text.c_str(), x - CurrentTextRect.UpperLeftCorner.X);
	s32 idx = txtLine ? font->getCharacterFromPos(txtLine->c_str(), x - CurrentTextRect.UpperLeftCorner.X) : -1;

	// click was on or left of the line
	if (idx != -1)
		return idx + startPos;

	// click was off the right edge of the line, go to end.
	if (txtLine)
		return txtLine->size() + startPos;
	else
		return startPos;
}


//! Breaks the single text line.
void CGUIEditBoxIRB::breakText()
{
	linenumber="";
	IGUISkin* skin = Environment->getSkin();

	if ((!WordWrap && !MultiLine) || !skin)
		return;

	BrokenText.clear(); // need to reallocate :/
	BrokenTextPositions.set_used(0);

	IGUIFont* font = OverrideFont;
	if (!OverrideFont)
		font = skin->getFont();

	if (!font)
		return;

	LastBreakFont = font;

	core::stringw line;
	core::stringw word;
	core::stringw whitespace;

	s32 lastLineStart = 0;
	s32 size = Text.size();
	s32 length = 0;
	s32 elWidth = RelativeRect.getWidth() - 6;
	wchar_t c;

	for (s32 i=0; i<size; ++i)
	{
		c = Text[i];
		bool lineBreak = false;

		if (c == L'\r') // Mac or Windows breaks
		{
			lineBreak = true;
			c = ' ';
			if (Text[i+1] == L'\n') // Windows breaks
			{
				Text.erase(i+1);
				--size;
			}
		}
		else if (c == L'\n') // Unix breaks
		{
			lineBreak = true;
			c = ' ';
		}

		// don't break if we're not a multi-line edit box
		if (!MultiLine)
			lineBreak = false;

		if (c == L' ' || c == 0 || i == (size-1))
		{
			if (word.size())
			{
				// here comes the next whitespace, look if
				// we can break the last word to the next line.
				s32 whitelgth = font->getDimension(whitespace.c_str()).Width;
				s32 worldlgth = font->getDimension(word.c_str()).Width;

				if (WordWrap && length + worldlgth + whitelgth > elWidth)
				{
					// break to next line
					length = worldlgth;
					BrokenText.push_back(line);
					BrokenTextPositions.push_back(lastLineStart);
					lastLineStart = i - (s32)word.size();
					line = word;
				}
				else
				{
					// add word to line
					line += whitespace;
					line += word;
					length += whitelgth + worldlgth;
				}

				word = L"";
				whitespace = L"";
			}

			whitespace += c;

			// compute line break
			if (lineBreak)
			{
				line += whitespace;
				line += word;
				
				BrokenText.push_back(line);
				linenumber += (core::stringw)BrokenText.size() + L"\n";
				BrokenTextPositions.push_back(lastLineStart);
				lastLineStart = i+1;
				line = L"";
				word = L"";
				whitespace = L"";
				length = 0;
			}
		}
		else
		{
			// yippee this is a word..
			word += c;
		}
	}
	
	line += whitespace;
	line += word;
	BrokenText.push_back(line);
	BrokenTextPositions.push_back(lastLineStart);
	Scrollbar->setMax(BrokenText.size()-1);
	linenumber += (core::stringw)BrokenText.size() + L"\n";
	//Linecounter->setText(linenumber.c_str());
	//Linecounter->move(core::vector2di(0,10));
	
}


void CGUIEditBoxIRB::setTextRect(s32 line)
{
	core::dimension2du d;

	IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;

	IGUIFont* font = OverrideFont ? OverrideFont : skin->getFont();

	if (!font)
		return;

	// get text dimension
	const u32 lineCount = (WordWrap || MultiLine) ? BrokenText.size() : 1;
	if (WordWrap || MultiLine)
	{
		d = font->getDimension(BrokenText[line].c_str());
	}
	else
	{
		d = font->getDimension(Text.c_str());
		d.Height = AbsoluteRect.getHeight();
	}
	d.Height += font->getKerningHeight();

	// justification
	switch (HAlign)
	{
	case EGUIA_CENTER:
		// align to h centre
		CurrentTextRect.UpperLeftCorner.X = (FrameRect.getWidth()/2) - (d.Width/2);
		CurrentTextRect.LowerRightCorner.X = (FrameRect.getWidth()/2) + (d.Width/2);
		break;
	case EGUIA_LOWERRIGHT:
		// align to right edge
		CurrentTextRect.UpperLeftCorner.X = FrameRect.getWidth() - d.Width;
		CurrentTextRect.LowerRightCorner.X = FrameRect.getWidth();
		break;
	default:
		// align to left edge
		CurrentTextRect.UpperLeftCorner.X = 0;
		CurrentTextRect.LowerRightCorner.X = d.Width;

	}

	switch (VAlign)
	{
	case EGUIA_CENTER:
		// align to v centre
		CurrentTextRect.UpperLeftCorner.Y =
			(FrameRect.getHeight()/2) - (lineCount*d.Height)/2 + d.Height*line;
		break;
	case EGUIA_LOWERRIGHT:
		// align to bottom edge
		CurrentTextRect.UpperLeftCorner.Y =
			FrameRect.getHeight() - lineCount*d.Height + d.Height*line;
		break;
	default:
		// align to top edge
		CurrentTextRect.UpperLeftCorner.Y = d.Height*line;
		break;
	}

	CurrentTextRect.UpperLeftCorner.X  -= HScrollPos;
	CurrentTextRect.LowerRightCorner.X -= HScrollPos;
	CurrentTextRect.UpperLeftCorner.Y  -= VScrollPos;
	CurrentTextRect.LowerRightCorner.Y = CurrentTextRect.UpperLeftCorner.Y + d.Height;

	CurrentTextRect += FrameRect.UpperLeftCorner;
	// Space
	CurrentTextRect.UpperLeftCorner.X+=LeftSpace;
	CurrentTextRect.UpperLeftCorner.Y+=UpperSpace;
	CurrentTextRect.LowerRightCorner.X-=RightSpace;
	CurrentTextRect.LowerRightCorner.Y-=LowerSpace;

}


s32 CGUIEditBoxIRB::getLineFromPos(s32 pos)
{
	if (!WordWrap && !MultiLine)
		return 0;

	s32 i=0;
	while (i < (s32)BrokenTextPositions.size())
	{
		if (BrokenTextPositions[i] > pos)
			return i-1;
		++i;
	}
	
	return (s32)BrokenTextPositions.size() - 1;
}


void CGUIEditBoxIRB::inputChar(wchar_t c)
{
	if (!IsEnabled)
		return;

	if (c != 0)
	{
		if (Text.size() < Max || Max == 0)
		{
			core::stringw s;

			if (MarkBegin != MarkEnd)
			{
				// replace marked text
				const s32 realmbgn = MarkBegin < MarkEnd ? MarkBegin : MarkEnd;
				const s32 realmend = MarkBegin < MarkEnd ? MarkEnd : MarkBegin;

				s = Text.subString(0, realmbgn);
				s.append(c);
				s.append( Text.subString(realmend, Text.size()-realmend) );
				Text = s;
				CursorPos = realmbgn+1;
			}
			else
			{
				// add new character
				s = Text.subString(0, CursorPos);
				s.append(c);
				s.append( Text.subString(CursorPos, Text.size()-CursorPos) );
				Text = s;
				++CursorPos;
			}

			BlinkStartTime = IRRdevice->getTimer()->getRealTime();
			setTextMarkers(0, 0);
		}
	}
	breakText();
	sendGuiEvent(EGET_EDITBOX_CHANGED);
	calculateScrollPos();
}


void CGUIEditBoxIRB::calculateScrollPos()
{
	if (!AutoScroll)
		return;

	// calculate horizontal scroll position
	s32 cursLine = getLineFromPos(CursorPos);
	setTextRect(cursLine);
	
	// don't do horizontal scrolling when wordwrap is enabled.
	if (!WordWrap)
	{
		// get cursor position
		IGUISkin* skin = Environment->getSkin();
		if (!skin)
			return;
		IGUIFont* font = OverrideFont ? OverrideFont : skin->getFont();
		if (!font)
			return;

		core::stringw *txtLine = MultiLine ? &BrokenText[cursLine] : &Text;
		s32 cPos = MultiLine ? CursorPos - BrokenTextPositions[cursLine] : CursorPos;

		s32 cStart = CurrentTextRect.UpperLeftCorner.X + HScrollPos +
			font->getDimension(txtLine->subString(0, cPos).c_str()).Width;

		s32 cEnd = cStart + font->getDimension(L"_ ").Width;

		if (FrameRect.LowerRightCorner.X < cEnd)
			HScrollPos = cEnd - FrameRect.LowerRightCorner.X;
		else if (FrameRect.UpperLeftCorner.X > cStart)
			HScrollPos = cStart - FrameRect.UpperLeftCorner.X;
		else
			HScrollPos = 0;

		// todo: adjust scrollbar
	}

	// vertical scroll position
	if (FrameRect.LowerRightCorner.Y < CurrentTextRect.LowerRightCorner.Y + VScrollPos)
		VScrollPos = CurrentTextRect.LowerRightCorner.Y - FrameRect.LowerRightCorner.Y + VScrollPos;

	else if (FrameRect.UpperLeftCorner.Y > CurrentTextRect.UpperLeftCorner.Y + VScrollPos)
		VScrollPos = CurrentTextRect.UpperLeftCorner.Y - FrameRect.UpperLeftCorner.Y + VScrollPos;
	else
		VScrollPos = 0;

	// todo: adjust scrollbar
	//Scrollbar->setMax(BrokenText.size());
}

//! set text markers
void CGUIEditBoxIRB::setTextMarkers(s32 begin, s32 end)
{
    if ( begin != MarkBegin || end != MarkEnd )
    {
        MarkBegin = begin;
        MarkEnd = end;
        sendGuiEvent(EGET_EDITBOX_MARKING_CHANGED);
    }
}

//! send some gui event to parent
void CGUIEditBoxIRB::sendGuiEvent(EGUI_EVENT_TYPE type)
{
	if ( Parent )
	{
        SEvent e;
        e.EventType = EET_GUI_EVENT;
        e.GUIEvent.Caller = this;
        e.GUIEvent.Element = 0;
        e.GUIEvent.EventType = type;

        Parent->OnEvent(e);
	}
}

//! Writes attributes of the element.
void CGUIEditBoxIRB::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
{
	// IGUIEditBox::serializeAttributes(out,options);

	out->addBool  ("OverrideColorEnabled",OverrideColorEnabled );
	out->addColor ("OverrideColor",       OverrideColor);
	// out->addFont("OverrideFont",OverrideFont);
	out->addInt   ("MaxChars",            Max);
	out->addBool  ("WordWrap",            WordWrap);
	out->addBool  ("MultiLine",           MultiLine);
	out->addBool  ("AutoScroll",          AutoScroll);
	out->addBool  ("PasswordBox",         PasswordBox);
	core::stringw ch = L" ";
	ch[0] = PasswordChar;
	out->addString("PasswordChar",        ch.c_str());
	out->addEnum  ("HTextAlign",          HAlign, GUIAlignmentNames);
	out->addEnum  ("VTextAlign",          VAlign, GUIAlignmentNames);

	IGUIEditBox::serializeAttributes(out,options);
}


//! Reads attributes of the element
void CGUIEditBoxIRB::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
{
	IGUIEditBox::deserializeAttributes(in,options);

	setOverrideColor(in->getAttributeAsColor("OverrideColor"));
	enableOverrideColor(in->getAttributeAsBool("OverrideColorEnabled"));
	setMax(in->getAttributeAsInt("MaxChars"));
	setWordWrap(in->getAttributeAsBool("WordWrap"));
	setMultiLine(in->getAttributeAsBool("MultiLine"));
	setAutoScroll(in->getAttributeAsBool("AutoScroll"));
	core::stringw ch = in->getAttributeAsStringW("PasswordChar");

	if (!ch.size())
		setPasswordBox(in->getAttributeAsBool("PasswordBox"));
	else
		setPasswordBox(in->getAttributeAsBool("PasswordBox"), ch[0]);

	setTextAlignment( (EGUI_ALIGNMENT) in->getAttributeAsEnumeration("HTextAlign", GUIAlignmentNames),
			(EGUI_ALIGNMENT) in->getAttributeAsEnumeration("VTextAlign", GUIAlignmentNames));

	// setOverrideFont(in->getAttributeAsFont("OverrideFont"));
}

} // end namespace gui
} // end namespace irr


