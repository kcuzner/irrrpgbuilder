#include "Vegetation.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;



Vegetation::Vegetation(int type)
{
	ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();

	//Here will need to create a new list of the enabled that will content the index of the "true", and the random will pick one of the number in the list.
	enabled=VegetationSeed::getInstance()->getEnabled();
	vector<int> newlist;
	for(int i = 0; i<(int)enabled.size(); i++)
	{
		if (enabled[i]==true)
			newlist.push_back(i); //New list will contain the list of the enabled items
	}
	int randomveg=1;

	//In case of the user that would disable all the tree and attemp to paint!
	//Should be prevented before attemting to create a vegetation object.
	//This is only a failsafe
	if (newlist.size()>0)
	{	
		randomveg = rand()%newlist.size();
		if (type == -1) type=newlist[randomveg];
	} else
		type = randomveg;
	
	

	//Old method. All the vegetation was enabled.
    //if(type == -1) type=rand()%VegetationSeed::getInstance()->getTotalOfTypes();//randomize tree type when receive -1

	// will store the type of the tree
	vegeType=type;

    stringc trunkMeshFile = "../media/vegetation/";
    trunkMeshFile.append(VegetationSeed::getInstance()->getTrunkMesh(type));
    stringc leafsMeshFile = "../media/vegetation/";
    leafsMeshFile.append(VegetationSeed::getInstance()->getLeafsMesh(type));

    IMesh* trunkMesh = smgr->getMesh(trunkMeshFile);
    IMesh* leafsMesh = smgr->getMesh(leafsMeshFile);

    if(trunkMesh)
    {
        trunkMesh->setHardwareMappingHint(EHM_STATIC);
		// load the mesh as an occtree, should be a faster to draw
		trunk = smgr->addOctreeSceneNode(trunkMesh,0,-1);
		trunk->setMaterialFlag(EMF_LIGHTING,true);
        //trunk = smgr->addMeshSceneNode(trunkMesh);
    }
    else
    {
        trunk = smgr->addEmptySceneNode();
    }

	trunk->setAutomaticCulling(EAC_FRUSTUM_BOX);
	trunk->setMaterialFlag(EMF_LIGHTING,true);
	trunk->setMaterialFlag(EMF_FOG_ENABLE,true);

	//two options to set vegetation scale: load from XML or export in the right size
	//trunk->setScale(vector3df(25,25,25));//PS: remove this line after decide this...

	leafsMesh->setHardwareMappingHint(EHM_STATIC);
    //leafs = smgr->addMeshSceneNode(leafsMesh,trunk);
	// load the mesh as an occtree, should be a faster to draw
	leafs = smgr->addOctreeSceneNode(leafsMesh,trunk,-1);
	leafs->setAutomaticCulling(EAC_FRUSTUM_BOX);
	leafs->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
	leafs->setMaterialFlag(EMF_BLEND_OPERATION,true);
	leafs->setMaterialFlag(EMF_FOG_ENABLE,true);
	leafs->setMaterialFlag(EMF_LIGHTING,true);

    //Fake Shadow
    fakeShadow = smgr->addMeshSceneNode(smgr->getMesh("../media/vegetation/shadow.obj"),trunk);
    fakeShadow->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
	fakeShadow->setMaterialFlag(EMF_BLEND_OPERATION,true);
    fakeShadow->setPosition(vector3df(0,(f32)0.03 + (rand()%10)*(f32)0.01 ,0));
	fakeShadow->setAutomaticCulling(EAC_FRUSTUM_BOX);
    fakeShadow->setMaterialFlag(EMF_FOG_ENABLE,true);

	// Temporary down, evaluating the need to display debug data for this
    //trunk->setDebugDataVisible(true);
}

Vegetation::~Vegetation()
{
	if (fakeShadow)
	{
		fakeShadow->remove();
		fakeShadow=NULL;
	}

    if (trunk)
	{
		trunk->remove();
		trunk=NULL;
	}
}


int Vegetation::getType()
{
	return vegeType;
}

vector3df Vegetation::getPosition()
{
    return trunk->getPosition();
}

void Vegetation::setPosition(vector3df pos)
{
    trunk->setPosition(pos);
}

void Vegetation::setRotation(vector3df rot)
{
    trunk->setRotation(rot);
}

void Vegetation::setScale(vector3df scale)
{
    trunk->setScale(scale);
}

void Vegetation::showDebugData(bool show)
{
    if(show)
        trunk->setDebugDataVisible(EDS_BBOX);
    else
        trunk->setDebugDataVisible(EDS_OFF);
}
