#include "DynamicObjectsManager.h"
#include "DynamicObject.h"
#include "../camera/CameraSystem.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

DynamicObjectsManager::DynamicObjectsManager()
{
	device = App::getInstance()->getDevice();

	newObj=new TemplateObject("");

	this->loadTemplates();

	//set the initial active object - the list must be 1 or more objs!
	activeObject = this->searchTemplate("frog");
	 //just initialize var
    objsCounter = 0;
	objectCounter = 0;
	dialogCaller = NULL;
	setname=L"";

}

DynamicObjectsManager::~DynamicObjectsManager()
{
    //dtor
}

bool DynamicObjectsManager::loadTemplates()
{
	// Test loading of templates data only
	this->loadSet();

	// Create the 2 needed objects at startup
	TemplateObject* pObject = searchTemplate("player_normal");
	
	//Create the player avatar needed in the game
	if (pObject->getName()=="player_normal")	
	{
		playerObject = new DynamicObject(pObject->getName(), pObject->meshFile, pObject->animations);
		playerObject->setScale(vector3df(pObject->getScale(),pObject->getScale(),pObject->getScale()));
		playerObject->setType(pObject->getType());
			
		playerObject->setTemplate(true);
		objects.push_back(playerObject);
		//setup material
		playerObject->setMaterialType(pObject->getMaterialType());
		playerObject->getNode()->setMaterialFlag(EMF_LIGHTING,true);
		// Load the script if it was defined in the XML
		if (pObject->script.size()>1)
		{
			stringc newScript = "";
			stringc filename = "../media/scripts/";
			filename += pObject->script;

			std::string line;
			ifstream fileScript (filename.c_str());
			if (fileScript.is_open())
			{
				while (! fileScript.eof() )
				{
					getline (fileScript,line);
					newScript += line.c_str();
					newScript += '\n';
				}
				fileScript.close();
			}
			playerObject->setScript(newScript);
		}
	}

	// Create the target object needed for the point&click gameplay
	TemplateObject* tObject = searchTemplate("target");	
	if (tObject->getName()=="target")
	{
		targetObject = new DynamicObject(tObject->getName(), tObject->meshFile, tObject->animations);
		targetObject->setTemplate(true);
		targetObject->getNode()->setVisible(false);
		targetObject->getNode()->setDebugDataVisible( false ? EDS_BBOX | EDS_SKELETON : EDS_OFF);
		 //setup material
		targetObject->setMaterialType(tObject->getMaterialType());
		targetObject->getNode()->setMaterialFlag(EMF_LIGHTING,true);
	}
	return true;
}

TemplateObject* DynamicObjectsManager::searchTemplate(stringc name)
{
	for (int i=0 ; i< (int)objTemplate.size() ; i++)
    {
		if (objTemplate[i])
		{
			if( (stringc)objTemplate[i]->getName() == name )
			{	
				return objTemplate[i];
			}
		}
    }
	printf ("Warning: Failed to find the proper object: %s\n",name.c_str());
    return NULL;
}

bool DynamicObjectsManager::loadBlock(IrrlichtDevice * device, core::stringc file )
{
	// --> Loader code
	// read configuration from xml file

        io::IXMLReaderUTF8* xml = device->getFileSystem()->createXMLReaderUTF8(file);
		if (!xml)
			return false;

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
		bool inside2 = false;

		// Language counter (using the XML hierachy)
		u32 count = 0;
		u32 linecount = 0;
		u32 npccount = 0;
		u32 playercount = 0;
		u32 propscount = 0;
		u32 editorcount = 0;

		core::stringc oldName = "";

		core::stringw currentNodeName = L"";

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

						
						objectName = (core::stringw)xml->getAttributeValue("name");

						core::stringw uptext = L"Loading template object: ";
						uptext += objectName;
						GUIManager::getInstance()->setTextLoader(uptext);
						

						if (oldName!=(core::stringc)newObj->getName())
						{
							
							oldName=(core::stringc)newObj->getName();

							// Add the old object to the list,only the pointer is stored
							this->objTemplate.push_back(newObj);

							// Create the new object 
							newObj = new TemplateObject(objectName);
						}
						
						newObj->setName((core::stringw)objectName);

						// Get the current set name and save it in the object
						newObj->type=setname;
						
						objectMesh = (core::stringc)xml->getAttributeValue("mesh");
						newObj->meshFile=objectMesh;

						objectType = xml->getAttributeValue("type");
						newObj->setType(objectType);

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
						objectScript = (core::stringc)xml->getAttributeValue("script");
						newObj->script=(core::stringw)objectScript;

						objectScale = (core::stringc)xml->getAttributeValue("scale");
						newObj->setScale((irr::f32)atof(objectScale.c_str()));
						
						objectMaterial = (core::stringc)xml->getAttributeValue("materialType");
			
						E_MATERIAL_TYPE mat = EMT_SOLID;
				
            			if(objectMaterial == stringc("transparent_1bit")) mat = EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
						if(objectMaterial == stringc("transparent_8bit")) mat = EMT_TRANSPARENT_ALPHA_CHANNEL;
						newObj->setMaterialType(mat);

						newObj->author = xml->getAttributeValue("author");
						newObj->description = xml->getAttributeValue("description");
						newObj->licence = xml->getAttributeValue("licence");
						newObj->category = xml->getAttributeValue("category");
						newObj->thumbnail = xml->getAttributeValue("thumbnail");
						
					}

					if (core::stringw("animation") == (core::stringw)xml->getNodeName())
					{


						DynamicObject_Animation currAnim;
						// Initialize the data
						currAnim.name = "";
						currAnim.startFrame = 0;
						currAnim.endFrame = 0;
						currAnim.speed = 0.0f;
						currAnim.loop = true;
						currAnim.sound = "";
						currAnim.walkspeed = 0.0f;
						currAnim.attackevent = 0;
						currAnim.soundevent = 0;
						currAnim.meshname="";
						currAnim.mesh=NULL;

						if (!inside2) 
						{
							printf ("Inside the requested block (animation)!\n");
							inside2=true;
						}
						currAnim.name = (core::stringc)xml->getAttributeValue("name");

						animStart = (core::stringc)xml->getAttributeValue("start");
						currAnim.startFrame = (irr::s32) atoi(animStart.c_str());
						
						animEnd = (core::stringc)xml->getAttributeValue("end");
						currAnim.endFrame = (irr::s32) atoi(animEnd.c_str());

						animSpeed = (core::stringc)xml->getAttributeValue("speed");
						currAnim.speed = (irr::f32)atof(animSpeed.c_str());

						animLoop = (core::stringc)xml->getAttributeValue("loop");
						if (animLoop=="false") currAnim.loop = false;
						else
							currAnim.loop = true;


						animSound = (core::stringc)xml->getAttributeValue("sound");
						currAnim.sound = animSound;


						animMoveSpeed = (core::stringc)xml->getAttributeValue("movespeed");
						currAnim.walkspeed = (irr::f32) atof(animMoveSpeed.c_str());
						
						animAttackEvent = (core::stringc)xml->getAttributeValue("attackevent");
						currAnim.attackevent = (irr::s32)atoi(animAttackEvent.c_str());

						animSoundEvent = (core::stringc)xml->getAttributeValue("soundevent");
						currAnim.soundevent = (irr::s32)atoi(animSoundEvent.c_str());

						
						
						newObj->animations.push_back(currAnim); //add the new animation to the template data
	
						/*
						printf (">>> Added animation %s for %s. S:%d, E:%d\n",currAnim.name.c_str(),objectName.c_str(),
							currAnim.startFrame, currAnim.endFrame);

						if ((currAnim.soundevent>0) && (currAnim.sound!=""))
							printf (">>>>> Sound %s will be triggered at frame %d\n",currAnim.sound,currAnim.soundevent);
						
						if (currAnim.attackevent>0)
							printf (">>>>> Damage will be done from frame %d\n",currAnim.attackevent);

						if (currAnim.speed>0) 
							printf (">>>>> Framerate of anim: %f\n",(float)currAnim.speed);
						else 
							printf (">>>>> WARNING! No animation framerate defined!!!\n");

						if (currAnim.walkspeed>0)
							printf (">>>>> Walkspeed of anim: %f\n",(float)currAnim.walkspeed);

						printf("\n"); */
					}
					
				}
                break;

				case io::EXN_ELEMENT_END:
					if (inside)
					{
						count++;
					}
					inside = false;
					inside2 = false;
					printf("The element has ended\n\n");
					break;
                
				default:
                        break;
                }
        }

		if (playercount>0)
		{
			printf (">>>>> Current player count is: %d\n",playercount);
		}

		if (npccount>0)
		{
			printf (">>>>> Current npc count is: %d\n",npccount);
		}

		if (propscount)
		{
			printf (">>>>> Current prop count is: %d\n",propscount);
		}

		if (editorcount>0)
		{
			printf (">>>>> Current editor object count is: %d\n",editorcount);
		}

		printf (">>>>> Total objects in this set: %d\n",linecount);

        if (xml)
                xml->drop(); // don't forget to delete the xml reader

		return true;
}


bool DynamicObjectsManager::loadSet()
{
	// File to seek is: dynamic_objects.xml
	// Will provide the path and "sets" to load
	// read configuration from xml file

	const u32 starttime = App::getInstance()->getDevice()->getTimer()->getRealTime();

	io::IXMLReaderUTF8* xml = App::getInstance()->getDevice()->getFileSystem()->createXMLReaderUTF8("../media/dynamic_objects/dynamic_objects.xml");
	if (!xml)
	{
		printf ("Failed to load the dynamic object template list!\n");
		return false;
	}

	core::stringc  MessageText = "";
	core::stringc  set = "";
	core::stringc  id = "";
	core::stringc  str = "";
	core::stringw  result = L"";

	bool inside = false;
	bool inside2 = false;

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
							
						set = xml->getAttributeValue("set");
						setname = (core::stringw)xml->getAttributeValue("name");
						if (setname!="")
							meshtypename.push_back(setname);
						//printf ("--- Set: %s\n",set);

						linecount++;

						// Load the block of data (A 'block' is a xml file containing the objects)
						loadBlock(device, set);
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

		core::stringw countstr = ((core::stringw)L"Object set count: ")+(core::stringw)(linecount);

		const u32 endtime = App::getInstance()->getDevice()->getTimer()->getRealTime();
		u32 time = endtime-starttime;

        if (xml)
                xml->drop(); // don't forget to delete the xml reader

		return true;
}

DynamicObject* DynamicObjectsManager::createActiveObjectAt(vector3df pos)
{
  	DynamicObject* newObj = new DynamicObject(activeObject->getName(),activeObject->meshFile,activeObject->animations);
	newObj->setScale(vector3df(activeObject->getScale(),activeObject->getScale(),activeObject->getScale()));
	newObj->setType(activeObject->getType());
	newObj->setTemplate(false);

	//setup material
	newObj->getNode()->setMaterialType(activeObject->getMaterialType());
	newObj->getNode()->setMaterialFlag(EMF_LIGHTING,true);
	// Load the script if it was defined in the XML
	if (activeObject->script.size()>1)
	{
		stringc newScript = "";
		stringc filename = "../media/scripts/";
		filename += activeObject->script;

		std::string line;
		ifstream fileScript (filename.c_str());
		if (fileScript.is_open())
		{
			while (! fileScript.eof() )
			{
				getline (fileScript,line);
				newScript += line.c_str();
				newScript += '\n';
			}
			fileScript.close();
		}
		newObj->setScript(newScript);
	}

    cout << "TEMPLATE NAME:" << activeObject->getName().c_str() << endl;

    newObj->setPosition(pos);

    //the unique name of an dynamic object contains his index at the objects vector
    newObj->setName(this->createUniqueName());
	// This is the reference name of the template this object is made of.
	newObj->setTemplateObjectName(activeObject->getName());

	// Add to the dynamic object list.
	objects.push_back(newObj);
	activeObject2=newObj;

    return newObj;
}

void DynamicObjectsManager::removeObject(stringc uniqueName)
{
    for (int i=0 ; i<(int)objects.size() ; i++)
    {
        if( stringc(objects[i]->getName()) == uniqueName )
        {
            delete ((DynamicObject*)objects[i]);
            objects.erase(objects.begin() + i);
            return;
        }
    }
}

// Singleton style of keeping the pointer of the manager.
// Can be used to recall the pointer of the manager
DynamicObjectsManager* DynamicObjectsManager::getInstance()
{
    static DynamicObjectsManager *instance = 0;
    if (!instance) instance = new DynamicObjectsManager();
    return instance;
}

// The "active" object is the current template that is used to create items on the map.
TemplateObject* DynamicObjectsManager::getActiveObject()
{
    return activeObject;
}


//Find the name in the template as use it as "active" one.
//This will be used to create the character or object on the map by the user.
//Normally the "name" is provided by a list.
void DynamicObjectsManager::setActiveObject(stringc name)
{
    for (int i=0 ; i<(int)objTemplate.size() ; i++)
    {
		core::stringw templateName=objTemplate[i]->getName();
		if((core::stringc)templateName == name )
    	{
    	    activeObject = ((TemplateObject*)objTemplate[i]);
    	    break;
    	}
    }
}

//! Provide a list of template objects names for the GUI (Templates) based on the object type/category
//! Used the GUI system to provide a list from the objects in the templates
vector<stringw> DynamicObjectsManager::getObjectsList(core::stringw objectType, core::stringw category)
{
    vector<stringw> listObjs;

    for (int i=0 ; i<(int)objTemplate.size() ; i++)
    {
		if (objTemplate[i]->type==objectType && category=="")
				listObjs.push_back( objTemplate[i]->getName() );
		else

		if (objTemplate[i]->type==objectType && objTemplate[i]->category==category)
		{
			listObjs.push_back( objTemplate[i]->getName() ); 
		}

    }

    return listObjs;
}

//! Create a list of the categories from the XML data in the templates.
vector<stringw> DynamicObjectsManager::getObjectsListCategories(core::stringw objectType)
{
    vector<stringw> listObjs;
	listObjs.push_back((core::stringw)"All");
	bool found = false;

    for (int i=0 ; i<(int)objTemplate.size() ; i++)
    {
		if (objTemplate[i]->type==objectType)
		{ 
			for (int a=0 ; a<(int)listObjs.size(); a++)
			{
				if (objTemplate[i]->category == listObjs[a])
				{
					found=true;
				}
			}
			if (!found && objTemplate[i]->category!="")
					listObjs.push_back( objTemplate[i]->category );
			found=false;
		}
    }

    return listObjs;
}

void DynamicObjectsManager::setObjectsID(TYPE objectType, s32 ID)
{
    for (int i=0 ; i<(int)objects.size() ; i++)
    {
		if (objects[i]->getType()==objectType)
			objects[i]->getNode()->setID(ID);
    }

}

//! Return a searched dynamic object based on it's name
DynamicObject* DynamicObjectsManager::getObjectByName(stringc name)
{
    for (int i=0 ; i< (int)objects.size() ; i++)
    {
		if (objects[i])
		{
			//printf("this object is number %d and it's name is: %s\n",i,objects[i]->getName());
			if( objects[i]->getName() == name )
				return objects[i];
		}
    }

    return NULL;
}

//! Return the player dynamic object pointer
DynamicObject* DynamicObjectsManager::getPlayer()
{
	return playerObject;
}

// Return the target dynamic object pointer
DynamicObject* DynamicObjectsManager::getTarget()
{
	return targetObject;
}

// Will try to find the matching name in the dynamic objects 
// and return the first dynamic object pointer that has that template name
// Used by the node preview GUI
scene::ISceneNode* DynamicObjectsManager::findActiveObject(void)
{
	for (int i=0 ; i< (int)objects.size() ; i++)
    {
		if (objects[i])
		{
			printf("this object is number %d and it's name is: %s\n",i,objects[i]->getTemplateObjectName());
			if( objects[i]->getTemplateObjectName() == activeObject->getName() )
				return objects[i]->getNode();
		}
    }

	return NULL;
}

// Save the dynamic objects configuration to XML (TinyXML)
void DynamicObjectsManager::saveToXML(TiXmlElement* parentElement)
{
    //write header
    TiXmlElement* dynamicObjectsXML2 = new TiXmlElement("dynamic_objects");

    for ( int i = 0 ; i < (int)objects.size() ; i++ )
    {
        //objects[i]->saveToXML(dynamicObjectsXML);
		TiXmlElement* dynamicObjectXML = new TiXmlElement("obj");
		//dynamicObjectXML->SetAttribute("name",name.c_str());
		dynamicObjectXML->SetAttribute("type",(int)objects[i]->getType());
		dynamicObjectXML->SetAttribute("x",stringc(this->objects[i]->getPosition().X).c_str());
		dynamicObjectXML->SetAttribute("y",stringc(this->objects[i]->getPosition().Y).c_str());
		dynamicObjectXML->SetAttribute("z",stringc(this->objects[i]->getPosition().Z).c_str());

		dynamicObjectXML->SetAttribute("s",stringc(this->objects[i]->getScale().X).c_str());

		dynamicObjectXML->SetAttribute("r",stringc(this->objects[i]->getRotation().Y).c_str());

		dynamicObjectXML->SetAttribute("template",objects[i]->templateObjectName.c_str());

		if (objects[i]->script.size()>0)
			dynamicObjectXML->SetAttribute("script",objects[i]->getScript().c_str());
		
		if (objects[i]->properties.life>0)
			dynamicObjectXML->SetAttribute("life",objects[i]->properties.life);
		
		if (objects[i]->properties.maxlife>0)
			dynamicObjectXML->SetAttribute("maxlife",objects[i]->properties.maxlife);
		
		if (objects[i]->properties.mana>0 && objects[i]->properties.mana<101)
			dynamicObjectXML->SetAttribute("mana",objects[i]->properties.mana);
		
		if (objects[i]->properties.maxmana>0 && objects[i]->properties.maxmana<101)
			dynamicObjectXML->SetAttribute("maxmana",objects[i]->properties.maxmana);
		
		if (objects[i]->properties.level>0 && objects[i]->properties.level<101)
			dynamicObjectXML->SetAttribute("level",objects[i]->properties.level);
		
		if (objects[i]->properties.experience>0)
			dynamicObjectXML->SetAttribute("XP",objects[i]->properties.experience);
		
		if (objects[i]->properties.mindamage>0)
			dynamicObjectXML->SetAttribute("mindamage",objects[i]->properties.mindamage);
		
		if (objects[i]->properties.maxdamage>0)
			dynamicObjectXML->SetAttribute("maxdamage",objects[i]->properties.maxdamage);
		
		if (objects[i]->properties.hurt_resist>0 && objects[i]->properties.hurt_resist<101)
			dynamicObjectXML->SetAttribute("hurtresist",objects[i]->properties.hurt_resist);
		
		if (objects[i]->properties.dodge_prop>0 && objects[i]->properties.dodge_prop<101)
			dynamicObjectXML->SetAttribute("dodgechance",stringc(objects[i]->properties.dodge_prop).c_str());
		
		if (objects[i]->properties.hit_prob>0 && objects[i]->properties.hit_prob<101)
			dynamicObjectXML->SetAttribute("hitchance",stringc(objects[i]->properties.hit_prob).c_str());

		if (objects[i]->properties.regenlife>0 && objects[i]->properties.regenlife<101)
			dynamicObjectXML->SetAttribute("regenlife",stringc(objects[i]->properties.regenlife).c_str());

		if (objects[i]->properties.regenmana>0 && objects[i]->properties.regenmana<101)
			dynamicObjectXML->SetAttribute("regenmana",stringc(objects[i]->properties.regenmana).c_str());

		dynamicObjectsXML2->LinkEndChild(dynamicObjectXML);

    }

    parentElement->LinkEndChild(dynamicObjectsXML2);
}

//! Load back data from XML into the dynamic objects (TinyXML)
bool DynamicObjectsManager::loadFromXML(TiXmlElement* parentElement)
{
    TiXmlNode* dynamicObjectXML = parentElement->FirstChild( "obj" );

    while( dynamicObjectXML != NULL )
    {

		stringc templateObj = "";
		int type=0;
		DynamicObject* newObj = NULL;

		stringc stype = dynamicObjectXML->ToElement()->Attribute("type");
		// Get the type of the object that was saved (if there is one)
		
		if (stype.size()>0)
			type=atoi(stype.c_str());


        stringc script = dynamicObjectXML->ToElement()->Attribute("script");

		if (type != OBJECT_TYPE_PLAYER)
            templateObj = dynamicObjectXML->ToElement()->Attribute("template");

        f32 posX = (f32)atof(dynamicObjectXML->ToElement()->Attribute("x"));
        f32 posY = (f32)atof(dynamicObjectXML->ToElement()->Attribute("y"));
        f32 posZ = (f32)atof(dynamicObjectXML->ToElement()->Attribute("z"));
        f32 rot = (f32)atof(dynamicObjectXML->ToElement()->Attribute("r"));

		// Create an object from the template
		if (type!=OBJECT_TYPE_PLAYER)
		{ 
			this->setActiveObject(templateObj);
			newObj = createActiveObjectAt(vector3df(posX,posY,posZ));
		} 
		else
		// If this is the player, retrieve only it's position (permanent dynamic object)
		{
			newObj = this->playerObject;
			newObj->setPosition(vector3df(posX,posY,posZ));
			CameraSystem::getInstance()->setPosition(vector3df(posX,posY,posZ));
			
		}
		// If a script is assigned to the mesh then load it.

		newObj->setRotation(vector3df(0,rot,0));
        newObj->setScript(convert(script));

		
        
		cproperty a=newObj->initProperties();
		
		// Default properties values for the player and the NPCS
		if (type==OBJECT_TYPE_NPC || type==OBJECT_TYPE_PLAYER)
		{
			// If LUA or a loaded value redefine a properties, it will override thoses values
			a.experience = 10; // for a NPC this will give 10 XP to the attacker if he win
			a.mindamage=1;
			a.maxdamage=3;
			a.life = 100;
			a.maxlife=100;
			a.hurt_resist=50;
			if (type==OBJECT_TYPE_PLAYER)
			{
				a.mana = 100;
				a.maxmana=100;
				a.dodge_prop=25;
				a.hit_prob=70;
				a.regenlife=1;
				a.regenmana=1;
				a.mindamage=3;
				a.maxdamage=10;
			} else
			{
				a.dodge_prop=12;
				a.hit_prob=50;
			}
			
		}

		// Loading values
		stringc life = "";
		life = dynamicObjectXML->ToElement()->Attribute("life");
		if (life.size()>0)
			a.life = (int)atoi(life.c_str());

		stringc maxlife = "";
		maxlife = dynamicObjectXML->ToElement()->Attribute("maxlife");
		if (maxlife.size()>0)
			a.maxlife = (int)atoi(maxlife.c_str());

		stringc mana = "";
		mana = dynamicObjectXML->ToElement()->Attribute("mana");
		if (mana.size()>0)
			a.mana = (int)atoi(mana.c_str());

		stringc maxmana = "";
		maxmana = dynamicObjectXML->ToElement()->Attribute("maxmana");
		if (maxmana.size()>0)
			a.maxmana = (int)atoi(maxmana.c_str());

		stringc experience = "";
		experience = dynamicObjectXML->ToElement()->Attribute("xp");
		if (experience.size()>0)
			a.experience = (int)atoi(experience.c_str());

		stringc level = "";
		level = dynamicObjectXML->ToElement()->Attribute("level");
		if (level.size()>0)
			a.level = (int)atoi(level.c_str());

		stringc mindamage = "";
		mindamage = dynamicObjectXML->ToElement()->Attribute("mindamage");
		if (mindamage.size()>0)
			a.mindamage = (int)atoi(mindamage.c_str());

		stringc maxdamage = "";
		maxdamage = dynamicObjectXML->ToElement()->Attribute("maxdamage");
		if (maxdamage.size()>0)
			a.maxdamage = (int)atoi(maxdamage.c_str());

		stringc hurtresist = "";
		hurtresist = dynamicObjectXML->ToElement()->Attribute("hurtresist");
		if (hurtresist.size()>0)
			a.hurt_resist = (int)atoi(hurtresist.c_str());

		stringc dodgechance = "";
		dodgechance = dynamicObjectXML->ToElement()->Attribute("dodgechance");
		if (dodgechance.size()>0)
			a.dodge_prop = (f32)atof(dodgechance.c_str());

		stringc hitchance = "";
		hitchance = dynamicObjectXML->ToElement()->Attribute("hitchance");
		if (hitchance.size()>0)
			a.hit_prob = (f32)atof(hitchance.c_str());

		stringc regenlife = "";
		regenlife = dynamicObjectXML->ToElement()->Attribute("regenlife");
		if (regenlife.size()>0)
			a.regenlife = (int)atoi(regenlife.c_str());

		stringc regenmana = "";
		regenmana = dynamicObjectXML->ToElement()->Attribute("regenmana");
		if (regenmana.size()>0)
			a.regenmana = (int)atoi(regenmana.c_str());

		newObj->setProperties(a);

		// Update the GUI with a description of the current loading task
			stringw nametext="Loading Dynamic object: ";
			nametext.append(newObj->getName().c_str());
			GUIManager::getInstance()->setTextLoader(nametext);

        dynamicObjectXML = parentElement->IterateChildren( "obj", dynamicObjectXML );
    }
	return true;
}

//! Freeze all the NPC and the player object (not moving) on the screen when asked to pause
void DynamicObjectsManager::freezeAll()
{
	for(int i=0;i<(int)objects.size();i++)
    {
		if (objects[i]->getType()==OBJECT_TYPE_NPC || objects[i]->getType()==OBJECT_TYPE_PLAYER)
		{
			scene::IAnimatedMeshSceneNode * nodeanim =(IAnimatedMeshSceneNode*)((DynamicObject*)objects[i])->getNode();
			nodeanim->setAnimationSpeed(0);
		}
    }
}

//! Unfreeze all NPC on the screen when asked to pause
void DynamicObjectsManager::unFreezeAll()
{
	for(int i=0;i<(int)objects.size();i++)
    {
		if (objects[i]->getType()==OBJECT_TYPE_NPC || objects[i]->getType()==OBJECT_TYPE_PLAYER)
		{
			DynamicObject_Animation anim=((DynamicObject*)objects[i])->currentAnim;
			scene::IAnimatedMeshSceneNode * nodeanim =(IAnimatedMeshSceneNode*)((DynamicObject*)objects[i])->getNode();
			nodeanim->setAnimationSpeed(anim.speed);
		}

    }
}


// Here the LUA script of the dynamic object store its pointer
// Useful for functions needing to get what is the latest Dynamic Object
// to call the dialog
void DynamicObjectsManager::setDialogCaller(DynamicObject* object)
{
	dialogCaller=object;
}


//! This function give back the last DynamicObject that called the dialog
//! if not defined then return the pointer to the player dynamic object pointer
DynamicObject* DynamicObjectsManager::getDialogCaller()
{
	if (dialogCaller)
		return dialogCaller;
	else
		return getPlayer();
}

//!the unique name of an dynamic object contains his index at the objects vector
stringc DynamicObjectsManager::createUniqueName()
{
    stringc uniqueName = "dynamic_object_";
    uniqueName += objsCounter++;

    return uniqueName;
}

//! Initialize the scripts (execute) in all the dynamic object list.
void DynamicObjectsManager::initializeAllScripts()
{
    for(int i=0;i<(int)objects.size();i++)
    {
		if (objects[i])
			((DynamicObject*)objects[i])->doScript();
			
    }
}

//! Will display or hide the shadow object under the dynamic objects (used for play/edit)
void DynamicObjectsManager::displayShadow(bool visible)
{
	for(int i=0;i<(int)objects.size();i++)
    {
		// Non interactive objects will not be refreshed (update callback)
		// Should help with performance and allow for more NPC/Interactive objects.
		if (objects[i])
		{
			if (objects[i]->getType()!=OBJECT_TYPE_EDITOR)
			{
				((DynamicObject*)objects[i])->getShadow()->setVisible(visible);
			}
		}
    }
}

//! Main update loop for all the dynamic objects
void DynamicObjectsManager::updateAll()
{

    for(int i=0;i<(int)objects.size();i++)
    {
		// Non interactive objects will not be refreshed (update callback)
		// Should help with performance and allow for more NPC/Interactive objects.
		if (objects[i])
		{
			if (objects[i]->getType()!=OBJECT_TYPE_NON_INTERACTIVE)
			{
				((DynamicObject*)objects[i])->update();
			}
		}
    }

	//Update player code
	Player::getInstance()->update(); // This one is timed now.
	
}

void DynamicObjectsManager::clearAllScripts()
{
    for(int i=0;i<(int)objects.size();i++)
    {
		if (objects[i])
		{
			((DynamicObject*)objects[i])->restoreParams();
			((DynamicObject*)objects[i])->clearScripts();
		}
    }
}

void DynamicObjectsManager::showDebugData(bool show)
{
    for(int i=0;i<(int)objects.size();i++)
	{
		// We don't need to have bounding box and other data over the player
		if (objects[i])
		{
			if (objects[i]->getType()!=OBJECT_TYPE_PLAYER)
			{
				((DynamicObject*)objects[i])->getNode()->setDebugDataVisible( show ? EDS_BBOX | EDS_SKELETON : EDS_OFF );
				objects[i]->objectLabelSetVisible(false);
			}
		}
	}
}

//! This is used for animation blending, need to be called at each update refresh
void DynamicObjectsManager::updateAnimationBlend()
{
	 for(int i=0;i<(int)objects.size();i++)
	{
		// We don't need to have bounding box and other data over the player
		if (objects[i])
		{
			if (objects[i]->getType()==OBJECT_TYPE_PLAYER || objects[i]->getType()==OBJECT_TYPE_NPC)
			{
				((IAnimatedMeshSceneNode*)((DynamicObject*)objects[i])->getNode())->animateJoints();
			}
		}
	}
}


// Put all the object in the idle animation mode. (Play/Edit mode)
void DynamicObjectsManager::objectsToIdle()
{
	 for(int i=0;i<(int)objects.size();i++)
	{
		// We don't need to have bounding box and other data over the player
		if (objects[i])
		{
			if (objects[i]->getType()==OBJECT_TYPE_PLAYER || objects[i]->getType()==OBJECT_TYPE_NPC)
			{
				((DynamicObject*)objects[i])->setAnimation("idle");
				((DynamicObject*)objects[i])->objectLabelSetVisible(false);

			}
		}
	}
}

// Create a meta selector for the collision response animation
// Also needed for every ray collision test (pool of triangle to test from)
// This is used for the enviromment, the NPC will be evaluated by distance (faster)
IMetaTriangleSelector* DynamicObjectsManager::createMeta()
{
	ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();
	meta=smgr->createMetaTriangleSelector();
	ITriangleSelector* triangle=0;

	// Put all the triangle selector into one meta selector.
	for(int i=0;i<(int)objects.size();i++)
	{
		if (objects[i])
		{
			if (objects[i]->isEnabled())
			//if (objects[i]->getLife()>0)
			{
				if (objects[i]->getType()!=OBJECT_TYPE_NPC)
				//Temporary change, won't allow NPC to get into the meta selector
				//if ((objects[i]->getType()==OBJECT_TYPE_NPC && objects[i]->getLife()>0) || (objects[i]->getType()!=OBJECT_TYPE_NPC))
				{
					triangle = objects[i]->getNode()->getTriangleSelector();
					s32 number = triangle->getTriangleCount();
					meta->addTriangleSelector(triangle);
					s32 number2  = meta->getTriangleCount();
				} 
			}
		}
	}
	return meta;
}


IMetaTriangleSelector* DynamicObjectsManager::getMeta()
{
	return meta;
}


//! Clean up the screen by removing everything.
// Might have to fix some details
// Used mostly when the application is closing
void DynamicObjectsManager::clean(bool full)
{

	// Remove all non-templates (old name)
	// "template object" in this term mean, an object that should be kept until 
	// we close the game (player, target object, etc)
	// Will have to change the naming convention as it's now using another approach 
	// (templates are loaded on demand now)
	for(int i=0;i<(int)objects.size();i++)
    {
        DynamicObject* d = objects[i];
		if (d)
		{
			d->clearEnemy();
			if (!d->isTemplate())
			{
				delete d;
          		objects[i]=NULL;

			}
			// else
			//	object_backup.push_back(d);
		}
    }
	// Cleanup
	objects.clear();
	
	if (!full)
		return;
    objsCounter = 0;
	

    activeObject = NULL;
	
	
   // Clearing all template data, need to update
   for(int i=0;i<(int)objTemplate.size();i++)
    {
        TemplateObject* d = objTemplate[i];
		delete d;
        objTemplate.erase(objTemplate.begin() + i);
    }
   objTemplate.clear();

}
