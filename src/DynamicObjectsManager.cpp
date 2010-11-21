#include "DynamicObjectsManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

/*DynamicObjectsManager::DynamicObjectsManager()
{
    //Load all objects from xml
    irr::io::IXMLReaderUTF8* xml = App::getInstance()->getDevice()->getFileSystem()->createXMLReaderUTF8("../media/dynamic_objects/dynamic_objects.xml");

    if(!xml)
    {
        printf("ERROR: dynamic_objects.xml not found!");
        exit(0);
    }

    xml->read();

    if( stringc(xml->getNodeName()) != stringc("IrrRPG_Builder_DynamicObjects" ) || xml->getAttributeValueAsFloat("version") !=  APP_VERSION)
    {
        printf("ERROR: dynamic_objects.xml version must be %f\n", APP_VERSION);
    }

    vector<stringc> objsIDs;

    while(xml->read())
    {
		//if( stringc(xml->getNodeName()) == stringc("animations") )
		//{

		//}
        if( stringc(xml->getNodeName()) == stringc("dynamic_object") )
        {
            stringc name = stringc(xml->getAttributeValue("name"));
            stringc mesh = stringc(xml->getAttributeValue("mesh"));
            stringc script = stringc(xml->getAttributeValue("script"));
            stringc scale = stringc(xml->getAttributeValue("scale"));
            stringc materialType = stringc(xml->getAttributeValue("materialType"));

            E_MATERIAL_TYPE mat = EMT_SOLID;

            if(materialType == stringc("transparent_1bit")) mat = EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
            if(materialType == stringc("transparent_8bit")) mat = EMT_TRANSPARENT_ALPHA_CHANNEL;

            printf("%s %s %s",name.c_str(), mesh.c_str(), script.c_str());
            printf("  %s %s\n",scale.c_str(), materialType.c_str());
			vector<DynamicObject_Animation> animations;
			if (name!="")
			{ 
				DynamicObject* newObj = new DynamicObject(name, mesh, animations);

				newObj->setMaterialType(mat);
				newObj->setScale(vector3df(atof(scale.c_str()),atof(scale.c_str()),atof(scale.c_str())));
				newObj->setTemplateObjectName(name);
				//newObj->setHasAnimation(false);

				objectsTemplate.push_back(newObj);

				newObj->getNode()->setVisible(false);

				objsIDs.push_back(name);
			}
        }
    }

    delete xml;

    //set the initial active object - the list must be 1 or more objs!
    activeObject = objectsTemplate[0];

    objsCounter = 0;
}*/


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
                currAnim.startFrame = atoi(currentAnimXML->ToElement()->Attribute("start"));
                currAnim.endFrame = atoi(currentAnimXML->ToElement()->Attribute("end"));
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
        ((DynamicObject*)objects[i])->getNode()->setDebugDataVisible( show ? EDS_BBOX : EDS_OFF );
}

void DynamicObjectsManager::initializeCollisions()
{
	ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();
	meta=smgr->createMetaTriangleSelector();
	ITriangleSelector* triangle=0;

	// Put all the triangle selector into one meta selector.
    for(int i=0;i<(int)objects.size();i++)
    {
		//triangle = ((DynamicObject*)objects[i])->getTriangleSelector();
		triangle = objects[i]->getNode()->getTriangleSelector();
		s32 number = triangle->getTriangleCount();
		printf ("There is about %i triangles in this selector.\n",number);
		
		meta->addTriangleSelector(triangle);
		s32 number2  = meta->getTriangleCount();
		printf ("There is about %i triangles in this metaselector.\n",number2);
		printf("Collisions: added object %i\n",i);
    }
	// Create the collision response animator for the player
	anim = smgr->createCollisionResponseAnimator(meta,Player::getInstance()->getNode(),vector3df(0.2f,0.5f,0.2f),vector3df(0,0,0));
	Player::getInstance()->setAnimator(anim);
	
	// Create the collision response animator for each NPC.
	for(int i=0;i<(int)objects.size();i++)
    {
		if (objects[i]->getType()==OBJECT_TYPE_NPC)
		{
			ISceneNodeAnimatorCollisionResponse* coll = smgr->createCollisionResponseAnimator(meta,objects[i]->getNode(),vector3df(0.2f,0.5f,0.2f),vector3df(0,0,0));
			objects[i]->getNode()->addAnimator(coll);
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
