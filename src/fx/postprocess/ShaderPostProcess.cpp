#include "ShaderPostProcess.h"

CShaderPostProcess::CShaderPostProcess(IrrlichtDevice* device, const core::stringw& name,
		io::path vs_file, core::stringc vs_entry, video::E_VERTEX_SHADER_TYPE vs_type,
		io::path ps_file, core::stringc ps_entry, video::E_PIXEL_SHADER_TYPE ps_type,
		video::E_MATERIAL_TYPE baseMaterial)
: IPostProcess(name), Device(device)
{
	// get the shader material fomr the material manager
	ShaderMaterial = new CShaderMaterial(device, name,vs_file, vs_entry, vs_type, ps_file, ps_entry, ps_type, baseMaterial);
	
	// set material parameters 
	getMaterial().Lighting = false;
	getMaterial().BackfaceCulling = false;
	getMaterial().ZBuffer = video::ECFN_ALWAYS; 
}

CShaderPostProcess::~CShaderPostProcess()
{
	// drop the shader material
	if (ShaderMaterial)
		ShaderMaterial->drop();
}

void CShaderPostProcess::render()
{
	// clear the projection matrix 
	Device->getVideoDriver()->setTransform(video::ETS_PROJECTION, core::IdentityMatrix); 
	
	// clear the view matrix 
	Device->getVideoDriver()->setTransform(video::ETS_VIEW, core::IdentityMatrix); 

	// set the transform
	Device->getVideoDriver()->setTransform(video::ETS_WORLD, core::IdentityMatrix ); 
	
	// select the post proc material
	Device->getVideoDriver()->setMaterial(getMaterial());

	// render the screen quad
	Device->getVideoDriver()->drawIndexedTriangleList(Vertices, 4, Indices, 2);
}
