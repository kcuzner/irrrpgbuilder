#include "XMLManager.h"

#include "tinyXML/tinyxml.h"

XMLManager::XMLManager()
{
    //ctor
}

XMLManager::~XMLManager()
{
    //dtor
}

XMLManager* XMLManager::getInstance()
{
    static XMLManager* instance = 0;
    if (!instance) instance = new XMLManager();
    return instance;
}

bool XMLManager::saveProject()
{
    TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );

	TiXmlElement * element = new TiXmlElement( "screen" );
	doc.LinkEndChild( element );

	element->SetAttribute("width",800);
	element->SetAttribute("height",600);
	element->SetAttribute("bits",32);

	doc.SaveFile( "../projects/myProject.xml" );
	return true;
}
