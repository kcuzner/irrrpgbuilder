#include "NodePreview.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace irr
{
namespace gui
{

	NodePreview::NodePreview(IGUIEnvironment* environment, IGUIElement* parent, const core::rect<s32>& rectangle, s32 id) 
		: IGUIElement(EGUIET_IMAGE, environment ,parent ,id ,rectangle)

{
	driver = environment->getVideoDriver();
	
	this->node = NULL;
	this->viewPort = rectangle;
		//this->getParent()->getAbsoluteClippingRect();
	
	rotation=0;

}
/*NodePreview::NodePreview(rect<s32> viewPort, ISceneNode* node)
{
    this->node = node;
    this->viewPort = viewPort;
	rotation=0;
}*/

NodePreview::~NodePreview()
{
}

void NodePreview::draw()
{
    if(!node) return;


	rotation=rotation+0.15f;
	if (rotation>360)
		rotation=0;
	
	IGUISkin* skin = Environment->getSkin();
	core::rect<s32> frameRect(AbsoluteRect);
	skin->draw3DSunkenPane(this, video::SColor(255,64,64,64) ,
		true, true, frameRect, &AbsoluteClippingRect);
	
	core::rect<s32> originalViewport = driver->getViewPort();
	
    driver->setViewPort(frameRect);

    matrix4 oldProjMat = driver->getTransform(video::ETS_PROJECTION);
    matrix4 oldViewMat = driver->getTransform(video::ETS_VIEW);
    matrix4 oldWorldMat = driver->getTransform(video::ETS_WORLD);

    matrix4 mat = core::IdentityMatrix;
	matrix4 mat2 = core::IdentityMatrix;
    //mat.setScale(1.8f * node->getScale().X);
	//mat.setScale((node->getBoundingBox().getExtent().Z)); 
	//+ node->getBoundingBox().getExtent().X )/3 );
	//mat.setScale(node->getScale().Y);
	
	//* node->getScale().X

    mat.setTranslation(vector3df(-node->getPosition().X * 1.8f,-node->getPosition().Y * 1.8f - 0.9f,-node->getPosition().Z * 1.8f));
	
	mat2.setRotationDegrees(vector3df(-35.0f,0,0.0f));
	mat.setRotationDegrees(vector3df(0.0f,rotation,0.0f));

    driver->setTransform ( video::ETS_PROJECTION, mat2 );
    driver->setTransform ( video::ETS_VIEW, mat );
    driver->setTransform ( video::ETS_WORLD, core::IdentityMatrix );

    node->render();

    driver->setTransform ( video::ETS_PROJECTION, oldProjMat );
    driver->setTransform ( video::ETS_VIEW, oldViewMat );
    driver->setTransform ( video::ETS_WORLD, oldWorldMat );

    driver->setViewPort(originalViewport);
	// draw children
	IGUIElement::draw();
	
}

void NodePreview::setNode(ISceneNode* node)
{
    this->node = node;
}

ISceneNode* NodePreview::getNode()
{
    return node;
}

void NodePreview::setViewport(core::rect<s32> viewPort)
{
    this->viewPort = viewPort;
}

rect<s32> NodePreview::getViewPort()
{
    return viewPort;
}

} // end namespace
} // end namespace