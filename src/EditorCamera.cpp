#include "EditorCamera.h"

#include "SoundManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//const irr::f32 EditorCamera::cameraHeight = 3.0f;
irr::f32 EditorCamera::cameraHeight = 4.0f;

EditorCamera::EditorCamera()
{
	camera=2;
	cam = App::getInstance()->getDevice()->getSceneManager()->addCameraSceneNode();
	setCamera(camera);
}

EditorCamera::~EditorCamera()
{
    //dtor
}

EditorCamera* EditorCamera::getInstance()
{
    static EditorCamera *instance = 0;
    if (!instance) instance = new EditorCamera();
    return instance;
}

vector3df EditorCamera::getPosition()
{
    return cam->getPosition();
}

void EditorCamera::setCamera(int tempCamera)
{
	camera = tempCamera;
	switch (camera)
	{
		// Camera 1 - Gameplay
		case 1: fov=0.65f;
				cameraHeight = 4.0f;
				distance = 1;
				break;
		// Camera 2 - Editing
		case 2: fov=0.45f;
				//fov=1.256637f;
				//cameraHeight = 3.0f;
				//distance = 1.0f;
				cameraHeight = 7.0f;
				distance = 0.8f;
				break;
	}
	cam->setFOV(fov);
    
    cam->setTarget(vector3df(0,0,cameraHeight/2.0f));
	cam->setPosition(vector3df(0,cameraHeight,0));
    cam->setFarValue(cameraHeight*3.0f);
	
    cam->setNearValue(0.1f);
}

int EditorCamera::getCamera()
{
	return camera;
}

void EditorCamera::setCameraHeight(irr::f32 increments)
{
	int max = 0;
	int min = 0;
	cameraHeight-=(increments/10);
	switch (camera)
	{
		case 1: max = 6;
				min = 2;	
				break;
		case 2: max = 12;
				min = 2;
				break;
	}
	if (cameraHeight>max) 
		cameraHeight=max;
	if (cameraHeight<min)
		cameraHeight=min;
	cam->setPosition(vector3df(cam->getPosition().X,cameraHeight,cam->getPosition().Z));
    cam->setFarValue(cameraHeight*3.0f);
	
	cam->setTarget(getTarget());
}

f32 EditorCamera::getCameraHeight()
{
	return cameraHeight;
}

void EditorCamera::moveCamera(vector3df pos)
{
    cam->setPosition(cam->getPosition() + pos);
	cam->setTarget(cam->getPosition() + vector3df(0,-cam->getPosition().Y,cam->getPosition().Y/distance));
}

void EditorCamera::setPosition(vector3df pos)
{
	cam->setPosition(vector3df(pos.X,cam->getPosition().Y,pos.Z-cameraHeight/distance));
	cam->setTarget(cam->getPosition() + vector3df(0,-cam->getPosition().Y,cam->getPosition().Y/distance));
}

ICameraSceneNode* EditorCamera::getNode()
{
	return cam;
}

vector3df EditorCamera::getTarget()
{
	vector3df target = vector3df(cam->getPosition() + vector3df(0,-cam->getPosition().Y,cam->getPosition().Y/distance) );
	return target;
}
