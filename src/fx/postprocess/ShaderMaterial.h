#ifndef _SHADERMATERIAL_H
#define _SHADERMATERIAL_H
 
#include <irrlicht.h>

using namespace irr;

// built in shader constants
// combine using bitwise or
enum E_SHADER_CONSTANT
{
	ESC_TIME =			0x1,
	ESC_RANDOM =		0x2,
	ESC_BUFFERWIDTH =	0x4,
	ESC_BUFFERHEIGHT =	0x8,
	ESC_PROJECTION =	0x10,
	ESC_VIEW =			0x20,
	ESC_WORLD =			0x40,
	ESC_WORLDVIEW =		0x80,
	ESC_WORLDVIEWPROJ =	0x100
};

class CShaderMaterial : public video::IShaderConstantSetCallBack
{
public:
	// constructor
	CShaderMaterial(IrrlichtDevice* device, const core::stringw& name,
		io::path vs_file, core::stringc vs_entry, video::E_VERTEX_SHADER_TYPE vs_type,
		io::path ps_file, core::stringc ps_entry, video::E_PIXEL_SHADER_TYPE ps_type,
		video::E_MATERIAL_TYPE baseMaterial);

protected:
	// the irrlicht device
	IrrlichtDevice* Device;

	// shader constant maps
	core::map<core::stringc, f32> PixelShaderConstant;
	core::map<core::stringc, f32> VertexShaderConstant;

	// texture names
	core::stringc TextureName[video::MATERIAL_MAX_TEXTURES];

	// material id
	core::stringw Name;

	// material
	video::SMaterial Material;

	// additional built in shader constants
	u32 PixelShaderFlags;
	u32 VertexShaderFlags;

public:
	// OnSetConstants callback to transfer shader constant to the gpu program
	virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userdata);
	
	// sets pixel shader constant
	void setPixelShaderConstant(const core::stringc& name, f32 value) { PixelShaderConstant[name] = value; }
	// returns pixel shader constant
	f32 getPixelShaderConstant(const core::stringc& name) { return PixelShaderConstant[name]; }
	// returns a pixel shader constant iterator
	core::map<core::stringc, f32>::Iterator getPixelShaderConstantIterator() { return PixelShaderConstant.getIterator(); }
	// sets pixel shader flag (bitwise or combination)
	void setPixelShaderFlag(E_SHADER_CONSTANT flag, bool enabled=true) { PixelShaderFlags = (PixelShaderFlags & (~flag)) | ((((u32)!enabled)-1) & flag); }
	// returns pixel shader flag
	bool getPixelShaderFlag(E_SHADER_CONSTANT flag) const { return (PixelShaderFlags& flag)!=0; }

	// set vertex shader constants
	void setVertexShaderConstant(const core::stringc& name, f32 value) { VertexShaderConstant[name] = value; }
	// returns vertex shader constant
	f32 getVertexShaderConstant(const core::stringc& name) { return VertexShaderConstant[name]; }
	// returnsa  vertex shader constant iterator
	core::map<core::stringc, f32>::Iterator getVertexShaderConstantIterator() { return VertexShaderConstant.getIterator(); }
	// sets vertex shader flag
	void setVertexShaderFlag(E_SHADER_CONSTANT flag, bool enabled=true) { VertexShaderFlags = (VertexShaderFlags & (~flag)) | ((((u32)!enabled)-1) & flag); }
	// returns vertex shader flag
	bool getVertexShaderFlag(E_SHADER_CONSTANT flag) const { return (VertexShaderFlags& flag)!=0; }
	
	// sets texture name (used for opengl gpu programs)
	void setTextureName(u32 index, const core::stringc& name) { TextureName[index] = name; }

	// returns the SMaterial struct of the shader material
	virtual video::SMaterial& getMaterial() { return Material; }

	// returns the material type of the shader material
	virtual video::E_MATERIAL_TYPE getMaterialType() const { return Material.MaterialType; }

	// returns the name of the material
	const core::stringw& getName() const { return Name; }
};

#endif
