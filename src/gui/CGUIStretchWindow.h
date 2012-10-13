#ifndef INC_CGUISTRETCHWINDOW_H
#define INC_CGUISTRETCHWINDOW_H

#include <irrlicht.h>

using namespace irr;
using namespace gui;

/** Class for opening/saving files. */
class CGUIStretchWindow : public IGUIElement {
      
   public:
       
        /** 
      \brief Constructor
      \param title - The title of the dialog
      \param environment - The GUI environment to be used
      \param parent - The parent of the dialog
      \param id - The ID of the dialog
      \param type - The type of dialog
      */
      CGUIStretchWindow(const wchar_t* title, IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);
		 // : IGUIElement(EGUIET_WINDOW, environment, parent, id, rectangle) {}

      /**
        \brief Destructor
        */
      virtual ~CGUIStretchWindow();
      
      /**
        \brief Returns the filename of the selected file. Returns NULL, if no file was selected.
        \return a const wchar_t*
        */
      
	  //! Returns pointer to the close button
	  IGUIButton* getCloseButton() const;
	
      virtual bool OnEvent(const SEvent& event);

      /**
      \brief Render function
      */
      virtual void draw();  

	  // Device 
	  inline void setDevice(irr::IrrlichtDevice * device) { this->device=device;}

	  core::rect<s32> getClientRect();

   protected:

	   void updateClientRect();

		bool Dragging;
		core::position2d<s32> DragStart;
		core::rect<s32> ClientRect;
      IGUIElement* EventParent;
	  IGUIButton* CloseButton;

	  private:

		  bool stretchbottom, stretchtop, stretchright, stretchleft;
		  irr::IrrlichtDevice * device;
		  
         
};

#endif /* CGUISTRETCHWINDOW_H */