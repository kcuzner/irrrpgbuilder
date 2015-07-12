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

	f32 time = device->getTimer()->getTime() / 10000.0f;
	time = time / 3.0f;
	services->setVertexShaderConstant(services->getVertexShaderConstantID("time"), (irr::f32*)&time, 1);
	services->setPixelShaderConstant(services->getPixelShaderConstantID("waterTime"), &time, 1);
	// Defines for the terrain shader constants
	layer=0;
	services->setPixelShaderConstant(services->getPixelShaderConstantID("terrainLayer0"),(int*)&layer,1);
	layer=1;
	services->setPixelShaderConstant(services->getPixelShaderConstantID("terrainLayer1"),(int*)&layer,1);
	layer=2;
	services->setPixelShaderConstant(services->getPixelShaderConstantID("terrainLayer2"),(int*)&layer,1);
	layer=3;
	services->setPixelShaderConstant(services->getPixelShaderConstantID("terrainLayer3"),(int*)&layer,1);
	layer=4;
	services->setPixelShaderConstant(services->getPixelShaderConstantID("terrainLayer4"),(int*)&layer,1);
    services->setPixelShaderConstant(services->getPixelShaderConstantID("editingTerrain"),(int*)&editingTerrain,1);

#ifdef EDITOR
	float plateau = (float)GUIManager::getInstance()->getScrollBarValue(GUIManager::SC_ID_TERRAIN_BRUSH_PLATEAU);
	services->setPixelShaderConstant(services->getPixelShaderConstantID("plateau"),(irr::f32*)&plateau,1);
#endif

    layer=10;
	float terrainscale = TerrainManager::getInstance()->getScale()/1024;
	int ter=(int)(10.0f*terrainscale);
    services->setPixelShaderConstant(services->getPixelShaderConstantID("terrainTextureScale"),(int*)&ter,1);

	// Retrieve the scale of the terrain
	layer=(int)TerrainManager::getInstance()->getTileMeshSize();

	services->setPixelShaderConstant(services->getPixelShaderConstantID("terrainScale"),(int*)&layer,1);
	services->setVertexShaderConstant(services->getVertexShaderConstantID("terrainScale"), (int*)&layer, 1);

  
	/*
    // set clip matrix
    core::matrix4 worldViewProj;
    worldViewProj = device->getVideoDriver()->getTransform(video::ETS_PROJECTION);
    worldViewProj *= device->getVideoDriver()->getTransform(video::ETS_VIEW);
    worldViewProj *= device->getVideoDriver()->getTransform(video::ETS_WORLD);

    services->setVertexShaderConstant(services->getVertexShaderConstantID("mWorldViewProj"), worldViewProj.pointer(), 16);
    services->setVertexShaderConstant(services->getVertexShaderConstantID("mWorldViewProj2"), worldViewProj.pointer(), 16);
	*/
  	
	// Defines for the IRB normal map constants
	core::matrix4 world = device->getVideoDriver()->getTransform(video::ETS_WORLD);
	services->setVertexShaderConstant("mWorld", world.pointer(), 16);

	vector3df TheCameraPosition = device->getSceneManager()->getActiveCamera()->getPosition();

	int TexAddress = 0;  int TexAddress2 = 1;  int TexAddress3 = 2;  int TexAddress4 = 3;
	services->setPixelShaderConstant("DiffuseMap", (int*)(&TexAddress), 1);   // 
	services->setPixelShaderConstant("NormalMap", (int*)(&TexAddress2), 1);  // 
	services->setPixelShaderConstant("SpecularMap", (int*)(&TexAddress3), 1);  // 
	services->setPixelShaderConstant("GlossMap", (int*)(&TexAddress4), 1);  // 

	services->setPixelShaderConstant("CamPosTEST", &TheCameraPosition.X, 4);
	services->setPixelShaderConstant("mWorld", world.pointer(), 16);



	// Defines for the water shader constants
	layer = 0;
	services->setPixelShaderConstant(services->getPixelShaderConstantID("oceanNormalMap"), (int*)&layer, 1);
	layer = 1;
	services->setPixelShaderConstant(services->getPixelShaderConstantID("oceanReflection"), (int*)&layer, 1);

	SColorf color = App::getInstance()->getDevice()->getSceneManager()->getAmbientLight();
	services->setPixelShaderConstant(services->getPixelShaderConstantID("AmbientLight"), reinterpret_cast<f32*>(&color), 4);

}

void ShaderCallBack::setMaterials(ISceneNode * node, vector<DynamicObject::DynamicObject_material> mat)
{
	scene::ISceneManager * smgr=App::getInstance()->getDevice()->getSceneManager();
	video::IVideoDriver * driver=App::getInstance()->getDevice()->getVideoDriver();

#ifdef DEBUG
	printf ("Here is the current material count for this model: %d\n",mat.size());
#endif

	for (u32 i=0; i<mat.size(); ++i)
	{

#ifdef DEBUG
		printf ("Here is the defined material type: %s\n",mat[i].shader.c_str());
#endif
		//New normal map shader created by VECTROTEK(Jacques Pretorius).From the Irrlicht forum
		if (mat[i].shader == "IRB_NORMAL_MAP")
		{
#ifdef DEBUG
			printf("Material is a the new normal map..\n");
			printf("Name of mesh is: %s\n", node->getName());
			

#endif
			ITexture * tex0 = NULL;
			ITexture * tex1 = NULL;
			ITexture * tex2 = NULL;
			ITexture * tex3 = NULL;

			if (mat[i].texture0.size()>0) //Diffuse slot + Alpha: Clip map 
				tex0 = driver->getTexture(core::stringc("../media/dynamic_objects/").append(mat[i].texture0));
			if (mat[i].texture1.size()>0) //Normal map + Alpha: Droplet map
				tex1 = driver->getTexture(core::stringc("../media/dynamic_objects/").append(mat[i].texture1));
			if (mat[i].texture2.size()>0) //Specular map
				tex2 = driver->getTexture(core::stringc("../media/dynamic_objects/").append(mat[i].texture2));
			if (mat[i].texture3.size()>0) //Gloss map
				tex3 = driver->getTexture(core::stringc("../media/dynamic_objects/").append(mat[i].texture3));

			static s32 irbnormalmap = 0;
			irbnormalmap = smgr->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
				"../media/shaders/normals.vert", "vertexMain", video::EVST_VS_2_0,
				"../media/shaders/normals.frag", "pixelMain", video::EPST_PS_2_0,
				this, EMT_TRANSPARENT_ALPHA_CHANNEL_REF,
				0, // ??
				EGSL_DEFAULT);


			if (tex0) //Need at least the diffuse slot to be applied
			{
				//Assign GLSL Shader
				node->getMaterial(i).setFlag(EMF_LIGHTING, false);
				node->getMaterial(i).setFlag(EMF_FOG_ENABLE, true);
				node->getMaterial(i).setTexture(0, tex0);
				node->getMaterial(i).setTexture(1, tex1);
				node->getMaterial(i).setTexture(2, tex2);
				node->getMaterial(i).setTexture(3, tex3);
				node->getMaterial(i).MaterialType = (E_MATERIAL_TYPE)irbnormalmap;
			}


		}
		if (mat[i].shader == "SOLID")
		{

#ifdef DEBUG
			printf("Material is a solid..\n");
#endif
			ITexture * tex0 = NULL;

			if (mat[i].texture0.size()>0) //Diffuse slot
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

#ifdef DEBUG
			printf("Material is a lightmap..\n");
#endif
			ITexture * tex0 = NULL;
			ITexture * tex1 = NULL;

			if (mat[i].texture0.size()>0) //Diffuse slot
				tex0 = driver->getTexture(core::stringc("../media/dynamic_objects/").append(mat[i].texture0));

			if (mat[i].texture1.size()>0) // Lightmap slot
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

#ifdef DEBUG
			printf("Material is a lightmap..\n");
#endif
			ITexture * tex0 = NULL;
			ITexture * tex1 = NULL;

			if (mat[i].texture0.size()>0) // Diffuse slot
				tex0 = driver->getTexture(core::stringc("../media/dynamic_objects/").append(mat[i].texture0));

			if (mat[i].texture1.size()>0) // Lightmap slot
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
			//This is the standard Irrlicht pipeline shader
			//Does not support animated models
			//Mesh does need to have it vertices tangents calculated by Irrlicht
			//Applied in dynamicObject.cpp -> setupObj() and the initialisation of the class (Tmesh)

			ITexture * tex0 = NULL;
			ITexture * tex1 = NULL;
#ifdef DEBUG
			printf("Material is a normal map..\n");
#endif
			if (mat[i].texture0.size()>0) // Diffuse slot
				tex0 = driver->getTexture(core::stringc("../media/dynamic_objects/").append(mat[i].texture0));

			if (mat[i].texture1.size()>0) // Normal map slot
				tex1 = driver->getTexture(core::stringc("../media/dynamic_objects/").append(mat[i].texture1));
			if (tex0)
			{
				node->getMaterial(i).MaterialType=EMT_NORMAL_MAP_SOLID;
				node->getMaterial(i).setTexture(0,tex0);
				node->getMaterial(i).setTexture(1,tex1);
			}

		}

		if (mat[i].shader == "TERRAIN")
		{

#ifdef DEBUG
			printf("Material is the terrain shader\n");
#endif
			//Get the texture names from the terrain manager
			//stringc texture0 = TerrainManager::getInstance()->getTerrainTexture(0);
			stringc texture1 = TerrainManager::getInstance()->getTerrainTexture(1);
			stringc texture2 = TerrainManager::getInstance()->getTerrainTexture(2);
			stringc texture3 = TerrainManager::getInstance()->getTerrainTexture(3);
			stringc texture4 = TerrainManager::getInstance()->getTerrainTexture(4);

			//static ITexture* layer0 = smgr->getVideoDriver()->getTexture(texture0.c_str());
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
				//node->setMaterialTexture(0,layer0);
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
				node->setMaterialTexture(0,layer1);
				node->setMaterialTexture(0,layer1);
                node->setMaterialTexture(1,layer2);
                node->setMaterialTexture(2,layer3);
                node->setMaterialTexture(3,layer4);
                node->setMaterialTexture(4,layer4);
#endif
			}

			//node->setMaterialTexture(0,layer1);

			//REDO the UV map so it's planar and the size appropriate
			IMesh * mesh = ((IMeshSceneNode *)node)->getMesh();
			f32 currentscale = 0.0009f;
			f32 ratio = TerrainManager::getInstance()->getScale()/1024;
			currentscale = currentscale / ratio;
			smgr->getMeshManipulator()->makePlanarTextureMapping(mesh, currentscale, currentscale, 1, vector3df(0,0,0));
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
			node->setMaterialFlag(EMF_BLEND_OPERATION,true); //This must be used for transparent material now.
		}


	}

}
