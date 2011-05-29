#ifndef LANGMANAGER_H
#define LANGMANAGER_H

#include "GUIManager.h"
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
		vector<stringw> getAboutText();
		irr::core::stringw getText(irr::core::stringc node);
		irr::core::stringc getLanguage();
		bool Load();
		void setDefaultLanguage(irr::core::stringc language);
        virtual ~LANGManager();
		

    protected:

    private:
        LANGManager();
		irr::core::stringc defaultLanguage;
		vector<Lang> language;
		vector<Lang> aboutext;		
};

#endif // LANGMANAGER_H
