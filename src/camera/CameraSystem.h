#ifndef CameraSystem_H
#define CameraSystem_H

#include "../App.h"  // Most important stuff needed here (device, etc)
#include "../objects/Player.h" //Player is needed to control the player character with the camera controls
#include "CSceneNodeAnimatorCameraMayaIRB.h" // New type of edit camera (modified Irrlich Maya camera)
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

// NOTE:
// The camera refresh are done using this sequences:

// APP->DynamicObjectManager->DynamicObject->CameraSystem

// If the DynamicObject is a PLAYER then the refresh is triggered.
// This help maintain a better sync when the player is moving

// NOTE2:
// The events are received in this class from this order only in "PlAY MODE":

// EventReceiver->App->CameraSystem
 

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
			VIEW_RTS = 1,
			VIEW_RTS_FIXED = 2,
			VIEW_RPG = 3,
			VIEW_FPS = 4,
			VIEW_HYBRID_FPS = 5,
			VIEW_COUNT = 6
		};

		enum CONTROL_TYPE
		{
			CONTROL_POINTNCLICK = 0,
			CONTROL_WASD = 1,
			CONTROL_COUNT = 2
		};

        static CameraSystem* getInstance();
		virtual ~CameraSystem();

		// Events received to for the controls of the camera
		void eventsKeyboard(s32 key);
		void eventsMouseKey(s32 key);
		void eventMouseWheel(f32 value);

		// Compare strings and set the key to the appropriate action
		void defineKeys(core::stringc key, core::stringc action);

		// Define the type of camera (ingame, edit, cutscene)
		void setCamera(CAMERA_TYPE tempCamera);
		CameraSystem::CAMERA_TYPE getCamera();

		//! Update the current viewtype of camera
		void updateGameCamera();

		//! Adds a camera scene node which is able to be controlle with the mouse similar
		//! like in the 3D Software Maya by Alias Wavefront.
		//! The returned pointer must not be dropped.
		virtual ICameraSceneNode* addCameraSceneNodeMaya(ISceneNode* parent=0,
			f32 rotateSpeed=-1500.f, f32 zoomSpeed=200.f,
			f32 translationSpeed=1500.f, s32 id=-1, f32 distance=70.f,
			bool makeActive=true);

		void setCameraHeight(f32 increments);
		f32 getCameraHeight();

 		ICameraSceneNode* getNode();
   
		void fixRatio(IVideoDriver * driver);

        //Might be removed, should be in APP, Player or Terrain classes
		scene::ILightSceneNode * light;
		scene::ILightSceneNode * sun;

		ICameraSceneNode* editCamMaya;
		ICameraSceneNode* cutsceneCam;
		ICameraSceneNode* gameCam;

		void moveCamera(vector3df pos);
		vector3df getPosition();
		void setPosition(vector3df pos);
		core::vector3df getTarget();
		inline vector3df getAngle() {return cameraAngle;}
		
		//Define the view types
		void setViewType(VIEW_TYPE view);  
		inline VIEW_TYPE getViewType() {return viewtype;}

		//Control of the Edit Camera
		inline void setMAYAPos(vector3df pos){anm->setPosition(pos);}
		inline void setMAYATarget(vector3df pos){anm->setTarget(pos);}
		
		// Define the range of the RTS/RPG zoom range
		inline void setGameCameraRange(f32 min,f32 max) {gameCamRangeMin=min; gameCamRangeMax=max;}
		inline core::position2df getGameCameraRange() {position2df ranges; ranges.X=gameCamRangeMin; ranges.Y=gameCamRangeMax; return ranges;}

		// Define the zoom distance of the RTS/RPG camera
		inline void setCameraZoom(f32 zoom){cameraHeight=zoom;}
		inline f32 getCameraZoom(){return cameraHeight;}
		
		// Define the up/down angles limits of the RTS/RPG camera
		inline vector2df getGameCameraAngleLimit() {return cameraAngleLimit;}
		inline void setGameCameraAngleLimit( vector2df limits) {cameraAngleLimit=limits;}

		// Define the angle of the RTS/RPG Camera (around the player)
		void setPointNClickAngle(vector2df angle); //Also changed from LUAGlobalCaller
		inline vector2df getPointNClickAngle() {return vector2df(cameraAngle.X,cameraAngle.Y);}
		inline void setBoneName(core::stringc name) {bonename=name;}
		inline stringc getBoneName() {return bonename;}
		inline void setBoneOffset(core::vector3df offset) {boneoffsetpos = offset;}
		inline vector3df getBoneOffset() {return boneoffsetpos;}
		
		
		
    protected:
    private:
        CameraSystem();
		void updatePointClickCam(); // For RTS control / views
		void findCamAngle(); // For RTS / RPG views types
		void updateFPSCamera();
		void updateRPGCamera();

		vector3df getDirections();

		IrrlichtDevice *device;

		// Used to store the defined keys for FPS/RPG camera control
		s32 keyforward;
		s32 keybackward;
		s32 keyleftside;
		s32 keyrightside;
		s32 keyaction;
		s32 keyinteraction;
		s32 keyinventory;

		bool speedtoggle;
		f32 speed;

		ICameraSceneNode* editCamFPS;
		ICameraSceneNode* currentCam;

		CSceneNodeAnimatorCameraMayaIRB* anm;
		
		// For the point&Click Camera
		irr::f32 cameraHeight;
		vector3df cameraAngle;
		vector3df oldCameraAngle;
		irr::f32 cameraRotationSpeed;
		irr::f32 cameraTargetHeight;
		vector2df cameraAngleLimit;

		//to know if we had an interface appeared while in FPS or RPG view
		bool interface_toggle;

		bool lightset;

		f32 fov;

		CAMERA_TYPE camera;
		VIEW_TYPE viewtype;
		CONTROL_TYPE controltype;

		f32 oldrot;

		f32 gameCamRangeMin;
		f32 gameCamRangeMax;

		bool initrotation; //for the camera rotation
		vector2df oldmouse;
		vector2df initangle; //initial angle of the RTS camera (Calculated from here)

		stringc bonename;
		vector3df boneoffsetpos;
};

#endif // CameraSystem_H
