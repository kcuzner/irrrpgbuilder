#include "CameraSystem.h"

#include "../sound/SoundManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

irr::f32 CameraSystem::cameraHeight = 4.0f;

CameraSystem::CameraSystem()
{

	refreshdelay = App::getInstance()->getDevice()->getTimer()->getRealTime();
	lightset=false;
	this->light=NULL;
	this->sun=NULL;
	camera=2;

	// Create the cutscene camera
	cutsceneCam = App::getInstance()->getDevice()->getSceneManager()->addCameraSceneNode();
	cutsceneCam->setFarValue(5000);
	cutsceneCam->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
	
	// Create the ingame camera (RTS mecanic)
	gameCam = App::getInstance()->getDevice()->getSceneManager()->addCameraSceneNode();
	gameCam->setFarValue(5000);
	gameCam->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
	viewtype = VIEW_RTS;
	controltype = CONTROL_POINTNCLICK;
	

	// New edit camera
	editCamMaya = addCameraSceneNodeMaya(0, -450.0f, 800.0f, 400.0f);
	editCamMaya->setFarValue(5000);
	editCamMaya->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
	editCamMaya->setPosition(vector3df(0,1000,-1000));
	editCamMaya->setTarget(vector3df(0,0,0));
	setCamera(camera);

	oldrot=-90+Player::getInstance()->getObject()->getRotation().Y;

	
	
}

CameraSystem::~CameraSystem()
{
	if (anm)
		anm->drop();

	if (cutsceneCam)
		cutsceneCam->drop();

	if (gameCam)
		gameCam->drop();

	if (editCamMaya)
		editCamMaya->drop();

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
		case 1: fov=0.85f;
				gameCam->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
				cameraHeight = 600.0f;
				cameraAngle = vector3df(135.0f,45.0f,0);
				gameCam->setRotation(cameraAngle);
				cameraRotationSpeed = 15.0f;
				cameraTargetHeight = 48.0f;
				currentCam = gameCam;
				gameCam->bindTargetAndRotation(true);
				this->updatePointClickCam();

				// Use the RTS view by default
				viewtype=VIEW_RTS;
				break;

		// Camera 2 - Editing
		case 2: fov=0.45f;
				cameraHeight = 1000.0f;
				currentCam = editCamMaya;
				editCamMaya->setUpVector(vector3df(0,1,0));
		        editCamMaya->setPosition(vector3df(0.0f,1000.0f,-1000.0f));
				editCamMaya->setTarget(vector3df(0.0f,0.0f,0.0f));

				//editCamMaya->setInputReceiverEnabled(false);
				editCamMaya->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
				editCamMaya->setInputReceiverEnabled(true);
				//editCamMaya->setInputReceiverEnabled(false);
				editCamMaya->setFarValue(cameraHeight*3.0f);
	
				break;

		// Camera 3 - Cutscene
		case 3: fov=0.45f;
				cutsceneCam->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
				currentCam=cutsceneCam;
				break;
	}
	App::getInstance()->getDevice()->getSceneManager()->setActiveCamera(currentCam);

	currentCam->setFOV(fov);
	currentCam->setTarget(currentCam->getTarget());

	// Add a specular light to the camera.
	if (lightset)
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

// Note: Taken and modified from IRRlicht
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
		case 1: max = 2000;
				min = 72;
				gameCam->setFarValue(10000.0f);
				gameCam->setNearValue(1.0f);
				break;
		case 2: max = 10000;
				min = 144;
				editCamMaya->setFarValue(35000.0f);
				editCamMaya->setNearValue(1.0f);
				break;
	}

	if (camera==1)
		updatePointClickCam();

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
		if (distance<1.0f) 
		{
			distance=1.0f;
			GUIManager::getInstance()->setConsoleText(L"Reached limit of camera zoom!",video::SColor(255,180,0,0));
		}
		//if (distance>144)
		//	editCamMaya->setFarValue(distance*4.0f);

	}
	else
	{
		if (camera==1)
		{
			// Get the distance and set it on the ingame camera (point & click)
			f32 distance=Player::getInstance()->getNode()->getPosition().getDistanceFrom(gameCam->getPosition());
			cameraHeight-=(increments*(distance/10));
			updatePointClickCam();
			f32 camdist = currentCam->getPosition().getDistanceFrom(currentCam->getTarget());
		}
	}
}

// Return the distance between the subject and the camera
f32 CameraSystem::getCameraHeight()
{
	return cameraHeight;
}


// This method update the point&click camera
void CameraSystem::updatePointClickCam()
{
	if (camera==1)

	{
	// Get the player and find a "reference" position based on it.
	core::vector3df camrefpos = Player::getInstance()->getObject()->getPosition();
	camrefpos.Y+=cameraTargetHeight;

	// Find the distance between the current camera and the reference position
	f32 camdistance = this->getPosition().getDistanceFrom(camrefpos);

	// Initialize the vector and set the distance of the camera toward it
	vector3df pos=vector3df(0,0,0);
	pos.X+=cameraHeight;

	// Offset from the reference position (the player)
	pos+=camrefpos;
	

	// This update the camera view when it's set as a RPG Camera
	// Parent is player, and use the player angle as reference
	if (viewtype==VIEW_RPG)
	{
		f32 camrefangle=0;

		if (Player::getInstance()->getObject()->isWalking())
			camrefangle = -90+(Player::getInstance()->getObject()->getRotation()).Y;
		else
			camrefangle = cameraAngle.X;

		// Limit camera and oldrot to stay in 0-360 degree range
		if (camrefangle<0)
			camrefangle+=360;

		if (camrefangle>360)
			camrefangle-=360;

		if (oldrot<0)
			oldrot+=360;

		if (oldrot>360)
			oldrot-=360;
		
		f32 diff = camrefangle-oldrot;
	
		// Turn on the other side if the difference is too great
		if (diff>181)
		{
			camrefangle=180-camrefangle;
			diff = camrefangle+oldrot;
		}

		// Turn on the other side if the difference is too great
		if (diff<-181)
		{
			camrefangle=180+camrefangle;
			diff = camrefangle+oldrot;
		}

		// The "matching speed" of the rotation
		f32 matchspeed = 0.1f;
		bool correction=true;

		if (diff<matchspeed && diff>matchspeed)
			correction=false;

		if (diff>matchspeed && correction)
		{

			//camrefangle=oldrot+(matchspeed);
			if (diff>45)
				camrefangle=oldrot+(matchspeed*5);
			if (diff>20 && diff<46)
				camrefangle=oldrot+(matchspeed*2);
			if (diff<21)
				camrefangle=oldrot+(matchspeed);

		}
		
		if (diff<-matchspeed && correction)
		{
			//camrefangle=oldrot-(matchspeed);
			if (diff<-45)
				camrefangle=oldrot-(matchspeed*5);
			if (diff<-20 && diff>-46)
				camrefangle=oldrot-(matchspeed*2);
			if (diff>-21)
				camrefangle=oldrot-(matchspeed);
		}

		if (Player::getInstance()->getObject()->isWalking())
			cameraAngle.X=camrefangle;

		oldrot=camrefangle;

	}

	// Do the rotation calculation
	pos.rotateXYBy(cameraAngle.Y, camrefpos);
	pos.rotateXZBy(-cameraAngle.X, camrefpos);

	// Set the position and angle of the cam
	currentCam->setPosition(pos);
	currentCam->setTarget(camrefpos);
	}
}

//! Will update the angle of the pointNClick camera by mouse offsets
void CameraSystem::SetPointNClickAngle(vector2df angle)
{
	//This is updated as fast as the CPU can. Will have to implement timed update to have the proper value of angle
	angle*=cameraRotationSpeed;
    cameraAngle.X+=-(angle.X/2);
	cameraAngle.Y+=-(angle.Y/2);

	// Limit the view
	if (cameraAngle.Y>89.0f)
		cameraAngle.Y=89.0f;
	if (cameraAngle.Y<-25.0f)
		cameraAngle.Y=-25.0f;
}

// Return the current camera node pointer
ICameraSceneNode* CameraSystem::getNode()
{
	return currentCam;
}


void CameraSystem::fixRatio(IVideoDriver * driver)
{
	dimension2d<u32> screensize = driver->getScreenSize();
	currentCam->setAspectRatio((irr::f32)screensize.Width/screensize.Height);
}

//! Move the current camera
void CameraSystem::moveCamera(vector3df pos)
{
    currentCam->setPosition(currentCam->getPosition() + pos);
	currentCam->setTarget(currentCam->getTarget() + pos);
}

void CameraSystem::setPosition(vector3df pos)
{
	currentCam->setPosition(pos);
}

core::vector3df CameraSystem::getTarget()
{
	return currentCam->getTarget();
}

void CameraSystem::setRTSView()
{
	viewtype=VIEW_RTS;
}

void CameraSystem::setRPGView()
{
	viewtype=VIEW_RPG;
}