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

	// Load the definition for all dynamic objects
	//ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();
	stringc pathFile = "../media/dynamic_objects/";
    //Load all objects from xml file

    TiXmlDocument doc("../media/dynamic_objects/dynamic_objects.xml");

    //try to parse the XML
	if (!doc.LoadFile())
    {
        cout << "ERROR LOADING DYNAMIC_OBJECTS.XML" << endl;
        exit(0);
    }

    #ifdef APP_DEBUG
    cout << "DEBUG : XML : LOADING DYNAMIC_OBJECTS.XML" << endl;
    #endif

    //locate root node
    TiXmlElement* root = doc.FirstChildElement( "IrrRPG_Builder_DynamicObjects" );

    if ( root )
    {
        //check file version
        if( atof(root->Attribute("version"))!= APP_VERSION )
        {
            #ifdef APP_DEBUG
            cout << "DEBUG : XML : INCORRECT DYNAMIC_OBJECTS.XML VERSION!" << endl;
            #endif

            exit(0);
        }

        vector<stringc> objsIDs;

        TiXmlNode* currentObjXML = root->FirstChild( "dynamic_object" );

        //Iterate dynamic_objects
		GUIManager::getInstance()->setTextLoader(L"Loading Dynamic objects lists...");
        while( currentObjXML != NULL )
        {
            //Get Dynamic Object Attributes
            stringc set = currentObjXML->ToElement()->Attribute("set");
			currentObjXML = root->IterateChildren( "dynamic_object", currentObjXML );

			//Process and parse the files if the set are found into the XML
			if (set.size()>0)
				processFile(set.c_str());

		}
	}

	//set the initial active object - the list must be 1 or more objs!
    activeObject = objectsTemplate[0];


    //just initialize var
    objsCounter = 0;
	objectCounter = 0;
	dialogCaller = NULL;

	// Collision creation (in steps)
	collisionCounter = 0;
	createcollisions=true;
	 cout << "DEBUG : XML : finished!" << endl;

}

DynamicObjectsManager::~DynamicObjectsManager()
{
    //dtor
}


bool DynamicObjectsManager::processFile(stringc filename)
{
	ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();
	stringc pathFile = "../media/dynamic_objects/";
    //Load all objects from xml file
    TiXmlDocument doc(filename.c_str());

    //try to parse the XML
	if (!doc.LoadFile())
    {
        cout << "ERROR LOADING DYNAMIC_OBJECTS.XML" << endl;
        exit(0);
    }

    #ifdef APP_DEBUG
    cout << "DEBUG : XML : LOADING DYNAMIC_OBJECTS.XML" << endl;
    #endif

    //locate root node
    TiXmlElement* root = doc.FirstChildElement( "IrrRPG_Builder_DynamicObjects" );

    if ( root )
    {
        //check file version
        if( atof(root->Attribute("version"))!= APP_VERSION )
        {
            #ifdef APP_DEBUG
            cout << "DEBUG : XML : INCORRECT DYNAMIC_OBJECTS.XML VERSION!" << endl;
            #endif

            exit(0);
        }

        vector<stringc> objsIDs;

        TiXmlNode* currentObjXML = root->FirstChild( "dynamic_object" );

        //Iterate dynamic_objects

        while( currentObjXML != NULL )
        {
            //Get Dynamic Object Attributes
            stringc name = currentObjXML->ToElement()->Attribute("name");
            stringc mesh = currentObjXML->ToElement()->Attribute("mesh");
            stringc scriptname = currentObjXML->ToElement()->Attribute("script");
			stringc type = currentObjXML->ToElement()->Attribute("type");
            stringc scale = currentObjXML->ToElement()->Attribute("scale");
            stringc materialType = currentObjXML->ToElement()->Attribute("materialType");

			// Update the GUI with a description of the current loading task
			stringw nametext="Loading Dynamic object: ";
			nametext.append(name.c_str());
			GUIManager::getInstance()->setTextLoader(nametext);


			//Read Object Animations
            TiXmlNode* currentAnimXML = currentObjXML->FirstChild( "animation" );

            vector<DynamicObject_Animation> animations;

            //Iterate animations
            while( currentAnimXML != NULL )
            {
                DynamicObject_Animation currAnim;

                currAnim.name = currentAnimXML->ToElement()->Attribute("name");
				stringw nametextanim = nametext;
				nametextanim.append(L", adding animation:");
				nametextanim.append(currAnim.name.c_str());
				GUIManager::getInstance()->setTextLoader(nametextanim);

				//GUIManager::getInstance()->setTextLoader(L"Loading Dynamic objects animation...");

				// Load the name of the animation mesh (if there is any)
				// If there is no mesh name, then will be "undefined"
				currAnim.meshname = currentAnimXML->ToElement()->Attribute("mesh");
				if (currAnim.meshname.size()==0)
				{
					currAnim.meshname = L"undefined";
					currAnim.mesh = NULL;
				}
				else
					currAnim.mesh = smgr->getMesh(pathFile+currAnim.meshname);

				// load the startframe for the current animation name
				// Default value is 0 as the first frame of animation
                stringc s_start = currentAnimXML->ToElement()->Attribute("start");
				if (s_start.size()>0)
					currAnim.startFrame = atoi(s_start.c_str());
				else
					currAnim.startFrame=0;

				// load the endframe for the current animation name
				// Default value is the start frame
                stringc s_end = currentAnimXML->ToElement()->Attribute("end");
				if (s_end.size()>0)
					currAnim.endFrame = atoi(s_end.c_str());
				else
					currAnim.endFrame=currAnim.startFrame;


				// Quick patch.. There something wrong with the loading here
				if (currAnim.endFrame==0)
					currAnim.endFrame=1;

				// TODO: Not totally implemented,
				// Sound file name to play when the animation event start
				currAnim.sound = currentAnimXML->ToElement()->Attribute("sound");

				// TODO: Not totally implemented
				// Specify at what frame the audio will start on the animation
				// the default value will be the first frame of animation
				// The audio string will determine is there sound to be played back
				stringc s_sound = currentAnimXML->ToElement()->Attribute("soundevent");
				if (s_sound.size()>0)
					currAnim.soundevent = atoi(s_sound.c_str());
				else
					currAnim.soundevent = currAnim.startFrame;

				// TODO: Not totally implemented, need to be redone in the dynamic object check
				// Specify at what frame the attack will occur (causing damage/impact)
				// -1 is the default value (not defined)
				// A value will trigger the combat system to cause damage
				stringc s_attack = currentAnimXML->ToElement()->Attribute("attackevent");
				if (s_attack.size()>0)
					currAnim.attackevent = atoi(s_attack.c_str());
				else
					currAnim.attackevent= -1;

				// Retrieve the speed of the animation, default to 30fps
				stringc a_speed = currentAnimXML->ToElement()->Attribute("speed");
				if (a_speed.size()>0)
					currAnim.speed = (f32)atof(a_speed.c_str());
				else
					currAnim.speed = 30.0f;

				// Load the walking speed of the NPC or Player (unit by 1/60th of a second)
				// Current default will move 60 inches (default unit) per second.
				// Something is strange, ATOF function round the number on the wxWidget build.
				stringc s_wspeed = currentAnimXML->ToElement()->Attribute("movespeed");
				if (s_wspeed.size()>0)
					currAnim.walkspeed = (f32)atof(s_wspeed.c_str());
				else
					currAnim.walkspeed = 0.0f;



				// Check for an defined animation loop mode. Default is set to true (looping)
				stringc s_loop = currentAnimXML->ToElement()->Attribute("loop");
				if (s_loop.size()>0 && s_loop==L"false")
					currAnim.loop = false;
				else
					currAnim.loop = true;

                currentAnimXML = currentObjXML->IterateChildren( "animation", currentAnimXML );
                animations.push_back(currAnim);

				// Update the gui while loading

            }

            // -- Create Dynamic Object --
            DynamicObject* newObj = new DynamicObject(name, mesh, animations);
			newObj->setType(type);

			// Load the script if it was defined in the XML
		    if (scriptname.size()>1)
			{
				stringc newScript = "";
				stringc filename = "../media/scripts/";
				filename += scriptname;

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

            //setup material
            E_MATERIAL_TYPE mat = EMT_SOLID;
            if(materialType == stringc("transparent_1bit")) mat = EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
            if(materialType == stringc("transparent_8bit")) mat = EMT_TRANSPARENT_ALPHA_CHANNEL;

            newObj->setMaterialType(mat);
			newObj->getNode()->setMaterialFlag(EMF_LIGHTING,true);

			// Set the scale only if it was written
			if (scale.size()>0)
					newObj->setScale(vector3df((f32)atof(scale.c_str()),(f32)atof(scale.c_str()),(f32)atof(scale.c_str())));

			// For the player class. The player is not a template. Could be used for other non template objects (editor objects)
			if (type=="player")
			{
				playerObject=newObj; // Shortcut for directly accessing the player dynamic object
				playerObject->setTemplate(true);
				objects.push_back(newObj);  // The player object is added to the list of the active dynamic objects (refresh)
			} else if(type=="editor" && name=="target")
			{
				targetObject=newObj;
				targetObject->setTemplate(true);
				targetObject->getNode()->setVisible(false);
				targetObject->getNode()->setDebugDataVisible( false ? EDS_BBOX | EDS_SKELETON : EDS_OFF);
			}
			else
			{	// other objects that are used as templates
				newObj->setTemplateObjectName(name);
				newObj->getNode()->setVisible(false);
				newObj->setTemplate(true);
				//store the new object
				objectsTemplate.push_back(newObj);
				objsIDs.push_back(name);
			}
            currentObjXML = root->IterateChildren( "dynamic_object", currentObjXML );
		}//while

    }//root
	return true;
}

bool DynamicObjectsManager::loadTemplates()
{
	return true;
}

DynamicObject* DynamicObjectsManager::createActiveObjectAt(vector3df pos)
{
    DynamicObject* newObj = activeObject->clone();

    cout << "TEMPLATE NAME:" << activeObject->getName().c_str() << endl;

    newObj->setPosition(pos);

	// Add to the dynamic object list.
	objects.push_back(newObj);

    //the unique name of an dynamic object contains his index at the objects vector
    newObj->setName(this->createUniqueName());
	newObj->setScript(activeObject->getScript());

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

DynamicObjectsManager* DynamicObjectsManager::getInstance()
{
    static DynamicObjectsManager *instance = 0;
    if (!instance) instance = new DynamicObjectsManager();
    return instance;
}

DynamicObject* DynamicObjectsManager::getActiveObject()
{
    return activeObject;
}

void DynamicObjectsManager::setActiveObject(stringc name)
{
    for (int i=0 ; i<(int)objectsTemplate.size() ; i++)
    {
    	if( ((DynamicObject*)objectsTemplate[i])->getName() == name )
    	{
    	    activeObject = ((DynamicObject*)objectsTemplate[i]);
    	    break;
    	}
    }
}

vector<stringc> DynamicObjectsManager::getObjectsList(TYPE objectType)
{
    vector<stringc> listObjs;

    for (int i=0 ; i<(int)objectsTemplate.size() ; i++)
    {
		if (objectsTemplate[i]->getType()==objectType)
    		listObjs.push_back( objectsTemplate[i]->getName() );
    }

    return listObjs;
}

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

DynamicObject* DynamicObjectsManager::getPlayer()
{
	return playerObject;
}

DynamicObject* DynamicObjectsManager::getTarget()
{
	return targetObject;
}

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
		
		// Default values for the player and the NPCS
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

void DynamicObjectsManager::freezeAll()
// Freeze all NPC on the screen when asked to pause
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

void DynamicObjectsManager::unFreezeAll()
// Unfreeze all NPC on the screen when asked to pause
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


// This function give back the last DynamicObject that called the dialog
DynamicObject* DynamicObjectsManager::getDialogCaller()
{
	if (dialogCaller)
		return dialogCaller;
	else
		return getPlayer();
}

stringc DynamicObjectsManager::createUniqueName()
{
    //the unique name of an dynamic object contains his index at the objects vector
    stringc uniqueName = "dynamic_object_";
    uniqueName += objsCounter++;

    return uniqueName;
}

void DynamicObjectsManager::initializeAllScripts()
{
    for(int i=0;i<(int)objects.size();i++)
    {
		if (objects[i])
			((DynamicObject*)objects[i])->doScript();
			
    }
}

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

void DynamicObjectsManager::updateAll()
{

	// New method.. Should be more efficient.

	// Use an object counter
	// each iteration will update a specific object, one by one
	// So at each cycle only one object is refreshed.
	/*objectCounter++;
	if (objectCounter>(int)objects.size()-1)
	{
		objectCounter=0;
		Player::getInstance()->update(); // This one is timed now.
	}

	if (objects[objectCounter])
	{
		if (objects[objectCounter]->getType()!=OBJECT_TYPE_NON_INTERACTIVE)
		{
			((DynamicObject*)objects[objectCounter])->update();
		}
	}*/

	// Have to redo this. This is less than efficient for a game!
	// This will refresh the objects in a single "strike", but will remove control for other things.
	// Will be bad.

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
	
	if (createcollisions)
		initializeCollisions();
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

IMetaTriangleSelector* DynamicObjectsManager::createMeta()
// Create a meta selector for the collision response animation
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
				if ((objects[i]->getType()==OBJECT_TYPE_NPC && objects[i]->getLife()>0) || (objects[i]->getType()!=OBJECT_TYPE_NPC))
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

void DynamicObjectsManager::startCollisions()
{
	collisionCounter=0;
	createcollisions=true;
}

void DynamicObjectsManager::initializeCollisions()
{
	ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();

	// Create the collision response animator for each NPC & Player.
	// Done at each update until the list is completed. Should give some time for other tasks.

	// Could perhaps do the meta the same way (Meta creation and this take the longest time)
	if (objects[collisionCounter])
	{
		if (objects[collisionCounter]->getType()==OBJECT_TYPE_NPC || objects[collisionCounter]->getType()==OBJECT_TYPE_PLAYER)
		{
			//if (objects[collisionCounter]->isEnabled())
			if (objects[collisionCounter]->getLife()>0)
			{
				createMeta();

				meta->removeTriangleSelector(objects[collisionCounter]->getNode()->getTriangleSelector());

				ISceneNodeAnimatorCollisionResponse* coll = smgr->createCollisionResponseAnimator(meta,objects[collisionCounter]->getNode(),vector3df(32.0f,72.0f,32.0f),vector3df(0,0,0));
				objects[collisionCounter]->getNode()->addAnimator(coll);
				objects[collisionCounter]->setAnimator(coll);

				meta->drop();
			}
		}
	}
	collisionCounter++;
	if (collisionCounter>=(int)objects.size())
	{
		collisionCounter=0;
		createcollisions=false;
	}
}

void DynamicObjectsManager::clearCollisions()
{

	// Remove the collision animators from the objects
	for(int i=0;i<(int)objects.size();i++)
    {
		if (objects[i])
		{
			core::list<ISceneNodeAnimator*>::ConstIterator begin = objects[i]->getNode()->getAnimators().begin();
			core::list<ISceneNodeAnimator*>::ConstIterator end = objects[i]->getNode()->getAnimators().end();

			for(int it=0; begin != end; ++it )
			{
				ISceneNodeAnimator* pAnim = *begin;
				if( pAnim->getType() == ESNAT_COLLISION_RESPONSE )
				{
					objects[i]->getNode()->removeAnimator(*begin);
					break;
				}
			}
		}
	}
}
IMetaTriangleSelector* DynamicObjectsManager::getMeta()
{
	return meta;
}

void DynamicObjectsManager::updateMetaSelector()
{
	// Update all the collision object (Mostly caused by an object being removed
	clearCollisions();
	createcollisions=true;
	collisionCounter=0;
}

void DynamicObjectsManager::clean(bool full)
{

	collisionResponseAnimators.clear();
	vector<DynamicObject*> object_backup;
	object_backup.clear();

	// Remove all non-templates
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

			} else
				object_backup.push_back(d);
		}
    }
	// Cleanup
	objects.clear();
	objects=object_backup;
	object_backup.clear();


	if (!full)
		return;
    objsCounter = 0;
	// Temporarily commented out, was crashing when closing the game in play mode
	// This doesnt seem to cause a memory leak for now.
  /*  for(int i=0;collisionResponseAnimators.size();i++)
    {
        ISceneNodeAnimatorCollisionResponse* sc = collisionResponseAnimators[i];

        sc->drop();
    }*/

    activeObject = NULL;
	 /*
    for(int i=0;i<objectsTemplate.size();i++)
    {
        DynamicObject* d = objectsTemplate[i];

        delete d;
    }*/

	//objectsTemplate.clear();

	for(int i=0;i<(int)objects.size();i++)
    {
        DynamicObject* d = objects[i];
		delete d;
        objects.erase(objects.begin() + i);
    }

   objects.clear();

}
