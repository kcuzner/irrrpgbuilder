#include "TerrainTile.h"
#include "../App.h"
#include "TerrainManager.h"
#include "../gui/GUIManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

//
const irr::f32 TerrainTile::vegetationRange = 60; // old value=60


TerrainTile::TerrainTile(ISceneManager* smgr, ISceneNode* parent, vector3df pos, stringc name, bool custom, bool param)
{
    this->smgr=smgr;

	ocean=NULL;
	node=NULL;
	selector=NULL;

	needrecalc=false; //Define if the tile need to be calculated

    scale = TerrainManager::getInstance()->getScale();
	if (!custom)
		createTerrain(parent,pos,name,param);

	srand(App::getInstance()->getDevice()->getTimer()->getRealTime());
}

void TerrainTile::createTerrain(ISceneNode* parent, vector3df pos, stringc name, bool param)
{
	stringc tilename = TerrainManager::getInstance()->getTileMeshName();

	if (tilename=="")
		tilename="../media/land.obj";

	IMesh* baseMesh = NULL;
	
	if (!param)
	{
		baseMesh = smgr->getMesh(tilename.c_str());
	}
	else
	{

		//Tries to create a mesh that is 1024 in size (mesh size), scaled then by IRB.
		//"tilesegment" is used to determine the density of the mesh, smaller=less dense
		vector2df tiles = vector2df(100,100);
		f32 size = TerrainManager::getInstance()->getScale();
		u32 tilesegment = (u32)(0.024414f*size);
		baseMesh = smgr->addHillPlaneMesh( "myHill",
		    core::dimension2d<f32>(f32(1024/tilesegment),f32(1024/tilesegment)),
			core::dimension2d<u32>(tilesegment,tilesegment), 0, 0,
			core::dimension2d<f32>(0,0),
			core::dimension2d<f32>(1,1));
	}

	SMesh* newMesh = NULL;

	newMesh = smgr->getMeshManipulator()->createMeshCopy(baseMesh);
	newMesh->setHardwareMappingHint(EHM_STATIC);

	if (node)
		node->drop();

	// Create the terrain mesh node
	node = smgr->addMeshSceneNode(newMesh,parent,100);
	node->setMaterialFlag(EMF_LIGHTING,false);
	//node->setMaterialFlag(EMF_WIREFRAME,true);
	// node->setMaterialFlag(EMF_BLEND_OPERATION,true);
	// Create the terrain mesh node
	nodescale = node->getBoundingBox().getExtent().X;
	TerrainManager::getInstance()->setTileMeshSize(nodescale);
	node->setName(name);
	
	node->setScale(vector3df(scale/nodescale,scale/nodescale,scale/nodescale));
	 
	node->setPosition(pos*scale);
    selector = smgr->createTriangleSelector(newMesh,node);
    node->setTriangleSelector(selector);
	assignTerrainShader(node);
    
	// Create the water mesh, using the same reference as the terrain, applied shader will use the vertices informations to set the transparency of the water.
	ocean=smgr->addMeshSceneNode(newMesh,node,0); // use "newMesh" as the same reference. Will use the vertices height to get the transparency for the water.
	ocean->setMaterialFlag(EMF_BLEND_OPERATION,true);
	assignWaterShader(ocean);
 
	
	meshBuffer = ((IMeshSceneNode*)node)->getMesh()->getMeshBuffer(0);
	mb_vertices = (S3DVertex*) meshBuffer->getVertices();
	mb_indices  = meshBuffer->getIndices();

	/*
	// Reset the vertices height of the mesh to 0.0f (Y axis)	
	for (unsigned int j = 0; j < meshBuffer->getVertexCount(); j += 1)
	{
	   mb_vertices[j].Pos.Y = 0.0f;
	} */

	driver = smgr->getGUIEnvironment()->getVideoDriver();

	recalculate();

	custom = false;
    
}

void TerrainTile::createCustom(ISceneNode* parent, vector3df pos, stringc name, stringc model)
{

	// Remove terrain if it was there
	if (ocean)
	{
		ocean->remove();
		ocean=NULL;
	}
	
	if (node)
	{
		node->remove();
		node=NULL;
	}


	//core::stringc path="../media/dynamic_objects/";
	//core::stringc file=path.append(model);
	
	
	core::stringc file=model;
	
	IMesh* baseMesh = smgr->getMesh(file.c_str());


	if (!baseMesh)
	{
		GUIManager::getInstance()->setConsoleText(L"ERROR! Failed to load custom tile",video::SColor(255,255,0,0));
		//Tries to create a mesh that is 1024 in size (mesh size), scaled then by IRB.
		//"tilesegment" is used to determine the density of the mesh, smaller=less dense
		vector2df tiles = vector2df(100,100);
		f32 size = TerrainManager::getInstance()->getScale();
		u32 tilesegment = (u32)(0.024414f*size);
		baseMesh = smgr->addHillPlaneMesh( "myHill",
		core::dimension2d<f32>(f32(1024/tilesegment),f32(1024/tilesegment)),
		core::dimension2d<u32>(tilesegment,tilesegment), 0, 0,
			core::dimension2d<f32>(0,0),
			core::dimension2d<f32>(1,1));
	}

	baseMesh->setHardwareMappingHint(EHM_STATIC);


	// Create the custom mesh node
	node = smgr->addMeshSceneNode(baseMesh,parent,100);

	// Create the terrain mesh node
	nodescale = node->getBoundingBox().getExtent().X;
	TerrainManager::getInstance()->setTileMeshSize(nodescale);
	node->setName(name);
	
    node->setPosition(pos*scale);
    selector = smgr->createTriangleSelector(baseMesh,node);
	meshBuffer = ((IMeshSceneNode*)node)->getMesh()->getMeshBuffer(0);
	mb_vertices = (S3DVertex*) meshBuffer->getVertices();
	mb_indices  = meshBuffer->getIndices();
    node->setTriangleSelector(selector);
	//custom = true;

	assignTerrainShader(node);
	nodescale = node->getBoundingBox().getExtent().X;
	node->setScale(vector3df(scale/nodescale,scale/nodescale,scale/nodescale));

	// Work for the water mesh.
	SMesh* newMesh = NULL;

	newMesh = smgr->getMeshManipulator()->createMeshCopy(baseMesh);
	newMesh->setHardwareMappingHint(EHM_STATIC);
	// Create the water mesh, using the same reference as the terrain, applied shader will use the vertices informations to set the transparency of the water.
	ocean=smgr->addMeshSceneNode(newMesh,node,0); // use "newMesh" as the same reference. Will use the vertices height to get the transparency for the water.
	ocean->setMaterialFlag(EMF_BLEND_OPERATION,true);
	assignWaterShader(ocean);

}


TerrainTile::~TerrainTile()
{
	for (int i=0 ; i<(int)vegetationVector.size() ; i++)
    {
    	Vegetation* temp = (Vegetation*)vegetationVector[i];
		delete temp;
    }
    vegetationVector.clear();

	if (selector)
		selector->drop();

	if (node)
	{
		node->remove();
	}

}

Vegetation* TerrainTile::getVegetationAt(vector3df pos)
{
    for (int i=0 ; i<(int)vegetationVector.size() ; i++)
    {
    	Vegetation* temp = (Vegetation*)vegetationVector[i];
    	if(temp->getPosition().getDistanceFrom(pos) < vegetationRange ) return temp;
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

IMesh* TerrainTile::getMesh()
{
    return ((IMeshSceneNode*)node)->getMesh();
}

void TerrainTile::mergeToTile(TerrainTile* tile)
{
    if(!tile || custom)
    {
        #ifdef DEBUG
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
					mb_vertices[j].Normal = n_mb_vertices[i].Normal; //New Copy the normal information
					needrecalc=true;
                }
            }
        }

        //smgr->getMeshManipulator()->recalculateNormals(((IMeshSceneNode*)node)->getMesh(),true);
		((IMeshSceneNode*)node)->getMesh()->setDirty();
		needrecalc = true;

    }
}

void TerrainTile::saveToXML(TiXmlElement* parentElement)
{
	// Save the terrain land
    
	f32 x = 0;
	f32 z = 0;

	x = node->getPosition().X;
	z = node->getPosition().Z;
	
    TiXmlElement* segmentXML = new TiXmlElement("terrainSegment");
    segmentXML->SetDoubleAttribute("x",x);
    segmentXML->SetDoubleAttribute("z",z);

	core::stringc file=TerrainManager::getInstance()->filename;
	core::stringc path=(stringc)App::getInstance()->getDevice()->getFileSystem()->getFileDir(file);
	core::stringc path2=(stringc)App::getInstance()->getDevice()->getFileSystem()->getRelativeFilename(path,
				App::getInstance()->getDevice()->getFileSystem()->getWorkingDirectory());

	core::stringc filename=(stringc)App::getInstance()->getDevice()->getFileSystem()->getRelativeFilename(file,
				App::getInstance()->getDevice()->getFileSystem()->getWorkingDirectory());

	path2.append("/");
	path2.append(filename);
	segmentXML->SetAttribute("mesh",path2.c_str());

	//Saving the vegetation information with the tile
	if (vegetationVector.size()>0)
	{
		for (int i=0 ; i<(int)vegetationVector.size() ; i++)
		{
			TiXmlElement* vertexXML = new TiXmlElement("tree");
			Vegetation * tree = (Vegetation*)vegetationVector[i];
    		
			if (tree!=NULL)
			{
				vector3df treepos=tree->getPosition();
				vertexXML->SetAttribute("v",tree->getType());
				vertexXML->SetDoubleAttribute("tx",tree->getPosition().X);
				vertexXML->SetDoubleAttribute("ty",tree->getPosition().Y);
				vertexXML->SetDoubleAttribute("tz",tree->getPosition().Z);
				vertexXML->SetDoubleAttribute("tr",tree->getNode()->getRotation().Y);
				vertexXML->SetDoubleAttribute("ts",tree->getNode()->getScale().X);
			}
			segmentXML->LinkEndChild(vertexXML);
		}
			
	}
	/*

	if (custom)
	{
		segmentXML->SetAttribute("custom",customname.c_str());
		if (node->getRotation().Y!=0)
			segmentXML->SetDoubleAttribute("custom_R", node->getRotation().Y);

		if (vegetationVector.size()>0)
		{
			
			for (int i=0 ; i<(int)vegetationVector.size() ; i++)
			{
				TiXmlElement* vertexXML = new TiXmlElement("vertex");
				Vegetation * tree = (Vegetation*)vegetationVector[i];
    		
				if (tree!=NULL)
				{
					vector3df treepos=tree->getPosition();
					vertexXML->SetAttribute("v",tree->getType());
					vertexXML->SetDoubleAttribute("tx",tree->getPosition().X);
					vertexXML->SetDoubleAttribute("ty",tree->getPosition().Y);
					vertexXML->SetDoubleAttribute("tz",tree->getPosition().Z);
					vertexXML->SetDoubleAttribute("tr",tree->getNode()->getRotation().Y);
					vertexXML->SetDoubleAttribute("ts",tree->getNode()->getScale().X);
				}
				segmentXML->LinkEndChild(vertexXML);
			}
			
		}
	}
	

	if (!custom)
	{
		//write all vertex
		IMeshBuffer* meshBuffer = ((IMeshSceneNode*)node)->getMesh()->getMeshBuffer(0);
		S3DVertex* mb_vertices = (S3DVertex*) meshBuffer->getVertices();
		u16* mb_indices  = meshBuffer->getIndices();

		for (unsigned int j = 0; j < meshBuffer->getVertexCount(); j += 1)
		{
			vector3df realPos = mb_vertices[j].Pos*(scale/nodescale) + node->getPosition();
			//if(realPos.Y != 0.0f ) //We should save everything!!!
			{
				TiXmlElement* vertexXML = new TiXmlElement("vertex");
            
				vertexXML->SetAttribute("id",j);
				//vertexXML->SetAttribute("y",stringc(realPos.Y).c_str());
				vertexXML->SetAttribute("y",stringc(mb_vertices[j].Pos.Y).c_str());

				Vegetation * tree = 0;
				for (int i=0 ; i<(int)vegetationVector.size() ; i++)
				{
    				Vegetation * temp = (Vegetation*)vegetationVector[i];
    				if(temp->getPosition().getDistanceFrom(realPos) < (vegetationRange/10) )
					{
						tree = temp;
						break;
					}
					else
						tree = 0;
				}

				if (tree!=0)
				{
					vector3df treepos=tree->getPosition();
					vertexXML->SetAttribute("v",tree->getType());
					vertexXML->SetDoubleAttribute("tx",tree->getPosition().X);
					vertexXML->SetDoubleAttribute("ty",tree->getPosition().Y);
					vertexXML->SetDoubleAttribute("tz",tree->getPosition().Z);
					vertexXML->SetDoubleAttribute("tr",tree->getNode()->getRotation().Y);
					vertexXML->SetDoubleAttribute("ts",tree->getNode()->getScale().X);
				}
				// vertexXML->SetAttribute("y",stringc((realPos.Y/(scale/nodescale))).c_str());
				segmentXML->LinkEndChild(vertexXML); 
			}
		} //End meshbuffer save
	} // End custom condition
	*/

	parentElement->LinkEndChild(segmentXML);
}

bool TerrainTile::loadFromXML(TiXmlElement* parentElement)
{
    

	s32 id = 0;
	f32 y = 0.0f;

	vector<TerrainData> vertices;
	vector3df pos=vector3df(0,0,0);
	f32 rota = 0;
	f32 scal = 0;
	f32 tsize = 0;
	int ttype = -1;
	bool atree=false;
	u32 counter=0;
	u32 counter1=0;
	u32 treecounter=0;

	if (!TerrainManager::getInstance()->isParametric()) //Old format for tiles
	{
		TiXmlNode* vertex = parentElement->FirstChild( "vertex" );
		printf("A non-parametric project is loaded, loading tile vertices...\n");
		while( vertex != NULL )
		{
			if (!custom)
			{
				id = atoi(vertex->ToElement()->Attribute("id"));
				y = (f32)atof(vertex->ToElement()->Attribute("y"));
				this->transformMeshByVertex(id,y,false,true);
				core::stringw title=L"Getting terrain vertices:";
				title.append(stringw(counter));
				title.append(L" loaded trees:");
				title.append(stringw(treecounter));
				GUIManager::getInstance()->setTextLoader(title);
				App::getInstance()->getDevice()->getGUIEnvironment()->drawAll();
				counter1++;
			}

			stringc sttype = vertex->ToElement()->Attribute("v");
			if (sttype.size()>0)
			{
				atree=true;
				ttype=atoi(sttype.c_str());
				stringc stposx = vertex->ToElement()->Attribute("tx");
				if (stposx.size()>0)
				{
					treecounter++;
					pos.X = (f32)atof(stposx.c_str());
					pos.Y = (f32)atof(vertex->ToElement()->Attribute("ty"));
					pos.Z = (f32)atof(vertex->ToElement()->Attribute("tz"));
					stringc tsizes = vertex->ToElement()->Attribute("ts");
					stringc ttr = vertex->ToElement()->Attribute("ts");
			
					if (tsizes.size()>0)
						tsize=(f32)atof(vertex->ToElement()->Attribute("ts"));

					if (ttr.size()>0)
						rota=(f32)atof(vertex->ToElement()->Attribute("tr"));
				
					if (!TerrainManager::getInstance()->isParametric())
					{
						printf("Terrain is NOT parametric\n");
						// Now create a new tree with the informations
						Vegetation* v = new Vegetation(ttype);
						v->setPosition(pos);
						if (tsize==0.0f) //Old format, try to compensate with "default values"
						{
							f32 treesize = (f32)(rand() % 100 + 50)/100;
							treesize*= 0.3f;
							v->setScale(vector3df(treesize*(scale/7.5f),treesize*(scale/7.5f),treesize*(scale/7.5f)));
							v->setRotation(vector3df(0,0,0));
						}
						else
						{
							v->setScale(vector3df(tsize,tsize,tsize));
							v->setRotation(vector3df(0,rota,0));
						}
						// Update the infos
						vegetationVector.push_back(v);
					}
				}

			}
			vertex = parentElement->IterateChildren( "vertex", vertex );
	 
		}
		
	}
	else
	{
		printf("A parametric project is loaded, loading tree informations...\n");
		TiXmlNode* tree = parentElement->FirstChild( "tree" );

		while( tree != NULL )
		{
		
			core::stringw title=L"Getting terrain vertices:";
			title.append(stringw(counter));
			title.append(L" loaded trees:");
			title.append(stringw(treecounter));
			GUIManager::getInstance()->setTextLoader(title);
			App::getInstance()->getDevice()->getGUIEnvironment()->drawAll();
			counter++;

			//Put back default values
			atree=false;
			ttype=-1;

			stringc sttype = tree->ToElement()->Attribute("v");
			if (sttype.size()>0)
			{
				atree=true;
				ttype=atoi(sttype.c_str());
				stringc stposx = tree->ToElement()->Attribute("tx");
				if (stposx.size()>0)
				{
					treecounter++;
					pos.X = (f32)atof(stposx.c_str());
					pos.Y = (f32)atof(tree->ToElement()->Attribute("ty"));
					pos.Z = (f32)atof(tree->ToElement()->Attribute("tz"));
					stringc tsizes = tree->ToElement()->Attribute("ts");
					stringc ttr = tree->ToElement()->Attribute("ts");
			
					if (tsizes.size()>0)
						tsize=(f32)atof(tree->ToElement()->Attribute("ts"));

					if (ttr.size()>0)
						rota=(f32)atof(tree->ToElement()->Attribute("tr"));
				
					if (TerrainManager::getInstance()->isParametric())
					{
						printf("Terrain is NOT parametric\n");
						// Now create a new tree with the informations
						Vegetation* v = new Vegetation(ttype);
						v->setPosition(pos);
						if (tsize==0.0f) //Old format, try to compensate with "default values"
						{
							f32 treesize = (f32)(rand() % 100 + 50)/100;
							treesize*= 0.3f;
							v->setScale(vector3df(treesize*(scale/7.5f),treesize*(scale/7.5f),treesize*(scale/7.5f)));
							v->setRotation(vector3df(0,0,0));
						}
						else
						{
							v->setScale(vector3df(tsize,tsize,tsize));
							v->setRotation(vector3df(0,rota,0));
						}
						// Update the infos
						vegetationVector.push_back(v);
					}
				}

			}
			if (!custom)// This slow down when loading and should be optimized.
			{
				if (TerrainManager::getInstance()->isParametric())
				{
					TerrainData data;
					data.id=id;
					data.value=y;
					data.pos=pos;
					data.type=ttype;
					data.tree=atree;
					data.rot=vector3df(0,rota,0);
					data.sca=vector3df(tsize,tsize,tsize);
					vertices.push_back(data);
				} else
					this->transformMeshByVertex(id,y,false,true);
			}

			tree = parentElement->IterateChildren( "tree", tree );
		}
	}
	if (TerrainManager::getInstance()->isParametric())
	{
		this->transformMeshByVertices(vertices, true);
	}
	//Temporary solution so we can edit the tiles (editing was only permitted on non-custom tiles)
	custom=false;
	needrecalc=true;
	//this->recalculate();
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
			vector3df realPos = vector3df (0.0f,0.0f,0.0f);
			//Should be able to place a tree anywhere on a custom model
			if (!custom)
				realPos = mb_vertices[j].Pos*(scale/nodescale) + node->getPosition();
			else
				realPos = clickPos;

            clickPos.Y = realPos.Y;
            if(realPos.getDistanceFrom(clickPos) < vegetationRange/2 && getVegetationAt(vector3df(realPos.X,realPos.Y,realPos.Z)))
            {
                Vegetation* toRemove = getVegetationAt(vector3df(realPos.X,realPos.Y,realPos.Z));

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
            vector3df realPos = vector3df (0.0f,0.0f,0.0f);
			//Should be able to place a tree anywhere on a custom model
			if (!custom)
				realPos = mb_vertices[j].Pos*(scale/nodescale) + node->getPosition();
			else
				realPos = clickPos;

            clickPos.Y = realPos.Y;
            if(realPos.getDistanceFrom(clickPos) < (vegetationRange/2) && !getVegetationAt(vector3df(realPos.X,realPos.Y,realPos.Z)))
            {
                Vegetation* v = new Vegetation();

                //v->setPosition(vector3df(realPos.X + (rand()%5)*0.1f - 0.25f,realPos.Y/(scale/nodescale),realPos.Z + (rand()%5)*0.1f - 0.25f));
				//v->setPosition(vector3df(realPos.X + (rand()%5)*scale/100,realPos.Y,realPos.Z + (rand()%5)*scale/100));
				v->setPosition(vector3df(realPos.X,realPos.Y,realPos.Z));
                f32 treesize = (f32)(rand() % 50 + 25);
                
				v->setScale(vector3df(treesize,treesize,treesize));

#ifdef DEBUG
				printf("Attempting to place a tree with this size: %f\n",treesize);
#endif
                vegetationVector.push_back(v);

                #ifdef APP_DEBUG
                cout << "DEBUG : TERRAIN TILE : VEGETATION CREATED: " << realPos.X << "," << realPos.Y << "," << realPos.Z << "   TOTAL:" << vegetationVector.size() << endl;
                #endif
	        }
	    }
	}
}

// Test the tile if it was being modified
bool TerrainTile::checkModified()
{
	bool modified = false;

    IMeshBuffer* meshBuffer = ((IMeshSceneNode*)node)->getMesh()->getMeshBuffer(0);
	S3DVertex* mb_vertices = (S3DVertex*) meshBuffer->getVertices();

    for (unsigned int j = 0; j < meshBuffer->getVertexCount(); j += 1)
	{
	    vector3df realPos = mb_vertices[j].Pos*(scale/nodescale) + node->getPosition();
	    if(realPos.Y != 0.0f )
        {
           modified = true;
		   j=meshBuffer->getVertexCount();
		   break;
        }
	}

	return modified;

}

void TerrainTile::transformMeshByVertices(vector<TerrainData> list, bool norecalc)
{
	
	core::stringw title=L"Generating terrain";
	GUIManager::getInstance()->setTextLoader(title);
	App::getInstance()->getDevice()->getGUIEnvironment()->drawAll();

	//Terrain first
	for (u32 a=0; a<list.size(); a++)
	{
		mb_vertices[list[a].id].Pos.Y = list[a].value;
	}

	App::getInstance()->quickUpdate();
	
	int counter=0;
	//Then plant the trees
	title=L"Generating trees";
	GUIManager::getInstance()->setTextLoader(title);
	App::getInstance()->getDevice()->getGUIEnvironment()->drawAll();
	for (u32 a=0; a<list.size(); a++)
	{
		if (list[a].tree && list[a].type>=0)
		{	// Now create a new tree with the informations
			Vegetation* v = new Vegetation(list[a].type);
			if (v)
			{
				counter++;
				v->setPosition(list[a].pos);
				v->setScale(list[a].sca);
				v->setRotation(list[a].rot);
			
				// Update the infos
				vegetationVector.push_back(v);
			}
		}
	}
	

	if (!norecalc)
		recalculate();
	else
	{	
		needrecalc=true;
		recalculate(true);
	}

}

void TerrainTile::transformMeshByVertex(s32 id, f32 y, bool addVegetation, bool norecalc)
{
	if (custom)
		return;

	mb_vertices[id].Pos.Y = y;

	if(addVegetation)
	{
	    vector3df treePos = vector3df(mb_vertices[id].Pos.X,y , mb_vertices[id].Pos.Z);
	    treePos *= node->getScale();
	    treePos.Y = y;
	    treePos += node->getPosition();
	    paintVegetation(treePos ,false);
	}

	App::getInstance()->quickUpdate();

	
	if (!norecalc)
		recalculate();
	else
	{	
		needrecalc=true;
		recalculate(true);
	}
}

void TerrainTile::recalculate(bool simple)
{
	if (custom)
		return;
	if (needrecalc)
	{
		//This part only refresh the visual model, will not change the collision shape
		smgr->getMeshManipulator()->recalculateNormals(((IMeshSceneNode*)node)->getMesh(),true);
		core::aabbox3df box=node->getBoundingBox();
		((IMeshSceneNode*)node)->getMesh()->setBoundingBox(core::aabbox3df(-box.getExtent().X/2,-256.0f,-box.getExtent().Z/2,box.getExtent().X/2,1024,box.getExtent().Z/2));
		((IMeshSceneNode*)node)->getMesh()->setDirty();
	}

	// Simple is used by the brush when carving, once the carve is done, if needrecalc is activated will redo the collision shape 
	// also used by terrainManager::recalculate()
	if (!simple && needrecalc)
	{
		// Attempt to update the triangle selector with the new mesh, will get the modified mesh and recreate the collision mesh
		ITriangleSelector * selector = smgr->createTriangleSelector(((IMeshSceneNode*)node)->getMesh(),node);
		//----ITriangleSelector * selector = smgr->createOctTreeTriangleSelector(((IMeshSceneNode*)node)->getMesh(),node);
		node->setTriangleSelector(selector);
		selector->drop();
		needrecalc = false;
#ifdef DEBUG
		printf("RECALCULATING SEGMENT: %s\n",getName().c_str());
#endif
	}

}


void TerrainTile::transformMesh(vector3df clickPos, f32 radius, f32 radius2, f32 strength, bool norecalc)
{

	if (custom)
		return;

    IMeshBuffer* meshBuffer = ((IMeshSceneNode*)node)->getMesh()->getMeshBuffer(0);

	S3DVertex* mb_vertices = (S3DVertex*) meshBuffer->getVertices();

	u16* mb_indices  = meshBuffer->getIndices();

	for (unsigned int j = 0; j < meshBuffer->getVertexCount(); j += 1)
	{
	    vector3df realPos = mb_vertices[j].Pos*(scale/nodescale) + node->getPosition();
        clickPos.Y = realPos.Y;

	    if(realPos.getDistanceFrom(clickPos) < radius)
	    {
			needrecalc=true; // This will flag the tile since it's in the radius and will modify the tile vertices.

            //f32 ratio = sin(radius - realPos.getDistanceFrom(clickPos));
			//- (realPos.getDistanceFrom(clickPos)-radius2)
			f32 ratio = radius;

			if (radius2-realPos.getDistanceFrom(clickPos)<0)
				ratio = (radius+radius2)-realPos.getDistanceFrom(clickPos);

	        mb_vertices[j].Pos.Y += (strength * (ratio)/(scale/nodescale));
			//printf("found something here: vertice %i, vertice Y: %f\n",j, mb_vertices[j].Pos.Y);
	    }

		//if(mb_vertices[j].Pos.Y > nodescale/4) mb_vertices[j].Pos.Y = nodescale/4;
		// Fix up/down limits
		if(mb_vertices[j].Pos.Y > nodescale*0.75f) mb_vertices[j].Pos.Y = nodescale*0.75f;
	    if(mb_vertices[j].Pos.Y < -(nodescale*0.25f)) mb_vertices[j].Pos.Y = -(nodescale*0.25f);
	}

	if (norecalc) 
	{	
		recalculate(true);
	}
	else
		recalculate();

}

void TerrainTile::transformMeshToValue(vector3df clickPos, f32 radius, f32 radius2, f32 strength, f32 value, bool norecalc)
{

	if (custom)
		return;

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
			needrecalc=true;
            //f32 ratio = sin(radius - realPos.getDistanceFrom(clickPos));
			f32 ratio = radius;

			if (radius2-realPos.getDistanceFrom(clickPos)<0)
				ratio = (radius+radius2)-realPos.getDistanceFrom(clickPos);

            if(mb_vertices[j].Pos.Y > value)
            {
				//mb_vertices[j].Pos.Y -= strength * ratio / (scale/nodescale);
                mb_vertices[j].Pos.Y -= strength * ratio / (scale/nodescale);
                if(mb_vertices[j].Pos.Y <= value) mb_vertices[j].Pos.Y = value;

            }
            if(mb_vertices[j].Pos.Y < value)
            {
		        //mb_vertices[j].Pos.Y += strength * ratio / (scale / nodescale);
				mb_vertices[j].Pos.Y += strength *ratio / (scale / nodescale);
                if(mb_vertices[j].Pos.Y >= value) mb_vertices[j].Pos.Y = value;
            }
	    }
	}

	if (norecalc) 
	{	
		recalculate(true);
	}
	else
		recalculate();
}

//Get the elevation from a ray test on the tile.
//For this to work, the collision mesh must be up to date
f32 TerrainTile::getHeightAt(vector3df pos)
{
	// Check from the top of the character
	// old value is 4000.0f
	irr::f32 maxRayHeight = 80.0f;
	scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();
	core::line3d<f32> ray;

	// Start the ray 500 unit from the character, ray lenght is 1000 unit.
    ray.start = pos+vector3df(0,+(maxRayHeight/2.0f),0);
    //ray.end = ray.start + (pos+vector3df(0,-maxRayHeight/2.0f,0) - ray.start).normalize() * maxRayHeight;
	ray.end = pos+vector3df(0,-(maxRayHeight/2),0);

	// Tracks the current intersection point with the level or a mesh
	core::vector3df intersection;
    // Used to show with triangle has been hit
    core::triangle3df hitTriangle;
	scene::ISceneNode * selectedSceneNode =
    collMan->getSceneNodeAndCollisionPointFromRay(
		ray,
		intersection,
		hitTriangle,
		100, //100 is the default ID for walkable (ground obj + props)
		0); // Check the entire scene (this is actually the implicit default)

	if (selectedSceneNode)
	{
		// return the height found.
		return intersection.Y;
	}
	else
		// if not return -1000 (Impossible value, so it failed)
		return -1000;
}

f32 TerrainTile::getVerticeHeight(vector3df pos)
{
	f32 radius = 100;
	f32 returnvalue = 0;
	f32 smallest = radius;
	IMeshBuffer* meshBuffer = ((IMeshSceneNode*)node)->getMesh()->getMeshBuffer(0);

	S3DVertex* mb_vertices = (S3DVertex*) meshBuffer->getVertices();

	u16* mb_indices  = meshBuffer->getIndices();

	//Check all indices in the mesh to find the one that is nearest the position
	for (unsigned int j = 0; j < meshBuffer->getVertexCount(); j += 1)
	{
	    vector3df realPos = mb_vertices[j].Pos*(scale/nodescale) + node->getPosition();
        pos.Y = realPos.Y; //Put the height at the same height

		//Check if its the smallest distance
	    if((realPos.getDistanceFrom(pos) < smallest))
	    {
			smallest = realPos.getDistanceFrom(pos);
			returnvalue = realPos.Y;            
	    }

	}
	return returnvalue;

}

void TerrainTile::showDebugData(bool show)
{
    for(int i=0;i<(int)vegetationVector.size();i++)
    {
        vegetationVector[i]->showDebugData(show);
    }
}

void TerrainTile::assignTerrainShader(irr::scene::ISceneNode *node)
{
	stringc texture0 = TerrainManager::getInstance()->getTerrainTexture(0);
	stringc texture1 = TerrainManager::getInstance()->getTerrainTexture(1);
	stringc texture2 = TerrainManager::getInstance()->getTerrainTexture(2);
	stringc texture3 = TerrainManager::getInstance()->getTerrainTexture(3);
	stringc texture4 = TerrainManager::getInstance()->getTerrainTexture(4);

	static ITexture* layer0 = smgr->getVideoDriver()->getTexture(texture0.c_str());
	static ITexture* layer1 = smgr->getVideoDriver()->getTexture(texture1.c_str());
	static ITexture* layer2 = smgr->getVideoDriver()->getTexture(texture2.c_str());
    static ITexture* layer3 = smgr->getVideoDriver()->getTexture(texture3.c_str());
	static ITexture* layer4 = smgr->getVideoDriver()->getTexture(texture4.c_str());

	static s32 materialTerrain = 0;
	// Disable 8 textures for the moment, will be enabled by the user directly.
	bool heighttextures = false;
    //Create a Custom GLSL Material (Terrain Splatting)
	if (heighttextures)
	{
		//Hardware support for 8 textures
		materialTerrain=smgr->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
        "../media/shaders/splat.vert", "vertexMain", video::EVST_VS_1_1,
        "../media/shaders/splat8.frag", "pixelMain", video::EPST_PS_1_4,
        ShaderCallBack::getInstance(), video::EMT_SOLID);

		//Assign Textures
		node->setMaterialTexture(0,layer0);
		node->setMaterialTexture(1,layer1);
		node->setMaterialTexture(2,layer2);
		node->setMaterialTexture(3,layer3);
		node->setMaterialTexture(4,layer4);

	}
	else
	{
		// Hardware support for 4 textures
		materialTerrain=smgr->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
        "../media/shaders/splat.vert", "vertexMain", video::EVST_VS_1_1,
        "../media/shaders/splat4.frag", "pixelMain", video::EPST_PS_1_4,
        ShaderCallBack::getInstance(), video::EMT_SOLID);

		//Assign Textures
#ifdef WIN32 // Strange 
		// On Windows
		node->setMaterialTexture(0,layer1);
		node->setMaterialTexture(1,layer2);
		node->setMaterialTexture(2,layer3);
		node->setMaterialTexture(3,layer4);
#else
		// On Linux
		node->setMaterialTexture(0,layer0);
		node->setMaterialTexture(1,layer1);
		node->setMaterialTexture(2,layer2);
		node->setMaterialTexture(3,layer3);
#endif
	}

	 //node->setMaterialTexture(0,layer1);

    //Assign GLSL Shader
	node->getMaterial(0).setFlag(EMF_LIGHTING,false);
	node->getMaterial(0).setFlag(EMF_FOG_ENABLE,true);
    node->setMaterialType((E_MATERIAL_TYPE)materialTerrain);
}

void TerrainTile::assignWaterShader(irr::scene::ISceneNode *node)
{
	 //Create a Custom GLSL Material (Water shader)
	static s32 materialOcean=smgr->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
        "../media/shaders/ocean.vert", "vertexMain", video::EVST_VS_1_1,
        "../media/shaders/ocean.frag", "pixelMain", video::EPST_PS_1_4,
        ShaderCallBack::getInstance(), video::EMT_TRANSPARENT_ALPHA_CHANNEL);

    static ITexture* oceanLayer0 = smgr->getVideoDriver()->getTexture("../media/waveNM.png");
    static ITexture* oceanLayer1 = smgr->getVideoDriver()->getTexture("../media/sky.jpg");
	static ITexture* oceanLayer2 = smgr->getVideoDriver()->getTexture("../media/WaterFoam01.jpg");
	// Water shader

	node->setMaterialType((E_MATERIAL_TYPE)materialOcean);
	node->setMaterialTexture(0,oceanLayer0);
	node->setMaterialTexture(1,oceanLayer1);
	node->setMaterialTexture(2,oceanLayer2);

	//node->setMaterialFlag(EMF_FOG_ENABLE,true);    
	//node->setMaterialFlag(EMF_BLEND_OPERATION,true);
}

void TerrainTile::clean()
{
	vegetationVector.clear();	
}