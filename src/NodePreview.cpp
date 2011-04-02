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
	this->backgroundcolor = video::SColor(255,64,64,64);
	this->background = true;
	this->border = false;
	rotation=0;

}
/*NodePreview::NodePreview(rect<s32> viewPort, ISceneNode* node)
{
    this->node = node;
    this->viewPort = viewPort;
	rotation=0;
}*/

void NodePreview::drawBackground(bool draw)
{
	this->background = draw;
}

void NodePreview::drawBorder(bool draw)
{
	this->border = draw;
}

void NodePreview::setBackgroundColor(video::SColor color)
{
	this->backgroundcolor = color;
}

NodePreview::~NodePreview()
{
}

void NodePreview::draw()
{
    if(!node) return;

	rotation=rotation+0.075f;
	if (rotation>360)
		rotation=0;
	
	IGUISkin* skin = Environment->getSkin();
	core::rect<s32> frameRect(AbsoluteRect);

	if (background)
	{
		skin->draw3DSunkenPane(this, backgroundcolor,
			true, true, frameRect, &AbsoluteClippingRect);
	}
	
	core::rect<s32> originalViewport = driver->getViewPort();
	
    driver->setViewPort(frameRect);

    matrix4 oldProjMat = driver->getTransform(video::ETS_PROJECTION);
    matrix4 oldViewMat = driver->getTransform(video::ETS_VIEW);
    matrix4 oldWorldMat = driver->getTransform(video::ETS_WORLD);

    /*matrix4 mat = core::IdentityMatrix;
	matrix4 mat2 = core::IdentityMatrix;

	mat.setScale(1.8f * node->getScale().X);
	mat.setScale((node->getBoundingBox().getExtent().Z)); 
	//+ node->getBoundingBox().getExtent().X )/3 );
	mat.setScale(node->getBoundingBox().getExtent().Y);
	//* node->getScale().X
    mat.setTranslation(vector3df(-node->getPosition().X * 1.8f,-node->getPosition().Y * 1.8f - 0.9f,-node->getPosition().Z * 1.8f));
	mat2.setRotationDegrees(vector3df(-35.0f,0,0.0f));
	mat.setRotationDegrees(vector3df(0.0f,rotation,0.0f));
	//driver->setTransform ( video::ETS_PROJECTION, mat2 );
    //driver->setTransform ( video::ETS_VIEW, mat );
    //driver->setTransform ( video::ETS_WORLD, core::IdentityMatrix ); */

	// Determine the best distance for the camera also find the center of the model.
	f32 scale1 = (node->getBoundingBox().getExtent().Y)/2;
	f32 distance = scale1;
	f32 scale2 = (node->getBoundingBox().getExtent().X)/2;
	f32 scale3 = (node->getBoundingBox().getExtent().Z)/2;
	if (scale2>distance)
		distance=scale2;
	if (scale3>distance)
		distance=scale3;

	
	f32 aspect = ((f32)frameRect.getWidth()/frameRect.getHeight()); // Calculated aspect look strange (elongated)
	SViewFrustum ViewArea;

	// Define a perspective projection
	ViewArea.getTransform ( video::ETS_PROJECTION ).buildProjectionMatrixPerspectiveFovLH(0.5f, aspect, 0.1f, 12000.0f);

	// Define a camera that is rotating around it's target
	vector3df camera = (node->getPosition()+vector3df(0,scale1,0) + vector3df(0,rotation,0).rotationToDirection()) *(distance*4.5f);
	ViewArea.getTransform(video::ETS_VIEW).buildCameraLookAtMatrixLH(camera, node->getPosition()+vector3df(0,scale1,0), vector3df(0,1,0));
    
	// Do the transformations (perspective cam)
	driver->setTransform ( video::ETS_PROJECTION, ViewArea.getTransform ( video::ETS_PROJECTION) );
	driver->setTransform ( video::ETS_VIEW, ViewArea.getTransform ( video::ETS_VIEW) );
    driver->setTransform ( video::ETS_WORLD, core::IdentityMatrix );

	//printf ("Determined scale is: %f\n",scale1);
	//printf ("Determined distance is: %f\n",(camera).getDistanceFrom(vector3df(0,scale1,0)));
	//printf ("Current Position is %f, %f, %f\n",node->getPosition().X,node->getPosition().Y,node->getPosition().Z);
	//printf ("Coordinates are: %f, %f, %f\n\n",(camera).X,(camera).Y,(camera).Z);

    node->render();

	// Put back the original transformations
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