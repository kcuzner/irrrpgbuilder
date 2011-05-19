#include "NodePreview.h"
#include "TerrainManager.h"
#include "Player.h"

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
	
	
	this->node = NULL;
	this->viewPort = rectangle;
		
	this->backgroundcolor = video::SColor(255,64,64,64);
	this->background = true;
	this->border = false;

	rotation=0;
	guienv=environment;
	smgr=NULL;
	fakecam=NULL;
	driver = environment->getVideoDriver();

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
	fakecam->remove();
}

void NodePreview::draw()
{
    if(!node || !smgr) return;
	
	IGUISkin* skin = Environment->getSkin();
	core::rect<s32> frameRect(AbsoluteRect);

	if (background)
	{
		skin->draw3DSunkenPane(this, backgroundcolor,
			true, true, frameRect, &AbsoluteClippingRect);
	}
	
	core::rect<s32> originalViewport = driver->getViewPort();
	scene::ICameraSceneNode * oldcam = smgr->getActiveCamera();
	
	// Determine the best distance for the camera also find the center of the model.
	f32 scale1 = (node->getBoundingBox().getExtent().Y)/2;
	f32 distance = scale1*2;
	f32 scale2 = (node->getBoundingBox().getExtent().X)/2;
	f32 scale3 = (node->getBoundingBox().getExtent().Z)/2;
	if (scale2>distance)
		distance=scale2*2;
	if (scale3>distance)
		distance=scale3*2;

	// Define the aspect ratio for the "camera"
	f32 aspect = ((f32)frameRect.getWidth()/frameRect.getHeight()); 

    driver->setViewPort(frameRect);
	f32 increment=0.5f;
	rotation=rotation+increment;

	if (rotation>360)
		rotation=0;

	//the camera have also the scale factor
	scale1=scale1*node->getScale().Y;
	scale2=scale2*node->getScale().X;
	scale3=scale3*node->getScale().Z;

	// Now find the best distance
	distance=scale1;
	if (scale2>distance)
		distance=scale2;
	if (scale3>distance)
		distance=scale3;

	if (fakecam) 
		smgr->setActiveCamera(fakecam);
	
	fakecam->setFarValue(distance*5);
	
	node->setPosition(vector3df(0,-1000,0));
	fakecam->setPosition(node->getPosition()+vector3df(0,scale1,0)+vector3df(0,rotation,0).rotationToDirection()*(distance*2.5f));
	fakecam->setTarget(node->getPosition()+vector3df(0,scale1,0));
	fakecam->setAspectRatio(aspect);

	// Hide the terrain
	TerrainManager::getInstance()->setVisible(false);

		
	if (!node->isVisible())
	{
		node->setVisible(true);
		//node2->render();
		smgr->drawAll();
		node->setVisible(false);
	}
	else
		//node2->render();
		smgr->drawAll();
		
	TerrainManager::getInstance()->setVisible(true);
		
    driver->setViewPort(originalViewport);	
	smgr->setActiveCamera(oldcam);
		
	// draw children
	IGUIElement::draw();

}

void NodePreview::setNode(ISceneNode* node)
{
	this->node=node;
	
	smgr = node->getSceneManager();
	if (!fakecam)
		fakecam=smgr->addCameraSceneNode(0,vector3df(72,36,72),vector3df(0,36,0),-1,false);
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