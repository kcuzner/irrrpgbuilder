#include "LANGManager.h"
#include "XML_Manager.h"


LANGManager::LANGManager()
{
    defaultLanguage = "";
	aboutext.clear();
	language.clear();
}

LANGManager::~LANGManager()
{
	//dtor
}

LANGManager* LANGManager::getInstance()
{
    static LANGManager* instance = 0;
    if (!instance) instance = new LANGManager();
    return instance;
}

irr::core::stringc LANGManager::getLanguage()
{
	return defaultLanguage;
}

// - Retrieve the proper string from stored memory.
// And put the content in the GUI Item directly (AboutText GUI)
void LANGManager::setAboutText(IGUIListBox* guiAboutText)
{
	for (int i=0 ; i<(int)aboutext.size() ; i++)
    {		
		guiAboutText->addItem(aboutext[i].text.c_str());        
    }
	// Clear the memory since it should not be used again
	//aboutext.clear();
}

vector<stringw> LANGManager::getAboutText()
{
	vector<stringw> result;
	for (int i=0 ; i<(int)aboutext.size() ; i++)
    {		
		result.push_back(aboutext[i].text.c_str());        
    }
	return result;
}

// - Retrieve the proper string from stored memory.
// And return the resulting string to the caller
irr::core::stringw LANGManager::getText(irr::core::stringc node)
{

	for (int i=0 ; i<(int)language.size() ; i++)
    {
		if( language[i].name == node )
        {
			return language[i].text;			
        }
    }
	return "";
}

void LANGManager::setDefaultLanguage(irr::core::stringc languagetext)
{
	defaultLanguage = languagetext.c_str();
	language.clear();
	// Load the data from the new XMLManager (irrXML instead of TinyXML)
	xmldata::getInstance()->loadLang(App::getInstance()->getDevice());
}