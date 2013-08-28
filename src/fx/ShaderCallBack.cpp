#include "ShaderCallBack.h"
#include "../terrain/TerrainManager.h"
#include "../camera/CameraSystem.h"
#include "../gui/GUIManager.h"
#include "../App.h"

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
	layer = 0;
}

ShaderCallBack* ShaderCallBack::getInstance()
{
    static ShaderCallBack* instance = 0;
    if (!instance) instance = new ShaderCallBack();
    return instance;
}

void ShaderCallBack::setFlagEditingTerrain(bool edit)
{
    editingTerrain = (int)edit;
}

void ShaderCallBack::OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
{
	layer=0;
	services->setPixelShaderConstant("terrainLayer0",(int*)&layer,1);
	layer=1;
	services->setPixelShaderConstant("terrainLayer1",(int*)&layer,1);
	layer=2;
	services->setPixelShaderConstant("terrainLayer2",(int*)&layer,1);
	layer=3;
	services->setPixelShaderConstant("terrainLayer3",(int*)&layer,1);
	layer=4;
	services->setPixelShaderConstant("terrainLayer4",(int*)&layer,1);
    services->setPixelShaderConstant("editingTerrain",(int*)&editingTerrain,1);

#ifdef EDITOR
	float plateau = (float)GUIManager::getInstance()->getScrollBarValue(SC_ID_TERRAIN_BRUSH_PLATEAU);
	services->setPixelShaderConstant("plateau",(float*)&plateau,1);
#endif

    layer=10;
    services->setPixelShaderConstant("terrainTextureScale",(int*)&layer,1);
	// Retrieve the scale of the terrain
	
	layer=(int)TerrainManager::getInstance()->getTileMeshSize();
	
	services->setPixelShaderConstant("terrainScale",(int*)&layer,1);
	
    //layer=100;
    //services->setPixelShaderConstant("fogDistance",(float*)&layer,1);

    layer=0;
    services->setPixelShaderConstant("oceanNormalMap",(int*)&layer,1);
    layer=1;
    services->setPixelShaderConstant("oceanReflection",(int*)&layer,1);

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
	//core::vector3df pos = vector3df(0,0,0);
	//scene::ICameraSceneNode * cam = device->getSceneManager()->getActiveCamera();
	//if (cam)
	//{
		// A light that is attached to the camera is used to light the terrain.
		//vector3df pos = CameraSystem::getInstance()->light->getAbsolutePosition();

		//services->setPixelShaderConstant("mLightPos", reinterpret_cast<f32*>(&pos), 3);
		//pos = cam->getPosition();
		//services->setPixelShaderConstant("mCamPos", reinterpret_cast<float*>(&pos),3);
	//}
	
	//device->getSceneManager()->getActiveCamera()->getPosition();

   
/*

    // set camera position
    pos = device->getSceneManager()->getActiveCamera()->getPosition();
    services->setVertexShaderConstant("camPos", reinterpret_cast<f32*>(&pos), 3);

    

   
	// set transposed world matrix

    core::matrix4 world = device->getVideoDriver()->getTransform(video::ETS_WORLD);
    world = world.getTransposed();

    services->setVertexShaderConstant("mTransWorld", world.pointer(), 16);
	 */


    f32 time=device->getTimer()->getTime()/10000.0f;
    services->setVertexShaderConstant("waterTime",&time,1);
}

void ShaderCallBack::setMaterials(ISceneNode * node, vector<DynamicObject_material> mat)
{
	scene::ISceneManager * smgr=App::getInstance()->getDevice()->getSceneManager();
	video::IVideoDriver * driver=App::getInstance()->getDevice()->getVideoDriver();

	printf ("Here is the current material count for this model: %d\n",mat.size());

	for (u32 i=0; i<mat.size(); ++i)
	{
		printf ("Here is the defined material type: %s\n",mat[i].shader.c_str());

		if (mat[i].shader == "SOLID")
		{
			printf("Material is a solid..\n");
			ITexture * tex0 = NULL;
		
			if (mat[i].texture0.size()>0)
				tex0 = driver->getTexture(core::stringc("../media/dynamic_objects/").append(mat[i].texture0));
			
			if (tex0)
			{
				node->getMaterial(i).MaterialType=EMT_SOLID;
				node->getMaterial(i).setTexture(0,tex0);
			}
		
		} 
		// Assign a custom lightmap to the model
		if (mat[i].shader == "LIGHTMAP")
		{
			printf("Material is a lightmap..\n");
			ITexture * tex0 = NULL;
			ITexture * tex1 = NULL;

			if (mat[i].texture0.size()>0)
				tex0 = driver->getTexture(core::stringc("../media/dynamic_objects/").append(mat[i].texture0));
			
			if (mat[i].texture1.size()>0)
				tex1 = driver->getTexture(core::stringc("../media/dynamic_objects/").append(mat[i].texture1));
		
			
			if (tex1)
			{
				node->getMaterial(i).MaterialType=EMT_LIGHTMAP;
				node->getMaterial(i).setTexture(1,tex1);
				if (tex0)
					node->getMaterial(i).setTexture(0,tex0);
			}
		
		} 
		
		if (mat[i].shader == "LIGHTMAP_LIGHTING")
		{
			printf("Material is a lightmap..\n");
			ITexture * tex0 = NULL;
			ITexture * tex1 = NULL;

			if (mat[i].texture0.size()>0)
				tex0 = driver->getTexture(core::stringc("../media/dynamic_objects/").append(mat[i].texture0));
			
			if (mat[i].texture1.size()>0)
				tex1 = driver->getTexture(core::stringc("../media/dynamic_objects/").append(mat[i].texture1));
				
			if (tex1)
			{
				node->getMaterial(i).MaterialType=EMT_LIGHTMAP_LIGHTING;
				node->getMaterial(i).setTexture(1,tex1);
				if (tex0)
					node->getMaterial(i).setTexture(0,tex0);
			}

		} 
		
		if (mat[i].shader == "NORMAL_MAP")
		{
			printf("Material is a normal map..\n");
			ITexture * tex = driver->getTexture(core::stringc("../media/dynamic_objects/").append(mat[i].texture1));
			if (tex)
			{
				node->getMaterial(i).MaterialType=EMT_NORMAL_MAP_SOLID;
				node->getMaterial(i).setTexture(1,tex);
			}

		}
		
		if (mat[i].shader == "TERRAIN")
		{
			printf("Material is the terrain shader\n");
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
			
			static s32 materialTerrain2 = 0;
			// Disable 8 textures for the moment, will be enabled by the user directly.
			bool heighttextures = false;
			//Create a Custom GLSL Material (Terrain Splatting)
			if (heighttextures)
			{
				//Hardware support for 8 textures
				materialTerrain2=smgr->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
				"../media/shaders/splat.vert", "vertexMain", video::EVST_VS_1_1,
				"../media/shaders/splat8.frag", "pixelMain", video::EPST_PS_1_4,
				this, video::EMT_SOLID);

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
				materialTerrain2=smgr->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
				"../media/shaders/splat.vert", "vertexMain", video::EVST_VS_1_1,
				"../media/shaders/splat4.frag", "pixelMain", video::EPST_PS_1_4,
				this, video::EMT_SOLID);

				//Assign Textures
#ifdef WIN32	// Strange 
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

			//REDO the UV map so it's planar and the size appropriate
			IMesh * mesh = ((IMeshSceneNode *)node)->getMesh();
			smgr->getMeshManipulator()->makePlanarTextureMapping(mesh, 0.0009f, 0.0009f, 1, vector3df(0,0,0));
			smgr->getMeshManipulator()->recalculateNormals(mesh);

			//Assign GLSL Shader
			node->getMaterial(i).setFlag(EMF_LIGHTING,false);
			node->getMaterial(i).setFlag(EMF_FOG_ENABLE,true);

			node->getMaterial(i).MaterialType=(E_MATERIAL_TYPE)materialTerrain2;

		}
		if (mat[i].shader == "WATER")
		{
			 //Create a Custom GLSL Material (Water shader)
			static s32 materialOcean1=smgr->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
				"../media/shaders/ocean1.vert", "vertexMain", video::EVST_VS_1_1,
				"../media/shaders/ocean1.frag", "pixelMain", video::EPST_PS_1_4,
				this, video::EMT_TRANSPARENT_ALPHA_CHANNEL);

			static ITexture* oceanLayer0 = smgr->getVideoDriver()->getTexture("../media/waveNM.png");
			static ITexture* oceanLayer1 = smgr->getVideoDriver()->getTexture("../media/sky.jpg");
			// Water shader

			node->getMaterial(i).MaterialType=(E_MATERIAL_TYPE)materialOcean1;
	
			node->getMaterial(i).setTexture(0,oceanLayer0);
			node->getMaterial(i).setTexture(1,oceanLayer1);

			node->setMaterialFlag(EMF_FOG_ENABLE,true);    
			node->setMaterialFlag(EMF_BLEND_OPERATION,true);
		}

	
	}
	
}