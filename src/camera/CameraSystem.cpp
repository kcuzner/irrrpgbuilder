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
	editCamMaya = addCameraSceneNodeMaya(0, -45.0f, 200.0f, 100.0f);
	editCamMaya->setFarValue(16000);
	editCamMaya->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
	editCamMaya->setPosition(vector3df(0,1000,-1000));
	editCamMaya->setTarget(vector3df(0,0,0));
	setCamera(camera);
}

CameraSystem::~CameraSystem()
{
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
	switch (camera)
	{
		// Camera 1 - Gameplay
		case 1: fov=0.65f;
			gameCam->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
				cameraHeight = 350.0f;
				currentCam = gameCam;
				currentCam->setPosition(vector3df(0,cameraHeight,0));
				gameCam->setFarValue(50000);
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
		ISceneNodeAnimator* anm = new CSceneNodeAnimatorCameraMayaIRB(App::getInstance()->getDevice()->getCursorControl(),
			rotateSpeed, zoomSpeed, translationSpeed, distance);

		node->addAnimator(anm);
		anm->drop();
	}

	return node;
}

void CameraSystem::setCameraHeight(irr::f32 increments)
{
	f32 max = 0;
	f32 min = 0;
	cameraHeight-=(increments/10);
	switch (camera)
	{
		/*case 1: max = 6;
				min = 2;
				break;*/
		case 1: max = 800;
				min = 100;
				break;
		case 2: max = 10000;
				min = 100;
				break;
	}
	if (cameraHeight>max)
		cameraHeight=max;
	if (cameraHeight<min)
		cameraHeight=min;
	if (camera==2 && cameraHeight!=min && cameraHeight!=max)
	{
		core::list<ISceneNodeAnimator*>::ConstIterator anims=editCamMaya->getAnimators().begin();
		CSceneNodeAnimatorCameraMayaIRB* anm=(CSceneNodeAnimatorCameraMayaIRB*)*anims;

		f32 distance = anm->getDistance();
		distance=distance+(increments*10.0f);
		anm->setDistance(distance);
		//anm->drop();

	}
	else
		gameCam->setPosition(vector3df(gameCam->getPosition().X,cameraHeight,gameCam->getPosition().Z));

    gameCam->setFarValue(cameraHeight*3.0f);
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
	currentCam->setPosition(vector3df(pos.X,currentCam->getPosition().Y,pos.Z-cameraHeight));
	currentCam->setTarget(getTarget());
	//cam->setTarget(cam->getPosition() + vector3df(0,-cam->getPosition().Y+36,cam->getPosition().Y));
}

// This method update the point&click camera
void CameraSystem::updatePointClickCam()
{


	// Get the player and find a "reference" position based on it.
	core::vector3df camrefpos = Player::getInstance()->getObject()->getPosition();
	camrefpos.Y=currentCam->getPosition().Y;
	camrefpos.Z=camrefpos.Z-cameraHeight;

	// Find the distance between the current camera and the reference position
	f32 camdistance = this->getPosition().getDistanceFrom(camrefpos);

	// Distance from what the camera start to move and follow the player
	if (camdistance>20)
	{
		f32 camdistance2 = 2400/camdistance;
		vector3df result = (camrefpos-currentCam->getPosition());
		currentCam->setPosition(currentCam->getPosition()+(result/camdistance2));
		currentCam->setTarget(getTarget());
	}


	// If the distance if greater than 250 unit the camera will follow the player at each move
	if (camdistance>250)
	{
		setPosition(Player::getInstance()->getObject()->getPosition());
	}
}

ICameraSceneNode* CameraSystem::getNode()
{
	return currentCam;
}

vector3df CameraSystem::getTarget()
{
	vector3df playerpos = Player::getInstance()->getObject()->getPosition();
	vector3df target = vector3df(currentCam->getPosition() + vector3df(0,-currentCam->getPosition().Y+playerpos.Y+36,currentCam->getPosition().Y) );
	return target;
}

void CameraSystem::fixRatio(IVideoDriver * driver)
{
	dimension2d<u32> screensize = driver->getScreenSize();
	currentCam->setAspectRatio((irr::f32)screensize.Width/screensize.Height);
}
