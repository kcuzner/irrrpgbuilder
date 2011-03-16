#ifndef CameraSystem_H
#define CameraSystem_H

#include "App.h"
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

        virtual ~CameraSystem();
    protected:
    private:
        CameraSystem();

        ICameraSceneNode* cam;
		//static const irr::f32 cameraHeight;
		static irr::f32 cameraHeight;
		int camera;
		int minCamera;
		int maxCamera;

		f32 fov;
};

#endif // CameraSystem_H
