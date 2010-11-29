#include "TerrainTile.h"
#include "App.h"
#include "TerrainManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

//
const irr::f32 TerrainTile::vegetationRange = 0.45f;


TerrainTile::TerrainTile(ISceneManager* smgr, ISceneNode* parent, vector3df pos, stringc name)
{
    this->smgr=smgr;

    scale = TerrainManager::getInstance()->getScale();

    static IMesh* baseMesh = smgr->getMesh("../media/baseTerrain.obj");
	//static IMesh* baseMesh = smgr->getMesh("../media/land.obj");

    SMesh* newMesh = smgr->getMeshManipulator()->createMeshCopy(baseMesh);
	
	newMesh->setHardwareMappingHint(EHM_STATIC);
	//newMesh->setMaterialFlag(EMF_WIREFRAME ,true);
    node=smgr->addMeshSceneNode(newMesh,parent,100);
	nodescale = node->getBoundingBox().getExtent().X;
	//node=smgr->addOctreeSceneNode(newMesh,parent,100,512,true);
    ocean=smgr->addMeshSceneNode(newMesh,node,0);
	
    node->setScale(vector3df(scale/nodescale,scale/nodescale,scale/nodescale));

    node->setPosition(pos*scale);

    selector = smgr->createTriangleSelector(newMesh,node);
    node->setTriangleSelector(selector);

    node->setMaterialTexture(0,smgr->getVideoDriver()->getTexture("../0.jpg"));

    int tileX = round32(pos.X);
    int tileZ = round32(pos.Z);

    //cout << name.c_str() << endl;

    node->setName(name);

    static ITexture* layer0 = smgr->getVideoDriver()->getTexture("../media/L1.jpg");
    static ITexture* layer1 = smgr->getVideoDriver()->getTexture("../media/L2.jpg");
    static ITexture* layer2 = smgr->getVideoDriver()->getTexture("../media/L3.jpg");
    static ITexture* layer3 = smgr->getVideoDriver()->getTexture("../media/L4.jpg");

    //Create a Custom GLSL Material (Terrain Splatting)
    static s32 materialTerrain=smgr->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
        "../media/Shaders/splat.vert", "vertexMain", video::EVST_VS_1_1,
        "../media/Shaders/splat.frag", "pixelMain", video::EPST_PS_1_1,
        ShaderCallBack::getInstance(), video::EMT_SOLID);
	
    //Assign Textures
    node->setMaterialTexture(0,layer0);
    node->setMaterialTexture(1,layer1);
    node->setMaterialTexture(2,layer2);
    node->setMaterialTexture(3,layer3);

    //Assign GLSL Shader
    node->setMaterialType((E_MATERIAL_TYPE)materialTerrain);
	


    //Create a Custom GLSL Material (Terrain Splatting)
    static s32 materialOcean=smgr->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
        "../media/Shaders/ocean.vert", "vertexMain", video::EVST_VS_1_1,
        "../media/Shaders/ocean.frag", "pixelMain", video::EPST_PS_1_1,
        ShaderCallBack::getInstance(), video::EMT_TRANSPARENT_ALPHA_CHANNEL);

    static ITexture* oceanLayer0 = smgr->getVideoDriver()->getTexture("../media/waveNM.png");
    static ITexture* oceanLayer1 = smgr->getVideoDriver()->getTexture("../media/sky.jpg");

    //Assign GLSL Shader
    ocean->setMaterialType((E_MATERIAL_TYPE)materialOcean);

    //Assign Textures
    ocean->setMaterialTexture(0,oceanLayer0);
    ocean->setMaterialTexture(1,oceanLayer1);

    node->setMaterialFlag(EMF_FOG_ENABLE,true);
    ocean->setMaterialFlag(EMF_FOG_ENABLE,true);
	srand ( App::getInstance()->getDevice()->getTimer()->getRealTime());

}

TerrainTile::~TerrainTile()
{
    for(int i=0;i<(int)vegetationVector.size();i++)
    {
        Vegetation* v = vegetationVector[i];

        delete v;
    }

    vegetationVector.clear();

    node->remove();

    //ocean->remove();///TODO: rever destrutor TerrainTile!

    selector->drop();
}

Vegetation* TerrainTile::getVegetationAt(vector3df pos)
{
    for (int i=0 ; i<(int)vegetationVector.size() ; i++)
    {
    	Vegetation* temp = (Vegetation*)vegetationVector[i];
    	if(temp->getPosition().getDistanceFrom(pos) < 0.5) return temp;
    }
    return 0;
}

stringc TerrainTile::getName()
{
    return node->getName();
}

ISceneNode* TerrainTile::getNode()
{
    return node;
}

f32 TerrainTile::getNodeScale()
{
	return nodescale;
}
ITriangleSelector* TerrainTile::getTriangleSelector()
{
    return selector;
}

IMeshBuffer* TerrainTile::getMeshBuffer()
{
    return ((IMeshSceneNode*)node)->getMesh()->getMeshBuffer(0);
}

void TerrainTile::mergeToTile(TerrainTile* tile)
{
    if(!tile)
    {
        #ifdef APP_DEBUG
        cout << "DEBUG : TERRAIN TILE : MERGE FAILED, TILE IS NULL: " << endl;
        #endif
        return;
    }
    else
    {
        IMeshBuffer* meshBuffer = ((IMeshSceneNode*)node)->getMesh()->getMeshBuffer(0);
        IMeshBuffer* neighborBuffer = tile->getMeshBuffer();

        S3DVertex* mb_vertices = (S3DVertex*) meshBuffer->getVertices();
        S3DVertex* n_mb_vertices = (S3DVertex*) neighborBuffer->getVertices();

        u16* mb_indices  = meshBuffer->getIndices();
        u16* n_mb_indices  = neighborBuffer->getIndices();

        for (unsigned int j = 0; j < meshBuffer->getVertexCount(); j += 1)
        {
            for (unsigned int i = 0; i < neighborBuffer->getVertexCount(); i += 1)
            {
                vector3df realPos = mb_vertices[j].Pos*(scale/nodescale) + node->getPosition();
                vector3df nRealPos = n_mb_vertices[i].Pos*(scale/nodescale) + tile->getNode()->getPosition();

                if((realPos.X == nRealPos.X) && (realPos.Z == nRealPos.Z))
                {
                    mb_vertices[j].Pos.Y = n_mb_vertices[i].Pos.Y;
                }
            }
        }

        smgr->getMeshManipulator()->recalculateNormals(((IMeshSceneNode*)node)->getMesh(),true);
		((IMeshSceneNode*)node)->getMesh()->setDirty();
    }
}

void TerrainTile::saveToXML(TiXmlElement* parentElement)
{
    int x = node->getPosition().X/node->getScale().X;
    int z = node->getPosition().Z/node->getScale().Z;

    TiXmlElement* segmentXML = new TiXmlElement("terrainSegment");
    segmentXML->SetAttribute("x",x);
    segmentXML->SetAttribute("z",z);

    //write all vertex
    IMeshBuffer* meshBuffer = ((IMeshSceneNode*)node)->getMesh()->getMeshBuffer(0);
	S3DVertex* mb_vertices = (S3DVertex*) meshBuffer->getVertices();
	u16* mb_indices  = meshBuffer->getIndices();

    for (unsigned int j = 0; j < meshBuffer->getVertexCount(); j += 1)
	{
	    vector3df realPos = mb_vertices[j].Pos*(scale/nodescale) + node->getPosition();

        bool vegetation = false;

	    for (int i=0 ; i<(int)vegetationVector.size() ; i++)
	    {
            if( ((Vegetation*)vegetationVector[i])->getPosition().getDistanceFrom(realPos) < vegetationRange) vegetation = true;
	    }

        if(realPos.Y != 0 || vegetation)
        {
            TiXmlElement* vertexXML = new TiXmlElement("vertex");
            vertexXML->SetAttribute("id",j);
            vertexXML->SetAttribute("y",stringc((realPos.Y/(scale/nodescale))).c_str());
            vertexXML->SetAttribute("v",vegetation);

            segmentXML->LinkEndChild(vertexXML);
        }
	}

	parentElement->LinkEndChild(segmentXML);
}

bool TerrainTile::loadFromXML(TiXmlElement* parentElement)
{
    TiXmlNode* vertex = parentElement->FirstChild( "vertex" );

    while( vertex != NULL )
    {
        s32 id = atoi(vertex->ToElement()->Attribute("id"));
        f32 y = (f32)atof(vertex->ToElement()->Attribute("y"));
        bool addVegetation = atoi(vertex->ToElement()->Attribute("v"));

        this->transformMeshByVertex(id,y,addVegetation);

        vertex = parentElement->IterateChildren( "vertex", vertex );
    }
	return true;
}

void TerrainTile::paintVegetation(vector3df clickPos, bool erase)
{
    IMeshBuffer* meshBuffer = ((IMeshSceneNode*)node)->getMesh()->getMeshBuffer(0);

	S3DVertex* mb_vertices = (S3DVertex*) meshBuffer->getVertices();

	u16* mb_indices  = meshBuffer->getIndices();

	for (unsigned int j = 0; j < meshBuffer->getVertexCount(); j += 1)
	{
	    if(erase)
        {
            vector3df realPos = mb_vertices[j].Pos*(scale/nodescale) + node->getPosition();
            clickPos.Y = realPos.Y;
            if(realPos.getDistanceFrom(clickPos) < vegetationRange && getVegetationAt(vector3df(realPos.X,realPos.Y/(scale/nodescale),realPos.Z)))
            {
                Vegetation* toRemove = getVegetationAt(vector3df(realPos.X,realPos.Y/(scale/nodescale),realPos.Z));

                for (int i=0 ; i<(int)vegetationVector.size() ; i++)
                {
                    if(vegetationVector[i] == toRemove)
                    {
                        vegetationVector.erase(vegetationVector.begin() + i);
                        delete toRemove;

                        #ifdef APP_DEBUG
                        cout << "DEBUG : TERRAIN TILE : VEGETATION REMOVED: " << realPos.X << "," << realPos.X/(scale/nodescale) << "," << realPos.Z << "   TOTAL:" << vegetationVector.size() << endl;
                        #endif

                        break;
                    }
                }
            }
        }
        else
        {
            vector3df realPos = mb_vertices[j].Pos*(scale/nodescale) + node->getPosition();
            clickPos.Y = realPos.Y;
            if(realPos.getDistanceFrom(clickPos) < (scale/10) * 0.5f && !getVegetationAt(vector3df(realPos.X,realPos.Y/(scale/nodescale),realPos.Z)))
            {
                Vegetation* v = new Vegetation();

                //v->setPosition(vector3df(realPos.X + (rand()%5)*0.1f - 0.25f,realPos.Y/(scale/nodescale),realPos.Z + (rand()%5)*0.1f - 0.25f));
				v->setPosition(vector3df(realPos.X + (rand()%5)*0.1f - 0.25f,realPos.Y,realPos.Z + (rand()%5)*0.1f - 0.25f));
                f32 treesize = (f32)(rand() % 100 + 50)/100;
				v->setScale(vector3df(treesize*(scale/7.5),treesize*(scale/7.5),treesize*(scale/7.5)));
				printf("Attempting to place a tree with this size: %f\n",treesize);
                vegetationVector.push_back(v);

                #ifdef APP_DEBUG
                cout << "DEBUG : TERRAIN TILE : VEGETATION CREATED: " << realPos.X << "," << realPos.X/(scale/nodescale) << "," << realPos.Z << "   TOTAL:" << vegetationVector.size() << endl;
                #endif
	        }
	    }
	}
}

void TerrainTile::transformMeshByVertex(s32 id, f32 y, bool addVegetation)
{
    IMeshBuffer* meshBuffer = ((IMeshSceneNode*)node)->getMesh()->getMeshBuffer(0);

	S3DVertex* mb_vertices = (S3DVertex*) meshBuffer->getVertices();

	u16* mb_indices  = meshBuffer->getIndices();

	mb_vertices[id].Pos.Y = y;

	if(addVegetation)
	{
	    vector3df treePos = vector3df(mb_vertices[id].Pos.X,y , mb_vertices[id].Pos.Z);
	    treePos *= node->getScale();
	    treePos.Y = y;
	    treePos += node->getPosition();
	    paintVegetation(treePos ,false);
	}

	smgr->getMeshManipulator()->recalculateNormals(((IMeshSceneNode*)node)->getMesh(),true);
}


void TerrainTile::transformMesh(vector3df clickPos, f32 radius, f32 strength)
{
    IMeshBuffer* meshBuffer = ((IMeshSceneNode*)node)->getMesh()->getMeshBuffer(0);

	S3DVertex* mb_vertices = (S3DVertex*) meshBuffer->getVertices();

	u16* mb_indices  = meshBuffer->getIndices();

	
	for (unsigned int j = 0; j < meshBuffer->getVertexCount(); j += 1)
	{
	    vector3df realPos = mb_vertices[j].Pos*(scale/nodescale) + node->getPosition();
        clickPos.Y = realPos.Y;
		
	    if(realPos.getDistanceFrom(clickPos) < radius)
	    {
            //f32 ratio = sin(radius - realPos.getDistanceFrom(clickPos));
			f32 ratio = radius - realPos.getDistanceFrom(clickPos);
	        mb_vertices[j].Pos.Y += (strength * (ratio)/(scale/nodescale));
			printf("found something here: vertice %i, vertice Y: %f\n",j, mb_vertices[j].Pos.Y);
	    }

	    if(mb_vertices[j].Pos.Y > scale/4) mb_vertices[j].Pos.Y = scale/4;
	    if(mb_vertices[j].Pos.Y < -0.25) mb_vertices[j].Pos.Y = -0.25;
	}


	smgr->getMeshManipulator()->recalculateNormals(((IMeshSceneNode*)node)->getMesh(),true);
	((IMeshSceneNode*)node)->getMesh()->setDirty();
}

void TerrainTile::transformMeshToZero(vector3df clickPos, f32 radius, f32 strength)
{
    if(strength<0) strength = -strength;

    IMeshBuffer* meshBuffer = ((IMeshSceneNode*)node)->getMesh()->getMeshBuffer(0);

	S3DVertex* mb_vertices = (S3DVertex*) meshBuffer->getVertices();

	u16* mb_indices  = meshBuffer->getIndices();

	for (unsigned int j = 0; j < meshBuffer->getVertexCount(); j += 1)
	{
	    vector3df realPos = mb_vertices[j].Pos*(scale/nodescale) + node->getPosition();
        clickPos.Y = realPos.Y;
	    if(realPos.getDistanceFrom(clickPos) < radius)
	    {
            //f32 ratio = sin(radius - realPos.getDistanceFrom(clickPos));
			f32 ratio = radius - realPos.getDistanceFrom(clickPos);

            if(mb_vertices[j].Pos.Y > 0)
            {
                mb_vertices[j].Pos.Y -= strength * ratio / (scale/nodescale);
                if(mb_vertices[j].Pos.Y <= strength) mb_vertices[j].Pos.Y = 0;
            }
            if(mb_vertices[j].Pos.Y < 0)
            {
                mb_vertices[j].Pos.Y += strength * ratio / (scale / nodescale);
                if(mb_vertices[j].Pos.Y >= -strength) mb_vertices[j].Pos.Y = 0;
            }
	    }
	}

	smgr->getMeshManipulator()->recalculateNormals(((IMeshSceneNode*)node)->getMesh(),true);
	((IMeshSceneNode*)node)->getMesh()->setDirty();
}

void TerrainTile::transformMeshDOWN(vector3df clickPos, f32 radius, f32 strength)
{
    IMeshBuffer* meshBuffer = ((IMeshSceneNode*)node)->getMesh()->getMeshBuffer(0);

	S3DVertex* mb_vertices = (S3DVertex*) meshBuffer->getVertices();

	u16* mb_indices  = meshBuffer->getIndices();

	for (unsigned int j = 0; j < meshBuffer->getVertexCount(); j += 1)
	{
	    vector3df realPos = mb_vertices[j].Pos*(scale/nodescale) + node->getPosition();
        clickPos.Y = realPos.Y;
	    if(realPos.getDistanceFrom(clickPos) < radius)
	    {
            //f32 ratio = sin(radius - realPos.getDistanceFrom(clickPos));
			f32 ratio = radius - realPos.getDistanceFrom(clickPos);
	        mb_vertices[j].Pos.Y -= strength * ratio / (scale/nodescale);
	    }

	    //if(mb_vertices[j].Pos.Y < 0 ) mb_vertices[j].Pos.Y = 0;
	    cout << mb_vertices[j].Pos.Y << endl;
	}

	smgr->getMeshManipulator()->recalculateNormals(((IMeshSceneNode*)node)->getMesh(),true);
	((IMeshSceneNode*)node)->getMesh()->setDirty();
}

f32 TerrainTile::getHeightAt(vector3df pos)
{
    IMeshBuffer* meshBuffer = ((IMeshSceneNode*)node)->getMesh()->getMeshBuffer(0);

	S3DVertex* mb_vertices = (S3DVertex*) meshBuffer->getVertices();

	f32 height = 0;
	vector3df nearestVertex = vector3df(1000,1000,1000);

	for (unsigned int j = 0; j < meshBuffer->getVertexCount(); j += 1)
	{
	    vector3df realPos = mb_vertices[j].Pos*(scale/nodescale) + node->getPosition();

	    if(pos.getDistanceFrom(vector3df(realPos.X,0,realPos.Z) ) < pos.getDistanceFrom( vector3df(nearestVertex.X,0,nearestVertex.Z) ))
	    {
	        nearestVertex = realPos;
	        height = realPos.Y;
	    }
	}

	return height;
}

void TerrainTile::showDebugData(bool show)
{
    for(int i=0;i<(int)vegetationVector.size();i++)
    {
        vegetationVector[i]->showDebugData(show);
    }
}
