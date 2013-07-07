#include "TerrainManager.h"

#include <sstream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

const float DEG2RAD = 3.14159f/180;

TerrainManager::TerrainManager()
{
    terrainEmptySegmentsMap.clear();
    terrainMap.clear();
	tileTagged=NULL;
}

TerrainManager::~TerrainManager()
{
    //dtor
}

TerrainManager* TerrainManager::getInstance()
{
    static TerrainManager *instance = 0;
    if (!instance) instance = new TerrainManager();
    return instance;
}

void TerrainManager::createEmptySegment(vector3df pos)
{
    //if(getHashCode(pos) == "0_0") return;
	if (pos.Y==-1000.0f)
		return;

    if(getEmptySegment(pos) || getSegment(pos))
    {
        #ifdef APP_DEBUG
        cout << "DEBUG : TERRAIN MANAGER : SEGMENT ALREADY EXIST: " << getHashCode(pos) << endl;
        #endif
        return;
    }

    ISceneNode* newEmptySegment = App::getInstance()->getDevice()->getSceneManager()->addCubeSceneNode(1.0f,0,100);
	newEmptySegment->setPosition(vector3df((pos.X)*scale,0,(pos.Z)*scale));
    newEmptySegment->setScale(vector3df(scale,0.01f,scale) ) ;
    ITriangleSelector* sel = App::getInstance()->getDevice()->getSceneManager()->createTriangleSelectorFromBoundingBox(newEmptySegment);
    newEmptySegment->setTriangleSelector(sel);

    newEmptySegment->setMaterialTexture(0,App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/editor/terrain_empty_segment.png"));
    newEmptySegment->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);

    newEmptySegment->setName(getHashCode(pos).c_str());
	
	//May have to define a bigger bounding box to be able to select better the empty tiles... This portion in test.
	//core::aabbox3df box=newEmptySegment->getBoundingBox();
	//((IMeshSceneNode*)newEmptySegment)->getMesh()->setBoundingBox(core::aabbox3df(-box.getExtent().X/2,0,-box.getExtent().Z/2,box.getExtent().X/2,1024,box.getExtent().Z/2));

    terrainEmptySegmentsMap.insert(TerrainEmptySegmentsMapPair(getHashCode(pos).c_str(),newEmptySegment));


    #ifdef APP_DEBUG
    cout << "DEBUG : TERRAIN MANAGER : CREATED NEW EMPTY TERRAIN SEGMENT : " << getHashCode(pos) << " TOTAL:" << terrainEmptySegmentsMap.size() << endl;
    #endif
}

void TerrainManager::setEmptyTileVisible(bool visible)
{
	std::map<std::string, ISceneNode*>::iterator it;

    //Set visibility for all empty nodes
	//By hiding the node, the selection was disabled. Solution is to put a transparent texture over the mesh
	//The mesh is still rendered, but is invisible
	for ( it=terrainEmptySegmentsMap.begin() ; it != terrainEmptySegmentsMap.end(); it++ )
    {
		((ISceneNode*)((*it).second))->setVisible(visible);
		/* //Empty tiles should not be selectable in gameplay mode
		if (visible)
		{
			((ISceneNode*)((*it).second))->setMaterialTexture(0,App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/editor/terrain_empty_segment.png"));
			((ISceneNode*)((*it).second))->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
		}
		else
		{
			((ISceneNode*)((*it).second))->setMaterialTexture(0,App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/editor/terrain_invisible_segment.png"));
			((ISceneNode*)((*it).second))->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
		}*/

    }
}

void TerrainManager::createSegment(vector3df pos, bool empty, bool noextra)
{
	if (pos.Y==-1000)
		return;

    //Must be rounded positions (to keep it in the grid)
    pos.X = (f32)round32(pos.X);
    pos.Y = (f32)round32(pos.Y);
    pos.Z = (f32)round32(pos.Z);

	bool wasthere = false;
    //if segment already exists it don't need to be created
    if( getSegment(pos) )
    {
		wasthere=true;
		printf("Hey there is a tile here!\n");
        #ifdef APP_DEBUG
        cout << "DEBUG : TERRAIN MANAGER : SEGMENT ALREADY EXIST: " << getHashCode(pos) << endl;
        #endif
        //return;
    }
  
    if (empty)
	{
		createEmptySegment(vector3df(pos.X,0,pos.Z));
	}
	else if (!wasthere)
	{
		TerrainTile* newTile=new TerrainTile(App::getInstance()->getDevice()->getSceneManager(),
											0,
											pos,
											getHashCode(pos).c_str());
		terrainMap.insert(TerrainMapPair(newTile->getName().c_str(),newTile));

		removeEmptySegment(pos, true);

#ifdef APP_DEBUG
cout << "DEBUG : TERRAIN MANAGER : CREATED NEW TERRAIN SEGMENT : " << getHashCode(pos) << " TOTAL:" << terrainMap.size() << endl;
#endif

		/*Merge New tile to Neighbors (correcting edges according the neighbord edge)
		//
		//  edited neighbor|new tile                 | edge correction
		//               __|                       __|
		//              /  |                  =>  /  |\
		//             /   |___                  /   | \___
		*/

		// If there is noextra (loading from) there should not be sewing
		if (!noextra)
		{
			newTile->mergeToTile(getSegment(vector3df(pos.X-1,0,pos.Z)));
			newTile->mergeToTile(getSegment(vector3df(pos.X+1,0,pos.Z)));
			newTile->mergeToTile(getSegment(vector3df(pos.X,0,pos.Z-1)));
			newTile->mergeToTile(getSegment(vector3df(pos.X,0,pos.Z+1)));
		}
	}
	if (!noextra && empty) // Don't create extra borders when the tile is loaded from XML
	{
		createEmptySegment(vector3df(pos.X-1,0,pos.Z));
		createEmptySegment(vector3df(pos.X+1,0,pos.Z));
		createEmptySegment(vector3df(pos.X,0,pos.Z-1));
		createEmptySegment(vector3df(pos.X,0,pos.Z+1));

		createEmptySegment(vector3df(pos.X+1,0,pos.Z+1));
		createEmptySegment(vector3df(pos.X-1,0,pos.Z+1));
		createEmptySegment(vector3df(pos.X-1,0,pos.Z-1));
		createEmptySegment(vector3df(pos.X+1,0,pos.Z-1));
	}
}

void TerrainManager::createCustomSegment(vector3df pos, core::stringc model)
{
	if (pos.Y==-1000)
		return;

    //Must be rounded positions (to keep it in the grid)
    pos.X = (f32)round32(pos.X);
    pos.Y = (f32)round32(pos.Y);
    pos.Z = (f32)round32(pos.Z);

	bool wasthere = false;
    //if segment already exists it don't need to be created
    if( getSegment(pos) )
    {
		wasthere=true;
		printf("Hey there is a tile here!\n");
        #ifdef APP_DEBUG
        cout << "DEBUG : TERRAIN MANAGER : SEGMENT ALREADY EXIST: " << getHashCode(pos) << endl;
        #endif
        //return;
    }
  
	if (!wasthere) // Will have to check for supporting custom tiles...
	{
		TerrainTile* newTile=new TerrainTile(App::getInstance()->getDevice()->getSceneManager(),
											0,
											pos,
											getHashCode(pos).c_str(),true);
		terrainMap.insert(TerrainMapPair(newTile->getName().c_str(),newTile));

		removeEmptySegment(pos, true);
		newTile->customname = model;
		newTile->createCustom(0,pos,getHashCode(pos).c_str(),model);
	}

#ifdef APP_DEBUG
cout << "DEBUG : TERRAIN MANAGER : CREATED NEW CUSTOM SEGMENT : " << getHashCode(pos) << " TOTAL:" << terrainMap.size() << endl;
#endif

	
}

TerrainTile* TerrainManager::getSegment(vector3df pos)
{
	if (pos.Y==-1000.0f)
		return NULL;

    std::map<std::string, TerrainTile*>::iterator it;
	it = terrainMap.find(getHashCode(pos).c_str());

    if(it==terrainMap.end())
        return 0;
    else
        return (TerrainTile*)it->second;
}

TerrainTile* TerrainManager::getSegment(std::string hashCode)
{
    std::map<std::string, TerrainTile*>::iterator it;
    it = terrainMap.find(hashCode);

    if(it==terrainMap.end())
        return 0;
    else
        return (TerrainTile*)it->second;
}

ISceneNode* TerrainManager::getEmptySegment(vector3df pos)
{
	if (pos.Y==-1000.0f)
		return NULL;

    std::map<std::string, ISceneNode*>::iterator it;
    it = terrainEmptySegmentsMap.find(getHashCode(pos));

    if(it==terrainEmptySegmentsMap.end())
        return 0;
    else
        return (ISceneNode*)it->second;
}

void TerrainManager::removeEmptySegment(vector3df pos, bool force)
{
	if (pos.Y==-1000.0f)
		return;
    //if(getHashCode(pos)=="0_0") return;

	// Won't allow to remove the last empty segment 
	if ((terrainEmptySegmentsMap.size()==1) && (terrainMap.size()==0) && !force) return;


    if(getEmptySegment(pos))
    {
        ISceneNode* temp = terrainEmptySegmentsMap.find(getHashCode(pos))->second;

        terrainEmptySegmentsMap.erase(getHashCode(pos));
        temp->remove();

        #ifdef APP_DEBUG
        cout << "DEBUG : TERRAIN MANAGER : EMPTY SEGMENT REMOVED: " << getHashCode(pos) << " TOTAL:" << terrainEmptySegmentsMap.size() << endl;
        #endif
    }
}

void TerrainManager::removeSegment(vector3df pos)
{

	 //Must be rounded positions (to keep it in the grid)
    pos.X = (f32)round32(pos.X);
    pos.Y = (f32)round32(pos.Y);
    pos.Z = (f32)round32(pos.Z);

	if (pos.Y==-1000.0f)
		return;

	posTagged = pos;

	getHashCode(pos);

    if(getSegment(pos))
    {
        tileTagged = terrainMap.find(getHashCode(pos))->second;
		// Test if the tile has been modified
		bool test = tileTagged->checkModified();
		if (test)
		{
			IGUIEnvironment * guienv = App::getInstance()->getDevice()->getGUIEnvironment();
			video::IVideoDriver * driver =  App::getInstance()->getDevice()->getVideoDriver();

			video::ITexture * flag = driver->getTexture("../media/art/status-dialog-warning-icon64.png");
			guienv->addMessageBox(core::stringw("Warning").c_str(),
				core::stringw("You modified this tile, do you really want to remove it?").c_str(),
				true,EMBF_YES+EMBF_NO,0,-1, flag);
			// Events returns in APP class will check what button was pressed and
			// call deleteTaggedSegment if the user pressed "YES"
			return;
		}
		else
			deleteTaggedSegment();
    }
	
}

void TerrainManager::deleteTaggedSegment()
{
	
	 terrainMap.erase(getHashCode(posTagged));

     delete tileTagged;

	 // Put back the empty segment
	 createEmptySegment(vector3df(posTagged.X,0,posTagged.Z));

     #ifdef APP_DEBUG
     cout << "DEBUG : TERRAIN MANAGER : SEGMENT REMOVED: " << getHashCode(pos) << " TOTAL:" << TerrainMap.size() << endl;
     #endif

}

std::string TerrainManager::getHashCode(vector3df pos)
{
    std::stringstream ss;
    ss << round32(pos.X);
    ss << "_";
    ss << round32(pos.Z);

    std::string s = ss.str();

    ss.clear();
    return s;
}

void TerrainManager::saveToXML(TiXmlElement* parentElement)
{
    //write header and number of segments

    TiXmlElement* terrainXML = new TiXmlElement("terrain");
    terrainXML->SetAttribute("segments",(int)terrainMap.size());

    std::map<std::string, TerrainTile*>::iterator it;

    //Save all segments to XML
    for ( it=terrainMap.begin() ; it != terrainMap.end(); it++ )
    {
        ((TerrainTile*)((*it).second))->saveToXML(terrainXML);
		App::getInstance()->quickUpdate();

    }


	//Save all empty segments to XML
	std::map<std::string, ISceneNode*>::iterator it2;
	
	for ( it2=terrainEmptySegmentsMap.begin() ; it2 != terrainEmptySegmentsMap.end(); it2++ )
	{
		vector3df pos = ((ISceneNode*)((*it2).second))->getPosition();
		TiXmlElement* segmentXML = new TiXmlElement("emptySegment");
			segmentXML->SetAttribute("x",((core::stringc)pos.X).c_str());
			segmentXML->SetAttribute("z",((core::stringc)pos.Z).c_str());
		terrainXML->LinkEndChild(segmentXML);
		App::getInstance()->quickUpdate();

	}
    parentElement->LinkEndChild(terrainXML);
}

bool TerrainManager::loadFromXML(TiXmlElement* parentElement)
{
    clean();
	
	TiXmlNode* tSegment = parentElement->FirstChild( "terrainSegment" );
	while( tSegment != NULL )
    {
        f32 x = (f32)atoi(tSegment->ToElement()->Attribute("x"));
        f32 z = (f32)atoi(tSegment->ToElement()->Attribute("z"));

        createSegment(vector3df( x/scale ,0, z/scale ),false,true);
        TerrainTile* tempTile = getSegment( vector3df( x/scale ,0, z/scale ) );

        if(tempTile)
        {
            tempTile->loadFromXML(tSegment->ToElement());
			App::getInstance()->quickUpdate();
        }

        tSegment = parentElement->IterateChildren( "terrainSegment", tSegment );
    }

	// Save empty segments to XML
	TiXmlNode* tSegment2 = parentElement->FirstChild( "emptySegment" );
	while( tSegment2 != NULL )
    {
        f32 x = (f32)atoi(tSegment2->ToElement()->Attribute("x"));
        f32 z = (f32)atoi(tSegment2->ToElement()->Attribute("z"));

        TerrainManager::getInstance()->createSegment(vector3df( x/scale ,0, z/scale ),true,true);
       
			App::getInstance()->quickUpdate();

        tSegment2 = parentElement->IterateChildren( "emptySegment", tSegment2 );
    }
	return true;
}

void TerrainManager::paintVegetation(MousePick mousePick, bool erase)
{
    if(mousePick.pickedNode != NULL && getSegment(mousePick.pickedNode->getName()))
    {
        TerrainTile* tempTile = getSegment(mousePick.pickedNode->getName());
        if(tempTile) tempTile->paintVegetation(mousePick.pickedPos, erase);
    }
}

/*
void TerrainManager::transformSegmentByVertex(std::string hashCode,s32 id, f32 y, bool addVegetation)
{
    TerrainTile* tempTile = getSegment(hashCode);

    if(tempTile)
    {
        tempTile->transformMeshByVertex(id,y,addVegetation);
    }
}
*/

void TerrainManager::transformSegments(MousePick mousePick, f32 radius, f32 strength)
{
    if(mousePick.pickedNode != NULL)
    {
        for (int i=-1 ; i<2 ; i++)
        {
            for (int j=-1 ; j<2 ; j++)
            {
				
                vector3df pos = vector3df((f32)(mousePick.pickedNode->getPosition().X/(mousePick.pickedNode->getScale().X) + (i*tilemeshsize)),
                                          0,
                                          (f32)(mousePick.pickedNode->getPosition().Z/(mousePick.pickedNode->getScale().Z) + (j*tilemeshsize)));

				pos.X = pos.X/tilemeshsize;
				pos.Y = pos.Y/tilemeshsize;
				pos.Z = pos.Z/tilemeshsize;
                TerrainTile* tempTile = getSegment(pos);
                if(tempTile) tempTile->transformMesh(mousePick.pickedPos,radius,strength);
            }
        }
    }
}

void TerrainManager::transformSegmentsToValue(MousePick mousePick, f32 radius, f32 strength, f32 value)
{
    if(mousePick.pickedNode != NULL)
    {
        for (int i=-1 ; i<2 ; i++)
        {
            for (int j=-1 ; j<2 ; j++)
            {
                vector3df pos = vector3df((f32)round32(mousePick.pickedNode->getPosition().X/mousePick.pickedNode->getScale().X + (i * tilemeshsize)),
                                          0,
                                          (f32)round32(mousePick.pickedNode->getPosition().Z/mousePick.pickedNode->getScale().Z + (j * tilemeshsize)));

				pos.X = pos.X/tilemeshsize;
				pos.Y = pos.Y/tilemeshsize;
				pos.Z = pos.Z/tilemeshsize;
                TerrainTile* tempTile = getSegment(pos);
                if(tempTile) tempTile->transformMeshToValue(mousePick.pickedPos,radius,strength,value);
            }
        }
    }
}

f32 TerrainManager::getHeightAt(vector3df pos)
{
    vector3df hashCode = pos/this->getScale();

    TerrainTile* segment = this->getSegment(hashCode);

    if(segment)
        return segment->getHeightAt(pos);
    else
        return -1000;
}

stringc TerrainManager::getTileMeshName()
{
	return this->tilemesh;
}

void TerrainManager::setTileMeshName(stringc name)
{
	this->tilemesh = name;
}

stringc TerrainManager::getTerrainTexture(u32 layer)
{
	if (layer==0)
		return this->terraintexture0;
	if (layer==1)
		return this->terraintexture1;
	if (layer==2)
		return this->terraintexture2;
	if (layer==3)
		return this->terraintexture3;
	if (layer==4)
		return this->terraintexture4;
	return terraintexture1;
}

void TerrainManager::setTerrainTexture(u32 layer, stringc name)
{
	if (layer==0)
		this->terraintexture0=name;
	if (layer==1)
		this->terraintexture1=name;
	if (layer==2)
		this->terraintexture2=name;
	if (layer==3)
		this->terraintexture3=name;
	if (layer==4)
		this->terraintexture4=name;
}

f32 TerrainManager::getTileMeshSize()
{
	return this->tilemeshsize;
}

void TerrainManager::setTileMeshSize(f32 newsize)
{
	this->tilemeshsize=newsize;
}

f32 TerrainManager::getScale()
{
    return this->scale;
}

void TerrainManager::setScale(f32 newsize)
{
	this->scale = newsize;
}

void TerrainManager::setVisible(bool visible)
{
	std::map<std::string, TerrainTile*>::iterator it = terrainMap.begin();

    for(;it != terrainMap.end();++it)
    {
        TerrainTile* t = it->second;

		t->getNode()->setVisible(visible);
    }

}

void TerrainManager::clean()
{
    std::map<std::string, TerrainTile*>::iterator it = terrainMap.begin();

    for(;it != terrainMap.end();++it)
    {
        TerrainTile* t = it->second;

        delete t;
    }

    terrainMap.clear();


    std::map<std::string, ISceneNode*>::iterator it2 = terrainEmptySegmentsMap.begin();

    for(;it2 != terrainEmptySegmentsMap.end();++it2)
    {
        ISceneNode* s = it2->second;

        s->remove();
    }

    terrainEmptySegmentsMap.clear();
}

void TerrainManager::showDebugData(bool show)
{
    std::map<std::string, TerrainTile*>::iterator it = terrainMap.begin();

    for(;it != terrainMap.end();++it)
    {
        TerrainTile* t = it->second;

        t->showDebugData(show);
    }
}

void TerrainManager::drawBrush()
{
#ifdef EDITOR


	IVideoDriver* driver = App::getInstance()->getDevice()->getVideoDriver();

	f32 height=0.0f;
	
	f32 radius = App::getInstance()->getBrushRadius();
	vector3df position = App::getInstance()->getMousePosition3D(100).pickedPos;
	if (position==vector3df(0,0,0))
		return;

	SMaterial m;
	m.Lighting=false;
	driver->setMaterial(m);
	driver->setTransform(video::ETS_WORLD, core::matrix4());


	// Render the size of the brush.
	f32 framesize = 5;
	int step=10;
	for (int i=0; i<(360); i=i+step)
	{
		float degInRad = i*DEG2RAD;
		vector3df pos=position;
		pos.X+=cos(degInRad)*radius;
		pos.Z+=sin(degInRad)*radius;
		height=getHeightAt(pos);
		if (height==-1000.0f)
			height=0.0f;
		pos.Y=height+5;

		float degInRad2 = (i+step)*DEG2RAD;
		vector3df pos2=position;
		pos2.X+=cos(degInRad2)*radius;
		pos2.Z+=sin(degInRad2)*radius;
		height=getHeightAt(pos2);
		if (height==-1000.0f)
			height=0.0f;
		pos2.Y=height+5;
		//driver->draw3DLine(pos,pos2,video::SColor(255,255,255,0));

		vector3df pos3=position;
		pos3.X+=cos(degInRad)*(radius+framesize);
		pos3.Z+=sin(degInRad)*(radius+framesize);
		pos3.Y=pos.Y;

		vector3df pos4=position;
		pos4.X+=cos(degInRad2)*(radius+framesize);
		pos4.Z+=sin(degInRad2)*(radius+framesize);
		pos4.Y=pos2.Y;

		driver->draw3DTriangle(triangle3df(pos4,pos3,pos),video::SColor(128,255,255,128));
		driver->draw3DTriangle(triangle3df(pos,pos2,pos4),video::SColor(128,255,255,128));

	}

	// Center circle for the brush give the center
	radius=5;
	framesize = 2;
	step=15;

	for (int i=0; i<(360); i=i+step)
	{
		float degInRad = i*DEG2RAD;
		vector3df pos=position;
		pos.X+=cos(degInRad)*radius;
		pos.Z+=sin(degInRad)*radius;
		height=getHeightAt(pos);
		
		if (height==-1000.0f)
			height=0.0f;

		pos.Y=height+5;

		float degInRad2 = (i+step)*DEG2RAD;
		vector3df pos2=position;
		pos2.X+=cos(degInRad2)*radius;
		pos2.Z+=sin(degInRad2)*radius;
		height=getHeightAt(pos2);
		
		if (height==-1000.0f)
			height=0.0f;

		pos2.Y=height+5;
		//driver->draw3DLine(pos,pos2,video::SColor(255,255,255,0));

		vector3df pos3=position;
		pos3.X+=cos(degInRad)*(radius+framesize);
		pos3.Z+=sin(degInRad)*(radius+framesize);
		pos3.Y=pos.Y;

		vector3df pos4=position;
		pos4.X+=cos(degInRad2)*(radius+framesize);
		pos4.Z+=sin(degInRad2)*(radius+framesize);
		pos4.Y=pos2.Y;

		driver->draw3DTriangle(triangle3df(pos4,pos3,pos),video::SColor(128,255,255,128));
		driver->draw3DTriangle(triangle3df(pos,pos2,pos4),video::SColor(128,255,255,128));

	}
#endif
}