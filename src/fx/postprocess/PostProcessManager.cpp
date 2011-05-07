#include "PostProcessManager.h"
#include "IPostProcess.h"
#include "ShaderPostProcess.h"

CPostProcessManager::CPostProcessManager(IrrlichtDevice* device)
 : Device(device)
{	
	// store driver flags
	bool flagMipMaps = Device->getVideoDriver()->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);
	bool flag32Bit = Device->getVideoDriver()->getTextureCreationFlag(video::ETCF_ALWAYS_32_BIT);

	// set new flags for rtt creation
	Device->getVideoDriver()->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);
	Device->getVideoDriver()->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT);

	// load the rtt and effect configuration 
	loadRTTConfig();
	loadEffectConfig();

	// prepare the final pass process
	RenderToScreen = new CShaderPostProcess(Device, "Final Pass", "vertex.fx", "main", video::EVST_VS_1_1, "screenquad.fx", "downscale2x2", video::EPST_PS_2_0, video::EMT_SOLID);
	RenderToScreen->getShaderMaterial()->setPixelShaderFlag(ESC_BUFFERWIDTH);
	RenderToScreen->getShaderMaterial()->setPixelShaderFlag(ESC_BUFFERHEIGHT);

	// prepare the depth material for depth pass
	DepthMaterial = new CShaderMaterial(Device, "Depth", "depthvertex.fx", "main", video::EVST_VS_1_1, "depthmaterial.fx", "main", video::EPST_PS_2_0, video::EMT_SOLID);
	DepthMaterial->setVertexShaderConstant("DistanceScale", 0.0006f);
	DepthMaterial->setVertexShaderFlag(ESC_WORLD);
	DepthMaterial->setVertexShaderFlag(ESC_VIEW);
	DepthMaterial->setVertexShaderFlag(ESC_PROJECTION);
	
	// restore driver flags
	Device->getVideoDriver()->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, flagMipMaps);
	Device->getVideoDriver()->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, flag32Bit);
}

CPostProcessManager::~CPostProcessManager()
{
	// drop all PostProcess Objects
	for(u32 i=0; i<EPPE_COUNT; i++)
	{
		for(u32 j=0; j<EffectChain[i].size(); j++)
		{
			IPostProcess* postProcess = EffectChain[i][j];
			if (postProcess)
				postProcess->drop();
		}
	}

	// finally drop the RenderToScreen PostProcess
	if (RenderToScreen)
		RenderToScreen->drop();

	// and drop the depth material
	if (DepthMaterial)
		DepthMaterial->drop();
}

void CPostProcessManager::SwapAuxBuffers()
{
	// swap the in and out buffers
	video::ITexture* tmp = RenderTargetMap["auxIn"];
	RenderTargetMap["auxIn"] = RenderTargetMap["auxOut"];
	RenderTargetMap["auxOut"] = tmp;
}

void CPostProcessManager::prepare(bool useDepth, const video::SColor& defaultDepth)
{
	// create texture with depth and normal information
	if (useDepth)
		renderDepth(defaultDepth);

	// set auxOut RTT as RenderTarget
	Device->getVideoDriver()->setRenderTarget(RenderTargetMap["auxOut"]);
}

void CPostProcessManager::render(E_POSTPROCESS_EFFECT effect)
{
	if (effect<EPPE_COUNT)
	{		
		// first swap the in and out buffers
		SwapAuxBuffers();

		// run through the effect chain
		for (u32 i=0; i<EffectChain[effect].size(); i++)
		{
			// retrieve the post process
			IPostProcess* postProcess = EffectChain[effect][i];
			
			// bind input buffer 
			if( postProcess->getRenderSource()!="")
				postProcess->getMaterial().setTexture(0, RenderTargetMap[postProcess->getRenderSource()]);

			// bind output buffer
			if( postProcess->getRenderTarget()!="")
				Device->getVideoDriver()->setRenderTarget(RenderTargetMap[postProcess->getRenderTarget()]);

			// render the post process
			postProcess->render();
		}
	}
}
    
void CPostProcessManager::update()
{
	// render the scene into the framebuffer after postprocessing
	RenderToScreen->getMaterial().setTexture(0, RenderTargetMap["auxOut"]);
	Device->getVideoDriver()->setRenderTarget(video::ERT_FRAME_BUFFER, true, true);
	RenderToScreen->render();
}

void CPostProcessManager::clearDepthPass()
{
	// clear all nodes from the depth pass
	DepthPassNodes.clear();
}

void CPostProcessManager::addNodeToDepthPass(irr::scene::ISceneNode *node)
{
	// add node to the depth pass array
	if(DepthPassNodes.binary_search(node) == -1)
		DepthPassNodes.push_back(node);
}

void CPostProcessManager::renderDepth(const video::SColor& defaultDepth)
{	
	if(DepthPassNodes.size())
	{
		// set depth render target texture as render target 
		Device->getVideoDriver()->setRenderTarget(RenderTargetMap["rttDepth"], true, true, defaultDepth);

		// animate and render the camera to ensure correct depth and normal information
		scene::ICameraSceneNode* camera = Device->getSceneManager()->getActiveCamera();
		if (camera)
		{
			camera->OnRegisterSceneNode(); 
			camera->OnAnimate(Device->getTimer()->getTime()); 
			camera->render(); 
		}
			
		// render all nodes that are stored in the depth pass array
		for(u32 i=0; i<DepthPassNodes.size(); i++)
		{
			// get the scene node from the array
			scene::ISceneNode* node = DepthPassNodes[i];
			
			// save the scene node material
			video::E_MATERIAL_TYPE tempMaterial = node->getMaterial(0).MaterialType;
			
			// apply the depth material
			node->setMaterialType(DepthMaterial->getMaterialType());
			
			// render the node
			node->render();

			// reset the scene node to the original material
			node->setMaterialType(tempMaterial);
		}
	}
}

CEffectChain& CPostProcessManager::getEffectChain(E_POSTPROCESS_EFFECT effect)
{ 
	// return the desired effect chain
	if (effect < EPPE_COUNT) 
		return EffectChain[effect];
	else 
		return EffectChain[EPPE_NO_EFFECT];
}

void CPostProcessManager::loadRTTConfig()
{
	// create a xml reader
	io::IrrXMLReader* xmlReader = io::createIrrXMLReader("../media/shaders/postprocess/config/rtt.xml");

	// we'll be looking for the rendertarget tag in the xml
	const core::stringw renderTargetTag(L"RenderTarget");

    while(xmlReader && xmlReader->read())
    {    
		switch(xmlReader->getNodeType())
		{    
		case io::EXN_ELEMENT:
			{
				// we are in the setup section and we find a rendertarget to parse
				if (renderTargetTag.equals_ignore_case(xmlReader->getNodeName()))
				{
					// get the rtt parameters
					core::stringw id = xmlReader->getAttributeValueSafe("id");
					u32 width = (u32) xmlReader->getAttributeValueAsInt("width");
					u32 height = (u32) xmlReader->getAttributeValueAsInt("height");
					f32 scale = (f32) xmlReader->getAttributeValueAsFloat("scale");
					video::ECOLOR_FORMAT colorFormat = (video::ECOLOR_FORMAT) xmlReader->getAttributeValueAsInt("colorFormat");
					
					// set width and height of the rtt
					if (scale > 0.0f)
					{
						width =  (u32) (scale * Device->getVideoDriver()->getScreenSize().Width);
						height =  (u32) (scale * Device->getVideoDriver()->getScreenSize().Height);
					}
					if (width==0 || height==0)
					{
						width=Device->getVideoDriver()->getScreenSize().Width;
						height=Device->getVideoDriver()->getScreenSize().Height;
					}
					// add the rendertarget with its properties and store it in the render target map
					video::ITexture* texture = Device->getVideoDriver()->addRenderTargetTexture(core::dimension2d<u32>(width, height), id, colorFormat);	
					RenderTargetMap[id] = texture;
				}
			}
			break;
		}
	}
	delete xmlReader;    
}

void CPostProcessManager::loadEffectConfig()
{
	// create a xml reader
	io::IrrXMLReader* xmlReader = io::createIrrXMLReader("../media/shaders/postprocess/config/effect.xml");

	// we'll be looking for these tags in the xml file
	const core::stringw effectTag(L"Effect");
	const core::stringw shaderPostProcessTag(L"ShaderPostProcess");
	const core::stringw renderSourceTag(L"RenderSource");
	const core::stringw renderTargetTag(L"RenderTarget");
	const core::stringw psConstant(L"PixelShaderConstant");
	const core::stringw vsConstant(L"VertexShaderConstant");
	const core::stringw textureTag(L"Texture");

	// each effect chain is a sequence of postprocesses
	CEffectChain* currentEffectChain = NULL;
	CShaderPostProcess* currentPostProcess = NULL;

    while(xmlReader && xmlReader->read())
    {    
		switch(xmlReader->getNodeType())
		{    
		case io::EXN_ELEMENT:
			{
				// we are in the effect section and we find a effect to parse
				if (effectTag.equals_ignore_case(xmlReader->getNodeName()))
				{
					// get the E_POSTPROCESS_EFFECT parameter
					s32 id = xmlReader->getAttributeValueAsInt("id");
					core::stringw name = xmlReader->getAttributeValueSafe("name");
					
					if (id>=0 && id<EPPE_COUNT)
					{
						E_POSTPROCESS_EFFECT effect = (E_POSTPROCESS_EFFECT) xmlReader->getAttributeValueAsInt("id");
						currentEffectChain = &EffectChain[effect];
						currentEffectChain->setName(name);
					}
					currentPostProcess = NULL;
				}
				
				// we are in the shader post process section and have a valid currentEffect
				if (shaderPostProcessTag.equals_ignore_case(xmlReader->getNodeName()) && currentEffectChain)
				{
					// get the postprocess name
					core::stringw name = xmlReader->getAttributeValueSafe("name");
					
					// get vertex shader config
					core::stringw vsFile = xmlReader->getAttributeValueSafe("vsFile");
					core::stringw vsEntry = "main";
					if (xmlReader->getAttributeValue("vsEntry"))
						vsEntry = xmlReader->getAttributeValueSafe("vsEntry");	
					video::E_VERTEX_SHADER_TYPE vsType = (video::E_VERTEX_SHADER_TYPE) xmlReader->getAttributeValueAsInt("vsType");
					
					// get pixel shader config
					core::stringw psFile = xmlReader->getAttributeValueSafe("psFile");
					core::stringw psEntry = "main";
					if (xmlReader->getAttributeValue("psEntry"))
						psEntry = xmlReader->getAttributeValueSafe("psEntry");	
					video::E_PIXEL_SHADER_TYPE psType = (video::E_PIXEL_SHADER_TYPE) xmlReader->getAttributeValueAsInt("psType");
					video::E_MATERIAL_TYPE baseMaterial = (video::E_MATERIAL_TYPE) xmlReader->getAttributeValueAsInt("baseMaterial");
					
					// get additional built in shader constants for vertexshader
					bool vsUseElapsedTime = xmlReader->getAttributeValueAsInt("vsUseElapsedTime") != 0;
					bool vsUseRandom = xmlReader->getAttributeValueAsInt("vsUseRandom") != 0;
					bool vsUseBufferWidth = xmlReader->getAttributeValueAsInt("vsUseBufferWidth") != 0;
					bool vsUseBufferHeight = xmlReader->getAttributeValueAsInt("vsUseBufferHeight") != 0;
					bool vsUseProjection = xmlReader->getAttributeValueAsInt("vsUseProjection") != 0;
					bool vsUseView = xmlReader->getAttributeValueAsInt("vsUseView") != 0;
					bool vsUseWorld = xmlReader->getAttributeValueAsInt("vsUseWorld") != 0;
					bool vsUseWorldView = xmlReader->getAttributeValueAsInt("vsUseWorldView") != 0;
					bool vsUseWorldViewProj = xmlReader->getAttributeValueAsInt("vsUseWorldViewProj") != 0;
					
					// get additional built in shader constants for pixelshader
					bool psUseElapsedTime = xmlReader->getAttributeValueAsInt("psUseElapsedTime") != 0;
					bool psUseRandom = xmlReader->getAttributeValueAsInt("psUseRandom") != 0;
					bool psUseBufferWidth = xmlReader->getAttributeValueAsInt("psUseBufferWidth") != 0;
					bool psUseBufferHeight = xmlReader->getAttributeValueAsInt("psUseBufferHeight") != 0;
					bool psUseProjection = xmlReader->getAttributeValueAsInt("psUseProjection") != 0;
					bool psUseView = xmlReader->getAttributeValueAsInt("psUseView") != 0;
					bool psUseWorld = xmlReader->getAttributeValueAsInt("psUseWorld") != 0;
					bool psUseWorldView = xmlReader->getAttributeValueAsInt("psUseWorldView") != 0;
					bool psUseWorldViewProj = xmlReader->getAttributeValueAsInt("psUseWorldViewProj") != 0;
					
					// create a new shader post process material
					currentPostProcess = new CShaderPostProcess(Device, name, vsFile, vsEntry, vsType, psFile, psEntry, psType, baseMaterial);
					
					// set pixel shader flags
					currentPostProcess->getShaderMaterial()->setPixelShaderFlag(ESC_TIME, psUseElapsedTime);
					currentPostProcess->getShaderMaterial()->setPixelShaderFlag(ESC_RANDOM, psUseRandom);
					currentPostProcess->getShaderMaterial()->setPixelShaderFlag(ESC_BUFFERWIDTH, psUseBufferWidth);
					currentPostProcess->getShaderMaterial()->setPixelShaderFlag(ESC_BUFFERHEIGHT, psUseBufferHeight);
					currentPostProcess->getShaderMaterial()->setPixelShaderFlag(ESC_PROJECTION, psUseProjection);
					currentPostProcess->getShaderMaterial()->setPixelShaderFlag(ESC_VIEW, psUseView);
					currentPostProcess->getShaderMaterial()->setPixelShaderFlag(ESC_WORLD, psUseWorld);
					currentPostProcess->getShaderMaterial()->setPixelShaderFlag(ESC_WORLDVIEW, psUseWorldView);
					currentPostProcess->getShaderMaterial()->setPixelShaderFlag(ESC_WORLDVIEWPROJ, psUseWorldViewProj);

					// set vertex shader flags
					currentPostProcess->getShaderMaterial()->setVertexShaderFlag(ESC_TIME, vsUseElapsedTime);
					currentPostProcess->getShaderMaterial()->setVertexShaderFlag(ESC_RANDOM, vsUseRandom);
					currentPostProcess->getShaderMaterial()->setVertexShaderFlag(ESC_BUFFERWIDTH, vsUseBufferWidth);
					currentPostProcess->getShaderMaterial()->setVertexShaderFlag(ESC_BUFFERHEIGHT, vsUseBufferHeight);
					currentPostProcess->getShaderMaterial()->setVertexShaderFlag(ESC_PROJECTION, vsUseProjection);
					currentPostProcess->getShaderMaterial()->setVertexShaderFlag(ESC_VIEW, vsUseView);
					currentPostProcess->getShaderMaterial()->setVertexShaderFlag(ESC_WORLD, vsUseWorld);
					currentPostProcess->getShaderMaterial()->setVertexShaderFlag(ESC_WORLDVIEW, vsUseWorldView);
					currentPostProcess->getShaderMaterial()->setVertexShaderFlag(ESC_WORLDVIEWPROJ, vsUseWorldViewProj);

					// push back the post process into the effect chain
					currentEffectChain->push_back(currentPostProcess);
				}
				
				// read vertex shader constants from the xml-file
				if (vsConstant.equals_ignore_case(xmlReader->getNodeName()) && 
					currentPostProcess)
				{
					// add the defined constants to the postprocess
					core::stringw name = xmlReader->getAttributeValueSafe("name");
					f32 value = xmlReader->getAttributeValueAsFloat("value");
					currentPostProcess->getShaderMaterial()->setVertexShaderConstant(name, value);
				}

				// read pixel shader constants from the xml-file
				if (psConstant.equals_ignore_case(xmlReader->getNodeName()) && 
					currentPostProcess)
				{
					// add the defined constants to the postprocess
					core::stringw name = xmlReader->getAttributeValueSafe("name");
					f32 value = xmlReader->getAttributeValueAsFloat("value");
					currentPostProcess->getShaderMaterial()->setPixelShaderConstant(name, value);
				}

				// read input texture properties from the xml-file
				if (textureTag.equals_ignore_case(xmlReader->getNodeName()) && 
					currentPostProcess)
				{
					// read texture properties
					u32 index = (u32) xmlReader->getAttributeValueAsInt("index");
					core::stringw texPath = xmlReader->getAttributeValueSafe("path");
					core::stringw texName = xmlReader->getAttributeValueSafe("name");
					video::E_TEXTURE_CLAMP texClamp = (video::E_TEXTURE_CLAMP)xmlReader->getAttributeValueAsInt("textureClamp");
					s8 texLODBias = (s8) xmlReader->getAttributeValueAsInt("lodBias");
					bool bilinearFilter = true;
					bool trilinearFilter = false;
					bool anisotropicFilter = false;
					if (xmlReader->getAttributeValue("bilinearFilter"))
						bilinearFilter = xmlReader->getAttributeValueAsInt("bilinearFilter") != 0;
					if (xmlReader->getAttributeValue("trilinearFilter"))
						trilinearFilter = xmlReader->getAttributeValueAsInt("trilinearFilter") != 0;
					if (xmlReader->getAttributeValue("anisotropicFilter"))
						anisotropicFilter = xmlReader->getAttributeValueAsInt("anisotropicFilter") != 0;

					if(index < video::MATERIAL_MAX_TEXTURES)
					{
						// set texture and clamp
						if(texPath!="")
							currentPostProcess->getMaterial().TextureLayer[index].Texture = Device->getVideoDriver()->getTexture(texPath);
						
						// set texture properties
						currentPostProcess->getMaterial().TextureLayer[index].TextureWrapU = texClamp;
						currentPostProcess->getMaterial().TextureLayer[index].TextureWrapV = texClamp;
						currentPostProcess->getMaterial().TextureLayer[index].LODBias = texLODBias;
						currentPostProcess->getMaterial().TextureLayer[index].BilinearFilter = bilinearFilter;
						currentPostProcess->getMaterial().TextureLayer[index].TrilinearFilter = trilinearFilter;
						currentPostProcess->getMaterial().TextureLayer[index].AnisotropicFilter = anisotropicFilter;
						
						// set texture name (used for glsl)
						if(texName!="")
							currentPostProcess->getShaderMaterial()->setTextureName(index, texName);
					}
				}

				// read render target for the postprocess from the xml-file
				if (renderSourceTag.equals_ignore_case(xmlReader->getNodeName()) && 
					currentEffectChain && currentPostProcess)
				{
					// set render target of the postprocess
					core::stringw texPath = xmlReader->getAttributeValueSafe("path");
					currentPostProcess->setRenderSource(texPath);
				}

				// read render target for the postprocess from the xml-file
				if (renderTargetTag.equals_ignore_case(xmlReader->getNodeName()) && 
					currentEffectChain && currentPostProcess)
				{
					// set render target of the postprocess
					core::stringw texPath = xmlReader->getAttributeValueSafe("path");
					currentPostProcess->setRenderTarget(texPath);
				}
				break;
			}
		}
	}
	delete xmlReader;    
}