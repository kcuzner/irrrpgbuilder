#include "CGUIFileSelector.h"

const s32 FOD_WIDTH = 640;
const s32 FOD_HEIGHT = 400;

s32 CGUIFileSelector::numFileSelectors = 0;


//! constructor
CGUIFileSelector::CGUIFileSelector(const wchar_t* title, IGUIEnvironment* environment, IGUIElement* parent, s32 id, E_FILESELECTOR_TYPE type)
: IGUIElement(EGUIET_FILE_OPEN_DIALOG, environment, parent, id, 
			  core::rect<s32>((parent->getAbsolutePosition().getWidth()-FOD_WIDTH)/2,
               (parent->getAbsolutePosition().getHeight()-FOD_HEIGHT)/2,   
              (parent->getAbsolutePosition().getWidth()-FOD_WIDTH)/2+FOD_WIDTH,
               (parent->getAbsolutePosition().getHeight()-FOD_HEIGHT)/2+FOD_HEIGHT)),
			  
			   Dragging(false), FileNameText(0), FileList(0), DialogType(type) {


//IGUIFileOpenDialog(environment, parent, id,
// core::rect<s32>((parent->getAbsolutePosition().getWidth()-FOD_WIDTH)/2,
//               (parent->getAbsolutePosition().getHeight()-FOD_HEIGHT)/2,   
//               (parent->getAbsolutePosition().getWidth()-FOD_WIDTH)/2+FOD_WIDTH,
//               (parent->getAbsolutePosition().getHeight()-FOD_HEIGHT)/2+FOD_HEIGHT)),   
//  Dragging(false), FileNameText(0), FileList(0), DialogType(type) {   
    #ifdef _DEBUG
     IGUIElement::setDebugName("CGUIFileSelector");
   #endif    
    
    Text = title;
    IsDirectoryChoosable = false;

	//simple flag to know when the requester has finished
	usecomplete = false;

   IGUISkin* skin = Environment->getSkin();
   IGUISpriteBank* sprites = 0;
   video::SColor color(255,255,255,255);
   if (skin) {
      sprites = skin->getSpriteBank();
      color = skin->getColor(EGDC_WINDOW_SYMBOL);
   }

   s32 buttonw = Environment->getSkin()->getSize(EGDS_WINDOW_BUTTON_WIDTH);
   s32 posx = RelativeRect.getWidth() - buttonw - 4;

   // ---------------------------------------------------------------

   OKButton = Environment->addButton(
   // core::rect<s32>(RelativeRect.getWidth()-80, 30, RelativeRect.getWidth()-10, 50), 
      core::rect<s32>(RelativeRect.getWidth()-160, RelativeRect.getHeight()-30, RelativeRect.getWidth()-90, RelativeRect.getHeight()-10), 
      this, -1, (DialogType==EFST_OPEN_DIALOG?L"Open":L"Save"));
   OKButton->setSubElement(true);
   OKButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);
   OKButton->grab();

   CancelButton = Environment->addButton(
      core::rect<s32>(RelativeRect.getWidth()-80, RelativeRect.getHeight()-30, RelativeRect.getWidth()-10, RelativeRect.getHeight()-10), 
      this, -1, skin ? skin->getDefaultText(EGDT_MSG_BOX_CANCEL) : L"Cancel");
   CancelButton->setSubElement(true);
   CancelButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
   CancelButton->grab();

   // Treebox is experimental... Will be used for the second type of file requester (2 variants will be implemented)
   TreeBox = Environment->addTreeView(irr::core::rect<s32>(10,10,170,200),0,-1,true, true, false);
   TreeBox->setSubElement(true);
   TreeBox->setLinesVisible(false);
   irr::gui::IGUITreeViewNode * fv=TreeBox->getRoot()->addChildBack(L"Favorites");
   irr::gui::IGUITreeViewNode * ld=TreeBox->getRoot()->addChildBack(L"Local drives");
   fv->setExpanded(true);
   ld->setExpanded(true);
   fv->addChildBack(L"Desktop");
   fv->addChildBack(L"My documents");
   fv->addChildBack(L"My pictures");
   fv->addChildBack(L"My music");
   ld->addChildBack(L"C:\\");
   ld->addChildBack(L"D:\\");
   ld->addChildBack(L"E:\\");

   //FileBox = Environment->addListBox(core::rect<s32>(10, 80, RelativeRect.getWidth()-90, 230), this, -1, true);
   FileBox = Environment->addListBox(core::rect<s32>(180, 50, RelativeRect.getWidth()-10, RelativeRect.getHeight()-60), this, -1, true);
   FileBox->setSubElement(true);
   FileBox->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
   FileBox->grab();

   //irr::gui::IGUIStaticText * but1 = Environment->addStaticText(L"Favorites",irr::core::rect<s32>(20,35,170,48),false,false,0,-1);
   irr::gui::IGUIStaticText * but2 = Environment->addStaticText(L"Files",irr::core::rect<s32>(190,35,300,48),false,false,0,-1);


   // Contain the names of the favorites folders
   PlacesBox = Environment->addListBox(core::rect<s32>(10, 210, 170, RelativeRect.getHeight()-60), this, -1, true);
   PlacesBox->setSubElement(true);
   PlacesBox->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
   PlacesBox->grab();

   // Contain the real location of those favorites folder (lazy programming :) )
   PlacesBoxReal = Environment->addListBox(core::rect<s32>(10, 50, 170, RelativeRect.getHeight()-60), this, -1, true);
   PlacesBoxReal->setSubElement(true);
   PlacesBoxReal->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
   PlacesBoxReal->grab();
   PlacesBoxReal->setEnabled(false);
   PlacesBoxReal->setVisible(false);

   //DriveBox = Environment->addComboBox(core::rect<s32>(10, 55, RelativeRect.getWidth()-90, 75), this, -1);
   DriveBox = Environment->addComboBox(core::rect<s32>(10, RelativeRect.getHeight()-55, 170, RelativeRect.getHeight()-35), this, -1);
   DriveBox->setSubElement(true);
   DriveBox->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
   DriveBox->grab();   

   //FileNameText = Environment->addEditBox(0, core::rect<s32>(10, 30, RelativeRect.getWidth()-90, 50), true, this, -1);
   FileNameText = Environment->addEditBox(0, core::rect<s32>(10, RelativeRect.getHeight()-30, RelativeRect.getWidth()-180, RelativeRect.getHeight()-10), true, this, -1);
   FileNameText->setSubElement(true);
   FileNameText->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
   FileNameText->setTextAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
   FileNameText->grab();
   
	//FileNameText = Environment->addEditBox(0, core::rect<s32>(10, 30, RelativeRect.getWidth()-90, 50), true, this, -1);
   PathNameText = Environment->addEditBox(0, core::rect<s32>(180, 10, RelativeRect.getWidth()-10, 30), true, this, -1);
   PathNameText->setSubElement(true);
   PathNameText->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
   PathNameText->setTextAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
   PathNameText->grab();

   FilterComboBox = Environment->addComboBox(core::rect<s32>(RelativeRect.getWidth()-160, RelativeRect.getHeight()-55, RelativeRect.getWidth()-10, RelativeRect.getHeight()-35), this, -1);
   FilterComboBox->setSubElement(true);
   FilterComboBox->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);
   FilterComboBox->grab();
    FilterComboBox->addItem(L"All Files");
   

   core::stringc str = "FileSelectorIcons";
    str += numFileSelectors++;
    SpriteBank = Environment->addEmptySpriteBank(str.c_str());
    if (SpriteBank) {
      SpriteBank->grab();
      FileBox->setSpriteBank(SpriteBank);
	  PlacesBox->setSpriteBank(SpriteBank);
    }
    DirectoryIconIdx = -1;
    FileIconIdx = -1;
   
   FileSystem = Environment->getFileSystem();
   
   if (FileSystem) 
   {
      FileSystem->grab();
       prev_working_dir = FileSystem->getWorkingDirectory();
      //printf("working directory saved: %s\n", prev_working_dir.c_str());

   }
   FileSystem->changeWorkingDirectoryTo("../projects");
   fillListBox();

// This will fill the drive list (still buggy) 
#ifdef WIN32
   
	int dr_type=99;
	wchar_t dr_avail[1024];
	wchar_t *temp=dr_avail;	
	/* 1st we fill the buffer */
	GetLogicalDriveStrings(32,dr_avail);
	while(*temp!=NULL) { // Split the buffer by null
		dr_type=GetDriveType((LPWSTR)temp);
		switch(dr_type) {
			case 0: // Unknown
			printf("%s : Unknown Drive type\n",temp);
			break;

			case 1: // Invalid
			printf("%s : Drive is invalid\n",temp);
			break;

			case 2: // Removable Drive
			printf("%s : Removable Drive\n",temp);
			DriveBox->addItem(temp);
			break;

			case 3: // Fixed
			printf("%s : Hard Disk (Fixed)\n",temp);
			DriveBox->addItem(temp);
			break;

			case 4: // Remote
			printf("%s : Remote (Network) Drive\n",temp);
			DriveBox->addItem(temp);
			break;

			case 5: // CDROM
			printf("%s : CD-Rom/DVD-Rom\n",temp);
			DriveBox->addItem(temp);
			break;

			case 6: // RamDrive
			printf("%s : Ram Drive\n",temp);
			DriveBox->addItem(temp);
			break;

		}
		temp += lstrlen((LPCWSTR)temp) +1; // incriment the buffer
	}

	// Get the desktop shortcut (places)
   // String buffer for holding the path.
TCHAR strPath[ MAX_PATH ];

// Get the special folder path. (Desktop)
SHGetSpecialFolderPath(
    0,       // Hwnd
    strPath, // String buffer.
    CSIDL_DESKTOPDIRECTORY, // CSLID of folder
    FALSE ); // Create if doesn't exists?
PlacesBox->addItem(L"Desktop",this->addIcon(Environment->getVideoDriver()->getTexture("../media/art/places_desktop.png")));
PlacesBoxReal->addItem(((irr::core::stringw)strPath).c_str());

// Get the special folder path. (My documents)
SHGetSpecialFolderPath(
    0,       // Hwnd
    strPath, // String buffer.
    CSIDL_PERSONAL, // CSLID of folder
    FALSE ); // Create if doesn't exists?
PlacesBox->addItem(L"My documents",this->addIcon(Environment->getVideoDriver()->getTexture("../media/art/places_documents.png")));
PlacesBoxReal->addItem(((irr::core::stringw)strPath).c_str());

// Get the special folder path. (My pictures)
SHGetSpecialFolderPath(
    0,       // Hwnd
    strPath, // String buffer.
    CSIDL_MYPICTURES, // CSLID of folder
    FALSE ); // Create if doesn't exists?
PlacesBox->addItem(L"My pictures",this->addIcon(Environment->getVideoDriver()->getTexture("../media/art/places_pictures.png")));
PlacesBoxReal->addItem(((irr::core::stringw)strPath).c_str());

// Get the special folder path. (My music)
SHGetSpecialFolderPath(
    0,       // Hwnd
    strPath, // String buffer.
    CSIDL_MYMUSIC, // CSLID of folder
    FALSE ); // Create if doesn't exists?
PlacesBox->addItem(L"My music",this->addIcon(Environment->getVideoDriver()->getTexture("../media/art/places_folder.png")));
PlacesBoxReal->addItem(((irr::core::stringw)strPath).c_str());

// Get the special folder path. (My video)
SHGetSpecialFolderPath(
    0,       // Hwnd
    strPath, // String buffer.
    CSIDL_MYVIDEO, // CSLID of folder
    FALSE ); // Create if doesn't exists?
PlacesBox->addItem(L"My videos",this->addIcon(Environment->getVideoDriver()->getTexture("../media/art/places_folder.png")));
PlacesBoxReal->addItem(((irr::core::stringw)strPath).c_str());

// Get the special folder path. (Public documents)
SHGetSpecialFolderPath(
    0,       // Hwnd
    strPath, // String buffer.
    CSIDL_COMMON_DOCUMENTS, // CSLID of folder
    FALSE ); // Create if doesn't exists?
PlacesBox->addItem(L"Public documents",this->addIcon(Environment->getVideoDriver()->getTexture("../media/art/places_folder.png")));
PlacesBoxReal->addItem(((irr::core::stringw)strPath).c_str());

// Get the special folder path. (Public pictures)
SHGetSpecialFolderPath(
    0,       // Hwnd
    strPath, // String buffer.
    CSIDL_COMMON_PICTURES, // CSLID of folder
    FALSE ); // Create if doesn't exists?
PlacesBox->addItem(L"Public pictures",this->addIcon(Environment->getVideoDriver()->getTexture("../media/art/places_folder.png")));
PlacesBoxReal->addItem(((irr::core::stringw)strPath).c_str());

// Get the special folder path. (Public music)
SHGetSpecialFolderPath(
    0,       // Hwnd
    strPath, // String buffer.
    CSIDL_COMMON_MUSIC, // CSLID of folder
    FALSE ); // Create if doesn't exists?
PlacesBox->addItem(L"Public music",this->addIcon(Environment->getVideoDriver()->getTexture("../media/art/places_folder.png")));
PlacesBoxReal->addItem(((irr::core::stringw)strPath).c_str());


// Get the special folder path. (Public videos)
SHGetSpecialFolderPath(
    0,       // Hwnd
    strPath, // String buffer.
    CSIDL_COMMON_VIDEO, // CSLID of folder
    FALSE ); // Create if doesn't exists?
PlacesBox->addItem(L"Public videos",this->addIcon(Environment->getVideoDriver()->getTexture("../media/art/places_folder.png")));
PlacesBoxReal->addItem(((irr::core::stringw)strPath).c_str());






#endif

}



//! destructor
CGUIFileSelector::~CGUIFileSelector() {
//   if (CloseButton)
//      CloseButton->drop();

   if (OKButton)
      OKButton->drop();

   if (CancelButton)
      CancelButton->drop();

   if (FileBox)
      FileBox->drop();

   if (FileNameText)
      FileNameText->drop();

   if (FileSystem)
      FileSystem->drop();

   if (FileList)
      FileList->drop();
      
   if (FilterComboBox)
       FilterComboBox->drop();
       
    if (SpriteBank)
        SpriteBank->drop();
       
}


//! returns the filename of the selected file. Returns NULL, if no file was selected.
const wchar_t* CGUIFileSelector::getFileName() const {
	irr::core::stringc text="";
	//text=(irr::core::stringc)FileNameText->getText();
	return fullpathname.c_str();
   //
}

//! Returns the directory of the selected file. Returns NULL, if no directory was selected.
const io::path& CGUIFileSelector::getDirectoryName()
{
	FileSystem->flattenFilename ( FileDirectory );
	return FileDirectory;
}

//! called if an event happened.
bool CGUIFileSelector::OnEvent(const SEvent& event) {
   switch(event.EventType) {
    case EET_KEY_INPUT_EVENT:
         switch (event.KeyInput.Key) {
           case KEY_RETURN:
             if (FileSystem) {
            FileSystem->changeWorkingDirectoryTo(core::stringc(FileNameText->getText()).c_str());
            fillListBox();
            PathNameText->setText(core::stringw(FileSystem->getWorkingDirectory()).c_str());
            }
             return true;
         }
         break;
   case EET_GUI_EVENT:
      switch(event.GUIEvent.EventType) {
        case EGET_COMBO_BOX_CHANGED:
			
         if (event.GUIEvent.Caller == FilterComboBox)
		 {
            fillListBox();
         } 
		 if (event.GUIEvent.Caller == DriveBox)
		 {  // change drive
            if (FileSystem) {    
				printf("Combo box changed!\n");
               FileSystem->changeWorkingDirectoryTo(core::stringc(DriveBox->getText()).c_str());
               fillListBox();
            } else 
			{ 
				printf("The file system is not readable at the moment...\n");
			}
         }
         break;
      case EGET_ELEMENT_FOCUS_LOST:
         Dragging = false;
         break;
      case EGET_BUTTON_CLICKED:
         if (event.GUIEvent.Caller == CloseButton ||
            event.GUIEvent.Caller == CancelButton) {
                if (FileSystem) {
              FileSystem->changeWorkingDirectoryTo(prev_working_dir.c_str());
              //printf("working directory reset to: %s\n", prev_working_dir.c_str());
            }
            sendCancelEvent();
            remove();
            return true;
         }
         else
         if (event.GUIEvent.Caller == OKButton && (IsDirectoryChoosable || matchesFileFilter(FileNameText->getText()))) {
            if (FileSystem) {
              FileSystem->changeWorkingDirectoryTo(prev_working_dir.c_str());
              //printf("working directory reset to: %s\n", prev_working_dir.c_str());
            }
            sendSelectedEvent();
            remove();
            return true;
         }
         break;

      case EGET_LISTBOX_CHANGED:
         {
			if (event.GUIEvent.Caller == FileBox)
			{
				s32 selected = FileBox->getSelected();
				if (FileList && FileSystem)
				{
					this->fullpathname=FileSystem->getWorkingDirectory();
					core::stringw strw;
					strw = FileSystem->getWorkingDirectory();
					if (strw[strw.size()-1] != '/')
						strw += "/";
					
					fullpathname = strw+FileBox->getListItem(selected);
					FileNameText->setText(FileBox->getListItem(selected));
					PathNameText->setText(strw.c_str());
				}
            }
			if (event.GUIEvent.Caller == PlacesBox)
			{
				s32 selected = PlacesBox->getSelected();
				FileSystem->changeWorkingDirectoryTo(PlacesBoxReal->getListItem(selected));
				fillListBox();
				printf("The placebox item has been pressed!\n");
			}

         }
         break;
      case EGET_LISTBOX_SELECTED_AGAIN: 
         {   
			if (event.GUIEvent.Caller == FileBox)
			{
				s32 selected = FileBox->getSelected();
				if (FileList && FileSystem) 
				{
					if (FileList->isDirectory(selected)) 
					{
						FileSystem->changeWorkingDirectoryTo(FileList->getFileName(selected));
						PathNameText->setText(core::stringw(FileSystem->getWorkingDirectory()).c_str());
						fillListBox();
						FileNameText->setText(L"");
					}
					else
					{
						this->fullpathname=FileSystem->getWorkingDirectory();
						core::stringw strw;
						strw = FileSystem->getWorkingDirectory();
						if (strw[strw.size()-1] != '/')
							strw += "/";
						
						fullpathname = strw+FileBox->getListItem(selected);
						FileNameText->setText(FileBox->getListItem(selected));
						PathNameText->setText(strw.c_str());
						return true;
					}
				}
			}
			if (event.GUIEvent.Caller == PlacesBox)
			{
				s32 selected = PlacesBox->getSelected();
				FileSystem->changeWorkingDirectoryTo(PlacesBoxReal->getListItem(selected));
				fillListBox();
				printf("The placebox item has been pressed!\n");
			}
		 }
         break;
      }
      break;
   case EET_MOUSE_INPUT_EVENT:
      switch(event.MouseInput.Event) {
      case EMIE_LMOUSE_PRESSED_DOWN:
         DragStart.X = event.MouseInput.X;
         DragStart.Y = event.MouseInput.Y;
         Dragging = true;
         Environment->setFocus(this);
         return true;
      case EMIE_LMOUSE_LEFT_UP:
         Dragging = false;
         Environment->removeFocus(this);
         return true;
      case EMIE_MOUSE_MOVED:
		 // Disable the drag function
		 Dragging=false;
         if (Dragging) {
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
void CGUIFileSelector::draw() {
   if (!IsVisible)
      return;

   IGUISkin* skin = Environment->getSkin();

   AbsoluteRect.LowerRightCorner.Y=Environment->getVideoDriver()->getScreenSize().Height;
   AbsoluteRect.LowerRightCorner.X=Environment->getVideoDriver()->getScreenSize().Width;
   AbsoluteClippingRect = AbsoluteRect;
   core::rect<s32> rect = AbsoluteRect;

   rect = skin->draw3DWindowBackground(this, false, skin->getColor(EGDC_ACTIVE_BORDER), 
      rect, &AbsoluteClippingRect);

   this->recalculateAbsolutePosition(true);
   this->AbsoluteRect=AbsoluteRect;

  /* if (Text.size()) {
      rect.UpperLeftCorner.X += 2;
      rect.LowerRightCorner.X -= skin->getSize(EGDS_WINDOW_BUTTON_WIDTH) + 5;

      IGUIFont* font = skin->getFont(EGDF_WINDOW);
      if (font)
         font->draw(Text.c_str(), rect, skin->getColor(EGDC_ACTIVE_CAPTION), false, true, 
         &AbsoluteClippingRect);
   }*/

   IGUIElement::draw();
}

//bool CGUIFileSelector::matchesFileFilter(core::stringw s) {
//  if (FilterComboBox->getSelected() >= FileFilters.size()) return true; // 'All Files' selectable
//  else {
//    s32 pos = s.findLast('.'); // Find the last '.' so we can check the file extension
//    return FileFilters[FilterComboBox->getSelected()].FileExtension.equals_ignore_case(core::stringw(&s.c_str()[pos+1])); 
//  }
//}

bool CGUIFileSelector::matchesFileFilter(core::stringw s) { 
   if (FileFilters.size() > 1) {
      s32 selected = FilterComboBox->getSelected();
      if (selected == 0) {
         for (s32 i = 0; i < (s32)FileFilters.size(); i++) {
            s32 pos = s.findLast('.'); // Find the last '.' so we can check the file extension
            if (FileFilters[i].FileExtension.equals_ignore_case(core::stringw(&s.c_str()[pos+1])))
               return true;
         }
         return false;
      }
      selected--;
      if (selected >= (s32)FileFilters.size()) return true; // 'All Files' selectable
      else {
         s32 pos = s.findLast('.'); // Find the last '.' so we can check the file extension
         return FileFilters[selected].FileExtension.equals_ignore_case(core::stringw(&s.c_str()[pos+1]));
      }
   }
   if (FilterComboBox->getSelected() >= (s32)FileFilters.size()) return true; // 'All Files' selectable
   else {
      s32 pos = s.findLast('.'); // Find the last '.' so we can check the file extension
      return FileFilters[FilterComboBox->getSelected()].FileExtension.equals_ignore_case(core::stringw(&s.c_str()[pos+1]));
   }
} 

bool CGUIFileSelector::matchesFileFilter(core::stringw s, core::stringw f) {
  s32 pos = s.findLast('.'); // Find the last '.' so we can check the file extension
  return f.equals_ignore_case(core::stringw(&s.c_str()[pos+1])); 
}

//! fills the listbox with files.
void CGUIFileSelector::fillListBox() {
   IGUISkin *skin = Environment->getSkin();

   if (!FileSystem || !FileBox || !skin)
      return;

   if (FileList)
      FileList->drop();

   FileBox->clear();

   FileList = FileSystem->createFileList();
   core::stringw s;

   for (u32 i=0; i<FileList->getFileCount(); ++i) {
      s = FileList->getFileName(i);
      // We just want a list of directories and those matching the file filter
      if (FileList->isDirectory(i))
          if (DirectoryIconIdx != -1) FileBox->addItem(s.c_str(), DirectoryIconIdx);
          else                        FileBox->addItem(s.c_str());
      else if (matchesFileFilter(s))
        if (FilterComboBox->getSelected() >= (s32)FileFilters.size())
            if (FileIconIdx != -1) {
              s32 iconIdx = FileIconIdx;
              for (u32 i = 0 ; i < FileFilters.size() ; i++) 
                if (matchesFileFilter(s, FileFilters[i].FileExtension))
                  iconIdx = FileFilters[i].FileIconIdx;
              FileBox->addItem(s.c_str(), iconIdx);
            } else  FileBox->addItem(s.c_str());
        else FileBox->addItem(s.c_str(), FileFilters[FilterComboBox->getSelected()].FileIconIdx);        

   }

   if (PathNameText) {
      s = FileSystem->getWorkingDirectory();
      PathNameText->setText(s.c_str());
   }
}


//! sends the event that the file has been selected.
void CGUIFileSelector::sendSelectedEvent() {
   SEvent event;
   event.EventType = EET_GUI_EVENT;
   event.GUIEvent.Caller = this;
   event.GUIEvent.EventType = EGET_FILE_SELECTED;
   Parent->OnEvent(event);
   usecomplete=true;
}

//! sends the event that the file choose process has been canceld
void CGUIFileSelector::sendCancelEvent() {
   SEvent event;
   event.EventType = EET_GUI_EVENT;
   event.GUIEvent.Caller = this;
   event.GUIEvent.EventType = EGET_FILE_CHOOSE_DIALOG_CANCELLED;
   Parent->OnEvent(event);
   usecomplete=true;
}

//void CGUIFileSelector::addFileFilter(wchar_t* name, wchar_t* ext, video::ITexture* texture) {
//  SFileFilter filter(name, ext, texture);
//  
//  filter.FileIconIdx = addIcon(texture);
//  
//  FileFilters.push_back(filter);  
//  
//  FilterComboBox->clear();
//  core::stringw strw;
//  for (u32 i = 0 ; i < FileFilters.size() ; i++) {
//    strw = FileFilters[i].FilterName;
//    strw += " (*.";
//    strw += FileFilters[i].FileExtension;
//    strw += ")";
//    FilterComboBox->addItem(strw.c_str());
//  }
//  FilterComboBox->addItem(L"All Files");
//  
//  fillListBox();
//}
void CGUIFileSelector::addFileFilter(wchar_t* name, wchar_t* ext, video::ITexture* texture) {
  SFileFilter filter(name, ext, texture);
 
  filter.FileIconIdx = addIcon(texture);
 
  FileFilters.push_back(filter);
 
  FilterComboBox->clear();
  core::stringw strw;

  if (FileFilters.size() > 1) {
     strw = "Supported ";
     for (u32 i = 0 ; i < FileFilters.size() ; i++) {
      strw += ".";
      strw += FileFilters[i].FileExtension;
      strw += " ";
     }
     FilterComboBox->addItem(strw.c_str());
  }
 
  for (u32 i = 0 ; i < FileFilters.size() ; i++) {
    strw = FileFilters[i].FilterName;
    strw += " (*.";
    strw += FileFilters[i].FileExtension;
    strw += ")";
    FilterComboBox->addItem(strw.c_str());
  }
  FilterComboBox->addItem(L"All Files");
 
  fillListBox();
} 

u32 CGUIFileSelector::addIcon(video::ITexture* texture) {
   if (!SpriteBank || !texture) return 0;
    
   // load and add the texture to the bank     
   SpriteBank->addTexture(texture); 
   u32 textureIndex = SpriteBank->getTextureCount() - 1; 
   // now lets get the sprite bank's rectangles and add some for our animation 
   core::array<core::rect<s32> >& rectangles = SpriteBank->getPositions(); 
   u32 firstRect = rectangles.size(); 
   // remember that rectangles are not in pixels, they enclose pixels! 
   // to draw a rectangle around the pixel at 0,0, it would rect<s32>(0,0, 1,1) 
   rectangles.push_back(core::rect<s32>(0,0, 16,16)); 


   // now we make a sprite.. 
   SGUISprite sprite; 
   sprite.frameTime = 30; 
   // add some frames of animation. 
   SGUISpriteFrame frame; 
   frame.rectNumber = firstRect; 
   frame.textureNumber = textureIndex; 

   // add this frame 
   sprite.Frames.push_back(frame); 
   // add the sprite 
   u32 spriteIndex = SpriteBank->getSprites().size(); 
   SpriteBank->getSprites().push_back(sprite);  

   return textureIndex;
}
