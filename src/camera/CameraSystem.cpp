#include "CameraSystem.h"

#include "../sound/SoundManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//const irr::f32 CameraSystem::cameraHeight = 3.0f;
irr::f32 CameraSystem::cameraHeight = 4.0f;

CameraSystem::CameraSystem()
{

	refreshdelay = App::getInstance()->getDevice()->getTimer()->getRealTime();
	camera=2;
	lightset=false;
	gameCam = App::getInstance()->getDevice()->getSceneManager()->addCameraSceneNode();
	gameCam->setFarValue(5000);
	gameCam->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
	

	// New edit camera

	editCamMaya = addCameraSceneNodeMaya(0, -450.0f, 800.0f, 400.0f);

	editCamMaya->setFarValue(5000);
	editCamMaya->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
	editCamMaya->setPosition(vector3df(0,1000,-1000));
	editCamMaya->setTarget(vector3df(0,0,0));
	setCamera(camera);
}

CameraSystem::~CameraSystem()
{
	anm->drop();
    //dtor
}

CameraSystem* CameraSystem::getInstance()
{
    static CameraSystem *instance = 0;
    if (!instance) instance = new CameraSystem();
    return instance;
}

vector3df CameraSystem::getPosition()
{
    return currentCam->getPosition();
}

void CameraSystem::setCamera(int tempCamera)
{
	camera = tempCamera;

	// Get the distance from the current cam
	f32 camdist = 0.0f;
	switch (camera)
	{
		// Camera 1 - Gameplay
		case 1: fov=0.50f;
			gameCam->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
				cameraHeight = 600.0f;
				currentCam = gameCam;
				currentCam->setPosition(vector3df(cameraHeight/3.0f,cameraHeight,cameraHeight/3.0f));
				break;

		// Camera 2 - Editing
		case 2: fov=0.45f;
				cameraHeight = 1000.0f;
				currentCam = editCamMaya;
				editCamMaya->setInputReceiverEnabled(false);
				//editCamMaya->setPosition(vector3df(1000,cameraHeight,-1000));
				editCamMaya->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
				editCamMaya->setInputReceiverEnabled(true);
				editCamMaya->setInputReceiverEnabled(false);
				//editCamMaya->setFarValue(cameraHeight*3.0f);
				break;
	}
	App::getInstance()->getDevice()->getSceneManager()->setActiveCamera(currentCam);

	currentCam->setFOV(fov);
	currentCam->setTarget(getTarget());




    //cam->setNearValue(12.0f);
	// Add a specular light to the camera.
	if (!lightset)
	{
		SColorf color = App::getInstance()->getDevice()->getSceneManager()->getAmbientLight();
		light = App::getInstance()->getDevice()->getSceneManager()->addLightSceneNode(gameCam,vector3df(0,0,250),video::SColorf(0.5f,0.5f,0.6f),250);
		sun = App::getInstance()->getDevice()->getSceneManager()->addLightSceneNode(0,vector3df(0,500,0),color,250);
		lightset=true;
	}
}

int CameraSystem::getCamera()
{
	return camera;
}

// Note: Taken directly from IRRlicht
//! Adds a camera scene node which is able to be controlled with the mouse similar
//! to in the 3D Software Maya by Alias Wavefront.
//! The returned pointer must not be dropped.
ICameraSceneNode* CameraSystem::addCameraSceneNodeMaya(ISceneNode* parent,
	f32 rotateSpeed, f32 zoomSpeed, f32 translationSpeed, s32 id, f32 distance,
	bool makeActive)
{
	ICameraSceneNode* node = App::getInstance()->getDevice()->getSceneManager()->addCameraSceneNode(parent, core::vector3df(),
			core::vector3df(0,0,100), id, makeActive);
	if (node)
	{
		anm = new CSceneNodeAnimatorCameraMayaIRB(App::getInstance()->getDevice()->getCursorControl(),
			rotateSpeed, zoomSpeed, translationSpeed, distance);

		node->addAnimator((ISceneNodeAnimator*)anm);
		
	}

	return node;
}

void CameraSystem::setCameraHeight(irr::f32 increments)
{
	f32 max = 0;
	f32 min = 0;
	
	switch (camera)
	{
		/*case 1: max = 6;
				min = 2;
				break;*/
		case 1: max = 1500;
				min = 180;
				break;
		case 2: max = 10000;
				min = 144;
				break;
	}
	if (cameraHeight>max)
		cameraHeight=max;
	if (cameraHeight<min)
		cameraHeight=min;
	if (camera==2 && cameraHeight!=min && cameraHeight!=max)
	{
		// Get the distance and set it on the edit camera
		f32 distance = anm->getDistance();
		distance=distance+(increments*(distance/10));
		anm->setDistance(distance);
		if (distance>144)
			editCamMaya->setFarValue(distance*4.0f);
	}
	else
	{
		// Get the distance and set it on the ingame camera (point & click)
		f32 distance=Player::getInstance()->getNode()->getPosition().getDistanceFrom(gameCam->getPosition());
		cameraHeight-=(increments*(distance/10));

		gameCam->setPosition(vector3df(gameCam->getPosition().X,Player::getInstance()->getNode()->getAbsolutePosition().Y+cameraHeight,
			gameCam->getPosition().Z));
		f32 camdist = currentCam->getPosition().getDistanceFrom(currentCam->getTarget());
		if (camdist>144)
			gameCam->setFarValue(camdist*2.5f);
	}

	
    //gameCam->setFarValue(cameraHeight*20.0f);
	//vector3df newtarget = this->getTarget();
	//newtarget.Y = newtarget.Y;
	//cam->setTarget(newtarget);
}

f32 CameraSystem::getCameraHeight()
{
	return cameraHeight;
}

void CameraSystem::moveCamera(vector3df pos)
{
	
    currentCam->setPosition(currentCam->getPosition() + pos);
	currentCam->setTarget(currentCam->getTarget() + pos);
	//cam->setTarget(cam->getPosition() + vector3df(0,-cam->getPosition().Y+36,cam->getPosition().Y));
}

void CameraSystem::setPosition(vector3df pos)
{
	currentCam->setPosition(vector3df(pos.X-cameraHeight/3,currentCam->getPosition().Y,pos.Z-cameraHeight/3));
	currentCam->setTarget(getTarget());
	//cam->setTarget(cam->getPosition() + vector3df(0,-cam->getPosition().Y+36,cam->getPosition().Y));
}

// This method update the point&click camera
void CameraSystem::updatePointClickCam()
{
	// Get the player and find a "reference" position based on it.
	core::vector3df camrefpos = Player::getInstance()->getObject()->getPosition();
	camrefpos.X=Player::getInstance()->getNode()->getAbsolutePosition().X-(cameraHeight/3);
	camrefpos.Y=Player::getInstance()->getNode()->getAbsolutePosition().Y+cameraHeight;
	camrefpos.Z=camrefpos.Z-(cameraHeight/3);

	// Find the distance between the current camera and the reference position
	f32 camdistance = this->getPosition().getDistanceFrom(camrefpos);

	// Distance from what the camera start to move and follow the player
	if (camdistance>20)
	{
		// Offset for the camera position
		f32 camdistance2 = 2400.0f/camdistance;

		// Offset for the target position
		f32 camdistance3 = 1600.0f/camdistance;

		// Calculate the new camera position
		vector3df result = (camrefpos-currentCam->getPosition());
		currentCam->setPosition(currentCam->getPosition()+(result/camdistance2));

		// Calculate the new target position 
		vector3df result_target = (getTarget()-currentCam->getTarget());
		currentCam->setTarget(currentCam->getTarget()+(result_target/camdistance3));
	}


	// If the distance if greater than 250 unit the camera will follow the player at each move
	if (camdistance>250)
	{
		setPosition(Player::getInstance()->getObject()->getPosition());
		currentCam->setTarget(this->getTarget());
	}
}

ICameraSceneNode* CameraSystem::getNode()
{
	return currentCam;
}

vector3df CameraSystem::getTarget()
{
	vector3df playerpos = Player::getInstance()->getObject()->getPosition();
	//vector3df target = vector3df(currentCam->getPosition() + vector3df(0,-currentCam->getPosition().Y+playerpos.Y+36,currentCam->getPosition().Y) );
	//return target;
	playerpos.Y+=36;
	return playerpos;
}

void CameraSystem::fixRatio(IVideoDriver * driver)
{
	dimension2d<u32> screensize = driver->getScreenSize();
	currentCam->setAspectRatio((irr::f32)screensize.Width/screensize.Height);
}
