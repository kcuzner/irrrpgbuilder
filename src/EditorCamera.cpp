#include "EditorCamera.h"

#include "SoundManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//const irr::f32 EditorCamera::cameraHeight = 4.0f;
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
		case 1: cam->setFOV(0.65f);
				cameraHeight = 4.0f;
				break;
		case 2: cam->setFOV(1.256637f);
				cameraHeight = 3.0f;
				break;
	}
    cam->setPosition(vector3df(0,cameraHeight,0));
    cam->setTarget(vector3df(0,0,cameraHeight/2.0f));
    cam->setFarValue(cameraHeight*3.0f);
	
    cam->setNearValue(0.1f);
}

int EditorCamera::getCamera()
{
	return camera;
}

void EditorCamera::setCameraHeight(irr::f32 increments)
{
	cameraHeight-=(increments/10);
	if (cameraHeight>10) 
		cameraHeight=10;
	if (cameraHeight<2)
		cameraHeight=2;
	cam->setPosition(vector3df(cam->getPosition().X,cameraHeight,cam->getPosition().Z));
    cam->setFarValue(cameraHeight*3.0f);
}

f32 EditorCamera::getCameraHeight()
{
	return cameraHeight;
}

void EditorCamera::moveCamera(vector3df pos)
{
    cam->setPosition(cam->getPosition() + pos);
	switch (camera)
	{
		case 1:	cam->setTarget(cam->getPosition() + vector3df(0,-cam->getPosition().Y,cam->getPosition().Y) );
				break;
		case 2:	cam->setTarget(cam->getPosition() + vector3df(0,-cam->getPosition().Y,cam->getPosition().Y/2));
				break;
	}
}

void EditorCamera::setPosition(vector3df pos)
{
	switch (camera)
	{
		case 1:	cam->setPosition(vector3df(pos.X,cam->getPosition().Y,pos.Z-cameraHeight));
				cam->setTarget(cam->getPosition() + vector3df(0,-cam->getPosition().Y,cam->getPosition().Y) );
				break;
		case 2: cam->setPosition(vector3df(pos.X,cam->getPosition().Y,pos.Z-cameraHeight/2));
				cam->setTarget(cam->getPosition() + vector3df(0,-cam->getPosition().Y,cam->getPosition().Y/2));
				break;
	}
}

vector3df EditorCamera::getTarget()
{
	vector3df target = vector3df(0.0f,0.0f,0.0f);
	switch (camera)
	{
		case 1: target = vector3df(cam->getPosition() + vector3df(0,-cam->getPosition().Y,cam->getPosition().Y) );
				break;
		case 2: target = vector3df(cam->getPosition() + vector3df(0,-cam->getPosition().Y,cam->getPosition().Y/2) );
				break;
	}
    return target;
}
