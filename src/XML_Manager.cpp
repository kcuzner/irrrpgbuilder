#include "XML_Manager.h"
#include "LANGManager.h"

// This class should be able to load all XML data for the need of IRB, and would only use IrrXML (part of the Irrlicht)
// Currently only the language (lang) have a working implementation
// The idead would be to load all definition in memory and then let IRB check the information from there and not load it each time.
// Should help improve response when loading stuff.

// Will also have to check on a way to save stuff in XML using Irrlicht. 

xmldata::xmldata()
{
}

xmldata::~xmldata()
{
}

xmldata* xmldata::getInstance()
{
	static xmldata *instance = 0;
	if (!instance) instance = new xmldata();
	return instance;
}

core::stringw xmldata::winconvert(core::stringw str)
// Convert accents from loaded XML files (irrXML)
// WARNING: Tested only on windows
// might not work on Linux or other platform.
{
	bool debug = false;
	core::stringw textline = L"";
	core::stringw text = L"";
	u32 base = 0;
	
	char test2 = ' ';

	for (u32 a=0; a<str.size(); a++)
	{
		// Get the character first
		text = str.subString(a,1);
		
		// Then check this character directly (convert to unsigned 32bit)
		base=(u32)text[0];

		if (base<256) // Standard characters
		{
			textline+=text;
		}
		
		// All characters after 256 are ignored except thoses
		// Character higher are re-aligned from the offset to match LATIN1

		// Reference to the table is here:
		// http://www.utf8-chartable.de/unicode-utf8-table.pl
		
		const u32 offset=65216;

		core::stringw replace = L" ";
		if ((base>255) && ((base-offset)<255))
		{
				replace[0]=(base-offset);
				textline+=replace;
		}
		
	}
	
	return textline;
}

// Not yet used.. Need to be completed, will replace code that is used in TinyXml
void xmldata::loadBlock(IrrlichtDevice * device, core::stringc file )
{
	// --> Loader code
	// read configuration from xml file

        io::IXMLReaderUTF8* xml = device->getFileSystem()->createXMLReaderUTF8(file);

		core::stringc  MessageText = "";
		// Data blocks defining the object itself
		core::stringc  objectName = "";
		core::stringc  objectMesh = "";
		core::stringc  objectType = "";
		core::stringc  objectScript = "";
		core::stringc  objectScale = "";
		core::stringc  objectMaterial = "";

		// Inner data block that define the object animations
		core::stringc  animName = "";
		core::stringc  animStart = "";
		core::stringc  animEnd = "";
		core::stringc  animSpeed = "";
		core::stringc  animLoop = "";
		core::stringc  animMoveSpeed = "";
		core::stringc  animAttackEvent = "";
		core::stringc  animSoundEvent = "";
		core::stringc  animSound = "";
		core::stringw  result = L"";

		bool inside = false;

		// Language counter (using the XML hierachy)
		u32 count = 0;
		u32 linecount = 0;
		u32 npccount = 0;
		u32 playercount = 0;
		u32 propscount = 0;
		u32 editorcount = 0;


        while(xml && xml->read())
        {
                switch(xml->getNodeType())
                {
                case io::EXN_TEXT:		
                        break;

                case io::EXN_ELEMENT:
                {
					if (core::stringw("dynamic_object") == xml->getNodeName())
					{
						if (!inside) 
						{
							printf ("Inside the requested block (object)!\n");
							inside=true;
						}
							
						objectName = xml->getAttributeValue("name");
						objectMesh = (core::stringc)"    Object mesh: " +xml->getAttributeValue("mesh");
						objectType = xml->getAttributeValue("type");

						// simply count the object types for the statistics
						if (objectType==(core::stringc)"npc")
							npccount++;
						if (objectType==(core::stringc)"non-interactive")
							propscount++;
						if (objectType==(core::stringc)"editor")
							editorcount++;
						if (objectType==(core::stringc)"player")
							playercount++;

						linecount++;

						//non-interactive
						objectScript = (core::stringc)"    Object script: " + xml->getAttributeValue("script");
						objectScale = (core::stringc)"    Object scale: " + xml->getAttributeValue("scale");
						objectMaterial = (core::stringc)"    Object material: " + xml->getAttributeValue("materialType");
						result=(core::stringw)objectMesh;
							
						/*list->addItem(winconvert((core::stringc)"Object name: " + objectName).c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,128,128,200));
						list->addItem(winconvert(result).c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,255,255,255));
						list->addItem(winconvert((core::stringc)"    Object type: " +objectType).c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,255,255,255));
						list->addItem(winconvert(objectScript).c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,255,255,255));
						list->addItem(winconvert(objectScale).c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,255,255,255));
						list->addItem(winconvert(objectMaterial).c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,255,255,255));
						stats->addItem(((core::stringw)L"  "+objectName).c_str());*/
						
					}

					if (core::stringw("animation") == xml->getNodeName())
					{
						/*animName = (core::stringc)"    Anim name: " +xml->getAttributeValue("name");

						result=(core::stringw)animName;
						list->addItem(result.c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,178,178,0));

						animStart = (core::stringc)"        Anim start: " +xml->getAttributeValue("start");
						list->addItem(winconvert(animStart).c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,255,255,0));

						animEnd = (core::stringc)"        Anim end: " +xml->getAttributeValue("end");
						list->addItem(winconvert(animEnd).c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,255,255,0));

						animSpeed = (core::stringc)"        Anim speed: " +xml->getAttributeValue("speed");
						list->addItem(winconvert(animSpeed).c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,255,255,0));

						animLoop = (core::stringc)"        Anim loop: " +xml->getAttributeValue("loop");
						list->addItem(winconvert(animLoop).c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,255,255,0));

						animSound = (core::stringc)"        Anim sound: " +xml->getAttributeValue("sound");
						result=(core::stringw)animSound;
						list->addItem(winconvert(result).c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,255,255,0));

						animMoveSpeed = (core::stringc)"        Anim walk move speed: " +xml->getAttributeValue("movespeed");
						list->addItem(winconvert(animMoveSpeed).c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,255,255,0));

						animAttackEvent = (core::stringc)"        Anim attack event frame: " +xml->getAttributeValue("attackevent");
						list->addItem(winconvert(animAttackEvent).c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,255,255,0));

						animSoundEvent = (core::stringc)"        Anim sound event frame: " +xml->getAttributeValue("soundevent");
						list->addItem(winconvert(animSoundEvent).c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,255,255,0));*/
						
					}
					
				}
                break;

				case io::EXN_ELEMENT_END:
					if (inside)
					{
						count++;
					}
					inside = false;
					printf("The element has ended\n\n");
					break;
                
				default:
                        break;
                }
        }

		/*
		if (playercount>0)
		{
			stats->addItem(((core::stringw)L"Players in this set: "+(core::stringw)playercount).c_str());
			stats->setItemOverrideColor(stats->getItemCount()-1,video::SColor(255,0,0,200));
		}

		if (npccount>0)
		{
			stats->addItem(((core::stringw)L"NPC`s in this set: "+(core::stringw)npccount).c_str());
			stats->setItemOverrideColor(stats->getItemCount()-1,video::SColor(255,0,0,200));
		}

		if (propscount)
		{
			stats->addItem(((core::stringw)L"Props this set: "+(core::stringw)propscount).c_str());
			stats->setItemOverrideColor(stats->getItemCount()-1,video::SColor(255,0,0,200));
		}

		if (editorcount>0)
		{
			stats->addItem(((core::stringw)L"Editor special objects: "+(core::stringw)editorcount).c_str());
			stats->setItemOverrideColor(stats->getItemCount()-1,video::SColor(255,0,0,200));
		}

		core::stringw countstr = ((core::stringw)L"-->Total items in set: ")+(core::stringw)(linecount);
		stats->addItem(countstr.c_str());*/

        if (xml)
			xml->drop(); // don't forget to delete the xml reader
	// <-- Loader code

}


// Not yet used.. Need to be completed, will replace code that is used in TinyXml
void xmldata::loadSet(IrrlichtDevice * device)
{
	// File to seek is: dynamic_objects.xml
	// Will provide the path and "sets" to load
	// --> Loader code
	// read configuration from xml file
	

	const u32 starttime = device->getTimer()->getRealTime();

        io::IXMLReaderUTF8* xml = device->getFileSystem()->createXMLReaderUTF8("Media/dynamic_objects.xml");

		core::stringc  MessageText = "";
		core::stringc  set = "";
		core::stringc  id = "";
		core::stringc  str = "";
		core::stringw  result = L"";

		bool inside = false;

		// Language counter (using the XML hierachy)
		u32 count = 0;
		u32 linecount = 0;

        while(xml && xml->read())
        {
                switch(xml->getNodeType())
                {
                case io::EXN_TEXT:		
                        break;

                case io::EXN_ELEMENT:
                {
					// Look for a specified node
					if (core::stringw("dynamic_object") == xml->getNodeName())
					{
						if (!inside) 
						{
							printf ("Inside the requested block!\n");
							inside=true;
						}
							
						/*
						set = xml->getAttributeValue("set");
						list->addItem(winconvert((core::stringc)"Current set: " + set).c_str());
						list->setItemOverrideColor(list->getItemCount()-1,video::SColor(255,0,0,255));

						linecount++;

						stats->addItem(winconvert((core::stringc)"Set #" + (core::stringc)linecount + ", model list: ").c_str());
						stats->setItemOverrideColor(stats->getItemCount()-1,video::SColor(255,0,0,255)); */

							// Load the block of data
						loadBlock (device, set);
						
						


					}
				}
                break;

				case io::EXN_ELEMENT_END:
						if (inside)
							count++;
						inside = false;
						printf("The element has ended\n\n");
						break;

                default:
                        break;
                }
        }

		/*core::stringw countstr = ((core::stringw)L"Object set count: ")+(core::stringw)(linecount);
		stats->addItem(countstr.c_str());

		const u32 endtime = device->getTimer()->getRealTime();
		u32 time = endtime-starttime;

		stats->addItem(((core::stringw)L"Parse time used: "+(core::stringw)time+L" ms.").c_str());*/


        if (xml)
                xml->drop(); // don't forget to delete the xml reader
	// <-- Loader code

}

void xmldata::loadLang(IrrlichtDevice * device)
{
	// --> Loader code
	// read configuration from xml file
		const u32 starttime = device->getTimer()->getRealTime();
        io::IXMLReaderUTF8* xml = device->getFileSystem()->createXMLReaderUTF8("../media/lang.xml");

		Lang CurrentLang;

		core::stringc  MessageText = "";
		core::stringc  language = "";
		core::stringc  description = "";
		core::stringw  result = L"";

		bool inside = false;
		bool found = false;

		// Language counter (using the XML hierachy)
		u32 count = 0;
		u32 linecount = 0;
		u32 linecount2 = 0;

        while(xml && xml->read())
        {
                switch(xml->getNodeType())
                {
                case io::EXN_TEXT:		
                        break;

                case io::EXN_ELEMENT:
                {
					
						if (core::stringw("language") == xml->getNodeName())
						{
							linecount=0;
							linecount2=0;
							if (!inside) 
							{
								inside=true;							
							}

                            language = winconvert(xml->getAttributeValue("name"));
							description = winconvert(xml->getAttributeValue("description"));
							if (LANGManager::getInstance()->defaultLanguage==language)
								found=true;
						}

						if (core::stringw("text") == xml->getNodeName())
						{


							if (language==LANGManager::getInstance()->defaultLanguage)
							{
								CurrentLang.name = winconvert(xml->getAttributeValue("id"));
								CurrentLang.text = winconvert(xml->getAttributeValue("str"));
								LANGManager::getInstance()->language.push_back(CurrentLang);
								linecount++;
							}
						}
						if (core::stringw("about") == xml->getNodeName())
						{
							if (language==LANGManager::getInstance()->defaultLanguage)
							{
								CurrentLang.name=L"txt_about";
								CurrentLang.text=winconvert(xml->getAttributeValue("str"));
								LANGManager::getInstance()->aboutext.push_back(CurrentLang);
							}
						}
				}
                break;

				case io::EXN_ELEMENT_END:
					if (!inside)
					{
						linecount=0;
						linecount2=0;
					}

					if (inside)
						count++;

					inside = false;
					break;
                default:
                        break;
                }
        }

		/*
		core::stringw countstr = ((core::stringw)L"Language count: ")+(core::stringw)(count);
		stats->addItem(countstr.c_str());

		const u32 endtime = device->getTimer()->getRealTime();
		u32 time = endtime-starttime;

		stats->addItem(((core::stringw)L"Parse time used: "+(core::stringw)time+L" ms.").c_str()); */

        if (xml)
                xml->drop(); // don't forget to delete the xml reader
	// <-- Loader code

}
