#include "NodePreview.h"
#include "../terrain/TerrainManager.h"
#include "../objects/Player.h"

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

	/*
    if(!smgr) return;
	//if(!node || !smgr) return;
	
	IGUISkin* skin = Environment->getSkin();
	core::rect<s32> frameRect(AbsoluteRect);

	if (background)
	{
		skin->draw3DSunkenPane(this, backgroundcolor,
			true, true, frameRect, &AbsoluteClippingRect);
	}
	
	core::rect<s32> originalViewport = driver->getViewPort();
	scene::ICameraSceneNode * oldcam = smgr->getActiveCamera();
	
	//init values
	f32 scale1 = 1.0f;
	f32 distance = 1.0f;
	f32 scale2 = 1.0f;
	f32 scale3 = 1.0f;

	if (node)
	{
		// Determine the best distance for the camera also find the center of the model.
		scale1 = (node->getBoundingBox().getExtent().Y)/2;
		distance = scale1*2;
		scale2 = (node->getBoundingBox().getExtent().X)/2;
		scale3 = (node->getBoundingBox().getExtent().Z)/2;
		if (scale2>distance)
			distance=scale2*2;
		if (scale3>distance)
			distance=scale3*2;
	}

	// Define the aspect ratio for the "camera"
	f32 aspect = ((f32)frameRect.getWidth()/frameRect.getHeight()); 

    driver->setViewPort(frameRect);
	f32 increment=0.5f;
	rotation=rotation+increment;

	if (rotation>360)
		rotation=0;

	//the camera have also the scale factor
	if (node)
	{
		scale1=scale1*node->getScale().Y;
		scale2=scale2*node->getScale().X;
		scale3=scale3*node->getScale().Z;
	}

	// Now find the best distance
	distance=scale1;
	if (scale2>distance)
		distance=scale2;
	if (scale3>distance)
		distance=scale3;

	if (fakecam) 
		smgr->setActiveCamera(fakecam);
	
	fakecam->setFarValue(distance*5);
	if (node)
	{
		//node->setPosition(vector3df(0,-1000,0));
		fakecam->setPosition(node->getPosition()+vector3df(0,scale1,0)+vector3df(0,rotation,0).rotationToDirection()*(distance*2.5f));
		fakecam->setTarget(node->getPosition()+vector3df(0,scale1,0));
		fakecam->setAspectRatio(aspect);
	}
	else
	{	// IF there is no target to display (doesnt seem to be activated when there are not object that are still loaded)
		fakecam->setPosition(vector3df(0.0f,-1000.0f,0.0f));
		fakecam->setTarget(vector3df(0.0f,-1000.0f,10.0f));
	}

	// Hide the terrain
	TerrainManager::getInstance()->setVisible(true);

	if (node)
	{
		if (!node->isVisible())
		{
			node->setVisible(true);
			//node2->render();
			//smgr->drawAll();
			node->render();
			node->setVisible(false);
		} else
			smgr->drawAll();
			//node->render();
	}
	else
		//node2->render();
		smgr->drawAll();
		
	TerrainManager::getInstance()->setVisible(true);
		
    driver->setViewPort(originalViewport);	
	smgr->setActiveCamera(oldcam);
	
	*/
	// draw children
	IGUIElement::draw();

}

void NodePreview::setNode(ISceneNode* node)
{
	if (node)
	{
		this->node=node;
	
		smgr = node->getSceneManager();
	} else
		this->node=NULL;

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