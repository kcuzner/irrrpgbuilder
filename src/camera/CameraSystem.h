#ifndef CameraSystem_H
#define CameraSystem_H

#include "../App.h"
#include "../objects/Player.h"
#include "CSceneNodeAnimatorCameraMaya.h"
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

		//! Adds a camera scene node which is able to be controlle with the mouse similar
		//! like in the 3D Software Maya by Alias Wavefront.
		//! The returned pointer must not be dropped.
		virtual ICameraSceneNode* addCameraSceneNodeMaya(ISceneNode* parent=0,
			f32 rotateSpeed=-1500.f, f32 zoomSpeed=200.f,
			f32 translationSpeed=1500.f, s32 id=-1, f32 distance=70.f,
			bool makeActive=true);

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
