#ifndef CameraSystem_H
#define CameraSystem_H

#include "../App.h"
#include "../objects/Player.h"
#include "ISceneNodeAnimatorCameraMaya.h"
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class CameraSystem
{
    public:
        static CameraSystem* getInstance();

		void setCamera(int tempCamera);
		int getCamera();

		void setCameraHeight(f32 increments);
		f32 getCameraHeight();

        void moveCamera(vector3df pos);
        void setPosition(vector3df pos);
		
		ICameraSceneNode* getNode();

        vector3df getPosition();
        vector3df getTarget();

		void fixRatio(IVideoDriver * driver);

        virtual ~CameraSystem();
		scene::ILightSceneNode * light;
		scene::ILightSceneNode * sun;

		ICameraSceneNode* editCamMaya;

    protected:
    private:
        CameraSystem();

        ICameraSceneNode* gameCam;
		
		ICameraSceneNode* editCamFPS;
		ICameraSceneNode* currentCam;
		//static const irr::f32 cameraHeight;
		static irr::f32 cameraHeight;


		int camera;
		int minCamera;
		int maxCamera;

		bool lightset;

		f32 fov;
};

#endif // CameraSystem_H
