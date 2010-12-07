#include "LANGManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

LANGManager::LANGManager()
{
    defaultLanguage = "";

    ///TODO:remove all IrrXML... TinyXML must be default

    irr::io::IXMLReaderUTF8* xml = App::getInstance()->getDevice()->getFileSystem()->createXMLReaderUTF8("../media/lang.xml");

    if(!xml) exit(0);

    while (xml->read())
    {
        switch (xml->getNodeType())
        {
            case irr::io::EXN_ELEMENT:
                if (core::stringw("IrrRPG_Builder_LanguageFile") == xml->getNodeName())
                {
                    if(xml->getAttributeValueAsFloat("version") != APP_VERSION)
                    {
                        printf("ERROR: LANGUAGE FILE VERSION MUST BE %f\n",APP_VERSION);
                        exit(0);
                    }

                    break;
                }
                break;
        }
    }

    delete xml;
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

stringc LANGManager::getText(stringc node)
{
    irr::io::IXMLReaderUTF8* xml = App::getInstance()->getDevice()->getFileSystem()->createXMLReaderUTF8("../media/lang.xml");

    if(!xml)
    {
        printf("ERROR: lang.xml not found!");
        exit(0);
    }

    while (xml->read())
    {
        switch (xml->getNodeType())
        {
            case irr::io::EXN_ELEMENT:
				
                if ((core::stringw("language") == xml->getNodeName()) && (stringc(xml->getAttributeValue("name")) == defaultLanguage))
                {	
					description = stringc(xml->getAttributeValue("description"));
                    //read the default language
                    while(xml->read())
                    {
                        switch (xml->getNodeType())
                        {
                            case irr::io::EXN_ELEMENT:
                                if ( stringc(xml->getNodeName()) == stringc("text") )
                                {
                                    if( stringc(xml->getAttributeValue("id")) == stringc(node.c_str()) )
                                    {
										if (stringc(node.c_str())!="txt_about")
											return stringc(xml->getAttributeValue("str"));
										else
										{
											
											int aa = atoi(stringc(xml->getAttributeValue("str")).c_str());
											printf("------------->WARNING WARNING About text is here!! %i \n",aa);
											stringc item = "str";
											for(int a=1; a<=aa; a++)
											{
												item = "str";
												item.append(stringc(a).c_str());
												printf("----------------> Here is the item name %s\n",item.c_str());

												GUIManager::getInstance()->addAboutTextItem(stringc(xml->getAttributeValue(item.c_str())));
												
											}
											return " ";
										}
                                    }
                                }
								break;

                        } 
						
                    } 
					break;
                } 
        }
    }

    delete xml;
	return "";
}

void LANGManager::setDefaultLanguage(stringc language)
{
	defaultLanguage = language.c_str();
}