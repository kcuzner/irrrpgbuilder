#ifndef EDITORCAMERA_H
#define EDITORCAMERA_H

#include "App.h"
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class EditorCamera
{
    public:
        static EditorCamera* getInstance();

		void setCamera(int tempCamera);
		int getCamera();

		void setCameraHeight(f32 increments);
		f32 getCameraHeight();

        void moveCamera(vector3df pos);
        void setPosition(vector3df pos);

        vector3df getPosition();
        vector3df getTarget();

        virtual ~EditorCamera();
    protected:
    private:
        EditorCamera();

        ICameraSceneNode* cam;
		//static const irr::f32 cameraHeight;
		static irr::f32 cameraHeight;
		int camera;
		int minCamera;
		int maxCamera;
};

#endif // EDITORCAMERA_H
