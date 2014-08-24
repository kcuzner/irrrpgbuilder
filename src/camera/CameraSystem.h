#ifndef CameraSystem_H
#define CameraSystem_H

#include "../App.h"
#include "../objects/Player.h"
#include "CSceneNodeAnimatorCameraMayaIRB.h"
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

		enum CAMERA_TYPE
		{
			CAMERA_GAME = 1,
			CAMERA_EDIT = 2,
			CAMERA_CUTSCENE = 3,
			CAMERA_COUNT = 4
		};

		enum VIEW_TYPE
		{
			VIEW_RTS = 0,
			VIEW_RTS_FIXED = 1,
			VIEW_RPG = 2,
			VIEW_FPS = 3,
			VIEW_HYBRID_FPS = 4,
			VIEW_COUNT = 5
		};

		enum CONTROL_TYPE
		{
			CONTROL_POINTNCLICK = 0,
			CONTROL_WASD = 1,
			CONTROL_COUNT = 2
		};

        static CameraSystem* getInstance();

		void setCamera(CAMERA_TYPE tempCamera);
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

 		void updatePointClickCam();

		ICameraSceneNode* getNode();
   
		void fixRatio(IVideoDriver * driver);

        virtual ~CameraSystem();
		scene::ILightSceneNode * light;
		scene::ILightSceneNode * sun;

		ICameraSceneNode* editCamMaya;
		ICameraSceneNode* cutsceneCam;
		ICameraSceneNode* gameCam;

		void setPointNClickAngle(vector2df angle);
		inline vector2df getPointNClickAngle() {return vector2df(cameraAngle.X,cameraAngle.Y);}

		void moveCamera(vector3df pos);
		vector3df getPosition();
		void setPosition(vector3df pos);
		core::vector3df getTarget();
		inline vector3df getAngle() {return cameraAngle;}
		void setRTSView();
		inline void setRTSFixedView() {viewtype=VIEW_RTS_FIXED;}
		void setRPGView();

		inline void setMAYAPos(vector3df pos){anm->setPosition(pos);}
		inline void setMAYATarget(vector3df pos){anm->setTarget(pos);}
		inline void setGameCameraRange(f32 min,f32 max) {gameCamRangeMin=min; gameCamRangeMax=max;}
		inline core::position2df getGameCameraRange() {position2df ranges; ranges.X=gameCamRangeMin; ranges.Y=gameCamRangeMax; return ranges;}
		inline vector2df getGameCameraAngleLimit() {return cameraAngleLimit;}
		inline void setGameCameraAngleLimit( vector2df limits) {cameraAngleLimit=limits;}
		inline void setCameraZoom(f32 zoom){cameraHeight=zoom;}
		inline f32 getCameraZoom(){return cameraHeight;}
		
    protected:
    private:
        CameraSystem();

		ICameraSceneNode* editCamFPS;
		ICameraSceneNode* currentCam;

		CSceneNodeAnimatorCameraMayaIRB* anm;
		
		// For the point&Click Camera
		static irr::f32 cameraHeight;
		vector3df cameraAngle;
		irr::f32 cameraRotationSpeed;
		irr::f32 cameraTargetHeight;
		vector2df cameraAngleLimit;


		CAMERA_TYPE camera;
		int minCamera;
		int maxCamera;

		bool lightset;

		f32 fov;

		VIEW_TYPE viewtype;
		CONTROL_TYPE controltype;

		f32 oldrot;

		u32 counter;

		u32 refreshdelay;

		f32 gameCamRangeMin;
		f32 gameCamRangeMax;
};

#endif // CameraSystem_H
