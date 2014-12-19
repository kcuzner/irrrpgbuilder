#include "TerrainManager.h"
#include "../events/EventReceiver.h"
#include "../gui/GUIManager.h"

#include <sstream>
#include <vector>

//For access to folders function of the os
//#include <sys/types.h>
//#include <sys/stat.h>
// Same for windows
#include <direct.h>
//#include <stdlib.h>
//#include <stdio.h>

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
	scale=1024.0f;
    terrainEmptySegmentsMap.clear();
    terrainMap.clear();
	tileTagged=NULL;
	timer = 0;
	needrecalc=false;
	lastbrushtime=0;
	brushstep = 10; //10 degree increment maximum for the brush circle
	empty_texture_scale = 1.0f;
	parametric=true;
	filename="";
}

TerrainManager::~TerrainManager()
{
	clean();
	
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
        #ifdef DEBUG
        cout << "DEBUG : TERRAIN MANAGER : SEGMENT ALREADY EXIST: " << getHashCode(pos) << endl;
        #endif
        return;
    }


    ISceneNode* newEmptySegment = App::getInstance()->getDevice()->getSceneManager()->addCubeSceneNode(1.0f,0,100);
	
	// in the work, for some reason this is slow when the texture is applied
	//scene::IAnimatedMesh * mesh = App::getInstance()->getDevice()->getSceneManager()->addHillPlaneMesh("",dimension2d<f32>(1024,1024),dimension2d<u32>(1,1),&tempnode->getMaterial(0));
	//App::getInstance()->getDevice()->getVideoDriver()->getMeshManipulator()->makePlanarTextureMapping(mesh,1);
	//ISceneNode* newEmptySegment = App::getInstance()->getDevice()->getSceneManager()->addMeshSceneNode(mesh);
	//tempnode->remove();
	
	//newEmptySegment->setMaterialFlag(EMF_BACK_FACE_CULLING,false);
	//newEmptySegment->setMaterialFlag(EMF_WIREFRAME,true);
	newEmptySegment->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
	newEmptySegment->setPosition(vector3df((pos.X)*scale,0,(pos.Z)*scale));
    newEmptySegment->setScale(vector3df(scale,0.01f,scale) ) ;

    ITriangleSelector* sel = App::getInstance()->getDevice()->getSceneManager()->createTriangleSelectorFromBoundingBox(newEmptySegment);
    newEmptySegment->setTriangleSelector(sel);

	App::getInstance()->getDevice()->getVideoDriver()->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS,true); 
    newEmptySegment->setMaterialTexture(0,App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/editor/terrain_empty_segment.png"));
    newEmptySegment->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
	newEmptySegment->getMaterial(0).getTextureMatrix(0).setTextureScale(empty_texture_scale*3.0f,empty_texture_scale*3.0f);
	//newEmptySegment->getMaterial(0).getTextureMatrix(0).setTextureScaleCenter(50,50);
    newEmptySegment->setName(getHashCode(pos).c_str());
    terrainEmptySegmentsMap.insert(TerrainEmptySegmentsMapPair(getHashCode(pos).c_str(),newEmptySegment));
	App::getInstance()->getDevice()->getVideoDriver()->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS,true); 


   // #ifdef DEBUG
   // cout << "DEBUG : TERRAIN MANAGER : CREATED NEW EMPTY TERRAIN SEGMENT : " << getHashCode(pos) << " TOTAL:" << terrainEmptySegmentsMap.size() << endl;
   // #endif
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
    }
}

//Scale the texture on all the present empty tiles to match the grid size
void TerrainManager::setEmptyTileGridScale(f32 scale)
{
	std::map<std::string, ISceneNode*>::iterator it;

	// For this to work, the texture MUST use the defined texture.
		f32 gridsize = 1.0f;
		 if (scale==64)
			 gridsize=1.0f;
		 if (scale==32)
			 gridsize=2.0f;
		 if (scale==16)
			 gridsize=4.0f;
		 if (scale==8)
			 gridsize=8.0f;
		 if (scale==4)
			 gridsize=16.0f;
		 if (scale==2)
			 gridsize=32.0f;
		 if (scale==128)
			 gridsize=0.5f;
		 if (scale==256)
			 gridsize=0.25f;
		 if (scale==512)
			 gridsize=0.125f;
		 if (scale==1024)
			 gridsize=0.0635f;

	//Scaleratio take into account the scale of the tile, the tile is ajusted for a 1024 size tile.
	 f32 scaleratio = this->scale/1024.0f;
	 gridsize = gridsize*scaleratio;

	for ( it=terrainEmptySegmentsMap.begin() ; it != terrainEmptySegmentsMap.end(); it++ )
    {
		((ISceneNode*)((*it).second))->getMaterial(0).getTextureMatrix(0).setTextureScale(gridsize,gridsize);
    }
	empty_texture_scale = gridsize; //Redefine the "default" texture size for a new empty tile.
}

// Create an empty tile matrix, the user should not have to create this by hand.
void TerrainManager::createEmptySegmentMatrix(u32 x, u32 y)
{
	if (x<1)
		x=1;
	if (y<1)
		y=1;

	vector3df pos=vector3df(0,0,0);
	for (u32 xi=0; xi<x; ++xi)
	{
		pos=vector3df(0,0,0);
		pos.X=-(f32(x)/2) + xi;
		for (u32 yi=0; yi<x; ++yi)
		{
			pos.Z=-((f32(y))/2)+yi;
			createEmptySegment(pos);
		}
	}

}

void TerrainManager::createSegment(vector3df pos, bool empty, bool noextra, bool param)
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
#ifdef DEBUG
        cout << "DEBUG : TERRAIN MANAGER : SEGMENT ALREADY EXIST: " << getHashCode(pos) << endl;
#endif
        return;
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
											getHashCode(pos).c_str(),false,param );
		terrainMap.insert(TerrainMapPair(newTile->getName().c_str(),newTile));

		removeEmptySegment(pos, true);

#ifdef DEBUG
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
		// Perhaps edges vertices could be put in a buffer for each tile
		// Then the sewing would be much faster
		// Could create 4 buffers (north south east west)
		// Then determine the new tile position and only check the proper buffer
		// Perhaps then use the buffers later to create a "skirt"
		if (!noextra)
		{
			//Have to do a faster way to do this.
			//perhaps get all the tiles around and set the position (4 corners in 1 pass)
			//And add a GUI to "sew" the tiles again. Need too much processing
			//newTile->mergeToTile(getSegment(vector3df(pos.X-1,0,pos.Z)));
			//newTile->mergeToTile(getSegment(vector3df(pos.X+1,0,pos.Z)));
			//newTile->mergeToTile(getSegment(vector3df(pos.X,0,pos.Z-1)));
			//newTile->mergeToTile(getSegment(vector3df(pos.X,0,pos.Z+1)));
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

ISceneNode * TerrainManager::createCustomTile(vector3df pos, core::stringc model)
{
	if (pos.Y==-1000)
		return NULL;

    //Must be rounded positions (to keep it in the grid)
    pos.X = (f32)round32(pos.X);
    pos.Y = (f32)round32(pos.Y);
    pos.Z = (f32)round32(pos.Z);


	TerrainTile* newTile = NULL;

	newTile=new TerrainTile(App::getInstance()->getDevice()->getSceneManager(),
											0,
											pos,
											getHashCode(pos).c_str(),true);

		newTile->customname = model;
		newTile->createCustom(0,pos,getHashCode(pos).c_str(),model);
		terrainMap.insert(TerrainMapPair(newTile->getName().c_str(),newTile));
	return NULL;
}

ISceneNode * TerrainManager::createCustomSegment(vector3df pos, core::stringc model)
{
	if (pos.Y==-1000)
		return NULL;

    //Must be rounded positions (to keep it in the grid)
    pos.X = (f32)round32(pos.X);
    pos.Y = (f32)round32(pos.Y);
    pos.Z = (f32)round32(pos.Z);


	TerrainTile* newTile = NULL;

	bool wasthere = false;
    //if segment already exists it don't need to be created
    if( getSegment(pos) )
    {
		wasthere=true;
#ifdef DEBUG
        cout << "DEBUG : TERRAIN MANAGER : SEGMENT ALREADY EXIST: " << getHashCode(pos) << endl;
#endif
        return NULL;
    }


	if (!wasthere)
	{
		removeEmptySegment(pos, true);

#ifdef DEBUG
	cout << "REmoving empty and placing the custom tile" << endl;
#endif

		newTile=new TerrainTile(App::getInstance()->getDevice()->getSceneManager(),
											0,
											pos,
											getHashCode(pos).c_str(),true);

		
		newTile->customname = model;
		newTile->createCustom(0,pos,getHashCode(pos).c_str(),model);

		terrainMap.insert(TerrainMapPair(newTile->getName().c_str(),newTile));

		//Safety check if the object/model cannot be loaded
		if (!newTile->getNode())
		{
			App::getInstance()->setAppState(App::APP_EDIT_WAIT_GUI);
			App::getInstance()->getDevice()->getGUIEnvironment()->addMessageBox(L"Loading error!",L"Failed to load the tile model or model is missing!");
			if(getSegment(pos))
			{
				tileTagged = terrainMap.find(getHashCode(pos))->second;
				deleteTaggedSegment();
			}
		} else
			return newTile->getNode();


	}

	return NULL;

#ifdef DEBUG
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

        #ifdef DEBUG
        cout << "DEBUG : TERRAIN MANAGER : EMPTY SEGMENT REMOVED: " << getHashCode(pos) << " TOTAL:" << terrainEmptySegmentsMap.size() << endl;
        #endif
    }
}

void TerrainManager::removeSegment(vector3df pos, bool custom)
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
		if (test && !custom)
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

     #ifdef DEBUG
 	 //cout << "DEBUG : TERRAIN MANAGER : SEGMENT REMOVED: " << getHashCode(posTagged) << " TOTAL:" << TerrainMap.size() << endl;
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


	core::stringc oldpath = (core::stringc)App::getInstance()->getDevice()->getFileSystem()->getWorkingDirectory();
	printf("here is the default path:%s\n",oldpath);
    TiXmlElement* terrainXML = new TiXmlElement("terrain");
	terrainXML->SetAttribute("texture0",terraintexture0.c_str());
	terrainXML->SetAttribute("texture1",terraintexture1.c_str());
	terrainXML->SetAttribute("texture2",terraintexture2.c_str());
	terrainXML->SetAttribute("texture3",terraintexture3.c_str());
	terrainXML->SetAttribute("texture4",terraintexture4.c_str());

	if (parametric)
	{   //New force a new type of object to be parametric. So we can see the difference with old projects
		terrainXML->SetAttribute("Type","Parametric"); 
	}
	terrainXML->SetAttribute("Type","Parametric"); 
	terrainXML->SetDoubleAttribute("Scale",(float)scale);

    terrainXML->SetAttribute("segments",(int)terrainMap.size());

    std::map<std::string, TerrainTile*>::iterator it;

	filename = App::getInstance()->filename;
	filename = filename.subString(0,(filename.size()-4));
	filename = App::getInstance()->getDevice()->getFileSystem()->getFileBasename(App::getInstance()->filename,false);
	printf("This is the current folder: %s\n",filename.c_str());
	

	//REference code (Should be tested on Linux
	//int status;
	//status = mkdir(filename.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	//Create only a folder if there are terrain tiles in the map
	if (terrainMap.size()>0)
	{
		int status;
		status = _mkdir(filename.c_str());
	}
	filename="";
    //Save all segments to XML
    for ( it=terrainMap.begin() ; it != terrainMap.end(); it++ )
    {
#ifdef EDITOR //Update the display only in editor
		App::getInstance()->quickUpdate();
#endif
		filename = App::getInstance()->filename;
		core::stringc pathxml=(stringc)App::getInstance()->getDevice()->getFileSystem()->getFileDir(filename);
		core::stringc filexml=(stringc)App::getInstance()->getDevice()->getFileSystem()->getFileBasename(filename,false);
		pathxml.append("/");
		pathxml.append(filexml);
		pathxml.append("/tile");
		pathxml.append((*it).second->getName());
		pathxml.append(".b3d");
		filename=pathxml;
        ((TerrainTile*)((*it).second))->saveToXML(terrainXML);

		core::stringc path=(stringc)App::getInstance()->getDevice()->getFileSystem()->getFileDir(filename);
		core::stringc path_2=(stringc)App::getInstance()->getDevice()->getFileSystem()->getRelativeFilename(filename,
				App::getInstance()->path);
		//printf("This is the Full XML filename: %s\n",filename.c_str());
		//printf("This is the relative XML filename: %s\n",path_2.c_str());
		filename="";
    }
    parentElement->LinkEndChild(terrainXML);
	
	//Save tiles separately
	// Always save the tiles as object from now on.
	{
		std::map<std::string, TerrainTile*>::iterator it;
		for ( it=terrainMap.begin() ; it != terrainMap.end(); it++ )
		{
			filename = App::getInstance()->filename;
			filename=filename.subString(0,(filename.size()-4));
			filename.append("/tile");
			filename.append((*it).second->getName());
			filename.append(".b3d");

			core::stringc file1=filename;
			core::stringc path=(stringc)App::getInstance()->getDevice()->getFileSystem()->getFileDir(file1);
			core::stringc path2=(stringc)App::getInstance()->getDevice()->getFileSystem()->getRelativeFilename(path,
				App::getInstance()->getDevice()->getFileSystem()->getWorkingDirectory());

			core::stringc filename=(stringc)App::getInstance()->getDevice()->getFileSystem()->getRelativeFilename(file1,
					App::getInstance()->getDevice()->getFileSystem()->getWorkingDirectory());

			path2.append("/");
			path2.append(filename);

			//filename=path2;
			path.append("/tile");
			path.append((*it).second->getName());
			path.append(".b3d");
			filename=path;

			IMeshWriter* mw = App::getInstance()->getDevice()->getSceneManager()->createMeshWriter(EMWT_B3D);
			IWriteFile* file = App::getInstance()->getDevice()->getFileSystem()->createAndWriteFile(filename.c_str());
			IMesh* mesh = (((*it).second))->getMesh();

			printf("This is the B3D filename: %s\n",filename.c_str());

			if (mesh)
				mw->writeMesh(file,  mesh);
	
			if (file)
				file->drop();
			
			if (mw)
				mw->drop();

			filename="";
		}
	}
}

bool TerrainManager::loadFromXML(TiXmlElement* parentElement)
{
    clean();

	core::stringc tempstr="";
	tempstr = parentElement->ToElement()->Attribute("texture0");
	if (tempstr.size()>0)
		terraintexture0 = tempstr;
	tempstr = parentElement->ToElement()->Attribute("texture1");
	if (tempstr.size()>0)
		terraintexture1 = tempstr;
	tempstr = parentElement->ToElement()->Attribute("texture2");
	if (tempstr.size()>0)
		terraintexture2 = tempstr;
	tempstr = parentElement->ToElement()->Attribute("texture3");
	if (tempstr.size()>0)
		terraintexture3 = tempstr;
	tempstr = parentElement->ToElement()->Attribute("texture4");
	if (tempstr.size()>0)
		terraintexture4 = tempstr;

	//New check if the tile is defined as parametric (0.3)
	stringc param = parentElement->ToElement()->Attribute("Type");
	bool isparam = true;
	if (param!="Parametric")
	{
		isparam=false;
		parametric=false;
		scale=1024; //Force to 1024 unit as old default, as it was not saved before 0.3
	}
	stringc tscale = parentElement->ToElement()->Attribute("Scale");
	if (tscale.size()>0)
	{
		if (atof(tscale.c_str())!=scale) //Change the scale only if it's not set the same as the default
			setScale(atof(tscale.c_str()));
	}


	TiXmlNode* tSegment = parentElement->FirstChild( "terrainSegment" );

	while( tSegment != NULL )
    {
#ifdef EDITOR //only update the display in editor
		App::getInstance()->quickUpdate();
#endif
        f32 x = (f32)atof(tSegment->ToElement()->Attribute("x"));
        f32 z = (f32)atof(tSegment->ToElement()->Attribute("z"));

		core::stringc mesh=tSegment->ToElement()->Attribute("mesh");

		if (mesh.size()>0)
			createCustomSegment(vector3df( x/scale ,0, z/scale ), mesh); //Create a terrain tile from a mesh
		else
			createSegment(vector3df( x/scale ,0, z/scale ),false,true,isparam); //Create a segment

		
        TerrainTile* tempTile = getSegment( vector3df( x/scale ,0, z/scale ) );


        if(tempTile)
        {
            tempTile->loadFromXML(tSegment->ToElement());
        }

        tSegment = parentElement->IterateChildren( "terrainSegment", tSegment );
    }
#ifdef EDITOR //only update in the editor
	App::getInstance()->quickUpdate();
#endif

	GUIManager::getInstance()->setTextLoader(L"Creating the collision meshes");
#ifdef EDITOR
	App::getInstance()->quickUpdate();
#endif
	recalculate();
	return true;
}

void TerrainManager::paintVegetation(App::MousePick mousePick, bool erase)
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

void TerrainManager::transformSegments(App::MousePick mousePick, f32 radius, f32 radius2, f32 strength,  bool norecalc)
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
                if(tempTile) 
					tempTile->transformMesh(mousePick.pickedPos,radius,radius2,strength,norecalc);
            }
        }
    }
}

void TerrainManager::transformSegmentsToValue(App::MousePick mousePick, f32 radius, f32 radius2, f32 strength, f32 value,  bool norecalc)
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
                if(tempTile) tempTile->transformMeshToValue(mousePick.pickedPos,radius,radius2,strength,value,norecalc);
            }
        }
    }
}

//Will recalculate the terrain tiles that are "marked" as needed for recalculation of collision shapes
void TerrainManager::recalculate()
{
	needrecalc=false;
    std::map<std::string, TerrainTile*>::iterator it;
    for ( it=terrainMap.begin() ; it != terrainMap.end(); it++ )
    {
		((TerrainTile*)((*it).second))->recalculate();
    }

}

void TerrainManager::resetVegetationHeight()
{
	needrecalc=false;
    std::map<std::string, TerrainTile*>::iterator it;
    for ( it=terrainMap.begin() ; it != terrainMap.end(); it++ )
    {
		((TerrainTile*)((*it).second))->resetVegetationHeight();
    }

}

f32 TerrainManager::getHeightAt(vector3df pos, f32 rayheight)
{
    vector3df hashCode = pos/this->getScale();

    TerrainTile* segment = this->getSegment(hashCode);

    if(segment)
        return segment->getHeightAt(pos, rayheight);
    else
        return -1000;
}

f32 TerrainManager::getVerticeHeight(vector3df pos)
{
	 vector3df hashCode = pos/this->getScale();

    TerrainTile* segment = this->getSegment(hashCode);

    if(segment)
		return segment->getVerticeHeight(pos);
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
		if (s)
		{
			s->remove();
			s=NULL;
		}
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

void TerrainManager::drawBrush(bool useray)
{
	IVideoDriver* driver = App::getInstance()->getDevice()->getVideoDriver();
	vector3df camref = App::getInstance()->getDevice()->getSceneManager()->getActiveCamera()->getPosition();

	f32 height=0.0f;

	f32 radius = App::getInstance()->getBrushRadius();
	f32 radius2 = App::getInstance()->getBrushRadius(1); // get the inner brush radius
	vector3df position = App::getInstance()->getMousePosition3D(100).pickedPos;
	if (position==vector3df(0,0,0))
		return;

	SMaterial m;
	m.Lighting=false;
	driver->setMaterial(m);
	driver->setTransform(video::ETS_WORLD, core::matrix4());

	u32 time = App::getInstance()->getDevice()->getTimer()->getRealTime();
	if (lastbrushtime>25)
		brushstep+=1;

	if (lastbrushtime<20)
		brushstep-=1;

	if (brushstep>60)
		brushstep=60;

	if (brushstep<10)
		brushstep=10;

	// Display the inner brush size if the user change the default value of 5 to another value
	if (useray)
		drawBrushCircleSmooth(position, radius, brushstep, false , useray);
	else
		drawBrushCircleSmooth(position, radius,int(brushstep/2.0f), false, useray);

	if (radius2!=5.0f)
	{
		if (useray)
			drawBrushCircleSmooth(position, radius2, int(brushstep*1.5f), false, useray);
		//else
			//drawBrushCircle(position, radius, brushstep, false, useray);
	}
	//drawBrushCircle(position, 5, 30, true, useray);
	drawBrushCircleSmooth(position, 5, 30, true, useray);
	u32 time1 = App::getInstance()->getDevice()->getTimer()->getRealTime();

	//Update the time comparison only when using the brush with ray
	if (useray)
		lastbrushtime=time1-time;

	//printf ("Here is the time delay:%d , step is now: %d\n",int(lastbrushtime),brushstep);

}

void TerrainManager::drawBrushCircle(vector3df position, f32 radius, int step,bool drawlines,bool useray)
{
	IVideoDriver* driver = App::getInstance()->getDevice()->getVideoDriver();
	vector3df camref = App::getInstance()->getDevice()->getSceneManager()->getActiveCamera()->getPosition();

	// Render the size of the brush.
	f32 framesize = position.getDistanceFrom(camref)/400.0f;

	f32 height=0.0f;
	// Display the inner brush size if the user change the default value of 5 to another value
	for (int i=0; i<(360); i=i+step)
	{
		float degInRad = i*DEG2RAD;
		vector3df pos=position;

		pos.X+=cos(degInRad)*radius;
		pos.Z+=sin(degInRad)*radius;

		if (useray)
			height=getHeightAt(pos);
		else
			height=getVerticeHeight(pos);

		if (height==-1000.0f)
			height=0.0f;

		pos.Y=height+framesize;

		float degInRad2 = (i+step)*DEG2RAD;

		vector3df pos2=position;

		pos2.X+=cos(degInRad2)*radius;
		pos2.Z+=sin(degInRad2)*radius;

		if (useray)
			height=getHeightAt(pos2);
		else
			height=getVerticeHeight(pos2);

		if (height==-1000.0f)
			height=0.0f;

		pos2.Y=height+framesize;

		if (drawlines)
			driver->draw3DLine(pos,pos2,video::SColor(255,255,255,0));
		else
		{
			vector3df pos3=position;
			pos3.X+=cos(degInRad)*(radius+framesize);
			pos3.Z+=sin(degInRad)*(radius+framesize);
			pos3.Y=pos.Y;

			vector3df pos4=position;
			pos4.X+=cos(degInRad2)*(radius+framesize);
			pos4.Z+=sin(degInRad2)*(radius+framesize);
			pos4.Y=pos2.Y;

			if (useray)
			{
				driver->draw3DTriangle(triangle3df(pos4,pos3,pos),video::SColor(128,255,255,200));
				driver->draw3DTriangle(triangle3df(pos,pos2,pos4),video::SColor(128,255,255,200));
			}
			else
			{
				driver->draw3DTriangle(triangle3df(pos4,pos3,pos),video::SColor(128,255,64,64));
				driver->draw3DTriangle(triangle3df(pos,pos2,pos4),video::SColor(128,255,64,64));
			}


		}

	}
}

void TerrainManager::drawBrushCircleSmooth(vector3df position, f32 radius, int step,bool drawlines,bool useray)
{
	IVideoDriver* driver = App::getInstance()->getDevice()->getVideoDriver();
	vector3df camref = App::getInstance()->getDevice()->getSceneManager()->getActiveCamera()->getPosition();

	// Render the size of the brush.
	f32 framesize = position.getDistanceFrom(camref)/400.0f;

	f32 height=0.0f;
	// Display the inner brush size if the user change the default value of 5 to another value
	std::vector<vector3df>buffer;
	std::vector<int>deg;

	vector3df pos=position;
	for (int i=0; i<(360+step); i=i+step)
	{
		float degInRad = i*DEG2RAD;
		vector3df pos=position;

		pos.X+=cos(degInRad)*radius;
		pos.Z+=sin(degInRad)*radius;

		if (useray)
			height=getHeightAt(pos);
		else
			height=getVerticeHeight(pos);

		if (height==-1000.0f)
			height=0.0f;

		pos.Y=height+framesize;

		buffer.push_back(pos); //put the positions in the buffer
		deg.push_back(i);
	}
	
	int step1=5;

	f32 radius1=radius; ///4;
	for (int i=0; i<(int)buffer.size()-1; i++)
	{

		for(int j=i; j<(i+1); j++)
		{
			//buffer[j]
			//buffer[j+1]
			int divide=15;
			for(float inter=0.0f; inter<1.0f; inter=inter+(1.0f/divide))
			{
				vector3df vertice1=buffer[j].getInterpolated(buffer[j+1],inter);
				vector2df rot=vector2df(0,deg[j]*DEG2RAD);
				vector2df rot1=vector2df(0,deg[j+1]*DEG2RAD);
				f32 finrot=(rot.getInterpolated(rot1,inter)).Y;
				vertice1.X=position.X+cos(finrot)*radius1;
				vertice1.Z=position.Z+sin(finrot)*radius1;

				vector3df vertice2=buffer[j].getInterpolated(buffer[j+1],inter+(1.0f/divide));
				finrot=(rot.getInterpolated(rot1,inter+(1.0f/divide))).Y;
				vertice2.X=position.X+cos(finrot)*radius1;
				vertice2.Z=position.Z+sin(finrot)*radius1;

				vector3df vertice3=buffer[j].getInterpolated(buffer[j+1],inter);
				finrot=(rot.getInterpolated(rot1,inter)).Y;
				vertice3.X=position.X+cos(finrot)*(radius1+framesize);
				vertice3.Z=position.Z+sin(finrot)*(radius1+framesize);

				vector3df vertice4=buffer[j].getInterpolated(buffer[j+1],inter+(1.0f/divide));
				finrot=(rot.getInterpolated(rot1,inter+(1.0f/divide))).Y;
				vertice4.X=position.X+cos(finrot)*(radius1+framesize);
				vertice4.Z=position.Z+sin(finrot)*(radius1+framesize);

				if (drawlines)
					driver->draw3DLine(vertice1,vertice2,video::SColor(255,64,255,0));
				else
				{
					if (useray)
					{//Yellow
						driver->draw3DTriangle(triangle3df(vertice1,vertice3,vertice2),video::SColor(128,255,255,200));
						driver->draw3DTriangle(triangle3df(vertice2,vertice3,vertice4),video::SColor(128,255,255,255));
					}
					else
					{ //Red 
						driver->draw3DTriangle(triangle3df(vertice1,vertice3,vertice2),video::SColor(128,255,64,64));
						driver->draw3DTriangle(triangle3df(vertice2,vertice3,vertice4),video::SColor(128,255,64,64));
					}

				}
			}
		}
	}
}

void TerrainManager::update()
{
#ifdef EDITOR
	App::APP_STATE app_state;
	app_state = App::getInstance()->getAppState();

	if (app_state == App::APP_EDIT_TERRAIN_TRANSFORM)
		drawBrush(!needrecalc);


	// Refresh the edition of terrain at 30FPS (Should be uniform now on all system)
	if(App::getInstance()->cursorIsInEditArea() )
	{
		u32 time = App::getInstance()->getDevice()->getTimer()->getRealTime();
		if (app_state == App::APP_EDIT_TERRAIN_TRANSFORM)
		{
			if(!EventReceiver::getInstance()->isMousePressed(0) && !EventReceiver::getInstance()->isMousePressed(1) && needrecalc)
				recalculate();

			//Calculate a time offset for the strenght
			f32 timeoffset = f32((time-timer)/1000.0f);
			f32 strength = GUIManager::getInstance()->getScrollBarValue(GUIManager::SC_ID_TERRAIN_BRUSH_STRENGTH)*(timeoffset/60.0f);

			//printf("Here is the value of the gui: %f, value of time offset: %f, offset: %d\n",strength,timeoffset,s32(time-timer));
			timer = time;

			if(EventReceiver::getInstance()->isKeyPressed(KEY_LCONTROL))
			{
				// Activate the "plateau" display in the shader
				ShaderCallBack::getInstance()->setFlagEditingTerrain(true);
				if(EventReceiver::getInstance()->isMousePressed(0))
				{
					needrecalc=true;
					transformSegmentsToValue(App::getInstance()->getMousePosition3D(100),
						GUIManager::getInstance()->getScrollBarValue(GUIManager::SC_ID_TERRAIN_BRUSH_RADIUS),
						GUIManager::getInstance()->getScrollBarValue(GUIManager::SC_ID_TERRAIN_BRUSH_RADIUS2),
						strength,
						GUIManager::getInstance()->getScrollBarValue(GUIManager::SC_ID_TERRAIN_BRUSH_PLATEAU), true);
				}
			}
			else
			{
				// De-Activate the "plateau" display in the shader
				ShaderCallBack::getInstance()->setFlagEditingTerrain(false);
				if(EventReceiver::getInstance()->isMousePressed(0))
				{
					needrecalc=true;
					transformSegments(App::getInstance()->getMousePosition3D(100),
						GUIManager::getInstance()->getScrollBarValue(GUIManager::SC_ID_TERRAIN_BRUSH_RADIUS),
						GUIManager::getInstance()->getScrollBarValue(GUIManager::SC_ID_TERRAIN_BRUSH_RADIUS2),
						strength, true);
				}
				else if(EventReceiver::getInstance()->isMousePressed(1) )
				{
					needrecalc=true;
					transformSegments(App::getInstance()->getMousePosition3D(100),
						GUIManager::getInstance()->getScrollBarValue(GUIManager::SC_ID_TERRAIN_BRUSH_RADIUS),
						GUIManager::getInstance()->getScrollBarValue(GUIManager::SC_ID_TERRAIN_BRUSH_RADIUS2),
						-strength, true);
				}
			}
		}


		if(app_state == App::APP_EDIT_TERRAIN_PAINT_VEGETATION)
		{
			//Add vegetation to the terrain
			if(EventReceiver::getInstance()->isMousePressed(0))
			{
				TerrainManager::getInstance()->paintVegetation(App::getInstance()->getMousePosition3D(100), false);
			}
			//Erase vegetation from the terrain
			if(EventReceiver::getInstance()->isMousePressed(1))
			{
				TerrainManager::getInstance()->paintVegetation(App::getInstance()->getMousePosition3D(100), true);
			}
		}
	}
#endif
}

// Rotate a custom tile left
void TerrainManager::rotateLeft(core::vector3df pos)
{
	if (pos.Y==-1000)
		return;

    //Must be rounded positions (to keep it in the grid)
    pos.X = (f32)round32(pos.X);
    pos.Y = (f32)round32(pos.Y);
    pos.Z = (f32)round32(pos.Z);

	 if(getSegment(pos))
    {
		tileTagged = terrainMap.find(getHashCode(pos))->second;
		if (tileTagged->custom)
		{
			core::vector3df rotbase = tileTagged->getNode()->getRotation();
			rotbase-=vector3df(0.0f,90.0f,0.0f);

			if (rotbase.Y<0)
				rotbase.Y=270;

			tileTagged->getNode()->setRotation(rotbase);

			tileTagged=NULL;
		}
	 }
}

// Rotate a custom tile right
void TerrainManager::rotateRight(vector3df pos)
{
	if (pos.Y==-1000)
		return;

    //Must be rounded positions (to keep it in the grid)
    pos.X = (f32)round32(pos.X);
    pos.Y = (f32)round32(pos.Y);
    pos.Z = (f32)round32(pos.Z);

	 if(getSegment(pos))
    {
		tileTagged = terrainMap.find(getHashCode(pos))->second;
		if (tileTagged->custom)
		{
			core::vector3df rotbase = tileTagged->getNode()->getRotation();
			rotbase+=vector3df(0.0f,90.0f,0.0f);

			if (rotbase.Y>360)
				rotbase.Y=0;

			tileTagged->getNode()->setRotation(rotbase);

			tileTagged=NULL;
		}
	 }
}
