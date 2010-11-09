#include "EditorCamera.h"

#include "SoundManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

const irr::f32 EditorCamera::cameraHeight = 3.0f;

EditorCamera::EditorCamera()
{
    cam = App::getInstance()->getDevice()->getSceneManager()->addCameraSceneNode();
    cam->setPosition(vector3df(0,cameraHeight,0));
    cam->setTarget(vector3df(0,0,cameraHeight/2.0f));
    cam->setFarValue(cameraHeight*2.0f);
    cam->setNearValue(0.1f);
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

void EditorCamera::moveCamera(vector3df pos)
{
    cam->setPosition(cam->getPosition() + pos);
    cam->setTarget(cam->getPosition() + vector3df(0,-cam->getPosition().Y,cam->getPosition().Y/2) );
}

void EditorCamera::setPosition(vector3df pos)
{
    cam->setPosition(vector3df(pos.X,cam->getPosition().Y,pos.Z-cameraHeight/2));
    cam->setTarget(cam->getPosition() + vector3df(0,-cam->getPosition().Y,cam->getPosition().Y/2) );
}

vector3df EditorCamera::getTarget()
{
    vector3df target = vector3df(cam->getPosition() + vector3df(0,-cam->getPosition().Y,cam->getPosition().Y/2) );

    return target;
}
