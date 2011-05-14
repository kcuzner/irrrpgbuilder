#include "NodePreview.h"
#include "TerrainManager.h"

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
	f32 increment=0.05f;

	bool camdriven=false;
	// disabling this for the moment. Found a way to scale all to 1 unit.
	//if (distance>3) 
	//	camdriven=true;
	if (!camdriven)
		increment=0.5f;

	rotation=rotation+increment;
	if (rotation>360)
		rotation=0;

	/*if (camdriven)
	{
		// the camera have also the scale factor
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
	
		fakecam->setPosition(node->getPosition()+vector3df(0,scale1/2,0)+vector3df(0,rotation,0).rotationToDirection()*(distance*2));
		fakecam->setTarget(node->getPosition()+vector3df(0,scale1,0));
		fakecam->setAspectRatio(aspect);

		// Hide the terrain
		TerrainManager::getInstance()->setVisible(false);

		
		if (!node->isVisible())
		{
			node->setVisible(true);
			smgr->drawAll();
			node->setVisible(false);
		}
		else
			//node->render();
			smgr->drawAll();
		
		TerrainManager::getInstance()->setVisible(true);
	}
	else*/
	{
	
		matrix4 oldProjMat = driver->getTransform(video::ETS_PROJECTION);
		matrix4 oldViewMat = driver->getTransform(video::ETS_VIEW);

		SViewFrustum ViewArea;
	

		// Define a perspective projection
		ViewArea.getTransform ( video::ETS_PROJECTION ).buildProjectionMatrixPerspectiveFovLH(0.5f, aspect, 0.1f, 9600.0f);

		// Define a camera that is rotating around it's target
		vector3df camera = (node->getPosition()+vector3df(0,scale1,0) + vector3df(0,rotation,0).rotationToDirection()); 
		camera.X = camera.X *(distance*3.0f);
		camera.Z = camera.Z *(distance*3.0f);
		
	
		ViewArea.getTransform(video::ETS_VIEW).buildCameraLookAtMatrixLH(camera, node->getPosition()+vector3df(0,scale1,0), vector3df(0,1,0));

		// Recalculate the view area from the camera position
		ViewArea.cameraPosition = camera;

		core::matrix4 m(core::matrix4::EM4CONST_NOTHING);
		m.setbyproduct_nocheck(ViewArea.getTransform(video::ETS_PROJECTION),
			ViewArea.getTransform(video::ETS_VIEW));
		ViewArea.setFrom(m);

		f32 scaleaa = node->getScale().Y;
		//printf ("Here is the current scale of the mesh: %f\n-----------------------\n",scaleaa);
	
		// Do the transformations (perspective cam)
		driver->setTransform ( video::ETS_PROJECTION, ViewArea.getTransform ( video::ETS_PROJECTION));
		driver->setTransform ( video::ETS_VIEW, ViewArea.getTransform ( video::ETS_VIEW));
	
		// Node seem to render the meshbuffers directly (scaling of the mesh does not seem to be accounted for)
		driver->setTransform(video::ETS_WORLD, node->getAbsoluteTransformation());
		node->render();
		

		// Put back the original transformations
		driver->setTransform ( video::ETS_PROJECTION, oldProjMat );
	}

    driver->setViewPort(originalViewport);	
	//if (camdriven)
	//	smgr->setActiveCamera(oldcam);
		
	// draw children
	IGUIElement::draw();

}

void NodePreview::setNode(ISceneNode* node)
{
    this->node = node;
	smgr = node->getSceneManager();
	//if (!fakecam)
	//	fakecam=smgr->addCameraSceneNode(0,vector3df(72,36,72),vector3df(0,36,0),-1,false);
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