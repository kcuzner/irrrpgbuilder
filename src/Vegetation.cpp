#include "Vegetation.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

Vegetation::Vegetation()
{
    ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();

    static IMesh* trunkMesh = smgr->getMesh("../media/vegetation/TreeHigh.obj");
	trunkMesh->setHardwareMappingHint(EHM_STATIC);
    trunk = smgr->addMeshSceneNode(trunkMesh);

	
    //trunk->setMaterialFlag(EMF_LIGHTING,false);

    IBillboardSceneNode* temp;
	
    s32 texture = rand()%4;
    ITexture* itexture = NULL;

    size = 0.8f;

    switch(texture)
    {
        case 2:
            itexture = App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/vegetation/vegetation2.png");
            break;
        case 1:
            itexture = App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/vegetation/vegetation1.png");
            break;
        default:
            itexture = App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/vegetation/vegetation0.png");
    }

    f32 radius = 0.5;

    //create 3 billboards..
    for (int i=0 ; i<3 ; i++)
    {
        vector3df pos;

        pos.Z = cos( ((360/3)*i)*PI/180 ) * radius;
        pos.X = sin( ((360/3)*i)*PI/180 ) * radius;
        pos.Y = 1.5;

        temp = smgr->addBillboardSceneNode(trunk,
                                           dimension2df(size,size),
                                           pos );

        temp->setMaterialTexture(0,itexture);
        temp->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
        //temp->setMaterialFlag(EMF_LIGHTING,false);
        temp->setMaterialFlag(EMF_FOG_ENABLE,true);

        leafs.push_back(temp);
    }

    temp = smgr->addBillboardSceneNode(trunk,dimension2df(size*(f32)0.7,size*(f32)0.7),vector3df(0,(f32)2,0));
    temp->setMaterialTexture(0,itexture);
    temp->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
    //temp->setMaterialFlag(EMF_LIGHTING,false);
    temp->setMaterialFlag(EMF_FOG_ENABLE,true);

    leafs.push_back(temp);


    //Fake Shadow
    fakeShadow = smgr->addMeshSceneNode(smgr->getMesh("../media/vegetation/shadow.obj"),trunk);
    fakeShadow->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
    fakeShadow->setPosition(vector3df(0,(f32)0.03 + (rand()%10)*(f32)0.01 ,0));


    trunk->setMaterialFlag(EMF_FOG_ENABLE,true);
    fakeShadow->setMaterialFlag(EMF_FOG_ENABLE,true);

    trunk->setDebugDataVisible(true);
}

Vegetation::~Vegetation()
{
    for (int i=0 ; i<(int)leafs.size() ; i++)
    {
        ((IBillboardSceneNode*)leafs[i])->remove();
    }

    leafs.clear();

    fakeShadow->remove();

    trunk->remove();
}

vector3df Vegetation::getPosition()
{
    return trunk->getPosition();
}

void Vegetation::setPosition(vector3df pos)
{
    trunk->setPosition(pos);
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
