#ifndef _IPOSTPROCESS_H
#define _IPOSTPROCESS_H
 
#include <irrlicht.h>

using namespace irr;

class IPostProcess : virtual public IReferenceCounted
{
public:
	// constructor
	IPostProcess(const core::stringw& name)
	{
		// create screen quad vertices
		Vertices[0] = video::S3DVertex(-1.f, -1.f, 0.f, 0.f, 0.f, 1.f, video::SColor(255,255,255,255), 0.f, 1.f); 
		Vertices[1] = video::S3DVertex(-1.f,  1.f, 0.f, 0.f, 0.f, 1.f, video::SColor(255,255,255,255), 0.f, 0.f); 
		Vertices[2] = video::S3DVertex( 1.f,  1.f, 0.f, 0.f, 0.f, 1.f, video::SColor(255,255,255,255), 1.f, 0.f); 
		Vertices[3] = video::S3DVertex( 1.f, -1.f, 0.f, 0.f, 0.f, 1.f, video::SColor(255,255,255,255), 1.f, 1.f); 
		
		// set indices
		Indices[0] = 0; 
		Indices[1] = 1; 
		Indices[2] = 2; 
		Indices[3] = 2; 
		Indices[4] = 3; 
		Indices[5] = 0; 
	
		// set name of the postprocess
		Name = name;
	}

protected:
	// screend quad mesh
	video::S3DVertex Vertices[4];
	u16 Indices[6];

	// name of the postprocess
	core::stringw Name;

	// input and output target for the postprocess
	core::stringw RenderTarget;
	core::stringw RenderSource;

public:
	// set the rendertarget
	virtual void setRenderTarget(const core::stringw& renderTarget) { RenderTarget = renderTarget; }
	// returns the rendertarget
	virtual const core::stringw& getRenderTarget() { return RenderTarget; }
	
	// sets the rendersource (texturelayer 0 of the postprocess material)
	virtual void setRenderSource(const core::stringw& renderSource) { RenderSource = renderSource; }
	// returns the rendersource
	virtual const core::stringw& getRenderSource() { return RenderSource; }

	// returns the name of the postprocess
	virtual const core::stringw& getName() { return Name; }
	
	// returns the SMaterial struct of the postprocess
	virtual video::SMaterial& getMaterial() = 0;

	// renders the postprocess
	virtual void render() = 0;
};

#endif
