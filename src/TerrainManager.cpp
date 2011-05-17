#include "TerrainManager.h"

#include <sstream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

TerrainManager::TerrainManager()
{
    terrainEmptySegmentsMap.clear();
    terrainMap.clear();
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
    if(getHashCode(pos) == "0_0") return;

    if(getEmptySegment(pos) || getSegment(pos))
    {
        #ifdef APP_DEBUG
        cout << "DEBUG : TERRAIN MANAGER : EMPTY SEGMENT ALREADY EXIST: " << getHashCode(pos) << endl;
        #endif
        return;
    }

    ISceneNode* newEmptySegment = App::getInstance()->getDevice()->getSceneManager()->addCubeSceneNode(1.0f);
	newEmptySegment->setPosition(vector3df((pos.X)*scale,0,(pos.Z)*scale));
    newEmptySegment->setScale(vector3df(scale,0.01f,scale) ) ;
    ITriangleSelector* sel = App::getInstance()->getDevice()->getSceneManager()->createTriangleSelectorFromBoundingBox(newEmptySegment);
    newEmptySegment->setTriangleSelector(sel);

    //newEmptySegment->setRotation(vector3df(0,rand()%360,0));

    newEmptySegment->setMaterialTexture(0,App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/editor/terrain_empty_segment.png"));
    newEmptySegment->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);

    newEmptySegment->setName(getHashCode(pos).c_str());

    terrainEmptySegmentsMap.insert(TerrainEmptySegmentsMapPair(getHashCode(pos).c_str(),newEmptySegment));

    #ifdef APP_DEBUG
    cout << "DEBUG : TERRAIN MANAGER : CREATED NEW EMPTY TERRAIN SEGMENT : " << getHashCode(pos) << " TOTAL:" << terrainEmptySegmentsMap.size() << endl;
    #endif
}

void TerrainManager::createSegment(vector3df pos)
{
    //Must be rounded positions (to keep it in the grid)
    pos.X = (f32)round32(pos.X);
    pos.Y = (f32)round32(pos.Y);
    pos.Z = (f32)round32(pos.Z);

    //if segment already exists it don't need to be created
    if( getSegment(pos) )
    {
        #ifdef APP_DEBUG
        cout << "DEBUG : TERRAIN MANAGER : SEGMENT ALREADY EXIST: " << getHashCode(pos) << endl;
        #endif
        return;
    }
    else
    {
        removeEmptySegment(pos);

        TerrainTile* newTile=new TerrainTile(App::getInstance()->getDevice()->getSceneManager(),
                                         0,
                                         pos,
                                         getHashCode(pos).c_str());

        terrainMap.insert(TerrainMapPair(newTile->getName().c_str(),newTile));

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
        newTile->mergeToTile(getSegment(vector3df(pos.X-1,0,pos.Z)));
        newTile->mergeToTile(getSegment(vector3df(pos.X+1,0,pos.Z)));
        newTile->mergeToTile(getSegment(vector3df(pos.X,0,pos.Z-1)));
        newTile->mergeToTile(getSegment(vector3df(pos.X,0,pos.Z+1)));

        createEmptySegment(vector3df(pos.X-1,0,pos.Z));
        createEmptySegment(vector3df(pos.X+1,0,pos.Z));
        createEmptySegment(vector3df(pos.X,0,pos.Z-1));
        createEmptySegment(vector3df(pos.X,0,pos.Z+1));
    }
}

TerrainTile* TerrainManager::getSegment(vector3df pos)
{
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
    std::map<std::string, ISceneNode*>::iterator it;
    it = terrainEmptySegmentsMap.find(getHashCode(pos));

    if(it==terrainEmptySegmentsMap.end())
        return 0;
    else
        return (ISceneNode*)it->second;
}

void TerrainManager::removeEmptySegment(vector3df pos)
{
    if(getHashCode(pos)=="0_0") return;

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

        TerrainManager::getInstance()->createSegment(vector3df( x/scale ,0, z/scale ));
        TerrainTile* tempTile = TerrainManager::getInstance()->getSegment( vector3df( x/scale ,0, z/scale ) );

        if(tempTile)
        {
            tempTile->loadFromXML(tSegment->ToElement());
        }

        tSegment = parentElement->IterateChildren( "terrainSegment", tSegment );
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
