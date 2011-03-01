#include "LANGManager.h"


LANGManager::LANGManager()
{
    defaultLanguage = "";
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

// -- Put the active language in memory for later retrieval 
bool LANGManager::Load()
{
	TiXmlDocument doc("../media/lang.xml");
	Lang CurrentLang;

	//try to parse the XML
	if (!doc.LoadFile())
    {
        cout << "ERROR LOADING LANG.XML" << endl;
        exit(0);
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
		//GUIManager::getInstance()->setTextLoader(L"Loading language...");
		TiXmlNode* currentObjXML = root->FirstChild( "language" );
        while( currentObjXML != NULL )
        {
			
			irr::core::stringc name = currentObjXML->ToElement()->Attribute("name");
			irr::core::stringc description = currentObjXML->ToElement()->Attribute("description");
            //Get Dynamic Object Attributes
			//printf("LANG: %s, %s\n",name.c_str(),description.c_str());

			TiXmlNode* currentLanguageXML = currentObjXML->FirstChild( "text" );
			while( currentLanguageXML != NULL )

			{
				if (name==defaultLanguage)
				{
					irr::core::stringc id = currentLanguageXML->ToElement()->Attribute("id");
					irr::core::stringc str = currentLanguageXML->ToElement()->Attribute("str");
					CurrentLang.name=id.c_str();
					CurrentLang.text=str.c_str();
					language.push_back(CurrentLang);
				
					/*if (id.size()>0)
						printf ("%s,%s\n",id.c_str(),str.c_str());*/
				}
			
				currentLanguageXML = currentObjXML->IterateChildren( "text", currentLanguageXML );
			}
			currentObjXML = root->IterateChildren( "language", currentObjXML );

		}
	}

	return true;
}

// - Retrieve the proper string from stored memory.
stringc LANGManager::getText(irr::core::stringc node)
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

void LANGManager::setDefaultLanguage(irr::core::stringc language)
{
	defaultLanguage = language.c_str();
	Load();
}