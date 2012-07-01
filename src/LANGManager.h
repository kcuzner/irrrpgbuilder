#ifndef LANGMANAGER_H
#define LANGMANAGER_H

#include "gui/GUIManager.h"
#include "tinyXML/tinyxml.h"

typedef struct{
	irr::core::stringw name;
	irr::core::stringw text;
}Lang;

class LANGManager
{
    public:
        static LANGManager* getInstance();
		void setAboutText(IGUIListBox* guiAboutText);
		vector<core::stringw> getAboutText();
		irr::core::stringw getText(irr::core::stringc node);
		irr::core::stringc getLanguage();
		bool Load();
		void setDefaultLanguage(irr::core::stringc language);
		vector<Lang> language;
		vector<Lang> aboutext;	
		irr::core::stringc defaultLanguage;	
        
		

    protected:

    private:
        LANGManager();
		virtual ~LANGManager();
		
};

#endif // LANGMANAGER_H
