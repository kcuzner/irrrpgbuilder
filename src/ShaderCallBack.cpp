#include "ShaderCallBack.h"
#include "TerrainManager.h"
#include "App.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


ShaderCallBack::ShaderCallBack()
{
    this->device=App::getInstance()->getDevice();
    editingTerrain = 0;
}

ShaderCallBack* ShaderCallBack::getInstance()
{
    static ShaderCallBack* instance = 0;
    if (!instance) instance = new ShaderCallBack();
    return instance;
}

void ShaderCallBack::setFlagEditingTerrain(bool edit)
{
    editingTerrain = edit;
}

void ShaderCallBack::OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
{
    layer=0;
    services->setPixelShaderConstant("terrainLayer0",(float*)&layer,1);
    layer=1;
    services->setPixelShaderConstant("terrainLayer1",(float*)&layer,1);
    layer=2;
    services->setPixelShaderConstant("terrainLayer2",(float*)&layer,1);
    layer=3;
    services->setPixelShaderConstant("terrainLayer3",(float*)&layer,1);
    layer=4;
    services->setPixelShaderConstant("terrainLayer4",(float*)&layer,1);

    services->setPixelShaderConstant("editingTerrain",(float*)&editingTerrain,1);

    layer=10;
    services->setPixelShaderConstant("terrainTextureScale",(float*)&layer,1);
	// Retrieve the scale of the terrain
	
	layer=(int)TerrainManager::getInstance()->getTileMeshSize();
	
	services->setPixelShaderConstant("terrainScale",(float*)&layer,1);
		
	//services->setPixelShaderConstant("terrainScale",(float*)&layer,1);

    //layer=100;
    //services->setPixelShaderConstant("fogDistance",(float*)&layer,1);

    layer=0;
    services->setPixelShaderConstant("oceanNormalMap",(float*)&layer,1);
    layer=1;
    services->setPixelShaderConstant("oceanReflection",(float*)&layer,1);

    SColorf color = App::getInstance()->getDevice()->getSceneManager()->getAmbientLight();
    services->setVertexShaderConstant("AmbientLight",reinterpret_cast<f32*>(&color), 4);

//services->setPixelShaderConstant("ambientGlobal",(float*)&SColorf(1,1,1,1),4);


/*    core::matrix4 invWorld = device->getVideoDriver()->getTransform(video::ETS_WORLD);
    invWorld.makeInverse();

    services->setVertexShaderConstant("mInvWorld", invWorld.pointer(), 16);
*/
    // set clip matrix
    core::matrix4 worldViewProj;
    worldViewProj = device->getVideoDriver()->getTransform(video::ETS_PROJECTION);
    worldViewProj *= device->getVideoDriver()->getTransform(video::ETS_VIEW);
    worldViewProj *= device->getVideoDriver()->getTransform(video::ETS_WORLD);

    services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);
    services->setVertexShaderConstant("mWorldViewProj2", worldViewProj.pointer(), 16);

/*
    layer=4;
    services->setPixelShaderConstant("ShadowMapSampler",(float*)&layer,1);

    layer=1000;
    services->setPixelShaderConstant("MaxD",(float*)&layer,1);

    layer=10;
    services->setPixelShaderConstant("MAPRES",(float*)&layer,1);
*/

    core::vector3df pos = vector3df(0,250,0);

    services->setVertexShaderConstant("mLightPos", reinterpret_cast<f32*>(&pos), 3);
/*

    // set camera position
    pos = device->getSceneManager()->getActiveCamera()->getPosition();
    services->setVertexShaderConstant("camPos", reinterpret_cast<f32*>(&pos), 3);

    // set light color

    video::SColorf col(2.0f,2.0f,2.0f,2.0f);

    services->setVertexShaderConstant("LightColor",reinterpret_cast<f32*>(&col), 4);

    // set transposed world matrix

    core::matrix4 world = device->getVideoDriver()->getTransform(video::ETS_WORLD);
    world = world.getTransposed();

    services->setVertexShaderConstant("mTransWorld", world.pointer(), 16);
*/

    f32 time=device->getTimer()->getTime()/10000.0f;
    services->setVertexShaderConstant("waterTime",&time,1);
}