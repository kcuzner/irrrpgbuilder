#include "LANGManager.h"


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

// -- Put the active language in memory for later retrieval 
bool LANGManager::Load()
{

	TiXmlDocument doc("../media/lang.xml");
	Lang CurrentLang;

	//try to parse the XML
	if (!doc.LoadFile())
    {
        cout << "ERROR LOADING LANG.XML" << endl;
        //exit(0);
		return false;
    }

    #ifdef APP_DEBUG
    cout << "DEBUG : XML : LOADING LANGUAGE.XML" << endl;
    #endif

    //locate root node
    TiXmlElement* root = doc.FirstChildElement( "IrrRPG_Builder_LanguageFile" );

	if ( root )
    {
        //check file version
        if( atof(root->Attribute("version"))!= APP_VERSION )
        {
            #ifdef APP_DEBUG
            cout << "DEBUG : XML : INCORRECT LANGUAGE.XML VERSION!" << endl;
            #endif

            exit(0);
        }

		vector<irr::core::stringc> objsIDs;
		

        //Iterate language file
		TiXmlNode* currentObjXML = root->FirstChild( "language" );
        while( currentObjXML != NULL )
        {
			
			irr::core::stringw name = currentObjXML->ToElement()->Attribute("name");
			irr::core::stringw description = convert(currentObjXML->ToElement()->Attribute("description"));
            
			
			// Load the translated text for the current language
			TiXmlNode* currentLanguageXML = currentObjXML->FirstChild( "text" );
			while( currentLanguageXML != NULL )
			{
				if (name==defaultLanguage)
				{
					irr::core::stringc id = currentLanguageXML->ToElement()->Attribute("id");
					irr::core::stringc str = currentLanguageXML->ToElement()->Attribute("str");
					CurrentLang.name=id.c_str();
					// This must be used to convert the accented characters
					CurrentLang.text=convert(str.c_str());
					language.push_back(CurrentLang);
				
				}
				currentLanguageXML = currentObjXML->IterateChildren( "text", currentLanguageXML );
			}

			// Load the about text (multiple lines)
			TiXmlNode* currentAboutXML = currentObjXML->FirstChild( "about" );
			CurrentLang.text=L"";
			while( currentAboutXML != NULL )
			{
				if (name==defaultLanguage)
				{
					irr::core::stringc str = currentAboutXML->ToElement()->Attribute("str");
					CurrentLang.name=L"txt_about";
					CurrentLang.text=convert(str);
					aboutext.push_back(CurrentLang);
				}
				currentAboutXML = currentObjXML->IterateChildren( "about", currentAboutXML );
			}
			currentObjXML = root->IterateChildren( "language", currentObjXML );

		}
	}

	return true;
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
	Load();
}