#include "CameraSystem.h"

#include "SoundManager.h"

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
	camera=2;
	lightset=false;
	cam = App::getInstance()->getDevice()->getSceneManager()->addCameraSceneNode();
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
    return cam->getPosition();
}

void CameraSystem::setCamera(int tempCamera)
{
	camera = tempCamera;
	switch (camera)
	{
		// Camera 1 - Gameplay
		case 1: fov=0.65f;
				cameraHeight = 350.0f;
				break;

		// Camera 2 - Editing
		case 2: fov=0.45f;
				cameraHeight = 1000.0f;
				break;
	}
	cam->setFOV(fov);
	cam->setTarget(getTarget());
	
	cam->setPosition(vector3df(0,cameraHeight,0));
    cam->setFarValue(cameraHeight*3.0f);
	
    //cam->setNearValue(12.0f);
	// Add a specular light to the camera.
	if (!lightset)
	{
		SColorf color = App::getInstance()->getDevice()->getSceneManager()->getAmbientLight();
		light = App::getInstance()->getDevice()->getSceneManager()->addLightSceneNode(cam,vector3df(0,0,250),video::SColorf(0.5f,0.5f,0.6f),250);
		sun = App::getInstance()->getDevice()->getSceneManager()->addLightSceneNode(0,vector3df(0,500,0),color,250);
		lightset=true;
	}
}

int CameraSystem::getCamera()
{
	return camera;
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
		cam->setPosition(vector3df(cam->getPosition().X,cameraHeight,cam->getPosition().Z+(increments*0.1f)));
	else
		cam->setPosition(vector3df(cam->getPosition().X,cameraHeight,cam->getPosition().Z));

    cam->setFarValue(cameraHeight*3.0f);
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
    cam->setPosition(cam->getPosition() + pos);
	cam->setTarget(getTarget());
	//cam->setTarget(cam->getPosition() + vector3df(0,-cam->getPosition().Y+36,cam->getPosition().Y));
}

void CameraSystem::setPosition(vector3df pos)
{
	cam->setPosition(vector3df(pos.X,cam->getPosition().Y,pos.Z-cameraHeight));
	cam->setTarget(getTarget());
	//cam->setTarget(cam->getPosition() + vector3df(0,-cam->getPosition().Y+36,cam->getPosition().Y));
}

ICameraSceneNode* CameraSystem::getNode()
{
	return cam;
}

vector3df CameraSystem::getTarget()
{
	vector3df playerpos = Player::getInstance()->getObject()->getPosition();
	vector3df target = vector3df(cam->getPosition() + vector3df(0,-cam->getPosition().Y+playerpos.Y+36,cam->getPosition().Y) );
	return target;
}

void CameraSystem::fixRatio(IVideoDriver * driver)
{
	dimension2d<u32> screensize = driver->getScreenSize();
	cam->setAspectRatio((irr::f32)screensize.Width/screensize.Height);
}