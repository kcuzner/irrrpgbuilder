#include "DynamicObjectsManager.h"
#include "DynamicObject.h"
#include "../camera/CameraSystem.h"
#include "../fx/ShaderCallBack.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

DynamicObjectsManager::DynamicObjectsManager()
{
	device = App::getInstance()->getDevice();
	playerObject=NULL;


	newObj=new TemplateObject("");

	this->loadTemplates();

	//set the initial active object - the list must be 1 or more objs!
	activeObject = this->searchTemplate("frog");
	 //just initialize var
    objsCounter_npc=0;
	objsCounter_regular=0;
	objsCounter_walkable=0;
	objsCounter_others=0;
	dialogCaller = NULL;
	setname=L"";
	objectcounter=0;

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
	if (pObject->getName()=="player_normal" && playerObject==NULL)	
	{
		playerObject = new DynamicObject(pObject->getName(), pObject->meshFile, pObject->animations);
		playerObject->setScale(vector3df(pObject->getScale(),pObject->getScale(),pObject->getScale()));
		playerObject->setTemplateScale(vector3df(pObject->getScale(),pObject->getScale(),pObject->getScale()));
		playerObject->setType(pObject->getType());
			
		playerObject->setTemplate(true);
		objects.push_back(playerObject);
		//setup material
		playerObject->setMaterialType(pObject->getMaterialType());
		playerObject->getNode()->setMaterialFlag(EMF_LIGHTING,true);
		
		
		// Fix a little problem by reseting the animation state back to idle.
		playerObject->setAnimation("walk");
		playerObject->setAnimation("idle");
		
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

	// Check to rename all duplicate names
	this->checkTemplateNames();
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
#ifdef DEBUG
	printf ("Warning: Failed to find the proper object: %s\n",name.c_str());
#endif
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
		core::stringc  objectSpecial = "";
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
		bool inside3 = false;

		// Language counter (using the XML hierachy)
		u32 count = 0;
		u32 linecount = 0;
		u32 npccount = 0;
		u32 lootcount = 0;
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
					if (core::stringw("material") == (core::stringw)xml->getNodeName())
					{

						if (!inside3) 
						{
							App::getInstance()->getDevice()->getLogger()->log(L"Inside the requested XML block (material)!");
							inside3=true;
						}

						core::stringc text="";
						DynamicObject::DynamicObject_material currMaterial;

						//get ID of material
						text = (core::stringc)xml->getAttributeValue("id");
						if (text.size()>0)
							currMaterial.id = (irr::u32)atoi(text.c_str());
						else
							currMaterial.id = 0;
							

						//get The shader of the material
						currMaterial.shader = (core::stringc)xml->getAttributeValue("shader");
						currMaterial.texture0 = (core::stringc)xml->getAttributeValue("texture0");
						currMaterial.texture1 = (core::stringc)xml->getAttributeValue("texture1");
						currMaterial.texture2 = (core::stringc)xml->getAttributeValue("texture2");
						currMaterial.texture3 = (core::stringc)xml->getAttributeValue("texture3");

						newObj->materials.push_back(currMaterial);
						
						//printf("Inside the node for the material!\n");
					}


					if (core::stringw("animation") == (core::stringw)xml->getNodeName())
					{


						DynamicObject::DynamicObject_Animation currAnim;
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
							//printf ("Inside the requested block (animation)!\n");
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
					}

					// Add the main mesh informations
					if (core::stringw("dynamic_object") == xml->getNodeName())
					{
						if (!inside) 
						{
							inside=true;
						}

						
						objectName = (core::stringw)xml->getAttributeValue("name");
						core::stringw uptext = L"Loading template object: ";
						uptext += objectName;
						GUIManager::getInstance()->setTextLoader(uptext);

						//if (oldName!=(core::stringc)newObj->getName())
						if (inside)
						{
							oldName=(core::stringc)newObj->getName();

							// Add the old object to the list,only the pointer is stored
							this->objTemplate.push_back(newObj);

							// Create the new object 
							newObj = new TemplateObject(objectName);
							objectcounter++;
							newObj->id=objectcounter;
						}
						
						newObj->setName((core::stringw)objectName);
						
						// Get the current set name and save it in the object template
						newObj->type=setname;
						newObj->special=DynamicObject::SPECIAL_NONE;

						objectSpecial = (core::stringc)xml->getAttributeValue("special");
						if (objectSpecial=="segment")
							newObj->special=DynamicObject::SPECIAL_SEGMENT;
						
						objectMesh = (core::stringc)xml->getAttributeValue("mesh");
						newObj->meshFile=objectMesh;

						objectType = xml->getAttributeValue("type");
						newObj->setType(objectType);

						// simply count the object types for the statistics
						if (objectType==(core::stringc)"npc")
							npccount++;
						if (objectType==(core::stringc)"loot")
							lootcount++;
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
			
				}
                break;

				case io::EXN_ELEMENT_END:
					if (inside)
					{
						count++;
					}
					inside = false;
					inside2 = false;
					inside3 = false;
					break;
                
				default:
                        break;
                }
        }

		/*
		if (playercount>0)
		{
			printf (">>>>> Current player count is: %d\n",playercount);
		}

		if (npccount>0)
		{
			printf (">>>>> Current npc count is: %d\n",npccount);
		}

		if (propscount>0)
		{
			printf (">>>>> Current prop count is: %d\n",propscount);
		}

		if (editorcount>0)
		{
			printf (">>>>> Current editor object count is: %d\n",editorcount);
		}

		printf (">>>>> Total objects in this set: %d\n",linecount);
		*/

        if (xml)
                xml->drop(); // don't forget to delete the xml reader

		//if (newObj)
		//	delete newObj;

		return true;
}


bool DynamicObjectsManager::loadSet()
{
	// File to seek is: dynamic_objects.xml
	// Will provide the path and "sets" to load
	// read configuration from xml file

	io::IXMLReaderUTF8* xmlmain = App::getInstance()->getDevice()->getFileSystem()->createXMLReaderUTF8("../media/dynamic_objects/dynamic_objects.xml");
	if (!xmlmain)
	{
#ifdef DEBUG
		printf ("Failed to load the dynamic object template list!\n");
#endif
		return false;
	}

	core::stringc  MessageText = "";
	core::stringc  set = "";
	core::stringc  id = "";
	core::stringc  str = "";
	core::stringw  result = L"";

	bool inside = false;

	// Language counter (using the XML hierachy)
	u32 count = 0;
	u32 linecount = 0;
        while(xmlmain && xmlmain->read())
        {
			switch(xmlmain->getNodeType())
            {
				case io::EXN_TEXT:		
					break;

                case io::EXN_ELEMENT:
                {
					// Look for a specified node
					if (core::stringw("dynamic_object") == xmlmain->getNodeName())
					{
						if (!inside) 
						{
							inside=true;
						}
							
						set = xmlmain->getAttributeValue("set");
						setname = (core::stringw)xmlmain->getAttributeValue("name");
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
						break;

                default:
                        break;
                }
        }

		// Add the last object to the template list
		if (newObj)		
			this->objTemplate.push_back(newObj);

		core::stringw countstr = ((core::stringw)L"Object set count: ")+(core::stringw)(linecount);

	    if (xmlmain)
                xmlmain->drop(); // don't forget to delete the xml reader

		return true;
}

DynamicObject* DynamicObjectsManager::createCustomObjectAt(vector3df pos, core::stringc meshfile)
{
  	DynamicObject* newObj = new DynamicObject(L"CUSTOM",meshfile,activeObject->animations,true);
	newObj->setType(DynamicObject::OBJECT_TYPE_NON_INTERACTIVE);
	newObj->setTemplate(false);

	//setup material
	//newObj->getNode()->setMaterialType(activeObject->getMaterialType());
	newObj->getNode()->setMaterialFlag(EMF_LIGHTING,true);
	// Load the script if it was defined in the XML
	
    newObj->setPosition(pos);

    //the unique name of an dynamic object contains his index at the objects vector
	newObj->setName(this->createUniqueName(newObj->getType()));
	// This is the reference name of the template this object is made of.
	newObj->setTemplateObjectName(L"CUSTOM");

	objects.push_back(newObj);
	activeObject2=newObj;

    return newObj;
}


DynamicObject* DynamicObjectsManager::createActiveObjectAt(vector3df pos)
{

  	DynamicObject* newObj = new DynamicObject(activeObject->getName(),activeObject->meshFile,activeObject->animations);
	newObj->setScale(vector3df(activeObject->getScale(),activeObject->getScale(),activeObject->getScale()));
	newObj->setTemplateScale(vector3df(activeObject->getScale(),activeObject->getScale(),activeObject->getScale()));
	newObj->setType(activeObject->getType());
	newObj->setTemplate(false);
	
	

	//setup material
	newObj->getNode()->setMaterialType(activeObject->getMaterialType());
	newObj->getNode()->setMaterialFlag(EMF_LIGHTING,true);

	//Add the materials definitions if they were defined
	if (activeObject->materials.size()>0)
	{
		ShaderCallBack::getInstance()->setMaterials(newObj->getNode(),activeObject->materials); //Apply the shader from the template
		newObj->setMaterials(activeObject->materials); //Save the template info inside the dynamic object
	}

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
			//printf("Opening script file: %s\n",filename.c_str());
			while (! fileScript.eof() )
			{
				std::string line2="";
				getline (fileScript,line);
				// --- tried to display the loading of the script to the console:: DEBUG
				//printf ("Line size is: %d, characters\n",line.size());
				/*int b=line.size();
				for (int a=0; a<b; a++)
				{
					std::string line1 = line.substr(a,1);
					char* charac = (char*)line1.c_str();
			
					unsigned char numeric =unsigned char (*charac);
					
					//printf("%s",charac);<
					
					if ((int)numeric>128)
					{
						line2+=numeric;
						printf ("Extended character!:%d \n",(int)numeric);
					} else
						line2+=numeric;

				}
				//printf("%s \n",line2.c_str());
				// ---
				//
				newScript += line2.c_str();*/
				newScript += line.c_str();
				newScript += '\n';
			}
			fileScript.close();
		}
		newObj->setScript(newScript);
	}

    cout << "TEMPLATE NAME:" << activeObject->getName().c_str() << endl;

	//init the new object and tries to set the animations
	//Doing a "walk - idle" seem to fix a problem.
	//Will have to look if the idle stance is set at default (should be pre-spawn)
    newObj->setPosition(pos);
	newObj->setWalkTarget(pos); //Set the walk target at the current position;

	core::stringc oldname = core::stringc("_").append(newObj->getName());

	newObj->setName(this->createUniqueName(newObj->getType()).append(oldname));
	// This is the reference name of the template this object is made of.
	newObj->setTemplateObjectName(activeObject->getName());


	if (newObj->getType()==DynamicObject::OBJECT_TYPE_NPC)
	{
		newObj->setAnimation("walk");
		newObj->setAnimation("idle");
		newObj->setWalkTarget(pos);
	}
	// Add to the dynamic object list.
	objects.push_back(newObj);
	activeObject2=newObj;

    return newObj;
}

core::stringc DynamicObjectsManager::getScript(stringc uniqueName)
{

	core::stringc result=L"";
	for (int i=0 ; i<(int)objects.size() ; i++)
    {
        if( stringc(objects[i]->getName()) == uniqueName )
        {
			result=objects[i]->getScript();
            break;
        }
    }
	return result;

}

void DynamicObjectsManager::removeObject(stringc uniqueName)
{
    for (int i=0 ; i<(int)objects.size() ; i++)
    {
        if( stringc(objects[i]->getName()) == uniqueName )
        {
#ifdef DEBUG
			printf("Deleting %s\n",uniqueName.c_str());
#endif
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

//Find the filename in the template and use the object as the active object
//Used mostly when the object template name was not found, using the "filename" as reference instead
bool DynamicObjectsManager::findTemplate(stringc filename)
{
	bool found=false;
	for (int i=0 ; i<(int)objTemplate.size() ; i++)
    {
		core::stringc templateFilename=objTemplate[i]->meshFile;
		if(templateFilename == filename )
    	{
    	    activeObject = ((TemplateObject*)objTemplate[i]);
			found=true;
    	    break;
    	} 
    }

	if (found)
		return true;
	else
		return false;
}

//Find the name in the template as use it as "active" one.
//This will be used to create the character or object on the map by the user.
//Normally the "name" is provided by a list.
bool DynamicObjectsManager::setActiveObject(stringc name)
{
	core::stringc notfound=(core::stringc)"Template not found:";
	notfound+=name; notfound+=(core::stringc)"!";
	TemplateObject* oldActive = activeObject;
    bool found=false;
	for (int i=0 ; i<(int)objTemplate.size() ; i++)
    {
		core::stringw templateName=objTemplate[i]->getName();
		if((core::stringc)templateName == name )
    	{
    	    activeObject = ((TemplateObject*)objTemplate[i]);
			found=true;
    	    break;
    	} 
    }

#ifdef DEBUG
	if (found)
		printf ("This item is the active object now: %s \n\n",name.c_str());
#endif 

	if (!found)
	{
		activeObject = oldActive;
		GUIManager::getInstance()->setConsoleText(notfound.c_str(),video::SColor(255,240,0,0));
		return false;
	} else
		return true;

}

//! Provide a list of template objects names for the GUI (Templates) based on the object type/category
//! Used the GUI system to provide a list from the objects in the templates
vector<stringw> DynamicObjectsManager::getObjectsList(core::stringw objectType, core::stringw category, DynamicObject::SPECIAL special)
{
    vector<stringw> listObjs;

    for (int i=0 ; i<(int)objTemplate.size() ; i++)
    {
		if (objTemplate[i]->type==objectType && category=="" && objTemplate[i]->special==special)
				listObjs.push_back( objTemplate[i]->getName() );
		else

		if (objTemplate[i]->type==objectType && objTemplate[i]->category==category && objTemplate[i]->special==special)
		{
			listObjs.push_back( objTemplate[i]->getName() ); 
		}

    }

    return listObjs;
}


vector<stringw> DynamicObjectsManager::getObjectsCollections(DynamicObject::SPECIAL special)
{
	vector<stringw> listObjs;
	
    for (int i=0 ; i<(int)objTemplate.size() ; i++)
    {
		if (objTemplate[i]->special==special)
		{
			bool add=true;
			for (int j=0; j<(int)listObjs.size(); j++)
			{
				if (objTemplate[i]->type==listObjs[j].c_str())
					add=false;
			}
			if (add)
				listObjs.push_back( objTemplate[i]->type ); 
		}

    }

    return listObjs;

}

//! Create a list of the categories from the XML data in the templates.
// Special is to build a list based on the special type of this template (DYNAMIC OBJECT, CUSTOM TILE, LOOT, ETC)
vector<stringw> DynamicObjectsManager::getObjectsListCategories(core::stringw objectType, DynamicObject::SPECIAL special)
{
    vector<stringw> listObjs;
	listObjs.push_back(LANGManager::getInstance()->getText("panel_selcombo_all").c_str());
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
			if (!found && objTemplate[i]->category!="" && objTemplate[i]->special==special)
					listObjs.push_back( objTemplate[i]->category );
			found=false;
		}
    }

    return listObjs;
}

vector<stringw> DynamicObjectsManager::getObjectsSceneList(DynamicObject::TYPE objectType)
{
	vector<stringw> listObjs;

    for (int i=0 ; i<(int)objects.size() ; i++)
    {
		if (objects[i]->getType()==objectType || objectType==DynamicObject::OBJECT_TYPE_NONE)
		{ 
			//Don`t want theses in the list
			if (objects[i]->getType()!=DynamicObject::OBJECT_TYPE_PLAYER && objects[i]->getType()!=DynamicObject::OBJECT_TYPE_EDITOR)
				listObjs.push_back( core::stringw(objects[i]->getName()));
		}
    }

    return listObjs;

}

void DynamicObjectsManager::setObjectsID(DynamicObject::TYPE objectType, s32 ID)
{
    for (int i=0 ; i<(int)objects.size() ; i++)
    {
		if (objects[i]->getType()==objectType)
			objects[i]->getNode()->setID(ID);
    }

}

void DynamicObjectsManager::setObjectsVisible(DynamicObject::TYPE objectType, bool visible)
{
    for (int i=0 ; i<(int)objects.size() ; i++)
    {
		if (objects[i]->getType()==objectType)
			objects[i]->getNode()->setVisible(visible);
    }

}

// Reset the state of walking of all the object types
// Normally used to reset the character states when the game start.
void DynamicObjectsManager::resetObjectsWalkTarget(DynamicObject::TYPE objectType)
{
    for (int i=0 ; i<(int)objects.size() ; i++)
    {
		if (objects[i]->getType()==objectType)
		{
			// Will need to inquire the NPC to know if he have a pre-spawn state.
			// Those object would need a prespawn state instead of idle
			objects[i]->setAnimation("idle");
		}
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
			//printf("this object is number %d and it's name is: %s\n",i,objects[i]->getTemplateObjectName());
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

		// Old method, was "dumping" directly the type as a INT - BAD, the reference could be changed
		//dynamicObjectXML->SetAttribute("type",(int)objects[i]->getType());

		// Cleaner way to save the type of the dynamic object in the project
		switch (objects[i]->getType())
		{
			case DynamicObject::OBJECT_TYPE_NPC:
				dynamicObjectXML->SetAttribute("type","npc");
				break;

			case DynamicObject::OBJECT_TYPE_INTERACTIVE:
				dynamicObjectXML->SetAttribute("type","interactive");
				break;

			case DynamicObject::OBJECT_TYPE_NON_INTERACTIVE:
				dynamicObjectXML->SetAttribute("type","non-interactive");
				break;

			case DynamicObject::OBJECT_TYPE_WALKABLE:
				dynamicObjectXML->SetAttribute("type","walkable");
				break;

			case DynamicObject::OBJECT_TYPE_PLAYER:
				dynamicObjectXML->SetAttribute("type","player");
				break;

			case DynamicObject::OBJECT_TYPE_EDITOR:
				dynamicObjectXML->SetAttribute("type","editor");
				break;

			case DynamicObject::OBJECT_TYPE_LOOT:
				dynamicObjectXML->SetAttribute("type","loot");
				break;

			default: //Should not be triggered
				dynamicObjectXML->SetAttribute("type","none");
				break;


		}
		dynamicObjectXML->SetAttribute("x",stringc(this->objects[i]->getPosition().X).c_str());
		dynamicObjectXML->SetAttribute("y",stringc(this->objects[i]->getPosition().Y).c_str());
		dynamicObjectXML->SetAttribute("z",stringc(this->objects[i]->getPosition().Z).c_str());
	
		// Values stored for the rotation of the object
		dynamicObjectXML->SetAttribute("rx",stringc(this->objects[i]->getRotation().X).c_str());
		dynamicObjectXML->SetAttribute("ry",stringc(this->objects[i]->getRotation().Y).c_str());
		dynamicObjectXML->SetAttribute("rz",stringc(this->objects[i]->getRotation().Z).c_str());

		// Values stored for the scale of the object
		dynamicObjectXML->SetAttribute("sx",stringc(this->objects[i]->getScale().X).c_str());
		dynamicObjectXML->SetAttribute("sy",stringc(this->objects[i]->getScale().Y).c_str());
		dynamicObjectXML->SetAttribute("sz",stringc(this->objects[i]->getScale().Z).c_str());


		dynamicObjectXML->SetAttribute("template",objects[i]->templateObjectName.c_str());

		dynamicObjectXML->SetAttribute("filename",objects[i]->fileName.c_str());

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

	// Need to find a better way to delete this when the file is saved.
	//if (dynamicObjectsXML2)
	//	delete(dynamicObjectsXML2);
}

//! Load back data from XML into the dynamic objects (TinyXML)
bool DynamicObjectsManager::loadFromXML(TiXmlElement* parentElement)
{
    TiXmlNode* dynamicObjectXML = parentElement->FirstChild( "obj" );

    while( dynamicObjectXML != NULL )
    {

		stringc templateObj = "";
		stringc fileObj= "";
		DynamicObject::TYPE type=DynamicObject::OBJECT_TYPE_NONE;
		DynamicObject* newObj = NULL;

		stringc stype = dynamicObjectXML->ToElement()->Attribute("type");
		// Get the type of the object that was saved (if there is one)
		
		
		// New way of getting type. A number is not clear enough
		//OBJECT_TYPE_NONE = 0,
		//OBJECT_TYPE_NPC = 1,
		//OBJECT_TYPE_INTERACTIVE = 2,
		//OBJECT_TYPE_NON_INTERACTIVE = 3,
		//OBJECT_TYPE_WALKABLE = 4,
		//OBJECT_TYPE_PLAYER	= 5,
		//OBJECT_TYPE_EDITOR	= 6,
		//OBJECT_TYPE_LOOT = 7,

		//Old method, could cause problems
		//if (stype.size()>0)
		//	type=atoi(stype.c_str());

		if (stype.size()>0)
		{
			if (stype=="1" || stype=="npc")
				type=DynamicObject::OBJECT_TYPE_NPC;

			if (stype=="2" || stype=="interactive")
				type=DynamicObject::OBJECT_TYPE_INTERACTIVE;

			if (stype=="3" || stype=="non-interactive")
				type=DynamicObject::OBJECT_TYPE_NON_INTERACTIVE;

			if (stype=="4" || stype=="walkable")
				type=DynamicObject::OBJECT_TYPE_WALKABLE;

			if (stype=="5" || stype=="player")
				type=DynamicObject::OBJECT_TYPE_PLAYER;

			if (stype=="6" || stype=="editor")
				type=DynamicObject::OBJECT_TYPE_EDITOR;

			if (stype=="7" || stype=="loot")
				type=DynamicObject::OBJECT_TYPE_LOOT;
		}

        stringc script = dynamicObjectXML->ToElement()->Attribute("script");

		//Get the position of the object
        f32 posX = (f32)atof(dynamicObjectXML->ToElement()->Attribute("x"));
        f32 posY = (f32)atof(dynamicObjectXML->ToElement()->Attribute("y"));
        f32 posZ = (f32)atof(dynamicObjectXML->ToElement()->Attribute("z"));

		//Get the rotation (old and new)
		core::stringc tempv=dynamicObjectXML->ToElement()->Attribute("r");

		//Better and more failsafe way of loading data from XML.
		f32 rot = 0.0f;
		f32 rotX = 0.0f;
		f32 rotY = 0.0f;
		f32 rotZ = 0.0f;
		f32 sclX = 0.0f;
		f32 sclY = 0.0f;
		f32 sclZ = 0.0f;

		if (tempv!="")
			rot=(f32)atof(tempv.c_str());

		tempv = dynamicObjectXML->ToElement()->Attribute("rx");
		if (tempv!="")
			rotX = (f32)atof(tempv.c_str());

		tempv = dynamicObjectXML->ToElement()->Attribute("ry");
        if (tempv!="")
			rotY = (f32)atof(tempv.c_str());

		tempv = dynamicObjectXML->ToElement()->Attribute("rz");
        if (tempv!="")
			rotZ = (f32)atof(tempv.c_str());

		//Get the scale
		tempv = dynamicObjectXML->ToElement()->Attribute("sx");
		if (tempv!="")
			sclX = (f32)atof(tempv.c_str());

		tempv = dynamicObjectXML->ToElement()->Attribute("sy");
        if (tempv!="")
			sclY = (f32)atof(tempv.c_str());
        
		tempv = dynamicObjectXML->ToElement()->Attribute("sz");
		if (tempv!="")
			sclZ = (f32)atof(tempv.c_str());

		// Create an object from the template
		if (type!=DynamicObject::OBJECT_TYPE_PLAYER)
		{ 

			templateObj = dynamicObjectXML->ToElement()->Attribute("template");
			fileObj = dynamicObjectXML->ToElement()->Attribute("filename");
			
			bool result=false;
			// If the "filename" was stored in the XML, then use this for retrieving the proper template
			// if not, then trie to load based on the template name.
			if (fileObj!="")
				result = findTemplate(fileObj);
			else
				result = setActiveObject(templateObj);
			
			
			// If those fail then will use the error mesh
			if (!result)
			{
				return false;
				//setActiveObject("error");
			}
			if (activeObject->getName()!="") //If the object has no name should not be created.
				newObj = createActiveObjectAt(vector3df(posX,posY,posZ));
		} 
		else
		{
			// If this is the player, retrieve only it's position (permanent dynamic object)
			// Will need to update player data.
			newObj = this->playerObject;
			newObj->setPosition(vector3df(posX,posY,posZ));
			newObj->setTemplateScale(newObj->getScale());
			
		}
		
		if (activeObject->getName()!="") //	Failsafe if for some reason there is no object found at all in the system
		{
			// if the "r" is not 0 then use this information to set the rotation of the model (compatibility mode)
			if (rot!=0 )
				newObj->setRotation(vector3df(0,rot,0));
			else
			{
				// This is the new rotation model. User can set any angle.
				newObj->setRotation(vector3df(rotX,rotY,rotZ));
			}

			//If there is no scale information then don't set the scale, let it as the template was.
			if ((sclX+sclY+sclZ)>0)
				newObj->setScale(vector3df(sclX,sclY,sclZ));


			// If a script is assigned to the mesh then load it.
			core::stringw ss = convert(script) ;

			newObj->setScript(ss);
			//newObj->setScript(script);
	
        
			DynamicObject::cproperty a=newObj->initProperties();
		
			// Default properties values for the player and the NPCS
			if (type==DynamicObject::OBJECT_TYPE_NPC || type==DynamicObject::OBJECT_TYPE_PLAYER)
			{
				// If LUA or a loaded value redefine a properties, it will override thoses values
				a.experience = 10; // for a NPC this will give 10 XP to the attacker if he win
				a.mindamage=1;
				a.maxdamage=3;
				a.life = 100;
				a.maxlife=100;
				a.hurt_resist=50;
				if (type==DynamicObject::OBJECT_TYPE_PLAYER)
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
	}

        dynamicObjectXML = parentElement->IterateChildren( "obj", dynamicObjectXML );
    }
	return true;
}


// check if there is duplicate names and then rename
void DynamicObjectsManager::checkTemplateNames()
{
	for (int j=0 ; j<(int)objTemplate.size() ; j++) 
	{
		//printf ("here checking for duplicate names: %i\n",j);
		irr::u32 duplicatecounter=1;
		for (int i=0 ; i<(int)objTemplate.size() ; i++)
		{
			//printf ("Name %s = %s? \n",((core::stringc)objTemplate[i]->getName()).c_str(),((core::stringc)objTemplate[j]->getName()).c_str());
			if (objTemplate[i]->getName()==objTemplate[j]->getName() && i!=j)
			{ 
				core::stringw newname = objTemplate[i]->getName();
				newname+=L"(";
				newname+=(core::stringw)duplicatecounter;
				newname+=L")";
				//printf("Found a duplicate name!!! %s\n",((core::stringc)newname).c_str());
				objTemplate[i]->setName(newname);
				duplicatecounter++;
			}
		}
	}
}

//! Freeze all the NPC and the player object (not moving) on the screen when asked to pause
void DynamicObjectsManager::freezeAll()
{
	for(int i=0;i<(int)objects.size();i++)
    {
		if (objects[i]->getType()==DynamicObject::OBJECT_TYPE_NPC || objects[i]->getType()==DynamicObject::OBJECT_TYPE_PLAYER)
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
		if (objects[i]->getType()==DynamicObject::OBJECT_TYPE_NPC || objects[i]->getType()==DynamicObject::OBJECT_TYPE_PLAYER)
		{
			DynamicObject::DynamicObject_Animation anim=((DynamicObject*)objects[i])->currentAnim;
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
stringc DynamicObjectsManager::createUniqueName(DynamicObject::TYPE objtype)
{
	stringc uniqueName="";

	if (objtype==DynamicObject::OBJECT_TYPE_NPC)
	{
		uniqueName = "dynamic_object_npc_";
		uniqueName += objsCounter_npc++;
		return uniqueName;
	}

	if (objtype==DynamicObject::OBJECT_TYPE_INTERACTIVE || objtype==DynamicObject::OBJECT_TYPE_NON_INTERACTIVE || objtype==DynamicObject::OBJECT_TYPE_LOOT)
	{
		uniqueName = "dynamic_object_";
		uniqueName += objsCounter_regular++;
		return uniqueName;
	}

	if (objtype==DynamicObject::OBJECT_TYPE_WALKABLE)
	{
		uniqueName = "dynamic_walkable_";
		uniqueName += objsCounter_walkable++;
		return uniqueName;
	}


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
			if (objects[i]->getType()!=DynamicObject::OBJECT_TYPE_EDITOR)
			{
				((DynamicObject*)objects[i])->getShadow()->setVisible(visible);
			}
		}
    }
}

//! Main update loop for all the dynamic objects
void DynamicObjectsManager::updateAll()
{

	bool foundplayer = false;
    for(int i=0;i<(int)objects.size();i++)
    {
		// Non interactive objects will not be refreshed (update callback)
		// Should help with performance and allow for more NPC/Interactive objects.
		if (objects[i])
		{

			if (objects[i]->getType()==DynamicObject::OBJECT_TYPE_PLAYER)
				foundplayer=true;
			if (objects[i]->getType()!=DynamicObject::OBJECT_TYPE_NON_INTERACTIVE  || objects[i]->getType()!=DynamicObject::OBJECT_TYPE_WALKABLE )
			{
				((DynamicObject*)objects[i])->update();
			}
		}
		if (!foundplayer)
			printf("The player was lost!!\n");
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
			if (objects[i]->getType()!=DynamicObject::OBJECT_TYPE_PLAYER)
			{
				((DynamicObject*)objects[i])->getNode()->setDebugDataVisible( show ? EDS_BBOX | EDS_SKELETON : EDS_OFF );
				objects[i]->objectLabelSetVisible(false);
			}
		}
	}
}

//! This is used for animation blending, need to be called at each update refresh
//! Since Irrlicht 1.8 this seem to cause problems with certain models
void DynamicObjectsManager::updateAnimationBlend()
{
	 for(int i=0;i<(int)objects.size();i++)
	{
		// We don't need to have bounding box and other data over the player
		if (objects[i])
		{
			if (objects[i]->getType()==DynamicObject::OBJECT_TYPE_PLAYER || objects[i]->getType()==DynamicObject::OBJECT_TYPE_NPC)
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
			if (objects[i]->getType()==DynamicObject::OBJECT_TYPE_PLAYER || objects[i]->getType()==DynamicObject::OBJECT_TYPE_NPC)
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
				if (objects[i]->getType()!=DynamicObject::OBJECT_TYPE_NPC)
				//Temporary change, won't allow NPC to get into the meta selector
				//if ((objects[i]->getType()==OBJECT_TYPE_NPC && objects[i]->getLife()>0) || (objects[i]->getType()!=OBJECT_TYPE_NPC))
				{
					triangle = objects[i]->getNode()->getTriangleSelector();
					s32 number = triangle->getTriangleCount();
					meta->addTriangleSelector(triangle);
					s32 number2  = meta->getTriangleCount();
					triangle->drop();
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

//! Add the player object pointer back in the dynamic object list
//! Used after clearing a scene
void DynamicObjectsManager::setPlayer()
{
	// Will add the player object to the list if the list is "live"
	//if (objects.size()>0)
		objects.push_back(playerObject);
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
			if (d->getType()!=DynamicObject::OBJECT_TYPE_PLAYER)
			{
				d->clearEnemy();
				if (!d->isTemplate())
				{
					delete d;
          			objects[i]=NULL;
				}
			}
		}
    }
	// Cleanup
	objects.clear();
	objects.push_back(playerObject); //Put back the player object pointer in the list after deleting it
	objsCounter_npc=0;
	objsCounter_regular=0;
	objsCounter_walkable=0;
	objsCounter_others=0;
	
	if (!full)
		return;
    
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

//! Return a list of dynamic objects around the given position
vector<DynamicObject*> DynamicObjectsManager::getObjectNearPosition(vector3df pos, f32 radius, DynamicObject::TYPE type)
{
	vector<DynamicObject*> list; // perhap will have to make it part of the class (LUA)
	for(int i=0;i<(int)objects.size();i++)
    {
        DynamicObject* d = objects[i];
		if (d)
		{
			if ((d->getType()==type) && (pos.getDistanceFrom(d->getPosition())<radius))
			{
				list.push_back(d);
			}
		}
    }
	
	return list;
}