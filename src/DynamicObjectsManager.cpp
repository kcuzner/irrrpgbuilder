#include "DynamicObjectsManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

DynamicObjectsManager::DynamicObjectsManager()
{
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
				currAnim.mesh = currentAnimXML->ToElement()->Attribute("mesh");
				if (currAnim.mesh.size()==0)
					currAnim.mesh = L"undefined";
				// load the startframe for the current animation name
                stringc s_start = currentAnimXML->ToElement()->Attribute("start");
				if (s_start.size()>0) 
					currAnim.startFrame = atoi(s_start.c_str()); 
				else 
					currAnim.startFrame=0;
				// load the endframe for the current animation name
                stringc s_end = currentAnimXML->ToElement()->Attribute("end");
				if (s_end.size()>0) 
					currAnim.endFrame = atoi(s_end.c_str()); 
				else 
					currAnim.endFrame=0;
				
				// TODO: Not totally implemented
				currAnim.sound = currentAnimXML->ToElement()->Attribute("sound");

                currAnim.speed = (f32)atof(currentAnimXML->ToElement()->Attribute("speed"));

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
				//printf("There is a dynamic object with a script! object name is %s and the script is:\n",filename.c_str());
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
			newObj->setScale(vector3df((f32)atof(scale.c_str()),(f32)atof(scale.c_str()),(f32)atof(scale.c_str())));
            newObj->setTemplateObjectName(name);
            newObj->getNode()->setVisible(false);
			

            //store the new object
            objectsTemplate.push_back(newObj);
            objsIDs.push_back(name);

            currentObjXML = root->IterateChildren( "dynamic_object", currentObjXML );

        }//while

    }//root

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

DynamicObject* DynamicObjectsManager::createActiveObjectAt(vector3df pos)
{
    DynamicObject* newObj = activeObject->clone();

    cout << "TEMPLATE NAME:" << activeObject->getName().c_str() << endl;

    newObj->setPosition(pos);

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
	
	createMeta();
	// Create the collision response animator for the player
	anim = smgr->createCollisionResponseAnimator(meta,Player::getInstance()->getNode(),vector3df(32.0f,72.0f,32.0f),vector3df(0,0,0));
	Player::getInstance()->setAnimator(anim);
	meta->drop();	
	// Create the collision response animator for each NPC.
	for(int i=0;i<(int)objects.size();i++)
    {
		if (objects[i]->getType()==OBJECT_TYPE_NPC)
		{
			createMeta();
			meta->removeTriangleSelector(objects[i]->getNode()->getTriangleSelector());
			ISceneNodeAnimatorCollisionResponse* coll = smgr->createCollisionResponseAnimator(meta,objects[i]->getNode(),vector3df(32.0f,72.0f,32.0f),vector3df(0,0,0));
			objects[i]->getNode()->addAnimator(coll);
			meta->drop();
		}
	}
	
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
		Player::getInstance()->setAnimator(anim);		
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

    /*
    activeObject = NULL;

    for(int i=0;i<objectsTemplate.size();i++)
    {
        DynamicObject* d = objectsTemplate[i];

        delete d;
    }
    objectsTemplate.clear();
    */


    for(int i=0;i<(int)objects.size();i++)
    {
        DynamicObject* d = objects[i];

        delete d;
    }
    objects.clear();
}
