#include "DynamicObjectsManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

DynamicObjectsManager::DynamicObjectsManager()
{
	// Load the definition for all dynamic objects
	ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();
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

			//Read Object Animations
            TiXmlNode* currentAnimXML = currentObjXML->FirstChild( "animation" );

            vector<DynamicObject_Animation> animations;

            //Iterate animations
            while( currentAnimXML != NULL )
            {
                DynamicObject_Animation currAnim;

                currAnim.name = currentAnimXML->ToElement()->Attribute("name");
				
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
				stringc s_wspeed = currentAnimXML->ToElement()->Attribute("walkspeed");
				if (s_wspeed.size()>0) 
					currAnim.walkspeed = (f32)atof(s_wspeed.c_str()); 
				else 
					currAnim.walkspeed = 1.0f;

				// Check for an defined animation loop mode. Default is set to true (looping)
				stringc s_loop = currentAnimXML->ToElement()->Attribute("loop");
				if (s_loop.size()>0 && s_loop==L"false") 
					currAnim.loop = false; 
				else 
					currAnim.loop = true;

                currentAnimXML = currentObjXML->IterateChildren( "animation", currentAnimXML );
                animations.push_back(currAnim);
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
				objects.push_back(newObj);  // The player object is added to the list of the active dynamic objects (refresh)
			}
			else
			{	// other objects that are used as templates 
				newObj->setTemplateObjectName(name);
				newObj->getNode()->setVisible(false);
				//store the new object
				objectsTemplate.push_back(newObj);
				objsIDs.push_back(name);
			}

            currentObjXML = root->IterateChildren( "dynamic_object", currentObjXML );

			

        }//while

    }//root
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

vector<stringc> DynamicObjectsManager::getObjectsList()
{
    vector<stringc> listObjs;

    for (int i=0 ; i<(int)objectsTemplate.size() ; i++)
    {
    	listObjs.push_back( objectsTemplate[i]->getName() );
    }

    return listObjs;
}

DynamicObject* DynamicObjectsManager::getObjectByName(stringc name)
{
    for (int i=0 ; i< (int)objects.size() ; i++)
    {
    	if( objects[i]->getName() == name ) return objects[i];
    }

    return NULL;
}

DynamicObject* DynamicObjectsManager::getPlayer()
{
	return playerObject;
}

void DynamicObjectsManager::saveToXML(TiXmlElement* parentElement)
{
    //write header
    TiXmlElement* dynamicObjectsXML = new TiXmlElement("dynamic_objects");

    for ( int i = 0 ; i < (int)objects.size() ; i++ )
    {
        objects[i]->saveToXML(dynamicObjectsXML);
    }

    parentElement->LinkEndChild(dynamicObjectsXML);
}

bool DynamicObjectsManager::loadFromXML(TiXmlElement* parentElement)
{
    TiXmlNode* dynamicObjectXML = parentElement->FirstChild( "obj" );

    while( dynamicObjectXML != NULL )
    {
        stringc script = dynamicObjectXML->ToElement()->Attribute("script");
        stringc templateObj = dynamicObjectXML->ToElement()->Attribute("template");

        f32 posX = (f32)atof(dynamicObjectXML->ToElement()->Attribute("x"));
        f32 posY = (f32)atof(dynamicObjectXML->ToElement()->Attribute("y"));
        f32 posZ = (f32)atof(dynamicObjectXML->ToElement()->Attribute("z"));
        f32 rot = (f32)atof(dynamicObjectXML->ToElement()->Attribute("r"));

        this->setActiveObject(templateObj);

        DynamicObject* newObj = createActiveObjectAt(vector3df(posX,posY,posZ));
		// If a script is assigned to the mesh then load it.
        newObj->setScript(script);
        newObj->setRotation(vector3df(0,rot,0));

        dynamicObjectXML = parentElement->IterateChildren( "obj", dynamicObjectXML );
    }
	return true;
}

stringc DynamicObjectsManager::createUniqueName()
{
    //the unique name of an dynamic object contains his index at the objects vector
    stringc uniqueName = "dynamic_object ";
    uniqueName += objsCounter++;

    return uniqueName;
}

void DynamicObjectsManager::initializeAllScripts()
{
    for(int i=0;i<(int)objects.size();i++)
    {
        ((DynamicObject*)objects[i])->doScript();
    }
}

void DynamicObjectsManager::updateAll()
{
    for(int i=0;i<(int)objects.size();i++)
    {
		// Non interactive objects will not be refreshed (update callback)
		// Should help with performance and allow for more NPC/Interactive objects.
		if (objects[i]->getType()!=OBJECT_TYPE_NON_INTERACTIVE)
			((DynamicObject*)objects[i])->update();
    }
}

void DynamicObjectsManager::clearAllScripts()
{
    for(int i=0;i<(int)objects.size();i++)
    {
        ((DynamicObject*)objects[i])->restoreParams();
        ((DynamicObject*)objects[i])->clearScripts();
    }
}

void DynamicObjectsManager::showDebugData(bool show)
{
    for(int i=0;i<(int)objects.size();i++)
        ((DynamicObject*)objects[i])->getNode()->setDebugDataVisible( show ? EDS_BBOX | EDS_SKELETON : EDS_OFF );
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
		triangle = objects[i]->getNode()->getTriangleSelector();
		s32 number = triangle->getTriangleCount();
		printf ("There is about %i triangles in this selector.\n",number);
		
		meta->addTriangleSelector(triangle);
		s32 number2  = meta->getTriangleCount();
		printf ("There is about %i triangles in this metaselector.\n",number2);
		printf("Collisions: added object %i\n",i);
    }
	return meta;
}

void DynamicObjectsManager::initializeCollisions()
{
	ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();
	
	//createMeta();
	// Create the collision response animator for the player
	//anim = smgr->createCollisionResponseAnimator(meta,Player::getInstance()->getNode(),vector3df(32.0f,72.0f,32.0f),vector3df(0,0,0));
	//Player::getInstance()->setAnimator(anim);
	//meta->drop();	
	// Create the collision response animator for each NPC.
	for(int i=0;i<(int)objects.size();i++)
    {
		if (objects[i]->getType()==OBJECT_TYPE_NPC || objects[i]->getType()==OBJECT_TYPE_PLAYER)
		{
			if (objects[i]->getType()==OBJECT_TYPE_NPC || objects[i]->getType()==OBJECT_TYPE_PLAYER)
			{
				createMeta();
				meta->removeTriangleSelector(objects[i]->getNode()->getTriangleSelector());
				//if (!objects[i]->enabled())
				//	meta->removeTriangleSelector(objects[i]->getNode()->getTriangleSelector());

				ISceneNodeAnimatorCollisionResponse* coll = smgr->createCollisionResponseAnimator(meta,objects[i]->getNode(),vector3df(32.0f,72.0f,32.0f),vector3df(0,0,0));
				objects[i]->getNode()->addAnimator(coll);
				objects[i]->setAnimator(coll);

				//meta->addTriangleSelector(objects[i]->getNode()->getTriangleSelector());
			
				meta->drop();
			}
		}
	}
	//meta->drop();
}

void DynamicObjectsManager::clearCollisions()
{
	meta->drop();
	Player::getInstance()->getNode()->removeAnimator(anim);
	
	// Remove the collision animators from the objects
	for(int i=0;i<(int)objects.size();i++)
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
	//((ISceneNodeAnimatorCollisionResponse*)collisionResponseAnimators[i])->drop();
    //collisionResponseAnimators.clear();
}
IMetaTriangleSelector* DynamicObjectsManager::getMeta()
{
	return meta;
}

void DynamicObjectsManager::updateMetaSelector(ITriangleSelector* tris, bool remove)
{
	ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();
	if (tris && remove)
	{
		s32 info1 = 0;
		info1 = meta->getTriangleCount();
		s32 info2 = tris->getTriangleCount();
		printf ("Selectors infos: Metaselector has %i polygons, current selector has %i polygons.\n",info1,info2);
		meta->removeTriangleSelector(tris);
		Player::getInstance()->getNode()->removeAnimator(anim);
		anim = smgr->createCollisionResponseAnimator(meta,Player::getInstance()->getNode(),vector3df(0.2f,0.5f,0.2f),vector3df(0,0,0));
		//Player::getInstance()->setAnimator(anim);		
		return;
	}
	if (tris && !remove)
	{ // TODO: Need a new function add/remove collision inside the dynamic object class
	}


}

void DynamicObjectsManager::clean()
{
    objsCounter = 0;
	// Temporarily commented out, was crashing when closing the game in play mode
	// This doesnt seem to cause a memory leak for now.
  /*  for(int i=0;collisionResponseAnimators.size();i++)
    {
        ISceneNodeAnimatorCollisionResponse* sc = collisionResponseAnimators[i];

        sc->drop();
    }*/
    collisionResponseAnimators.clear();
    activeObject = NULL;
	 /*
    for(int i=0;i<objectsTemplate.size();i++)
    {
        DynamicObject* d = objectsTemplate[i];

        delete d;
    }*/
    
	objectsTemplate.clear();
    
    for(int i=0;i<(int)objects.size();i++)
    {
        DynamicObject* d = objects[i];

        delete d;
    }
    objects.clear();
}
